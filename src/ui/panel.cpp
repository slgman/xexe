#include <imgui-cocos.hpp>

#include <Geode/Geode.hpp>
#include <filesystem>
#include <string>

#include "config/options.hpp"
#include "core/engine.hpp"
#include "core/tick_driver.hpp"
#include "macro/macro_engine.hpp"

using namespace geode::prelude;

static void setupUi() {
    auto& io = ImGui::GetIO();
    auto fontPath = Mod::get()->getResourcesDir() / "Roboto-Regular.ttf";
    if (std::filesystem::exists(fontPath)) {
        io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 16.0f);
    }
}

static void drawUi() {
    auto& eng = Engine::get();
    auto& opt = BotOptions::get();
    auto& d = eng.ticks();
    auto& mac = eng.macros();

    ImGui::Begin("BotBase");

    ImGui::Checkbox("Enabled", &opt.enabled);
    ImGui::Separator();

    ImGui::Text("Mode");
    if (ImGui::RadioButton("Record", eng.mode() == Engine::Mode::Record)) {
        eng.setMode(Engine::Mode::Record);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Playback", eng.mode() == Engine::Mode::Playback)) {
        eng.setMode(Engine::Mode::Playback);
    }

    ImGui::Separator();
    ImGui::Text("Timing");
    ImGui::InputDouble("TPS", &opt.tps, 1.0, 10.0, "%.1f");
    if (opt.tps < 1.0) {
        opt.tps = 1.0;
    }
    ImGui::InputDouble("Speed", &opt.speed, 0.05, 0.25, "%.2f");
    if (opt.speed <= 0.0) {
        opt.speed = 0.01;
    }
    ImGui::Checkbox("Speedhack audio", &opt.speedhackAudio);
    ImGui::Checkbox("Lock delta", &opt.lockDelta);
    ImGui::Combo("Lock mode", &opt.lockDeltaMode, "Fast\0Accurate\0");
    ImGui::Checkbox("Real-time steps", &opt.realTime);
    if (!opt.realTime) {
        int maxUpr = static_cast<int>(opt.maxStepsPerVisual);
        if (ImGui::InputInt("Max steps / visual", &maxUpr)) {
            opt.maxStepsPerVisual = static_cast<uint32_t>(std::max(1, maxUpr));
        }
    }

    ImGui::Separator();
    ImGui::Text("Macro");
    ImGui::Checkbox("Block live inputs (play)", &opt.blockLiveInputsWhilePlaying);
    ImGui::Checkbox("Intentional death", &opt.intentionalDeath);

    static char nameBuf[128] = "untitled";
    ImGui::InputText("Name", nameBuf, sizeof(nameBuf));
    mac.name = nameBuf;

    ImGui::Text("Actions: %zu  cursor: %zu  frame: %u", mac.actions.size(),
                mac.cursor, d.currentFrame());

    if (ImGui::Button("Save macro")) {
        mac.save(mac.defaultPath());
    }
    ImGui::SameLine();
    if (ImGui::Button("Load macro")) {
        mac.load(mac.defaultPath());
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        mac.actions.clear();
        mac.cursor = 0;
    }

    ImGui::Separator();
    ImGui::Text("Frame advance");
    if (ImGui::Checkbox("Paused (frame advance)", &d.frameAdvance)) {
        // noop — flag is read by TickDriver
    }
    if (ImGui::Button("Step +1")) {
        d.frameAdvance = true;
        d.stepRequest = true;
    }

    ImGui::Separator();
    if (ImGui::Button("Close panel")) {
        ImGuiCocos::get().toggle();
    }

    ImGui::End();
}

$on_mod(Loaded) {
    Engine::get().boot();

    ImGuiCocos::get()
        .setup(setupUi)
        .draw(drawUi)
        .setVisible(true);
}
