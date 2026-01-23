#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "Config.h"

namespace dualpalettetest {

/**
 * @brief Test scene to demonstrate and validate dual palette functionality.
 * 
 * This scene shows:
 * - Background layer (layer 0) using NES palette
 * - Sprite layer (layer 1) using GB palette
 * - Visual verification that different palettes are applied correctly
 */
class DualPaletteTestScene : public pixelroot32::core::Scene {
public:
    DualPaletteTestScene();
    ~DualPaletteTestScene();
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    class TestBackground;
    class TestSprite;
    
    TestBackground* background;
    TestSprite* testSprite;
};

}
