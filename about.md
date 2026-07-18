# BotBase Template

Minimal Geometry Dash bot **core** you can fork and extend.

## Included

- TPS / physics step bypass (midhooks)
- Multi-step lock-delta (fast + accurate paths)
- Macro record / playback with input queue inside `processQueuedButtons`
- Practice-mode checkpoint fix
- Death / reset / fullReset intentional-death handling
- Audio pitch speedhack
- ImGuiCocos control panel

## Not included

- Video recorder / DSP
- Trajectory prediction
- Custom ImGui / tabby UI
- Hitboxes, labels, autoclicker

## Build

```bat
geode build
```

Requires `GEODE_SDK` and dependency `hjfod.imgui-cocos`.
