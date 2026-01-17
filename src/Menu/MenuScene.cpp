#include "MenuScene.h"
#include "MenuConstants.h"
#include "core/Engine.h"
#include "Config.h"

#include "examples/BrickBreaker/BrickBreakerScene.h"
#include "examples/Pong/PongScene.h"
#include "examples/TicTacToe/TicTacToeScene.h"
#include "examples/Snake/SnakeScene.h"
#include "examples/SpaceInvaders/SpaceInvadersScene.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

// Define instances here
// pong::PongScene pongScene;
// brickbreaker::BrickBreakerScene brickScene;
tictactoe::TicTacToeScene tttScene;
snake::SnakeScene snakeScene;
spaceinvaders::SpaceInvadersScene spaceInvadersScene;

using Color = pr32::graphics::Color;

void MenuScene::init() {
    int screenWidth = engine.getRenderer().getWidth();
    int screenHeight = engine.getRenderer().getHeight();

    float cx = screenWidth / 2;
    
    // Title
    titleLabel = new pr32::graphics::ui::UILabel("GAME SELECT", 0, menu::TITLE_Y, Color::White, menu::TITLE_FONT_SIZE); 
    titleLabel->centerX(screenWidth);
    addEntity(titleLabel);

    // Buttons
    float btnW = menu::BTN_WIDTH;
    float btnH = menu::BTN_HEIGHT;
    float btnX = cx - (btnW / 2.0f);
    float startY = menu::BTN_START_Y;
    float gap = menu::BTN_GAP;

    // Use index 4 (Space/Action) for the activation button
    // pongButton = new pr32::graphics::ui::UIButton("PONG", menu::BTN_SELECT, btnX, startY, btnW, btnH, []() {
    //     engine.setScene(&pongScene);
    // });
    // addEntity(pongButton);

    // brickButton = new pr32::graphics::ui::UIButton("BRICKBREAK", menu::BTN_SELECT, btnX, startY + btnH + gap, btnW, btnH, []() {
    //     engine.setScene(&brickScene);
    // });
    // addEntity(brickButton);

    float stepY = btnH + gap;

    tttButton = new pr32::graphics::ui::UIButton("TIC TAC TOE", menu::BTN_SELECT, btnX, startY, btnW, btnH, []() {
        engine.setScene(&tttScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    addEntity(tttButton);

    snakeButton = new pr32::graphics::ui::UIButton("SNAKE", menu::BTN_SELECT, btnX, startY + stepY, btnW, btnH, []() {
        engine.setScene(&snakeScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    addEntity(snakeButton);

    spaceInvadersButton = new pr32::graphics::ui::UIButton("SPACE INVADERS", menu::BTN_SELECT, btnX, startY + 2*stepY, btnW, btnH, []() {
        engine.setScene(&spaceInvadersScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    addEntity(spaceInvadersButton);

    // Footer - Instructions
    // Cyan color for footer
    lblNavigate = new pr32::graphics::ui::UILabel("UP/DOWN: Navigate", 0, screenHeight - menu::NAV_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblNavigate->centerX(screenWidth);
    addEntity(lblNavigate);

    lblSelect = new pr32::graphics::ui::UILabel("A: Select", 0, screenHeight - menu::SEL_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblSelect->centerX(screenWidth);
    addEntity(lblSelect);

    tttButton->setSelected(false);
    tttButton->setStyle(Color::White, Color::Cyan, false);

    snakeButton->setSelected(false);
    snakeButton->setStyle(Color::White, Color::Cyan, false);

    spaceInvadersButton->setSelected(false);
    spaceInvadersButton->setStyle(Color::White, Color::Cyan, false);

    selectedIndex = 0;
    wasUpPressed = false;
    wasDownPressed = false;
}

void MenuScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);
    
    // Input Handling
    auto& input = engine.getInputManager();

    // Sound test
    if (input.isButtonPressed(menu::BTN_SELECT)) { // Space / A
        // Play simple blip
        pr32::audio::AudioEvent ev;
        ev.type = pr32::audio::WaveType::PULSE;
        ev.frequency = menu::SOUND_BLIP_FREQ;
        ev.duration = menu::SOUND_BLIP_DUR;
        ev.volume = menu::SOUND_VOL_BLIP;
        ev.duty = 0.5f;
        engine.getAudioEngine().playEvent(ev);
    }

    // Workaround for multiple triggers: Use isButtonDown + local state to detect rising edge
    bool isUp = input.isButtonDown(menu::BTN_NAV_UP);
    if (isUp && !wasUpPressed) { // UP Rising Edge
        selectedIndex--;
        if (selectedIndex < 0) selectedIndex = menu::GAME_COUNT - 1;
        updateButtonStyles();
        
        // Navigation sound
        pr32::audio::AudioEvent ev;
        ev.type = pr32::audio::WaveType::TRIANGLE;
        ev.frequency = menu::SOUND_NAV_FREQ;
        ev.duration = menu::SOUND_NAV_DUR;
        ev.volume = menu::SOUND_VOL_NAV;
        engine.getAudioEngine().playEvent(ev);
    }
    wasUpPressed = isUp;

    bool isDown = input.isButtonDown(menu::BTN_NAV_DOWN);
    if (isDown && !wasDownPressed) { // DOWN Rising Edge
        selectedIndex++;
        if (selectedIndex >= menu::GAME_COUNT) selectedIndex = 0;
        updateButtonStyles();

        // Navigation sound
        pr32::audio::AudioEvent ev;
        ev.type = pr32::audio::WaveType::TRIANGLE;
        ev.frequency = menu::SOUND_NAV_FREQ;
        ev.duration = menu::SOUND_NAV_DUR;
        ev.volume = menu::SOUND_VOL_NAV;
        engine.getAudioEngine().playEvent(ev);
    }
    wasDownPressed = isDown;

    // Trigger button logic (checks for button 4 press if selected)
    // UIButton::handleInput uses isButtonPressed internally which might also have issues,
    // but let's test navigation first.
    // pongButton->handleInput(input);
    // brickButton->handleInput(input);
    tttButton->handleInput(input);
    snakeButton->handleInput(input);
    spaceInvadersButton->handleInput(input);
}

void MenuScene::updateButtonStyles() {
    // Style: Selected = Cyan BG + White Text
    //        Unselected = No BG + White Text
    
    // pongButton->setSelected(selectedIndex == 0);
    // pongButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 0)); 

    // brickButton->setSelected(selectedIndex == 1);
    // brickButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 1));

    tttButton->setSelected(selectedIndex == 0);
    tttButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 0));

    snakeButton->setSelected(selectedIndex == 1);
    snakeButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 1));

    spaceInvadersButton->setSelected(selectedIndex == 2);
    spaceInvadersButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 2));
}

void MenuScene::draw(pixelroot32::graphics::Renderer& renderer) {
    renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, pixelroot32::graphics::Color::Black);
    Scene::draw(renderer);
}
