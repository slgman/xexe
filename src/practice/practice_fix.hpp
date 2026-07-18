#pragma once

#include <deque>
#include <functional>
#include <utility>
#include <vector>

#include "practice/player_state.hpp"

class PracticeFix {
public:
    PracticeCheckpoint* forced = nullptr;
    bool loadStored = false;
    bool diedNormally = false;
    bool backstepping = false;
    bool shouldLoadPlatformer = false;

    std::deque<PracticeCheckpoint> frameHistory;
    std::vector<PracticeCheckpoint> manualCheckpoints;
    std::deque<std::pair<CheckpointObject*, CheckpointGameObject*>> platformerStack;
    std::vector<GameObject*> brokenObjects;

    bool p1Left = false, p1Right = false;
    bool p2Left = false, p2Right = false;

    PracticeCheckpoint make(CheckpointObject* obj, uint64_t attemptStart);
    void apply(PracticeCheckpoint& cp);

    void rememberFrame(CheckpointObject* obj, uint64_t attemptStart);
    void wipeFrames();
    bool canRewind() const;
    void rewind(std::function<void(CheckpointObject*)> loadFn);
    void dropNewestFrame();

    void storeManual(CheckpointObject* obj, uint64_t attemptStart);
    void applyLatestManual();
    void popManual();
    void clearManual();
    void clearPlatformer(bool deactivate);

    void trackPlatformerInputs(std::vector<PlayerButtonCommand>& cmds);
    void noteBroken(GameObject* obj) { brokenObjects.push_back(obj); }
};
