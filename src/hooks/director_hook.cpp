#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>

#include "core/engine.hpp"
#include "core/task_queue.hpp"
#include "core/tick_driver.hpp"

using namespace geode::prelude;

struct BBDirector : Modify<BBDirector, CCDirector> {
    void drawScene() {
        if (!Engine::get().active()) {
            return CCDirector::drawScene();
        }

        auto& eng = Engine::get();
        eng.jobs().update(this->getDeltaTime());
        eng.ticks().syncAudioPitch();

        CCDirector::drawScene();
        eng.ticks().runDeferredJobs();
    }
};
