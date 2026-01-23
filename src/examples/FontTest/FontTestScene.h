#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "Config.h"

namespace fonttest {

/**
 * @brief Test scene to demonstrate and validate native bitmap font system.
 * 
 * This scene shows:
 * - Text rendering with different sizes (1x, 2x, 3x)
 * - Centered text
 * - All ASCII characters (space to tilde)
 * - Edge cases (empty strings, unsupported characters, long text)
 * - Visual verification that font rendering works correctly
 */
class FontTestScene : public pixelroot32::core::Scene {
public:
    FontTestScene();
    ~FontTestScene();
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    class TestBackground;
    class TestText;
    
    TestBackground* background;
    TestText* testText;
};

}
