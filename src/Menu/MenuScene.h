#pragma once
#include "core/Scene.h"
#include "graphics/ui/UILabel.h"
#include "graphics/ui/UIButton.h"

class MenuScene : public pixelroot32::core::Scene {
public:
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    pixelroot32::graphics::ui::UILabel* titleLabel;
    pixelroot32::graphics::ui::UIButton* pongButton;
    pixelroot32::graphics::ui::UIButton* brickButton;
    pixelroot32::graphics::ui::UIButton* tttButton;
    pixelroot32::graphics::ui::UIButton* geometryButton;
    pixelroot32::graphics::ui::UIButton* snakeButton;
    pixelroot32::graphics::ui::UIButton* spaceInvadersButton;
    
    int selectedIndex = 0;
    void updateButtonStyles();
    
    // Workaround for InputManager debounce/stateChanged bug
    bool wasUpPressed = false;
    bool wasDownPressed = false;

    pixelroot32::graphics::ui::UILabel* lblNavigate;
    pixelroot32::graphics::ui::UILabel* lblSelect;
};
