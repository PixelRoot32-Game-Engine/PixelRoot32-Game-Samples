## GameSample – PixelRoot32 Getting Started Project

This repository is a small **getting‑started project** that shows how to use the
[PixelRoot32 Game Engine](lib/PixelRoot32-Game-Engine/README.md) inside a
PlatformIO project.

It wires the engine to:

- an **ESP32 + TFT_eSPI display + buttons + speaker**, and
- a **native desktop build (SDL2)** for fast iteration without hardware.

From a simple menu you can launch four example games shipped with the engine:
**Pong**, **Brick Breaker**, **Tic‑Tac‑Toe**, and **Geometry Jump**.

---

## Prerequisites

- **VS Code + PlatformIO extension**
- **C++17 toolchain**
- **PixelRoot32 engine** (already included here under `lib/PixelRoot32-Game-Engine`)

For the ESP32 build:

- ESP32 DevKit‑style board
- 240x240 ST7789 (or similar) SPI TFT wired for **TFT_eSPI**
- 5 digital buttons (Up, Down, Left, Right, Action)
- Small speaker + amplifier (internal DAC or external I2S DAC)

For the native desktop build:

- SDL2 2.x development libraries installed on your system
- `platformio.ini` updated so the `include` and `lib` paths for SDL2 match
  your local installation (see `[env:native]` section).

---

## How to Build and Run

### 1. Open the project

- Clone this repository (or copy it into your own workspace).
- Open the folder in VS Code.
- When prompted, let PlatformIO initialize the project.

### 2. Run on ESP32 (hardware)

- In the **PlatformIO** sidebar, select the environment: `env:esp32dev`.
- Connect your ESP32 board via USB.
- Click **Upload** to build and flash.
- On reset you should see the **GAME SELECT** menu on the TFT display.

If your wiring or screen rotation is different, adjust:

- pin and rotation constants in [`include/Config.h`](include/Config.h)
- display and button defines in `platformio.ini` under `[env:esp32dev]`.

### 3. Run natively on PC (SDL2)

- Make sure SDL2 is installed and the paths in `[env:native]` in
  [`platformio.ini`](platformio.ini) are valid for your system.
- Select the `env:native` environment.
- Build and run from PlatformIO (or using the **Run**/**Debug** targets).

The native build uses `src/main_native.cpp` and opens a window that behaves
like the ESP32 screen, with keyboard controls mapped to the virtual buttons.

---

## What This Sample Demonstrates

- Wiring **PixelRoot32** to real hardware and a native SDL2 backend.
- Using the engine’s **scene system** with a simple menu (`MenuScene`).
- Hooking up **UI elements** (`UILabel`, `UIButton`) to navigate between games.
- Driving the **audio engine** with NES‑style sound events.
- Handling input via `InputConfig` (buttons on ESP32, scancodes on desktop).

Internally, the menu scene instantiates the example scenes from `src/examples`
and switches `engine.setScene(...)` when you select a game.

---

## 📁 Project Structure

```txt
GameSample/
├── include/                # Project header files
├── lib/
│   └── PixelRoot32-Game-Engine/ # The Game Engine Library
├── src/                    # Source code
│   ├── examples/           # Example games (Pong, BrickBreaker, etc.)
│   ├── drivers/            # Hardware-specific drivers (TFT_eSPI, SDL2)
│   ├── Menu/               # Main Menu Scene
│   ├── main.cpp            # Entry point for ESP32
│   └── main_native.cpp     # Entry point for Native (PC)
├── platformio.ini          # Build configuration
└── README.md
```

## Code Overview

Key files in this project:

- [`src/main.cpp`](src/main.cpp) – entry point for **ESP32** (Arduino framework).
- [`src/main_native.cpp`](src/main_native.cpp) – entry point for **native** builds.
- [`src/Menu/MenuScene.cpp`](src/Menu/MenuScene.cpp) – main menu and scene switching.
- [`include/Config.h`](include/Config.h) – display resolution and shared config.
- [`lib/PixelRoot32-Game-Engine`](lib/PixelRoot32-Game-Engine) – engine core library.

The engine is configured in `main.cpp` by creating the display, input, and
audio backends and passing them to `pixelroot32::core::Engine`:

```cpp
#include <drivers/esp32/TFT_eSPI_Drawer.h>
#include <drivers/esp32/ESP32_DAC_AudioBackend.h>
#include <core/Engine.h>
#include "Config.h"
#include "Menu/MenuScene.h"

namespace pr32 = pixelroot32;

pr32::drivers::esp32::TFT_eSPI_Drawer drawer;
pr32::drivers::esp32::ESP32_DAC_AudioBackend audioBackend(25, 11025);

pr32::graphics::DisplayConfig displayConfig(
    &drawer,
    DISPLAY_ROTATION,
    DISPLAY_HEIGHT,
    DISPLAY_WIDTH
);

pr32::input::InputConfig inputConfig(5, 13, 12, 14, 32, 33);
pr32::audio::AudioConfig audioConfig(&audioBackend, audioBackend.getSampleRate());

pr32::core::Engine engine(displayConfig, inputConfig, audioConfig);
MenuScene menuScene;

void setup() {
    engine.init();
    menuScene.init();
    engine.setScene(&menuScene);
}

void loop() {
    engine.run();
}
```

The native entry point in [`src/main_native.cpp`](src/main_native.cpp) is
almost identical but replaces the drawer and audio backend with SDL2‑based
implementations and uses `int main(...)` instead of Arduino’s `setup/loop`.

---

## Using PixelRoot32 in Your Own Projects

This sample is meant as a **starting point**:

- Copy `lib/PixelRoot32-Game-Engine` into your own PlatformIO project (or add
  it as a Git submodule).
- Implement your own `DrawSurface` and audio backends under `src/drivers`.
- Follow the patterns in `main.cpp` / `main_native.cpp` to configure
  `Engine`, set your initial scene, and build your own gameplay on top.

For detailed engine documentation, refer to:

- [`lib/PixelRoot32-Game-Engine/README.md`](lib/PixelRoot32-Game-Engine/README.md)
- [`lib/PixelRoot32-Game-Engine/API_REFERENCE.md`](lib/PixelRoot32-Game-Engine/API_REFERENCE.md)
