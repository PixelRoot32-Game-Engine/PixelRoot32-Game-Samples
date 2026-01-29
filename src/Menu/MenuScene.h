#pragma once
#include "core/Scene.h"
#include "graphics/ui/UILabel.h"
#include "graphics/ui/UIButton.h"
#include "graphics/ui/UIVerticalLayout.h"

enum class MenuState {
    MAIN,
    GAMES,
    UIELEMENTS,
    BUTTONS,
    LABELS,
    CHECKBOXES,
    LAYOUTS
};

class MenuScene : public pixelroot32::core::Scene {
public:
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    MenuState currentState = MenuState::MAIN;
    
    // UI Elements
    pixelroot32::graphics::ui::UILabel* titleLabel;
    pixelroot32::graphics::ui::UIVerticalLayout* buttonLayout;

    // Examples menu buttons
    pixelroot32::graphics::ui::UIButton* gamesButton;
    pixelroot32::graphics::ui::UIButton* cameraDemoButton;
    pixelroot32::graphics::ui::UIButton* spritesDemoButton;
    pixelroot32::graphics::ui::UIButton* tileMapDemoButton;
    pixelroot32::graphics::ui::UIButton* fontTestButton;
    pixelroot32::graphics::ui::UIButton* dualPaletteTestButton;
    pixelroot32::graphics::ui::UIButton* uiElementsButton;
    
    // Games menu buttons
    pixelroot32::graphics::ui::UIButton* pongButton;
    pixelroot32::graphics::ui::UIButton* brickBrackeButton;
    pixelroot32::graphics::ui::UIButton* snakeButton;
    pixelroot32::graphics::ui::UIButton* spaceInvadersButton;
    pixelroot32::graphics::ui::UIButton* tttButton;
    pixelroot32::graphics::ui::UIButton* metroidvaniaButton;

    // UIElements menu buttons
    pixelroot32::graphics::ui::UIButton* buttonsButton;
    pixelroot32::graphics::ui::UIButton* labelsButton;
    pixelroot32::graphics::ui::UIButton* checkboxesButton;
    pixelroot32::graphics::ui::UIButton* layoutsButton;

    // Layouts menu buttons
    pixelroot32::graphics::ui::UIButton* verticalLayoutButton;
    pixelroot32::graphics::ui::UIButton* horizontalLayoutButton;
    pixelroot32::graphics::ui::UIButton* gridLayoutButton;
    pixelroot32::graphics::ui::UIButton* anchorLayoutButton;
    
    // Navigation labels
    pixelroot32::graphics::ui::UILabel* lblNavigate;
    pixelroot32::graphics::ui::UILabel* lblSelect;
    pixelroot32::graphics::ui::UILabel* lblBack;
    
    // Helper methods
    void setupMainMenu();
    void setupGamesMenu();
    void setupUIElementsMenu();
    void setupLayoutsMenu();
    void showMenu(MenuState state);
    void goBack();
};
