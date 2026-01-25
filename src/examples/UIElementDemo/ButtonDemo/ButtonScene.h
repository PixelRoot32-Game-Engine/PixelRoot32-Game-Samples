#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UIVerticalLayout.h"
#include "graphics/ui/UILabel.h"
#include "graphics/ui/UIButton.h"
#include "Config.h"

namespace buttondemo {

/**
 * @brief Demo scene to demonstrate UIButton usage within a UIVerticalLayout.
 */
class ButtonScene : public pixelroot32::core::Scene {
public:
    ButtonScene();
    ~ButtonScene();
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
