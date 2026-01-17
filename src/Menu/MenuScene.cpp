#include "MenuScene.h"
#include "MenuConstants.h"
#include "core/Engine.h"
#include "Config.h"

#include "examples/BrickBreaker/BrickBreakerScene.h"
#include "examples/Pong/PongScene.h"
#include "examples/TicTacToe/TicTacToeScene.h"
#include "examples/GeometryJump/GeometryJumpScene.h"
#include "examples/Snake/SnakeScene.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

// Define instances here
pong::PongScene pongScene;
brickbreaker::BrickBreakerScene brickScene;
tictactoe::TicTacToeScene tttScene;
geometryjump::GeometryJumpScene geometryScene;
snake::SnakeScene snakeScene;

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
    pongButton = new pr32::graphics::ui::UIButton("PONG", menu::BTN_SELECT, btnX, startY, btnW, btnH, []() {
        engine.setScene(&pongScene);
    });
    addEntity(pongButton);

    brickButton = new pr32::graphics::ui::UIButton("BRICKBREAK", menu::BTN_SELECT, btnX, startY + btnH + gap, btnW, btnH, []() {
        engine.setScene(&brickScene);
    });
    addEntity(brickButton);

    tttButton = new pr32::graphics::ui::UIButton("TIC TAC TOE", menu::BTN_SELECT, btnX, startY + 2*btnH + gap, btnW, btnH, []() {
        engine.setScene(&tttScene);
    });
    addEntity(tttButton);

    geometryButton = new pr32::graphics::ui::UIButton("GEOMETRY JUMP", menu::BTN_SELECT, btnX, startY + 3*btnH + gap, btnW, btnH, []() {
        engine.setScene(&geometryScene);
    });
    addEntity(geometryButton);

    snakeButton = new pr32::graphics::ui::UIButton("SNAKE", menu::BTN_SELECT, btnX, startY + 4*btnH + gap, btnW, btnH, []() {
        engine.setScene(&snakeScene);
    });
    addEntity(snakeButton);

    // Footer - Instructions
    // Cyan color for footer
    lblNavigate = new pr32::graphics::ui::UILabel("UP/DOWN: Navigate", 0, screenHeight - menu::NAV_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblNavigate->centerX(screenWidth);
    addEntity(lblNavigate);

    lblSelect = new pr32::graphics::ui::UILabel("A: Select", 0, screenHeight - menu::SEL_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblSelect->centerX(screenWidth);
    addEntity(lblSelect);

    selectedIndex = 0;
    wasUpPressed = false;
    wasDownPressed = false;
    //updateButtonStyles();
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
    pongButton->handleInput(input);
    brickButton->handleInput(input);
    tttButton->handleInput(input);
    geometryButton->handleInput(input);
    snakeButton->handleInput(input);
}

void MenuScene::updateButtonStyles() {
    // Style: Selected = Cyan BG + White Text
    //        Unselected = No BG + White Text
    
    pongButton->setSelected(selectedIndex == 0);
    pongButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 0)); 

    brickButton->setSelected(selectedIndex == 1);
    brickButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 1));

    tttButton->setSelected(selectedIndex == 2);
    tttButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 2));

    geometryButton->setSelected(selectedIndex == 3);
    geometryButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 3));

    snakeButton->setSelected(selectedIndex == 4);
    snakeButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 4));
}

void MenuScene::draw(pixelroot32::graphics::Renderer& renderer) {
    renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, pixelroot32::graphics::Color::Black);
    Scene::draw(renderer);
}
