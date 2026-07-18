#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include "../config/options.hpp"
#include "core/engine.hpp"
#include "practice/practice_fix.hpp"

using namespace geode::prelude;

struct BBPlayer : Modify<BBPlayer, PlayerObject> {
    void playSpawnEffect() {
        if (Engine::get().practice().loadStored) {
            return;
        }
        PlayerObject::playSpawnEffect();
    }

    void playDeathEffect() {
        if (BotOptions::get().preventDeath) {
            return;
        }
        PlayerObject::playDeathEffect();
    }

    void removePendingCheckpoint() {
        // bot owns checkpoint lifecycle via PracticeFix
        return;
    }

    void tryPlaceCheckpoint() {
        if (!GameManager::get()->getGameVariable("0027")) {
            return;
        }
        const double timeout = m_quickCheckpointMode ? 0.2 : 1.0;
        auto* layer = m_gameLayer;
        if (!layer) {
            return;
        }
        if ((layer->m_gameState.m_totalTime - m_lastCheckpointTime) > timeout) {
            layer->m_uiLayer->onCheck(nullptr);
            m_shouldTryPlacingCheckpoint = false;
            m_lastCheckpointTime = m_totalTime;
        }
    }

    void releaseAllButtons() {
        if (!Engine::get().active()) {
            return PlayerObject::releaseAllButtons();
        }
        auto* base = GJBaseGameLayer::get();
        if (!base) {
            return PlayerObject::releaseAllButtons();
        }
        // don't wipe p2 holds when not dual; keep holds during record unless intentional death
        if ((this == base->m_player2 && !base->m_gameState.m_isDualMode) ||
            BotOptions::get().intentionalDeath || Engine::get().playing()) {
            PlayerObject::releaseAllButtons();
        }
    }
};
