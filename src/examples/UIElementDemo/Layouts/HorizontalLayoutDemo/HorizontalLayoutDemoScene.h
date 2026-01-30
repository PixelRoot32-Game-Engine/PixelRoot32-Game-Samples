#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UIHorizontalLayout.h"
#include "graphics/ui/UILabel.h"
#include "EngineConfig.h"

namespace horizontallayoutdemo {

/**
 * @brief Demo scene to demonstrate UIHorizontalLayout usage.
 * 
 * This scene shows:
 * - Horizontal layout with multiple UI elements
 * - Navigation with LEFT/RIGHT buttons
 * - UILabel elements within the layout
 */
class HorizontalLayoutDemoScene : public pixelroot32::core::Scene {
public:
    HorizontalLayoutDemoScene();
    ~HorizontalLayoutDemoScene();
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    class DemoBackground;
    
    DemoBackground* background;
    pixelroot32::graphics::ui::UIHorizontalLayout* horizontalLayout;
    pixelroot32::graphics::ui::UILabel* titleLabel;
    pixelroot32::graphics::ui::UILabel* instructionLabel;
};

}
