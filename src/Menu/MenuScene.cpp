#include "MenuScene.h"
#include "MenuConstants.h"
#include "core/Engine.h"
#include "Config.h"
#include "graphics/Renderer.h"

#include "examples/Pong/PongScene.h"
#include "examples/TicTacToe/TicTacToeScene.h"
#include "examples/Snake/SnakeScene.h"
#include "examples/SpaceInvaders/SpaceInvadersScene.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

// Define instances here
pong::PongScene pongScene;
// brickbreaker::BrickBreakerScene brickScene;
tictactoe::TicTacToeScene tttScene;
snake::SnakeScene snakeScene;
spaceinvaders::SpaceInvadersScene spaceInvadersScene;

using Color = pr32::graphics::Color;

#ifdef PIXELROOT32_ENABLE_2BPP_SPRITES
static const uint8_t MENU_LOGO_2BPP_DATA[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x15, 0x15, 0x15, 0x15, 0x40,
    0x05, 0x11, 0x11, 0x10, 0x10, 0x50,
    0x11, 0x15, 0x15, 0x14, 0x04, 0x44,
    0x05, 0x01, 0x11, 0x10, 0x01, 0x50,
    0x01, 0x01, 0x41, 0x15, 0x15, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const Color MENU_LOGO_2BPP_PALETTE[] = {
    Color::Transparent,
    Color::Cyan,
    Color::Blue
};

static const pr32::graphics::Sprite2bpp MENU_LOGO_2BPP = {
    MENU_LOGO_2BPP_DATA,
    MENU_LOGO_2BPP_PALETTE,
    24,
    8,
    3
};
#endif

class MenuBackground : public pr32::core::Entity {
public:
    MenuBackground()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);
    }
};

#ifdef PIXELROOT32_ENABLE_2BPP_SPRITES
class MenuLogo : public pr32::core::Entity {
public:
    MenuLogo()
        : pr32::core::Entity(0.0f, 0.0f, 24.0f, 8.0f, pr32::core::EntityType::GENERIC) {
        setRenderLayer(1);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        int screenWidth = renderer.getWidth();
        int x = (screenWidth - MENU_LOGO_2BPP.width) / 2;
        int y = menu::TITLE_Y - 10;
        renderer.drawSprite(MENU_LOGO_2BPP, x, y, false);
    }
};
#endif

void MenuScene::init() {
    int screenWidth = engine.getRenderer().getWidth();
    int screenHeight = engine.getRenderer().getHeight();

    float cx = screenWidth / 2;

    addEntity(new MenuBackground());

#ifdef PIXELROOT32_ENABLE_2BPP_SPRITES
    addEntity(new MenuLogo());
#endif

    titleLabel = new pr32::graphics::ui::UILabel("Game List", 0, menu::TITLE_Y, Color::White, 1);
    titleLabel->centerX(screenWidth);
    titleLabel->setRenderLayer(2);
    addEntity(titleLabel);

    float btnW = menu::BTN_WIDTH;
    float btnH = menu::BTN_HEIGHT;
    float btnX = cx - (btnW / 2.0f);
    float startY = menu::BTN_START_Y;
    float gap = menu::BTN_GAP;

    pongButton = new pr32::graphics::ui::UIButton("PONG", menu::BTN_SELECT, btnX, startY, btnW, btnH, []() {
        engine.setScene(&pongScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    pongButton->setRenderLayer(2);
    addEntity(pongButton);

    float stepY = btnH + gap;

    tttButton = new pr32::graphics::ui::UIButton("TIC TAC TOE", menu::BTN_SELECT, btnX, startY + stepY, btnW, btnH, []() {
        engine.setScene(&tttScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    tttButton->setRenderLayer(2);
    addEntity(tttButton);

    snakeButton = new pr32::graphics::ui::UIButton("SNAKE", menu::BTN_SELECT, btnX, startY + 2*stepY, btnW, btnH, []() {
        engine.setScene(&snakeScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    snakeButton->setRenderLayer(2);
    addEntity(snakeButton);

    spaceInvadersButton = new pr32::graphics::ui::UIButton("SPACE INVADERS", menu::BTN_SELECT, btnX, startY + 3*stepY, btnW, btnH, []() {
        engine.setScene(&spaceInvadersScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    spaceInvadersButton->setRenderLayer(2);
    addEntity(spaceInvadersButton);

    lblNavigate = new pr32::graphics::ui::UILabel("UP/DOWN: Navigate", 0, screenHeight - menu::NAV_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblNavigate->centerX(screenWidth);
    lblNavigate->setRenderLayer(2);
    addEntity(lblNavigate);

    lblSelect = new pr32::graphics::ui::UILabel("A: Select", 0, screenHeight - menu::SEL_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblSelect->centerX(screenWidth);
    lblSelect->setRenderLayer(2);
    addEntity(lblSelect);

    selectedIndex = -1;
    wasUpPressed = false;
    wasDownPressed = false;
    updateButtonStyles();
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
    tttButton->handleInput(input);
    snakeButton->handleInput(input);
    spaceInvadersButton->handleInput(input);
}

void MenuScene::updateButtonStyles() {
    // Style: Selected = Cyan BG + White Text
    //        Unselected = No BG + White Text
    
    pongButton->setSelected(selectedIndex == 0);
    pongButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 0)); 

    tttButton->setSelected(selectedIndex == 1);
    tttButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 1));

    snakeButton->setSelected(selectedIndex == 2);
    snakeButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 2));

    spaceInvadersButton->setSelected(selectedIndex == 3);
    spaceInvadersButton->setStyle(Color::White, Color::Cyan, (selectedIndex == 3));
}

void MenuScene::draw(pixelroot32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}
