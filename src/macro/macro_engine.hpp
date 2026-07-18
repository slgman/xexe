#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "macro/action.hpp"

// Owns the action list, cursor, RNG seed bookkeeping, record/playback helpers.
class MacroEngine {
public:
    std::vector<MacroAction> actions;
    size_t cursor = 0;

    uint64_t baseSeed = 0;
    uint64_t attemptSeed = 0;
    uint64_t shakeState = 0;
    bool overrideSeed = false;
    uint64_t forcedSeed = 2137;

    bool mirror = false;
    bool mirrorInvert = false;
    bool forceNext = false;
    bool flipControlsOnProcess = false;

    std::string name;

    std::unordered_map<int, MacroAction> lastButtons;

    void onLevelReset(uint32_t newFrame);
    void onLevelExit();
    void advance() { ++cursor; }

    bool controlsSwapped() const;
    bool mapPlayer(bool player2) const;

    uint64_t& gameRng();

    [[nodiscard]] std::optional<MacroAction> peekCurrent() const;
    [[nodiscard]] std::optional<MacroAction> peekNext() const;
    [[nodiscard]] std::optional<MacroAction> takeForFrame(uint32_t frame);

    void pushButton(uint32_t frame, int button, bool hold, bool p2);
    void pushSpecial(MacroAction action);
    void trimFrom(uint32_t frame);

    bool load(std::filesystem::path path);
    bool save(std::filesystem::path path) const;
    std::filesystem::path defaultPath() const;
};
