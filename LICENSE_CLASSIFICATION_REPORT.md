# License Classification Report

## Summary
This report classifies the source files of the `PixelRoot32-Game-Engine` project by comparing them with the original `ESP32-Game-Engine-main` project. The classification is based on file name equality.

- **Unchanged MIT Files**: 0
- **Modified MIT Files**: 13
- **New GPL v3 Files**: 34

## Classification Details

### 1. Unchanged MIT Files
*None found.* (All matching files have been modified to support the new architecture, namespaces, and include paths).

### 2. Modified MIT Files
These files exist in both projects (matched by filename) but have been modified in the new project. They carry the "Modified MIT" license header.

**Core**
- `include/core/Entity.h`
- `src/core/Scene.cpp`
- `include/core/Scene.h`
- `src/core/SceneManager.cpp`
- `include/core/SceneManager.h`

**Graphics**
- `include/graphics/DisplayConfig.h`
- `src/graphics/Renderer.cpp`
- `include/graphics/Renderer.h`

**Physics**
- `src/physics/CollisionSystem.cpp`
- `include/physics/CollisionSystem.h`

**Input**
- `include/input/InputConfig.h`
- `src/input/InputManager.cpp`
- `include/input/InputManager.h`

### 3. New GPL v3 Files
These files exist only in the new project. They carry the "GNU GPL v3" license header.

**Audio**
- `src/audio/AudioEngine.cpp`
- `include/audio/AudioEngine.h`
- `src/audio/MusicPlayer.cpp`
- `include/audio/MusicPlayer.h`
- `include/audio/AudioBackend.h`
- `include/audio/AudioConfig.h`
- `include/audio/AudioMusicTypes.h`
- `include/audio/AudioTypes.h`
- `src/drivers/esp32/ESP32_DAC_AudioBackend.cpp`
- `include/drivers/esp32/ESP32_DAC_AudioBackend.h`
- `src/drivers/esp32/ESP32_I2S_AudioBackend.cpp`
- `include/drivers/esp32/ESP32_I2S_AudioBackend.h`
- `src/drivers/native/SDL2_AudioBackend.cpp`
- `include/drivers/native/SDL2_AudioBackend.h`

**Graphics**
- `src/graphics/particles/ParticleEmitter.cpp`
- `include/graphics/particles/ParticleEmitter.h`
- `include/graphics/particles/Particle.h`
- `include/graphics/particles/ParticleConfig.h`
- `include/graphics/particles/ParticlePresets.h`
- `src/graphics/ui/UIButton.cpp`
- `include/graphics/ui/UIButton.h`
- `src/graphics/ui/UILabel.cpp`
- `include/graphics/ui/UILabel.h`
- `include/graphics/ui/UIElement.h`
- `src/graphics/Camera2D.cpp`
- `include/graphics/Camera2D.h`
- `src/graphics/Color.cpp`
- `include/graphics/Color.h`
- `include/graphics/DrawSurface.h`
- `include/drivers/esp32/TFT_eSPI_Drawer.h`
- `include/drivers/native/SDL2_Drawer.h`

**Core/Physics/Math/Config**
- `src/core/Engine.cpp` (Replaces EDGE.cpp)
- `include/core/Engine.h` (Replaces EDGE.h)
- `include/core/Actor.h`
- `include/core/PhysicsActor.h`
- `src/core/PhysicsActor.cpp`
- `src/physics/CollisionPrimitives.cpp`
- `include/physics/CollisionTypes.h`
- `include/math/MathUtil.h`
- `include/Config.h`
- `src/platforms/mock/*` (Mock implementations for testing)
