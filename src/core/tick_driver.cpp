#include "core/tick_driver.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <chrono>
#include <cmath>

#include "core/engine.hpp"
#include "macro/macro_engine.hpp"
#include "practice/practice_fix.hpp"
#include "util/midhook.hpp"

using namespace geode::prelude;

double TickDriver::getTimeWarp() const {
    if (auto* pl = PlayLayer::get()) {
        return static_cast<double>(pl->m_gameState.m_timeWarp);
    }
    return 1.0;
}

double TickDriver::visualDt() const {
    return 1.0 / (m_fps * BotOptions::get().speed);
}

void TickDriver::computeSteps(float dt, float targetDt) {
    auto& opt = BotOptions::get();
    dt += static_cast<float>(stepOverflow);

    float wanted = targetDt * std::fmin(getTimeWarp(), 1.0f);
    if (wanted <= 0.0f) {
        return;
    }

    int steps = static_cast<int>(std::floor(dt / wanted));

    if (!opt.realTime && !opt.dynamicStepCap) {
        stepCap = opt.maxStepsPerVisual;
    }

    uint32_t limit = stepCap;

    if (respawnCooldown > 0) {
        stepOverflow = 0.0;
    }

    // Scale UPR by (tps / display fps) when visual updates are enabled
    if (opt.visualStepScale && !opt.dynamicStepCap) {
        float fps = GameManager::get()->m_customFPSTarget;
        if (fps <= 10.0f) {
            fps = 240.0f;
        }
        int modifier = static_cast<int>(opt.tps) / static_cast<int>(fps);
        limit *= static_cast<uint32_t>(std::max(1, modifier));
    }

    if (!opt.realTime) {
        steps = std::min(steps, static_cast<int>(limit));
    }

    stepOverflow = dt - steps * wanted;
    shouldDraw = false;

    if (!opt.realTime && steps == static_cast<int>(limit)) {
        stepOverflow = 0.0;
    }

    if (isFrameAdvance()) {
        steps = 1;
        stepOverflow = 0.0;
        shouldDraw = true;
        if (auto* pl = PlayLayer::get()) {
            pl->m_extraDelta = 0.0f;
        }
    }

    plannedSteps = steps;
    remainingSteps = steps;
}

// ---- lock-delta paths ----------------------------------------------------

static void runFastLockDelta(float realDt, std::function<void(float)> update) {
    auto& eng = Engine::get();
    auto& driver = eng.ticks();
    auto& opt = BotOptions::get();

    driver.allowActionProcess = false;
    auto next = eng.macros().peekCurrent();

    if (!next.has_value()) {
        driver.computeSteps(
            static_cast<float>(realDt * driver.getTimeWarp() * opt.speed),
            static_cast<float>(driver.physicsDt()));
        if (driver.remainingSteps >= 1) {
            update(static_cast<float>(realDt * opt.speed));
        }
        return;
    }

    driver.computeSteps(
        static_cast<float>(realDt * driver.getTimeWarp() * opt.speed),
        static_cast<float>(driver.physicsDt()));

    int steps = driver.plannedSteps;
    while (steps > 0) {
        auto input = eng.macros().peekCurrent();
        uint64_t safe = input.has_value()
                            ? static_cast<uint64_t>(input->frame) - driver.currentFrame()
                            : static_cast<uint64_t>(steps);
        safe = std::min(safe, static_cast<uint64_t>(steps));

        if (safe > 0) {
            driver.remainingSteps = static_cast<int>(safe);
            update(static_cast<float>(driver.physicsDt() * safe));
            steps -= static_cast<int>(safe);
        }

        if (steps > 0) {
            driver.remainingSteps = 1;
            update(static_cast<float>(realDt * opt.speed));
            --steps;
        }
    }
}

static void runAccurateLockDelta(float realDt, std::function<void(float)> update) {
    GJBaseGameLayer* layer = PlayLayer::get();
    if (!layer) {
        layer = LevelEditorLayer::get();
    }
    if (!layer) {
        return;
    }

    auto& eng = Engine::get();
    auto& driver = eng.ticks();
    auto& opt = BotOptions::get();

    auto measureSsb = [&]() -> float {
        float tfp = layer->timeForPos(layer->m_player1->m_position, 0,
                                      layer->m_gameState.m_currentChannel, true, 0);
        float delta = std::abs(tfp - driver.lastTimeForPos);
        driver.lastTimeForPos = tfp;
        if (delta <= 0.f) {
            delta = static_cast<float>(driver.physicsDt());
        }
        return delta;
    };

    auto* pl = PlayLayer::get();
    const bool liveSsb = opt.ssbFix && pl && !pl->m_isPaused &&
                         !pl->m_hasCompletedLevel && pl->m_started &&
                         !pl->m_isPlatformer;

    float ssb = liveSsb ? measureSsb() : static_cast<float>(driver.physicsDt());
    float delta = static_cast<float>(driver.physicsDt());

    driver.computeSteps(
        static_cast<float>(realDt * driver.getTimeWarp() * opt.speed), ssb);

    driver.shouldDraw = false;
    for (int i = 0; i < driver.remainingSteps - 1; ++i) {
        update(delta);
        if (liveSsb && pl && !pl->m_hasCompletedLevel && driver.remainingSteps > 0) {
            ssb = measureSsb();
            driver.computeSteps(
                static_cast<float>(realDt * driver.getTimeWarp() * opt.speed), ssb);
        }
    }

    driver.shouldDraw = true;
    if (driver.remainingSteps > 0) {
        update(delta);
    }
}

void TickDriver::requestBackstep(int frames) {
    if (frames <= 0) {
        return;
    }
    frameAdvance = true;
    backstepPending = true;
    backstepCount = frames;
}

bool TickDriver::consumeBackstepRequest() {
    if (!backstepPending) {
        return false;
    }
    backstepPending = false;
    return true;
}

static void executeBackstep(int n) {
    auto* pl = PlayLayer::get();
    if (!pl) {
        return;
    }
    auto& eng = Engine::get();
    auto& pf = eng.practice();
    auto& opt = BotOptions::get();
    if (!opt.backwardsStepping || !opt.enableFrameHistory) {
        return;
    }

    for (int i = 0; i < n - 1 && pf.frameHistory.size() > 1; ++i) {
        pf.dropNewestFrame();
    }
    if (pf.frameHistory.size() <= 1) {
        return;
    }

    pf.loadStored = true;
    pf.backstepping = true;
    pl->resetLevel();
    pf.loadStored = false;
}

void TickDriver::runFrameUpdates(std::function<void(float)> update, float realDt,
                                 bool frozen) {
    auto& eng = Engine::get();
    allowActionProcess = true;
    auto& opt = BotOptions::get();

    if (frozen) {
        refreshOnly = true;
        update(realDt);
        refreshOnly = false;
        return;
    }

    GJBaseGameLayer* layer = PlayLayer::get();
    bool isPlay = true;
    if (!layer) {
        layer = LevelEditorLayer::get();
        isPlay = false;
    }

    if (!layer || (isPlay && static_cast<PlayLayer*>(layer)->m_isPaused)) {
        stepOverflow = 0.0;
        update(realDt);
        return;
    }

    if (isFrameAdvance()) {
        stepOverflow = 0.0;
        respawnCooldown = 0;
        if (!takeStepRequest()) {
            return;
        }
        shouldDraw = true;
    }

    if (consumeBackstepRequest()) {
        int n = backstepCount;
        defer([n](float) { executeBackstep(n); });
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();

    bool useAccurate = opt.lockDelta &&
                       (opt.lockDeltaMode == static_cast<int>(LockMode::Accurate));

    if (opt.lockDelta && isPlay) {
        if (preferFastLockDelta() && !useAccurate) {
            runFastLockDelta(realDt, update);
        } else {
            runAccurateLockDelta(realDt, update);
        }
    } else {
        shouldDraw = true;
        if (respawnCooldown > 0) {
            stepOverflow = 0.0;
            layer->m_extraDelta = 0.0;
        }
        update(static_cast<float>(realDt * opt.speed));
    }

    if (opt.dynamicStepCap && plannedSteps > 0) {
        auto end = std::chrono::high_resolution_clock::now();
        double secPerStep =
            std::chrono::duration<double>(end - start).count() / plannedSteps;
        double dynamicDt = opt.targetFps * secPerStep;
        if (std::isfinite(dynamicDt) && dynamicDt > 0.0) {
            stepCap = static_cast<uint32_t>(
                std::max(1, static_cast<int>(std::floor(1.0 / dynamicDt))));
        }
    }
}

void TickDriver::abortStepLoop() {
    remainingSteps = 0;
    plannedSteps = 0;
    stepOverflow = 0.0;
}

void TickDriver::defer(std::function<void(float)> fn) {
    m_deferred.push_front(std::move(fn));
}

void TickDriver::runDeferredJobs() {
    if (m_deferred.empty()) {
        return;
    }
    for (auto& fn : m_deferred) {
        fn(0.0f);
    }
    runFrameUpdates(
        [](float dt) {
            cocos2d::CCDirector::sharedDirector()->getScheduler()->update(dt);
        },
        0.0f, true);
    m_deferred.clear();
}

void TickDriver::applyFps(double fps) {
    if (fps <= 0.0) {
        return;
    }
    m_fps = fps;
    CCDirector::sharedDirector()->setAnimationInterval(1.0 / m_fps);
    auto* gm = GameManager::sharedState();
    gm->m_customFPSTarget = static_cast<float>(m_fps);
    gm->setGameVariable("0116", true);
}

void TickDriver::syncAudioPitch() {
    FMOD::ChannelGroup* master = nullptr;
    FMODAudioEngine::get()->m_system->getMasterChannelGroup(&master);
    if (!master) {
        return;
    }
    auto& opt = BotOptions::get();
    master->setPitch(opt.speedhackAudio ? static_cast<float>(opt.speed) : 1.0f);
}

uint32_t TickDriver::currentFrame() const {
    if (PlayLayer::get()) {
        return m_frame + static_cast<uint32_t>(frameAtAttemptStart);
    }
    if (auto* lel = LevelEditorLayer::get()) {
        return static_cast<uint32_t>(std::max(
            0, static_cast<int>(lel->m_gameState.m_currentProgress / 2) - 1));
    }
    return 0;
}

bool TickDriver::preferFastLockDelta() const {
    auto& opt = BotOptions::get();
    return opt.lockDelta && opt.lockDeltaMode == static_cast<int>(LockMode::Fast) &&
           Engine::get().playing();
}

// ---- midhooks: TPS scale, step count, frame tick -------------------------

static void onPhysDt(SafetyHookContext& ctx) {
    auto* layer = GJBaseGameLayer::get();
    if (!layer || !Engine::get().active()) {
        return;
    }
    if (Engine::get().ticks().refreshOnly) {
        return;
    }
    ctx.xmm1.f64[0] *= BotOptions::get().tps / 60.0;
    ctx.rip += 0x08;
}

static void onStepCount(SafetyHookContext& ctx) {
    auto& eng = Engine::get();
    if (!eng.active()) {
        return;
    }
    auto& d = eng.ticks();
    bool fastPath = d.preferFastLockDelta() || !d.lockDeltaActive();
    if (!fastPath && PlayLayer::get()) {
        return;
    }
    // Game counts up to (but not including) 2 → seed counter as 2 - N
    ctx.rdx = 2 - d.remainingSteps;
}

static void onRestorePhysDt(SafetyHookContext& ctx) {
    auto& eng = Engine::get();
    if (!eng.active()) {
        return;
    }
    auto& d = eng.ticks();
    bool fastPath = d.preferFastLockDelta() || !d.lockDeltaActive();
    if (!fastPath && PlayLayer::get()) {
        return;
    }
    ctx.xmm9.f64[0] = d.physicsDt() * d.remainingSteps;
}

static void onFrameTick(SafetyHookContext&) {
    auto& eng = Engine::get();
    if (!eng.active()) {
        return;
    }
    auto* layer = GJBaseGameLayer::get();
    if (!layer || layer->m_resumeTimer > 0) {
        return;
    }
    auto* pl = PlayLayer::get();
    auto& d = eng.ticks();
    if (!layer->m_playerDied && pl) {
        d.bumpFrame();
        d.warpedTime += d.physicsDt() * pl->m_gameState.m_timeWarp;
    }
}

static void onEarlyFrameCapture(SafetyHookContext&) {
    auto& eng = Engine::get();
    if (!eng.active() || eng.ticks().refreshOnly) {
        return;
    }
    auto* pl = PlayLayer::get();
    if (!pl || pl->m_playerDied) {
        return;
    }
    auto& opt = BotOptions::get();
    if (!opt.enableFrameHistory || !opt.backwardsStepping) {
        return;
    }

    CheckpointObject* cp = pl->createCheckpoint();
    if (!cp) {
        return;
    }
    cp->retain();
    eng.practice().rememberFrame(cp, eng.ticks().frameAtAttemptStart);
}

// Windows 2.2074 offsets — re-scan if you retarget another build
$execute {
    util::installMid(geode::base::get() + 0x237A7C, "physDt", onPhysDt);
    util::installMid(geode::base::get() + 0x237DCE, "stepCount", onStepCount);
    util::installMid(geode::base::get() + 0x238F6E, "restorePhysDt", onRestorePhysDt);
    util::installMid(geode::base::get() + 0x238BAA, "frameTick", onFrameTick);
    util::installMid(geode::base::get() + 0x237E42, "earlyFrame", onEarlyFrameCapture);
}
