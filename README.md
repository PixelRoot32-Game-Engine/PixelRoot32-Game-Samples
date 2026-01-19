## GameSample – PixelRoot32 Getting Started Project

This repository is a small **getting‑started project** that shows how to use the
[PixelRoot32 Game Engine](https://github.com/Gperez88/PixelRoot32-Game-Engine) inside a
PlatformIO project.

It wires the engine to:

- an **ESP32 + TFT_eSPI display + buttons + speaker**, and
- a **native desktop build (SDL2)** for fast iteration without hardware.

From a simple menu you can launch several example games shipped with the engine,
such as **Pong**, **Snake**, **Space Invaders**, **Tic‑Tac‑Toe**, and the
**Camera Demo**. For deeper engine walk‑throughs based on real games, see:
[`Space Invaders`](#example-space-invaders--engine-overview),
[`CameraDemo`](#example-camerademo--camera-parallax-and-platforms),
and [`Snake`](#example-snake--entity-pooling-and-discrete-game-loop) below.

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
│   ├── examples/           # Example games (Pong, Snake, SpaceInvaders, etc.)
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
- [`lib/PixelRoot32-Game-Engine`](https://github.com/Gperez88/PixelRoot32-Game-Engine) – engine core library.

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

## Example: Space Invaders – Engine Overview

The **Space Invaders** example (under
[`src/examples/SpaceInvaders`](src/examples/SpaceInvaders)) shows how to build
 a complete game on top of PixelRoot32. It is a good reference for
 understanding how scenes, entities, rendering, input, audio, and collisions
 work together.

### Core architecture

- The global `Engine` provides access to the renderer, input manager, audio
  engine, and music player.
- `SpaceInvadersScene` derives from `pixelroot32::core::Scene` and implements
  the usual lifecycle:
  - `init()` to set up the game and start background music.
  - `update(deltaTime)` to handle gameplay logic each frame.
  - `draw(renderer)` (inherited from `Scene`) to draw all entities.
- The scene owns and manages entities such as the player, aliens, bunkers,
  projectiles, and background.

### Entities and game objects

Space Invaders uses several entity types:

- `PlayerActor` for the player ship, reading input and exposing `wantsToShoot()`.
- `AlienActor` for each enemy, with its own hit box, animation, and score value.
- `ProjectileActor` for both player and enemy bullets, distinguished by a
  `ProjectileType`. The scene keeps a fixed pool of projectiles and activates
  or deactivates them as needed instead of allocating during gameplay.
- `BunkerActor` for destructible cover, with simple health/damage logic.
- `TilemapBackground`, a small `Entity` that draws a starfield tilemap.

All of these derive from `Entity` and are registered with the scene via
`addEntity(...)`, so the engine can update and draw them automatically.

### Rendering

The example uses a mix of tilemaps, sprites, and simple effects:

- A tilemap (`STARFIELD_MAP`) is used for the starfield background.
- Aliens, player, bunkers, and bullets are drawn as sprites in front of the
  background.
- A small `ExplosionAnimation` object handles the player explosion using a few
  1bpp sprites and a simple frame-advance timer.
- Render layers ensure the background is drawn first and gameplay entities on
  top.

### Input and player control

- The player reads horizontal input and fire button state from the engine’s
  `InputManager`.
- `SpaceInvadersScene` enforces high-level rules such as:
  - Only allowing one active player bullet at a time.
  - Requiring the fire button to be released before another shot is accepted
    (`fireInputReady` flag).

This separation keeps low-level movement inside the player actor, and
game-design rules inside the scene.

### Audio: SFX and music

The example demonstrates both **sound effects** and **music**:

- Sound effects use `AudioEvent` with parameters such as `WaveType`, frequency,
  duration, volume, and duty cycle.
- Music is defined as arrays of `MusicNote` and wrapped in `MusicTrack`
  objects, then played via `engine.getMusicPlayer().play(track)`.
- Different background tracks (slow/medium/fast) and `setTempoFactor()` are
  used to synchronize music tempo with alien movement speed as enemies are
  cleared, reproducing the classic “tension ramp” of Space Invaders.

### Collisions and game state

- Player bullets use swept-circle vs rectangle tests (`sweepCircleVsRect`)
  between their previous and current positions to robustly hit fast-moving
  targets.
- Additional axis-aligned rectangle checks (`getHitBox().intersects(...)`)
  are used as a simple fallback.
- Similar logic is used for enemy bullets against bunkers and the player.
- The scene tracks game state:
  - Active gameplay.
  - Player hit (temporary pause while the explosion plays, then respawn).
  - Win and loss conditions, with appropriate music cues.

Space Invaders is the recommended example to study first when you want to
understand how to structure a full game on top of PixelRoot32.

---

## Example: CameraDemo – Camera, Parallax and Platforms

The **CameraDemo** example (under
[`src/examples/CameraDemo`](src/examples/CameraDemo)) focuses on horizontal
 scrolling worlds, camera following, parallax backgrounds, and simple
 platform physics.

### World vs screen

- The level is defined as a tilemap wider than the display.
- The player (`PlayerCube`) has a world size via `setWorldSize`, while the
  camera defines which portion of the world is visible.
- `CameraDemoScene` configures horizontal camera bounds based on the total
  level width so the camera never scrolls outside the playable area.

### Camera following and parallax

- The camera tracks the player by following the center of the cube
  (`camera.followTarget(centerX, centerY)`).
- The renderer uses `setDisplayOffset` with different factors to draw:
  - A far background layer (hills/sky) scrolling slowly.
  - A mid-ground layer scrolling at medium speed.
  - The main tilemap and player scrolling at full speed.
- This produces a cheap but effective parallax effect suitable for ESP32.

### Tilemap and platforms

- The ground and platforms are encoded in a tilemap using a small set of
  1bpp sprites (empty, ground, platform).
- A fixed array of `PlatformRect` records the collision geometry derived
  from the tilemap (world coordinates and sizes).
- The player update receives this platform list and performs platform-style
  collision and jumping on top of it.

CameraDemo is the best reference for building side-scrolling or platformer
games with camera tracking and parallax backgrounds.

---

## Example: Snake – Entity Pooling and Discrete Game Loop

The **Snake** example (under [`src/examples/Snake`](src/examples/Snake))
 demonstrates how to build a grid-based game with a discrete tick loop and
 no dynamic allocations during gameplay.

### Scene structure and background

- `SnakeScene` derives from `Scene` and owns all game state: snake segments,
  food position, score, direction, timers, and game-over flags.
- A small `SnakeBackground` entity draws the playfield border and reserves
  the top rows for UI.

### Entity pooling

- The game pre-allocates a pool of `SnakeSegmentActor` objects once and
  reuses them to represent the snake body.
- `snakeSegments` is an array view into this pool; segments are moved,
  inserted, or reused instead of constructing new actors.
- This pattern avoids runtime allocation and is ideal for constrained
  targets like the ESP32.

### Grid-based movement and timing

- The snake moves on a logical grid (`GRID_WIDTH`, `GRID_HEIGHT`,
  `CELL_SIZE`).
- Movement is driven by a discrete timer: when `now - lastMoveTime` exceeds
  `moveInterval`, the head advances one cell.
- Each time the snake eats food, score increases, a new segment is added
  (reusing the pool), and `moveInterval` is reduced down to a minimum to
  increase difficulty.

### Input, collisions, and HUD

- Input (up/down/left/right) changes the next direction, with simple rules
  to forbid immediate reversal (e.g., from up to down).
- Collision rules:
  - Leaving the board area triggers game over.
  - The head touching a “dead” segment or invalid state also triggers
    game over.
- The HUD shows the score and a “GAME OVER / press to restart” message;
  short audio cues differentiate moving, eating, and dying.

Snake is a compact reference for building games with discrete time steps,
pooled entities, and minimal but responsive feedback.

---

## Example: SpritesDemo – Multi-Layer Sprites & Animation Groups

The **SpritesDemo** example (under [`src/examples/SpritesDemo`](src/examples/SpritesDemo)) showcases the engine's capability to render complex, multi-colored characters using layered 1bpp sprites and group-based animations.

### Multi-Layer Composition

- Each character is a `MultiSprite` composed of 4 separate 1bpp layers (`LAYER_0` to `LAYER_3`).
- Each layer is assigned a specific color from the palette (Black, LightBlue, Yellow, Red), combining to form a rich, colorful sprite without requiring full RGB bitmaps.
- This technique mimics NES/GameBoy-style sprite composition, saving memory while allowing for colorful assets.

### Animation Groups & Switching

- The demo defines **4 distinct animation groups** (ANIM_0 to ANIM_3), each containing a sequence of different characters.
- A `SpritesDemoActor` manages the playback:
  - **Frame Stepping**: Advances the animation frame every 150ms.
  - **Group Switching**: Automatically cycles to the next animation group every 2 seconds.
- This demonstrates how to decouple animation logic (`SpriteAnimation`) from rendering (`Renderer::drawMultiSprite`), allowing for dynamic and state-driven visuals.

> **Note:** The sprites used in this demo were generated using the [PixelRoot32-Sprite-Sheet-Compiler](https://github.com/PixelRoot32-Game-Engine/PixelRoot32-Sprite-Sheet-Compiler/tree/master).

---

## Using PixelRoot32 in Your Own Projects

This sample is meant as a **starting point**:

- Copy `lib/PixelRoot32-Game-Engine` into your own PlatformIO project (or add
  it as a Git submodule).
- Implement your own `DrawSurface` and audio backends under `src/drivers`.
- Follow the patterns in `main.cpp` / `main_native.cpp` to configure
  `Engine`, set your initial scene, and build your own gameplay on top.

For detailed engine documentation, refer to:

- [`lib/PixelRoot32-Game-Engine/README.md`](https://github.com/Gperez88/PixelRoot32-Game-Engine/blob/main/README.md)
- [`lib/PixelRoot32-Game-Engine/API_REFERENCE.md`](https://github.com/Gperez88/PixelRoot32-Game-Engine/blob/main/API_REFERENCE.md)
