#include <Geode/Geode.hpp>

#include "core/engine.hpp"
#include "core/tick_driver.hpp"

using namespace geode::prelude;

// Skip action updates while running a frozen (dt=0 refresh) pass.
constexpr int kActionMgrUpdate = 0x38B90;
static void* s_actionMgrOrig = nullptr;

static void actionMgrUpdate(cocos2d::CCActionManager* mgr, float dt) {
    Engine::get().ticks().actionManager = mgr;
    if (!Engine::get().active()) {
        reinterpret_cast<void (*)(cocos2d::CCActionManager*, float)>(
            s_actionMgrOrig)(mgr, dt);
        return;
    }
    if (Engine::get().ticks().refreshOnly) {
        return;
    }
    reinterpret_cast<void (*)(cocos2d::CCActionManager*, float)>(s_actionMgrOrig)(
        mgr, dt);
}

$execute {
    s_actionMgrOrig =
        reinterpret_cast<void*>(geode::base::getCocos() + kActionMgrUpdate);
    (void)Mod::get()->hook(s_actionMgrOrig, &actionMgrUpdate,
                           "CCActionManager::update",
                           tulip::hook::TulipConvention::Fastcall);
}
