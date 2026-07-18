#pragma once

#include <memory>

class TickDriver;
class TaskQueue;
class MacroEngine;
class PracticeFix;

// Central singleton. Subsystems live behind a pimpl so hooks stay thin.
class Engine {
public:
    enum class Mode {
        Record,
        Playback,
    };

    static Engine& get() {
        static Engine instance;
        return instance;
    }

    Engine(Engine const&) = delete;
    Engine& operator=(Engine const&) = delete;

    void boot();
    bool ready() const { return m_ready; }
    bool active() const;

    bool recording() const { return m_mode == Mode::Record; }
    bool playing() const { return m_mode == Mode::Playback; }
    void setMode(Mode mode) { m_mode = mode; }
    Mode mode() const { return m_mode; }

    TickDriver& ticks();
    TaskQueue& jobs();
    MacroEngine& macros();
    PracticeFix& practice();

    Mode m_mode = Mode::Record;

private:
    Engine();
    ~Engine();

    bool m_ready = false;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
