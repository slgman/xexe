# BotBase Template

Clean bot foundation extracted from common GD bot patterns (TPS fix, lock-delta, macro loop, practice fix). Code is restructured and renamed so it is not a line-for-line clone of any single project.

## Layout

```text
src/
  core/       Engine singleton, tick driver (TPS/lock-delta), task queue
  macro/      Action list + record/playback
  practice/   Practice fix + player snapshot
  hooks/      Geode Modify + midhooks
  util/       Midhook helper (safetyhook)
  ui/         ImGuiCocos panel
  config/     Runtime options
```

## Features

| Feature | Where |
|--------|--------|
| Step calculation / overflow | `core/tick_driver.*` |
| Fast + accurate lock-delta | `TickDriver::runFrameUpdates` |
| TPS midhook / step count / frame tick | `tick_driver.cpp` `$execute` midhooks |
| Input playback loop | `hooks/base_layer_hook.cpp` → `processQueuedButtons` |
| Death / reset / fullReset | `hooks/play_layer_hook.cpp` |
| Practice fix | `practice/*` |
| Audio speedhack | `TickDriver::syncAudioPitch` + director hook |
| ImGui panel | `ui/panel.cpp` |

## Offsets

Midhook addresses target **Windows 2.2081** (`mod.json`, same offsets as the silicate reference). Re-verify offsets when updating GD.

## Getting started

1. Put a `Roboto-Regular.ttf` under `resources/` (or change the font path in `ui/panel.cpp`).
2. `geode build`
3. Open the panel in-game and toggle Record / Play, TPS, lock-delta.
