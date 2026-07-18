#include "practice/practice_fix.hpp"

#include <Geode/Geode.hpp>
#include <algorithm>

#include "config/options.hpp"
#include "core/engine.hpp"
#include "core/tick_driver.hpp"
#include "macro/macro_engine.hpp"

using namespace geode::prelude;

PracticeCheckpoint PracticeFix::make(CheckpointObject* obj, uint64_t attemptStart) {
    PracticeCheckpoint cp{};
    auto* pl = PlayLayer::get();
    if (!pl) {
        return cp;
    }

    auto& eng = Engine::get();
    cp.p1 = PlayerSnapshot::capture(pl->m_player1);
    cp.p2 = PlayerSnapshot::capture(pl->m_player2);
    cp.object = obj;
    cp.platformerStack = platformerStack.size();
    cp.attemptStartFrame = attemptStart;
    cp.frame = eng.ticks().currentFrame();
    cp.seedState = eng.macros().gameRng();
    cp.shakeState = eng.macros().shakeState;
    cp.tps = BotOptions::get().tps;
    cp.warpedTime = eng.ticks().warpedTime;
    cp.persistentItems = pl->m_effectManager->m_persistentItemCountMap;
    cp.variance = pl->m_varianceValues;
    cp.calcNonEffectObjects = pl->m_calcNonEffectObjects;
    cp.calcNonEffectObjectsSize = pl->m_calcNonEffectObjectsSize;
    cp.brokenObjects = brokenObjects;
    return cp;
}

void PracticeFix::apply(PracticeCheckpoint& cp) {
    auto* pl = PlayLayer::get();
    if (!pl) {
        return;
    }

    cp.p1.restore(pl->m_player1);
    cp.p2.restore(pl->m_player2);

    pl->m_effectManager->m_persistentItemCountMap = cp.persistentItems;
    pl->m_varianceValues = cp.variance;
    pl->m_calcNonEffectObjects = cp.calcNonEffectObjects;
    pl->m_calcNonEffectObjectsSize = cp.calcNonEffectObjectsSize;

    while (cp.platformerStack < platformerStack.size()) {
        bool earlier = std::any_of(
            platformerStack.begin(), platformerStack.end() - 1,
            [this](const auto& pair) {
                return pair.second == platformerStack.back().second;
            });
        if (!earlier) {
            platformerStack.back().second->m_checkpointActivated = false;
            platformerStack.back().second->resetCheckpoint();
        }
        platformerStack.pop_back();
    }

    auto& eng = Engine::get();
    eng.ticks().frameAtAttemptStart = cp.attemptStartFrame;
    eng.ticks().warpedTime = cp.warpedTime;
    eng.ticks().setFrame(static_cast<uint32_t>(cp.frame - cp.attemptStartFrame));
    eng.macros().gameRng() = cp.seedState;
    eng.macros().shakeState = cp.shakeState;

    if (cp.tps != BotOptions::get().tps) {
        BotOptions::get().tps = cp.tps;
    }

    brokenObjects = cp.brokenObjects;
    for (auto* obj : brokenObjects) {
        if (!obj) {
            continue;
        }
        obj->m_isDisabled = true;
        obj->m_isDisabled2 = true;
        obj->setOpacity(0);
    }
}

void PracticeFix::rememberFrame(CheckpointObject* obj, uint64_t attemptStart) {
    auto cp = make(obj, attemptStart);
    auto maxFrames = BotOptions::get().practiceFrameBuffer;
    while (frameHistory.size() >= maxFrames) {
        if (frameHistory.front().object) {
            frameHistory.front().object->release();
        }
        frameHistory.pop_front();
    }
    frameHistory.push_back(cp);
}

void PracticeFix::wipeFrames() {
    for (auto& f : frameHistory) {
        if (f.object) {
            f.object->release();
        }
    }
    frameHistory.clear();
}

bool PracticeFix::canRewind() const {
    return PlayLayer::get() && frameHistory.size() >= 2;
}

void PracticeFix::rewind(std::function<void(CheckpointObject*)> loadFn) {
    if (frameHistory.size() < 2) {
        return;
    }
    if (!diedNormally) {
        dropNewestFrame();
    }
    auto& cp = frameHistory.back();
    loadFn(cp.object);
    apply(cp);
}

void PracticeFix::dropNewestFrame() {
    if (frameHistory.empty()) {
        return;
    }
    if (frameHistory.back().object) {
        frameHistory.back().object->release();
    }
    frameHistory.pop_back();
}

void PracticeFix::storeManual(CheckpointObject* obj, uint64_t attemptStart) {
    manualCheckpoints.push_back(make(obj, attemptStart));
}

void PracticeFix::applyLatestManual() {
    if (manualCheckpoints.empty()) {
        clearPlatformer(true);
        return;
    }
    apply(manualCheckpoints.back());
}

void PracticeFix::popManual() {
    if (!manualCheckpoints.empty()) {
        manualCheckpoints.pop_back();
    }
}

void PracticeFix::clearManual() {
    brokenObjects.clear();
    manualCheckpoints.clear();
}

void PracticeFix::clearPlatformer(bool deactivate) {
    while (!platformerStack.empty()) {
        if (deactivate) {
            platformerStack.back().second->m_checkpointActivated = false;
            platformerStack.back().second->resetCheckpoint();
        }
        platformerStack.pop_back();
    }
}

void PracticeFix::trackPlatformerInputs(std::vector<PlayerButtonCommand>& cmds) {
    for (auto const& cmd : cmds) {
        bool& left = cmd.m_isPlayer2 ? p2Left : p1Left;
        bool& right = cmd.m_isPlayer2 ? p2Right : p1Right;
        if (cmd.m_button == PlayerButton::Left) {
            left = cmd.m_isPush;
        } else if (cmd.m_button == PlayerButton::Right) {
            right = cmd.m_isPush;
        }
    }
}
