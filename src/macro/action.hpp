#pragma once

#include <cstdint>

// Single macro event. Frames are absolute within an attempt timeline.
struct MacroAction {
    enum class Kind : uint8_t {
        None = 0,
        Jump = 1,
        Left = 2,
        Right = 3,
        Death = 4,
        Restart = 5,
        RestartFull = 6,
        TpsChange = 7,
    };

    uint32_t frame = 0;
    Kind kind = Kind::None;
    bool holding = false;
    bool player2 = false;
    double tps = 0.0;
    uint64_t seed = 0;

    static MacroAction button(uint32_t frame, int button, bool hold, bool p2) {
        MacroAction a;
        a.frame = frame;
        a.kind = static_cast<Kind>(button);
        a.holding = hold;
        a.player2 = p2;
        return a;
    }

    static MacroAction death(uint32_t frame, uint64_t seed) {
        MacroAction a;
        a.frame = frame;
        a.kind = Kind::Death;
        a.seed = seed;
        return a;
    }

    static MacroAction restart(uint32_t frame, uint64_t seed, bool full) {
        MacroAction a;
        a.frame = frame;
        a.kind = full ? Kind::RestartFull : Kind::Restart;
        a.seed = seed;
        return a;
    }

    static MacroAction tps(uint32_t frame, double value) {
        MacroAction a;
        a.frame = frame;
        a.kind = Kind::TpsChange;
        a.tps = value;
        return a;
    }
};
