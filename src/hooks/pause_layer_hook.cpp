#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

#include "core/engine.hpp"
#include "core/tick_driver.hpp"
#include "macro/macro_engine.hpp"
#include "practice/practice_fix.hpp"
#include "util/midhook.hpp"

using namespace geode::prelude;

struct BBPauseLayer : Modify<BBPauseLayer, PauseLayer> {
    void goEdit() {
        auto& eng = Engine::get();
        eng.ticks().frameAdvance = false;
        PauseLayer::goEdit();
        eng.practice().clearManual();
        eng.macros().onLevelExit();
    }
};

// Keep buttons held while pausing/resuming during playback
static void skipReleaseOnPlayback(SafetyHookContext& ctx) {
    if (Engine::get().playing()) {
        ctx.rip += 5;
    }
}

$execute {
    util::installMid(geode::base::get() + 0x3BA239, "pauseKeepButtons",
                     skipReleaseOnPlayback);
    util::installMid(geode::base::get() + 0x3BA27C, "pauseKeepButtons",
                     skipReleaseOnPlayback);
    util::installMid(geode::base::get() + 0x3BA879, "resumeKeepButtons",
                     skipReleaseOnPlayback);
}
