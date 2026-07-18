#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include "core/engine.hpp"
#include "practice/practice_fix.hpp"

using namespace geode::prelude;

struct BBPlayer : Modify<BBPlayer, PlayerObject> {
    void playSpawnEffect() {
        // Skip flash when restoring a stored practice frame
        if (Engine::get().practice().loadStored) {
            return;
        }
        PlayerObject::playSpawnEffect();
    }
};
