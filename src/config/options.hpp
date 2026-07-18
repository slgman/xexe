#pragma once

// Runtime knobs for the bot core. Plain POD so you can wire save/load later.
struct BotOptions {
    bool enabled = true;

    double tps = 240.0;
    double speed = 1.0;
    double fps = 240.0;

    bool realTime = true;
    uint32_t maxStepsPerVisual = 10;
    bool visualStepScale = true;
    bool dynamicStepCap = false;
    double targetFps = 60.0;

    bool lockDelta = true;
    // 0 = Performance (fast multi-step), 1 = Accuracy (per-physics-step)
    int lockDeltaMode = 0;  

    bool speedhackAudio = true;
    bool blockLiveInputsWhilePlaying = true;
    bool intentionalDeath = false;

    uint32_t practiceFrameBuffer = 240;
    bool enableFrameHistory = true;
    bool backwardsStepping = true;

    // accurate lock-delta scroll-speed bug fix path
    bool ssbFix = false;

    // lightweight death helpers (no trajectory)
    bool preventDeath = false;
    bool noclip = false;
    int noclipTarget = 0; // 0 = both, 1 = p1 only, 2 = p2 only

    static BotOptions& get() {
        static BotOptions instance;
        return instance;
    }
};
