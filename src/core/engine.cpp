#include "core/engine.hpp"

#include <Geode/Geode.hpp>
#include <filesystem>

#include "config/options.hpp"
#include "core/task_queue.hpp"
#include "core/tick_driver.hpp"
#include "macro/macro_engine.hpp"
#include "practice/practice_fix.hpp"

using namespace geode::prelude;

struct Engine::Impl {
    TickDriver ticks;
    TaskQueue jobs;
    MacroEngine macros;
    PracticeFix practice;
};

Engine::Engine() : m_impl(std::make_unique<Impl>()) {}
Engine::~Engine() = default;

TickDriver& Engine::ticks() { return m_impl->ticks; }
TaskQueue& Engine::jobs() { return m_impl->jobs; }
MacroEngine& Engine::macros() { return m_impl->macros; }
PracticeFix& Engine::practice() { return m_impl->practice; }

void Engine::boot() {
    namespace fs = std::filesystem;
    auto dir = Mod::get()->getPersistentDir(true);
    fs::create_directories(dir / "macros");
    fs::create_directories(dir / "backups");

    // Soft-disable click-between-frames physics bypass if present
    if (auto* cbf = Loader::get()->getInstalledMod("syzzi.click_between_frames")) {
        cbf->setSettingValue("soft-toggle", true);
        cbf->setSettingValue("physics-bypass", false);
    }

    m_ready = true;
    log::info("BotBase engine ready");
}

bool Engine::active() const {
    return m_ready && BotOptions::get().enabled;
}
