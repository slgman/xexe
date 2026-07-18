#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

struct TimedJob {
    double interval = 0.0;
    double elapsed = 0.0;
    bool repeat = false;
    bool finished = false;
    std::function<void()> callback;

    void tick(float dt);
};

// Lightweight repeating/one-shot job list (autosave timers, delayed work, etc.)
class TaskQueue {
public:
    using JobId = uint64_t;
    using Callback = std::function<void()>;

    JobId add(Callback cb, double intervalSeconds, bool repeat = false);
    void remove(JobId id);
    void setInterval(JobId id, double intervalSeconds);
    void update(float dt);

private:
    JobId m_nextId = 0;
    std::unordered_map<JobId, TimedJob> m_jobs;
};
