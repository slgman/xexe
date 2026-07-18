#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include "core/engine.hpp"
#include "core/tick_driver.hpp"

using namespace geode::prelude;

// Routes every scheduler tick through lock-delta / step logic.
struct BBScheduler : Modify<BBScheduler, CCScheduler> {
    void update(float dt) override {
        auto& eng = Engine::get();
        if (!eng.active() || eng.ticks().refreshOnly) {
            CCScheduler::update(dt);
            return;
        }

        eng.ticks().runFrameUpdates(
            [this](float stepDt) { this->CCScheduler::update(stepDt); }, dt,
            false);
    }
};
