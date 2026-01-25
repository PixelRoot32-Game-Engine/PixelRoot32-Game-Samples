#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UIVerticalLayout.h"
#include "graphics/ui/UILabel.h"
#include "Config.h"

namespace labeldemo {

/**
 * @brief Demo scene to demonstrate UILabel usage within a UIVerticalLayout.
 */
class LabelScene : public pixelroot32::core::Scene {
public:
    LabelScene();
    ~LabelScene();
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
