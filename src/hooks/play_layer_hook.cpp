#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include "config/options.hpp"
#include "core/engine.hpp"
#include "core/tick_driver.hpp"
#include "macro/action.hpp"
#include "macro/macro_engine.hpp"
#include "practice/practice_fix.hpp"
#include "util/midhook.hpp"

using namespace geode::prelude;

struct BBPlayLayer : Modify<BBPlayLayer, PlayLayer> {
    CheckpointObject* markCheckpoint() {
        if (!Engine::get().active()) {
            return PlayLayer::markCheckpoint();
        }
        auto* cp = createCheckpoint();
        storeCheckpoint(cp);
        if (cp && cp->m_physicalCheckpointObject) {
            cp->m_physicalCheckpointObject->activateObject();
        }
        return cp;
    }

    void storeCheckpoint(CheckpointObject* obj) {
        if (!Engine::get().active()) {
            return PlayLayer::storeCheckpoint(obj);
        }

        auto& pf = Engine::get().practice();
        if (this->m_activatedCheckpoint) {
            obj->retain();
            addToSection(obj->m_physicalCheckpointObject);
            pf.platformerStack.push_back({obj, this->m_activatedCheckpoint});
            return;
        }

        obj->retain();
        addToSection(obj->m_physicalCheckpointObject);
        pf.storeManual(obj, Engine::get().ticks().frameAtAttemptStart);
    }

    void loadFromCheckpoint(CheckpointObject* obj) {
        if (!Engine::get().active()) {
            return PlayLayer::loadFromCheckpoint(obj);
        }

        auto& pf = Engine::get().practice();
        if (pf.forced) {
            PlayLayer::loadFromCheckpoint(pf.forced->object);
            pf.apply(*pf.forced);
            return;
        }
        if (pf.loadStored) {
            pf.rewind([this](auto* cp) { this->PlayLayer::loadFromCheckpoint(cp); });
            pf.diedNormally = false;
            return;
        }
        if (pf.manualCheckpoints.empty()) {
            PlayLayer::loadFromCheckpoint(obj);
            return;
        }
        if (pf.shouldLoadPlatformer) {
            PlayLayer::loadFromCheckpoint(pf.platformerStack.back().first);
            return;
        }

        auto last = pf.manualCheckpoints.back();
        pf.wipeFrames();
        PlayLayer::loadFromCheckpoint(last.object);
        pf.applyLatestManual();
    }

    void delayedResetLevel() {
        // Only real deaths go through delayedReset — used to tag Death vs Restart
        if (BotOptions::get().intentionalDeath) {
            Engine::get().ticks().deathIsNatural = true;
        }
        PlayLayer::delayedResetLevel();
    }

    void softFullReset() {
        auto win = CCDirector::sharedDirector()->getWinSize();
        m_gameState.m_totalTime = 0.0;
        m_gameState.m_unkDouble3 = 0.0;
        m_gameState.m_levelTime = 0.0;
        m_player1->m_totalTime = 0.0;
        m_player2->m_totalTime = 0.0;
        m_attempts = 0;
        m_jumps = 0;
        m_objectsDeactivated = true;
        m_freezeStartCamera = true;
        Engine::get().practice().clearPlatformer(true);
        this->resetLevel();
        if (m_attemptLabel) {
            m_attemptLabel->setPosition(CCPoint{
                win.width,
                win.width * 0.5f + static_cast<float>(m_gameState.m_cameraPosition.x)});
        }
    }

    void fullReset() {
        auto& eng = Engine::get();
        if (!eng.active()) {
            return PlayLayer::fullReset();
        }

        auto& d = eng.ticks();
        if (BotOptions::get().intentionalDeath) {
            d.deathIsFullReset = true;
            this->softFullReset();
        } else if (d.expectsDeath) {
            if (auto* ell = this->getChildByID("EndLevelLayer")) {
                ell->removeFromParent();
            }
            this->softFullReset();
        } else {
            PlayLayer::fullReset();
            eng.practice().clearPlatformer(true);
            d.frameAtAttemptStart = 0;
        }
    }

    void consumeExpectedDeaths(uint64_t deathFrame) {
        auto& eng = Engine::get();
        while (auto input = eng.macros().takeForFrame(static_cast<uint32_t>(deathFrame))) {
            if (input->kind == MacroAction::Kind::Death) {
                eng.ticks().expectsDeath = true;
            } else {
                // put it back by rewinding cursor — we already advanced
                if (eng.macros().cursor > 0) {
                    --eng.macros().cursor;
                }
                break;
            }
        }
    }

    bool intentional() const {
        return BotOptions::get().intentionalDeath ||
               Engine::get().ticks().expectsDeath;
    }

    bool injectCheckpointForReset(uint64_t deathFrame) {
        auto& eng = Engine::get();
        auto& pf = eng.practice();
        auto& d = eng.ticks();

        if (this->intentional()) {
            d.frameAtAttemptStart = deathFrame + 1;
            if (!pf.platformerStack.empty()) {
                this->m_checkpointArray->addObject(pf.platformerStack.back().first);
                pf.shouldLoadPlatformer = true;
                return true;
            }
            return false;
        }
        if (!pf.manualCheckpoints.empty()) {
            this->m_checkpointArray->addObject(pf.manualCheckpoints.back().object);
            return true;
        }
        if (pf.forced) {
            this->m_checkpointArray->addObject(pf.forced->object);
            return true;
        }
        if (pf.loadStored && !pf.frameHistory.empty()) {
            this->m_checkpointArray->addObject(pf.frameHistory.back().object);
            return true;
        }

        d.frameAtAttemptStart = 0;
        pf.clearPlatformer(true);
        return false;
    }

    void reseedOnReset() {
        auto& eng = Engine::get();
        auto& mac = eng.macros();
        uint64_t& state = mac.gameRng();

        if (!eng.ticks().expectsDeath) {
            mac.attemptSeed = mac.baseSeed;
        }

        if (eng.recording()) {
            if (mac.overrideSeed) {
                state = mac.forcedSeed;
            }
            mac.attemptSeed = state;
            if (eng.practice().manualCheckpoints.empty() &&
                !eng.practice().loadStored &&
                !BotOptions::get().intentionalDeath) {
                state = 214013 * state + 2531011;
                mac.baseSeed = state;
                mac.attemptSeed = state;
                uint64_t v = state;
                for (auto& x : m_varianceValues) {
                    x = static_cast<float>(v & 0xFFFF) / 32768.0f - 1.0f;
                    v = 214013 * v + 2531011;
                }
            }
        } else {
            state = mac.attemptSeed;
            uint64_t v = state;
            for (auto& x : m_varianceValues) {
                x = static_cast<float>(v & 0xFFFF) / 32768.0f - 1.0f;
                v = 214013 * v + 2531011;
            }
        }
    }

    void restoreHolds(uint64_t deathFrame) {
        auto& eng = Engine::get();
        eng.practice().trackPlatformerInputs(m_queuedButtons);
        m_queuedButtons.clear();

        if (eng.playing()) {
            if (eng.practice().manualCheckpoints.empty()) {
                m_player1->releaseAllButtons();
                m_player2->releaseAllButtons();
            }
            if (eng.ticks().expectsDeath) {
                while (auto input = eng.macros().peekCurrent()) {
                    if (input->frame == deathFrame) {
                        eng.macros().advance();
                    } else {
                        break;
                    }
                }
            }
            BotOptions::get().intentionalDeath = false;
            eng.ticks().deathIsNatural = false;
            eng.ticks().expectsDeath = false;
            this->processQueuedButtons(0.0f, true);
            eng.ticks().stepOverflow = 0.0;
            return;
        }

        if (eng.recording() && BotOptions::get().intentionalDeath) {
            m_player1->releaseAllButtons();
            m_player2->releaseAllButtons();
            return;
        }

        if (eng.recording()) {
            auto& mac = eng.macros();
            bool p1 = m_uiLayer->m_p1Jumping || m_uiLayer->m_p1TouchId != -1;
            bool p2 = m_uiLayer->m_p2Jumping || m_uiLayer->m_p2TouchId != -1;
            auto& pf = eng.practice();
            bool p1L = pf.p1Left, p2L = pf.p2Left;
            bool p1R = pf.p1Right, p2R = pf.p2Right;
            if (mac.controlsSwapped()) {
                std::swap(p1, p2);
            }
            p1 |= p2 && !m_levelSettings->m_twoPlayerMode;

            auto requeue = [&](bool held, int btn, bool isP2, PlayerObject* player) {
                if (held != player->m_holdingButtons[btn]) {
                    this->queueButton(btn, held, mac.mapPlayer(isP2), 0);
                }
            };
            requeue(p1, 1, false, m_player1);
            if (m_isPlatformer) {
                requeue(p1L, 2, false, m_player1);
                requeue(p1R, 3, false, m_player1);
            }
            if (m_levelSettings->m_twoPlayerMode) {
                requeue(p2, 1, true, m_player2);
                if (m_isPlatformer) {
                    requeue(p2L, 2, true, m_player2);
                    requeue(p2R, 3, true, m_player2);
                }
            }
            mac.lastButtons.clear();
        }
    }

    void writeDeathAction(uint64_t deathFrame) {
        auto& eng = Engine::get();
        if (!BotOptions::get().intentionalDeath) {
            return;
        }
        auto& d = eng.ticks();
        auto seed = eng.macros().attemptSeed;
        if (d.deathIsNatural) {
            eng.macros().pushSpecial(
                MacroAction::death(static_cast<uint32_t>(deathFrame), seed));
        } else if (d.deathIsFullReset) {
            eng.macros().pushSpecial(
                MacroAction::restart(static_cast<uint32_t>(deathFrame), seed, true));
        } else {
            eng.macros().pushSpecial(
                MacroAction::restart(static_cast<uint32_t>(deathFrame), seed, false));
        }
    }

    void finishIntentional() {
        auto& d = Engine::get().ticks();
        BotOptions::get().intentionalDeath = false;
        d.deathIsNatural = false;
        d.expectsDeath = false;
        d.deathIsFullReset = false;
        d.stepOverflow = 0.0;
    }

    void resetLevel() {
        auto& eng = Engine::get();
        if (!eng.active()) {
            m_player1->releaseAllButtons();
            m_player2->releaseAllButtons();
            return PlayLayer::resetLevel();
        }

        m_practiceMusicSync = true;
        auto& d = eng.ticks();
        d.stepOverflow = 0.0;
        d.respawnCooldown = 2;

        // Distinct death frame from the last live frame
        d.bumpFrame();
        uint64_t deathFrame = d.currentFrame();

        this->consumeExpectedDeaths(deathFrame);
        bool injected = this->injectCheckpointForReset(deathFrame);
        d.clearFrame();

        this->reseedOnReset();
        PlayLayer::resetLevel();

        if (injected) {
            eng.practice().shouldLoadPlatformer = false;
            this->m_checkpointArray->removeLastObject();
            d.refreshOnly = true;
            this->m_extraDelta = 0.0f;
            CCScheduler::get()->update(0.0f);
            d.refreshOnly = false;
        }

        eng.macros().onLevelReset(d.currentFrame());
        this->restoreHolds(deathFrame);
        this->writeDeathAction(deathFrame);

        if (!BotOptions::get().intentionalDeath) {
            eng.macros().flipControlsOnProcess = true;
            this->processQueuedButtons(0.0f, true);
            eng.macros().flipControlsOnProcess = false;
        }

        this->finishIntentional();
        eng.practice().diedNormally = false;
        eng.practice().backstepping = false;
        d.abortStepLoop();
    }

    void updateAttempts() {
        if (Engine::get().practice().backstepping) {
            return;
        }
        PlayLayer::updateAttempts();
    }

    void onQuit() {
        auto& eng = Engine::get();
        PlayLayer::onQuit();
        eng.practice().wipeFrames();
        eng.practice().clearManual();
        eng.macros().onLevelExit();
    }

    void removeCheckpoint(bool p0) {
        if (!Engine::get().active()) {
            return PlayLayer::removeCheckpoint(p0);
        }
        auto& pf = Engine::get().practice();
        if (pf.manualCheckpoints.empty()) {
            return;
        }
        auto& last = pf.manualCheckpoints.back();
        auto* obj = last.object->m_physicalCheckpointObject;
        this->removeObjectFromSection(obj);
        if (auto* glow = obj->m_glowSprite) {
            glow->release();
            glow->removeMeAndCleanup();
        }
        obj->removeMeAndCleanup();
        last.object->release();
        pf.popManual();
    }

    void removeAllCheckpoints() {
        if (!Engine::get().active()) {
            return PlayLayer::removeAllCheckpoints();
        }
        if (Engine::get().ticks().deathIsFullReset) {
            return;
        }
        while (!Engine::get().practice().manualCheckpoints.empty()) {
            removeCheckpoint(false);
        }
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!Engine::get().active()) {
            return PlayLayer::init(level, useReplay, dontCreateObjects);
        }

        auto& eng = Engine::get();
        eng.practice().wipeFrames();
        eng.practice().clearPlatformer(false);

        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        m_clickOnSteps = false;
        m_clickBetweenSteps = false;
        eng.ticks().frameAtAttemptStart = 0;
        eng.ticks().lastTimeForPos = 0.0f;
        return true;
    }

    void destroyPlayer(PlayerObject* player, GameObject* gameObject) {
        if (gameObject == m_anticheatSpike) {
            return PlayLayer::destroyPlayer(player, gameObject);
        }

        auto& eng = Engine::get();
        auto& opt = BotOptions::get();

        if (opt.noclip) {
            const bool isP1 = player == m_player1;
            const bool block =
                (opt.noclipTarget == 0) ||
                (opt.noclipTarget == 1 && isP1) ||
                (opt.noclipTarget == 2 && !isP1);
            if (block) {
                return;
            }
        }

        PlayLayer::destroyPlayer(player, gameObject);

        if (opt.preventDeath && gameObject != m_anticheatSpike) {
            eng.ticks().requestBackstep(1);
            this->processQueuedButtons(0.0f, true);
        }

        if (!eng.practice().loadStored) {
            eng.practice().diedNormally = true;
        }
    }

    void pauseGame(bool p0) {
        m_gameState.m_pauseCounter = 0;
        PlayLayer::pauseGame(p0);
    }
};

// Instant checkpoint placement (skip 1-frame delay)
constexpr int kQueueCheckpoint = 0x4ce060;

static void queueCheckpointHook(void* a, void* b) {
    if (!Engine::get().active()) {
        return reinterpret_cast<void (*)(void*, void*)>(
            geode::base::get() + kQueueCheckpoint)(a, b);
    }
    auto* pl = PlayLayer::get();
    if (!pl || pl->m_player1->m_isDead || pl->m_player2->m_isDead) {
        return;
    }
    Engine::get().ticks().defer([pl](float) { pl->markCheckpoint(); });
}

static void reseedOnResetMid(SafetyHookContext&) {
    if (!Engine::get().active()) {
        return;
    }
    auto& mac = Engine::get().macros();
    mac.gameRng() = mac.attemptSeed;
    mac.shakeState = mac.attemptSeed & 0x7FFF;
}

$execute {
    (void)Mod::get()->hook(
        reinterpret_cast<void*>(geode::base::get() + kQueueCheckpoint),
        &queueCheckpointHook, "PlayLayer::queueCheckpoint",
        tulip::hook::TulipConvention::Default);

    util::installMid(geode::base::get() + 0x3b90fb, "resetSeed", reseedOnResetMid);
}
