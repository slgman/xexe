#include "core/task_queue.hpp"

void TimedJob::tick(const float dt) {
    elapsed += dt;
    if (elapsed < interval) {
        return;
    }
    elapsed = 0.0;
    callback();
    finished = !repeat;
}

TaskQueue::JobId TaskQueue::add(Callback cb, double intervalSeconds, bool repeat) {
    ++m_nextId;
    m_jobs.emplace(m_nextId, TimedJob{
        .interval = intervalSeconds,
        .repeat = repeat,
        .callback = std::move(cb),
    });
    return m_nextId;
}

void TaskQueue::remove(JobId id) {
    m_jobs.erase(id);
}

void TaskQueue::setInterval(JobId id, double intervalSeconds) {
    auto it = m_jobs.find(id);
    if (it != m_jobs.end()) {
        it->second.interval = intervalSeconds;
    }
}

void TaskQueue::update(float dt) {
    for (auto it = m_jobs.begin(); it != m_jobs.end();) {
        it->second.tick(dt);
        if (it->second.finished) {
            it = m_jobs.erase(it);
        } else {
            ++it;
        }
    }
}
