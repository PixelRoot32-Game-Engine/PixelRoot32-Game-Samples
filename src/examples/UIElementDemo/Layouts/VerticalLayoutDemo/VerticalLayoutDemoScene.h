#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UIVerticalLayout.h"
#include "graphics/ui/UILabel.h"
#include "EngineConfig.h"

namespace verticallayoutdemo {

/**
 * @brief Demo scene to demonstrate UIVerticalLayout usage.
 * 
 * This scene shows:
 * - Vertical layout with multiple UI elements
 * - Navigation with UP/DOWN buttons
 * - UILabel elements within the layout
 */
class VerticalLayoutDemoScene : public pixelroot32::core::Scene {
public:
    VerticalLayoutDemoScene();
    ~VerticalLayoutDemoScene();
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    class DemoBackground;
    
    DemoBackground* background;
    pixelroot32::graphics::ui::UIVerticalLayout* verticalLayout;
    pixelroot32::graphics::ui::UILabel* titleLabel;
    pixelroot32::graphics::ui::UILabel* instructionLabel;
};

}
