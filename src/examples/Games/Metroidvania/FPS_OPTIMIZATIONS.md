# FPS Optimizations – Metroidvania on ESP32

Goal: increase from ~14 stable FPS to ~20 FPS without changing gameplay or game architecture.

## Context

- **Engine**: PixelRoot32 with tile-based collision (O(1) per cell), already optimized according to `COLLISION_LAYERS_IMPLEMENTATION_PLAN.md`.
- **Display**: 240×240, 30×30 tile map with 8×8 px tiles (3 layers: background, platforms, stairs).
- **Main Bottleneck**: Draw time (3 tilemaps + player) and reads from Flash (PROGMEM) during collision and rendering.

---

## 1. Implemented: Stairs Cache in RAM (PlayerActor)

**What it does**: In `init`, a bitmask is built in RAM (1 bit per stair cell) from `stairsIndices` (PROGMEM). All "is there a stair here?" checks read from RAM instead of Flash.

**Where**: `PlayerActor::buildStairsCache()`, called from `MetroidvaniaScene::init()` after `setStairs()`.

**Expected Impact**: Fewer Flash reads in `isOverlappingStairs()` and the vertical collision loop (hasLadder). On ESP32, Flash is slower than RAM; this can contribute 1–2 FPS depending on the scene.

**Cost**: ~113 bytes of RAM (30×30 bits) and a single sweep of the stairs layer at startup.

---

## 2. Build and Configuration (PlatformIO)

- **Code Optimization**: In `platformio.ini`, `esp32dev` environment, add:

  ```ini
  build_flags = 
    ... 
    -O2
  ```

  If you already use `-Os` for size, try `-O2` only for the FPS target; it usually provides better speed.

- **FPS overlay**: If you don't need the on-screen counter in release, remove `-D PIXELROOT32_ENABLE_FPS_DISPLAY` from the ESP32 env. This avoids `drawText` every frame (or every N frames when the number updates). Small gain without touching game logic.

- **CPU Frequency**: Ensure the board is at 240 MHz (default on many ESP32). In `platformio.ini`:

  ```ini
  board_build.f_cpu = 240000000L
  ```

- **Display SPI**: You have `SPI_FREQUENCY=40000000`. If the display and wiring allow, trying 80 MHz can reduce the transfer time to the TFT (check the panel's datasheet). Any gain here is noticeable in all drawing operations.

---

## 3. Game (without touching engine)

- **Player Animation**: `ANIMATION_FRAME_TIME_MS = 120` in `GameConstants.h`. Increasing to 150–160 ms reduces the frequency of frame changes and state decisions; movement looks almost the same. Small saving without impacting mechanics.

- **Avoid unnecessary work in `update()`**: Collision logic is already bounded (few cells per frame). Keep it this way; don't add more entities or checks in the main loop.

---

## 4. Engine / Renderer (if you can touch the core)

- **Viewport culling**: The engine's 4bpp `drawTileMap` already performs viewport culling; with a 240×240 full-screen map, all visible tiles are drawn. There is no extra margin there without changing map size or camera.

- **Single pass for multiple layers**: Currently, 3 calls to `drawTileMap` are made (background, platforms, stairs). A more invasive optimization would be a "composite tilemap" that, for each visible cell, chooses the tile from the highest non-empty layer and draws only once. This would require changes in the editor and data structure; it is not considered "non-invasive" for this document.

- **Data in RAM for the frame**: The `indices` for each layer are in PROGMEM. Copying only the visible window of indices to a RAM buffer every frame (or when the camera moves) would reduce Flash reads during drawing, at the cost of more RAM and logic in the Renderer.

---

## 6. Physical Resolution Recommendation

> [!IMPORTANT]
> To maintain a stable frame rate of **30 FPS** or higher in this Metroidvania sample, it is highly recommended to use a physical display resolution of **128x128 pixels**.
>
> Using a **240x240** resolution (even with internal scaling) limits performance to approximately **14 FPS** due to the maximum 40MHz SPI bus speed constraint of the ST7789 driver. For high-action games like this one, the lower physical resolution provides the best experience.

## 5. Summary of recommended actions (by priority)

| Priority | Action | Invasiveness | Expected Impact |
|-----------|--------|-------------|------------------|
| 1 | Stairs cache in RAM (already done) | Low | 1–2 FPS |
| 2 | Add `-O2` for ESP32 | None | Variable, often 1–3 FPS |
| 3 | Remove FPS overlay in release build | None | Small |
| 4 | Increase `ANIMATION_FRAME_TIME_MS` to 150 | Low | Small |
| 5 | Check CPU 240 MHz and, if applicable, SPI 80 MHz | None / Low | Variable |

Combining 1 + 2 + 3 (+ 4 if you want) it's reasonable to target about 18–20 stable FPS; if the bottleneck remains at the TFT, the next path would be trying higher SPI or reducing resolution/drawn area (more invasive).
