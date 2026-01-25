#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UIAnchorLayout.h"
#include "graphics/ui/UILabel.h"
#include "graphics/ui/UIButton.h"
#include "Config.h"

namespace anchorlayoutdemo {

/**
 * @brief Demo scene to demonstrate UIAnchorLayout usage.
 * 
 * This scene shows:
 * - Anchor layout with UI elements fixed to screen edges/corners
 * - Dynamic positioning regardless of potential screen resize (simulated)
 * - HUD-like interface
 */
class AnchorLayoutDemoScene : public pixelroot32::core::Scene {
public:
    AnchorLayoutDemoScene();
    ~AnchorLayoutDemoScene();
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    class DemoBackground;
    
    DemoBackground* background;
    pixelroot32::graphics::ui::UIAnchorLayout* anchorLayout;
    
    // HUD Elements
    pixelroot32::graphics::ui::UILabel* scoreLabel;
    pixelroot32::graphics::ui::UILabel* livesLabel;
    pixelroot32::graphics::ui::UILabel* fpsLabel;
    pixelroot32::graphics::ui::UILabel* weaponLabel;
    pixelroot32::graphics::ui::UILabel* centerLabel;
    pixelroot32::graphics::ui::UIButton* startButton;
};

}
