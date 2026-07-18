#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <vector>

#include "config/options.hpp"
#include "core/engine.hpp"
#include "core/tick_driver.hpp"
#include "macro/action.hpp"
#include "macro/macro_engine.hpp"
#include "practice/practice_fix.hpp"
#include "util/midhook.hpp"

using namespace geode::prelude;

// Patches that keep button state across reset (Win 2.2074)
$execute {
    // GJBaseGameLayer::resetLevelVariables — NOP button release block
    if (!Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x23B4EA),
                           {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90})) {
        log::error("patch resetLevelVariables failed");
    }
    // UILayer::handleKeypress
    if (!Mod::get()->patch(
            reinterpret_cast<void*>(base::get() + 0x4cd95c),
            {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
             0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90})) {
        log::error("patch handleKeypress failed");
    }
    // PlayLayer::resetLevel
    if (!Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x3B994C),
                           {0xeb, 0x5e})) {
        log::error("patch resetLevel failed");
    }
    // PlayLayer::resumeAndRestart
    if (!Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x3BA508),
                           {0x90, 0x90, 0x90, 0x90, 0x90})) {
        log::error("patch resumeAndRestart failed");
    }
}

struct BBBaseLayer : Modify<BBBaseLayer, GJBaseGameLayer> {
    void update(float dt) override {
        auto& eng = Engine::get();
        auto& d = eng.ticks();
        if (d.refreshOnly || !eng.active()) {
            GJBaseGameLayer::update(dt);
            return;
        }

        bool locked = d.lockDeltaActive() && PlayLayer::get();
        if (!locked) {
            d.computeSteps(dt, static_cast<float>(d.physicsDt()));
        }

        if (d.respawnCooldown > 0) {
            --d.respawnCooldown;
            d.plannedSteps = std::min(d.plannedSteps, 1);
            d.remainingSteps = std::min(d.remainingSteps, 1);
        }

        if (locked || d.remainingSteps != 0) {
            GJBaseGameLayer::update(dt);
        }
    }

    double getModifiedDelta(float dt) {
        if (!Engine::get().active()) {
            return GJBaseGameLayer::getModifiedDelta(dt);
        }

        float mod = static_cast<float>(GJBaseGameLayer::getModifiedDelta(dt));
        if (auto* lel = LevelEditorLayer::get(); lel && lel->m_playbackActive) {
            return mod;
        }

        auto& d = Engine::get().ticks();
        if (d.refreshOnly) {
            return 0.0;
        }

        return d.physicsDt() * std::fmin(m_gameState.m_timeWarp, 1.0f);
    }

    void destroyObject(GameObject* obj) {
        if (this->m_isPracticeMode) {
            Engine::get().practice().noteBroken(obj);
        }
        GJBaseGameLayer::destroyObject(obj);
    }

    void recordCommand(PlayerButtonCommand cmd) {
        if (cmd.m_isPlayer2 && !this->m_levelSettings->m_twoPlayerMode) {
            cmd.m_isPlayer2 = false;
        }
        auto& eng = Engine::get();
        eng.macros().pushButton(
            eng.ticks().currentFrame(), static_cast<int>(cmd.m_button),
            cmd.m_isPush, eng.macros().mapPlayer(cmd.m_isPlayer2));
    }

    void dumpQueuedToMacro() {
        for (auto& cmd : m_queuedButtons) {
            recordCommand(cmd);
        }
    }

    // Apply one macro event into the game input queue / control flow.
    void applyMacroAction(MacroAction& action) {
        auto& eng = Engine::get();
        auto& d = eng.ticks();

        switch (action.kind) {
            case MacroAction::Kind::Death:
                d.expectsDeath = true;
                eng.macros().attemptSeed = eng.macros().gameRng();
                return;
            case MacroAction::Kind::RestartFull:
                d.expectsDeath = true;
                eng.macros().attemptSeed = eng.macros().gameRng();
                static_cast<PlayLayer*>(this)->fullReset();
                return;
            case MacroAction::Kind::Restart:
                d.expectsDeath = true;
                eng.macros().attemptSeed = eng.macros().gameRng();
                static_cast<PlayLayer*>(this)->resetLevel();
                return;
            case MacroAction::Kind::TpsChange:
                BotOptions::get().tps = action.tps;
                d.remainingSteps = 0;
                d.plannedSteps = 0;
                return;
            default:
                break;
        }

        int button = static_cast<int>(action.kind);
        if (button < 1 || button > 3) {
            return;
        }

        this->queueButton(button, action.holding,
                          eng.macros().mapPlayer(action.player2), 0);
    }

    // Ready input playback / record loop
    void processQueuedButtons(float dt, bool clearInputQueue) {
        auto& eng = Engine::get();
        if (!eng.active()) {
            return GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);
        }

        eng.practice().trackPlatformerInputs(m_queuedButtons);

        if (BotOptions::get().blockLiveInputsWhilePlaying && eng.playing()) {
            m_queuedButtons.clear();
        }

        if (eng.recording()) {
            dumpQueuedToMacro();
        } else {
            // Playback: drain every action scheduled for this physics frame
            uint32_t frame = eng.ticks().currentFrame();
            while (auto input = eng.macros().takeForFrame(frame)) {
                applyMacroAction(input.value());
            }
        }

        GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);
    }
};

// Shake RNG midhooks (deterministic practice)
static void shakeRng(SafetyHookContext& ctx) {
    uint64_t& state = Engine::get().macros().shakeState;
    state = static_cast<uint64_t>((214013 * state + 2531011) >> 16) & 0x7FFF;
    ctx.rax = static_cast<uintptr_t>(state);
}

$execute {
    util::installMid(geode::base::get() + 0x23E173, "shakeRng", shakeRng);
    util::installMid(geode::base::get() + 0x23E1A1, "shakeRng", shakeRng);
    util::installMid(geode::base::get() + 0x23E1CB, "shakeRng", shakeRng);
    util::installMid(geode::base::get() + 0x23E1E9, "shakeRng", shakeRng);

    // Reset overflow when lock-delta toggles
    // (no reactive handle — call from UI when changing the flag)
}
