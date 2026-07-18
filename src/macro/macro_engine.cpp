#include "macro/macro_engine.hpp"

#include <Geode/Geode.hpp>
#include <algorithm>
#include <fstream>

#include "config/options.hpp"
#include "core/engine.hpp"
#include "core/tick_driver.hpp"

using namespace geode::prelude;

void MacroEngine::onLevelReset(uint32_t newFrame) {
    if (Engine::get().recording()) {
        trimFrom(newFrame);
        cursor = actions.size();
    } else {
        if (actions.empty()) {
            cursor = 0;
            return;
        }
        cursor = static_cast<size_t>(std::distance(
            actions.begin(),
            std::find_if(actions.begin(), actions.end(),
                         [newFrame](const MacroAction& a) {
                             return a.frame >= newFrame;
                         })));
    }
}

void MacroEngine::onLevelExit() { cursor = 0; }

bool MacroEngine::controlsSwapped() const {
    return GameManager::get()->getGameVariable("0010");
}

bool MacroEngine::mapPlayer(bool player2) const {
    return player2 ^ controlsSwapped();
}

// Global RNG state used by GD (Win 2.2074)
uint64_t& MacroEngine::gameRng() {
    return *reinterpret_cast<uint64_t*>(geode::base::get() + 0x6c2e90);
}

std::optional<MacroAction> MacroEngine::peekCurrent() const {
    if (cursor >= actions.size()) {
        return std::nullopt;
    }
    return actions[cursor];
}

std::optional<MacroAction> MacroEngine::peekNext() const {
    if (cursor + 1 >= actions.size()) {
        return std::nullopt;
    }
    return actions[cursor + 1];
}

std::optional<MacroAction> MacroEngine::takeForFrame(uint32_t frame) {
    if (cursor >= actions.size()) {
        return std::nullopt;
    }
    if (actions[cursor].frame == frame) {
        return actions[cursor++];
    }
    return std::nullopt;
}

void MacroEngine::pushButton(uint32_t frame, int button, bool hold, bool p2) {
    if (!actions.empty() && actions.back().frame > frame) {
        return;
    }
    actions.push_back(MacroAction::button(frame, button, hold, p2));
}

void MacroEngine::pushSpecial(MacroAction action) {
    actions.push_back(std::move(action));
}

void MacroEngine::trimFrom(uint32_t frame) {
    auto it = std::remove_if(actions.begin(), actions.end(),
                             [frame](const MacroAction& a) {
                                 return a.frame >= frame;
                             });
    actions.erase(it, actions.end());
}

// Simple binary: magic "MB01", seed, tps, count, then packed actions
#pragma pack(push, 1)
struct ActionBlob {
    uint32_t frame;
    uint8_t kind;
    uint8_t flags; // bit0 hold, bit1 p2
    double tps;
    uint64_t seed;
};
#pragma pack(pop)

bool MacroEngine::save(std::filesystem::path path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        log::error("macro save failed: {}", path);
        return false;
    }

    char magic[4] = {'M', 'B', '0', '1'};
    out.write(magic, 4);
    out.write(reinterpret_cast<const char*>(&baseSeed), sizeof(baseSeed));
    double tps = BotOptions::get().tps;
    out.write(reinterpret_cast<const char*>(&tps), sizeof(tps));
    uint32_t count = static_cast<uint32_t>(actions.size());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (auto const& a : actions) {
        ActionBlob blob{};
        blob.frame = a.frame;
        blob.kind = static_cast<uint8_t>(a.kind);
        blob.flags = (a.holding ? 1 : 0) | (a.player2 ? 2 : 0);
        blob.tps = a.tps;
        blob.seed = a.seed;
        out.write(reinterpret_cast<const char*>(&blob), sizeof(blob));
    }

    log::info("saved {} actions → {}", count, path);
    return true;
}

bool MacroEngine::load(std::filesystem::path path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        log::error("macro load failed: {}", path);
        return false;
    }

    char magic[4]{};
    in.read(magic, 4);
    if (magic[0] != 'M' || magic[1] != 'B' || magic[2] != '0' || magic[3] != '1') {
        log::error("bad macro magic");
        return false;
    }

    in.read(reinterpret_cast<char*>(&baseSeed), sizeof(baseSeed));
    double tps = 240.0;
    in.read(reinterpret_cast<char*>(&tps), sizeof(tps));
    BotOptions::get().tps = tps;

    uint32_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    actions.clear();
    actions.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        ActionBlob blob{};
        in.read(reinterpret_cast<char*>(&blob), sizeof(blob));
        MacroAction a;
        a.frame = blob.frame;
        a.kind = static_cast<MacroAction::Kind>(blob.kind);
        a.holding = blob.flags & 1;
        a.player2 = blob.flags & 2;
        a.tps = blob.tps;
        a.seed = blob.seed;
        actions.push_back(a);
    }

    attemptSeed = baseSeed;
    cursor = 0;
    Engine::get().setMode(Engine::Mode::Playback);
    log::info("loaded {} actions ← {}", count, path);
    return true;
}

std::filesystem::path MacroEngine::defaultPath() const {
    auto n = name.empty() ? std::string("untitled") : name;
    return Mod::get()->getPersistentDir(true) / "macros" / (n + ".mb");
}
