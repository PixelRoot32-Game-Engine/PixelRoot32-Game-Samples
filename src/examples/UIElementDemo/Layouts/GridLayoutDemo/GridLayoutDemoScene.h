#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UIGridLayout.h"
#include "graphics/ui/UILabel.h"
#include "graphics/ui/UIButton.h"
#include "graphics/ui/UIPaddingContainer.h"
#include "EngineConfig.h"

namespace gridlayoutdemo {

/**
 * @brief Demo scene to demonstrate UIGridLayout usage.
 * 
 * This scene shows:
 * - Grid layout with multiple UI elements
 * - Navigation with UP/DOWN/LEFT/RIGHT buttons
 * - UILabel elements within the grid
 * - PaddingContainer example combined with GridLayout
 */
class GridLayoutDemoScene : public pixelroot32::core::Scene {
public:
    GridLayoutDemoScene();
    ~GridLayoutDemoScene();
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    class DemoBackground;
    
    DemoBackground* background;
    pixelroot32::graphics::ui::UIGridLayout* gridLayout;
    pixelroot32::graphics::ui::UIPaddingContainer* paddingContainer;
    pixelroot32::graphics::ui::UILabel* titleLabel;
    pixelroot32::graphics::ui::UILabel* instructionLabel;
    pixelroot32::graphics::ui::UILabel* infoLabel;
};

}
