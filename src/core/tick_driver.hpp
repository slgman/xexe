#pragma once

#include <cstdint>
#include <forward_list>
#include <functional>

#include "config/options.hpp"

// Owns physics timing: TPS, step overflow, lock-delta multi-step, frame index.
class TickDriver {
public:
    enum class LockMode {
        Fast = 0,      // batch safe steps between inputs
        Accurate = 1,  // one physics dt per inner update
    };

    int respawnCooldown = 0;
    bool refreshOnly = false;
    bool allowActionProcess = true;
    bool shouldDraw = true;

    double stepOverflow = 0.0;
    int plannedSteps = 0;
    int remainingSteps = 1;
    uint32_t stepCap = 1;

    // Intentional death / macro control flags
    bool expectsDeath = false;
    bool deathIsNatural = false;
    bool deathIsFullReset = false;
    uint64_t frameAtAttemptStart = 0;
    double warpedTime = 0.0;
    float lastTimeForPos = 0.0f;

    bool frameAdvance = false;
    bool stepRequest = false;

    bool backstepPending = false;
    int backstepCount = 1;

    void* actionManager = nullptr;

    double getTimeWarp() const;
    double physicsDt() const { return 1.0 / BotOptions::get().tps; }
    double visualDt() const;

    void computeSteps(float dt, float targetDt);
    void abortStepLoop();
    void runFrameUpdates(std::function<void(float)> update, float realDt, bool frozen);
    void runDeferredJobs();
    void defer(std::function<void(float)> fn);

    void requestBackstep(int frames = 1);
    bool consumeBackstepRequest(); // used by deferred path

    void applyFps(double fps);
    void syncAudioPitch();

    uint32_t currentFrame() const;
    void bumpFrame() { ++m_frame; }
    void clearFrame() {
        m_frame = 0;
        warpedTime = 0.0;
    }
    void setFrame(uint32_t f) { m_frame = f; }

    bool isFrameAdvance() const { return frameAdvance; }
    bool takeStepRequest() {
        bool v = stepRequest;
        stepRequest = false;
        return v;
    }

    bool lockDeltaActive() const { return BotOptions::get().lockDelta; }
    bool preferFastLockDelta() const;

private:
    uint32_t m_frame = 0;
    double m_fps = 240.0;
    std::forward_list<std::function<void(float)>> m_deferred;
};
