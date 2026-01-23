#include "MenuScene.h"
#include "MenuConstants.h"
#include "core/Engine.h"
#include "Config.h"
#include "graphics/Renderer.h"

#include "examples/Pong/PongScene.h"
#include "examples/TicTacToe/TicTacToeScene.h"
#include "examples/Snake/SnakeScene.h"
#include "examples/SpaceInvaders/SpaceInvadersScene.h"
#include "examples/CameraDemo/CameraDemoScene.h"
#include "examples/DualPaletteTest/DualPaletteTestScene.h"
#include "examples/FontTest/FontTestScene.h"
#include "examples/SpritesDemo/SpritesDemoScene.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

// Define instances here
pong::PongScene pongScene;
// brickbreaker::BrickBreakerScene brickScene;
tictactoe::TicTacToeScene tttScene;
snake::SnakeScene snakeScene;
spaceinvaders::SpaceInvadersScene spaceInvadersScene;
camerademo::CameraDemoScene cameraDemoScene;
dualpalettetest::DualPaletteTestScene dualPaletteTestScene;
fonttest::FontTestScene fontTestScene;
#ifdef PIXELROOT32_ENABLE_2BPP_SPRITES
spritesdemo::SpritesDemoScene spritesDemoScene;
#endif

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
    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
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

    // Create vertical layout for buttons
    float btnW = menu::BTN_WIDTH;
    float btnH = menu::BTN_HEIGHT;
    float btnX = cx - (btnW / 2.0f);
    float startY = menu::BTN_START_Y;
    float layoutHeight = screenHeight - startY - menu::NAV_INSTR_Y_OFFSET - 10; // Space for instructions
    
    buttonLayout = new pr32::graphics::ui::UIVerticalLayout(btnX, startY, btnW, layoutHeight);
    buttonLayout->setPadding(0);
    buttonLayout->setSpacing(menu::BTN_GAP);
    buttonLayout->setScrollEnabled(true);
    buttonLayout->setNavigationButtons(menu::BTN_NAV_UP, menu::BTN_NAV_DOWN);
    buttonLayout->setButtonStyle(Color::White, Color::Cyan, Color::White, Color::Black);
    buttonLayout->setRenderLayer(2);
    addEntity(buttonLayout);

    // Create buttons and add them only to the layout (layout handles rendering and positioning)
    pongButton = new pr32::graphics::ui::UIButton("PONG", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&pongScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(pongButton);

    tttButton = new pr32::graphics::ui::UIButton("TIC TAC TOE", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&tttScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(tttButton);

    snakeButton = new pr32::graphics::ui::UIButton("SNAKE", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&snakeScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(snakeButton);

    spaceInvadersButton = new pr32::graphics::ui::UIButton("SPACE INVADERS", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&spaceInvadersScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(spaceInvadersButton);

    cameraDemoButton = new pr32::graphics::ui::UIButton("CAMERA DEMO", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&cameraDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(cameraDemoButton);

    dualPaletteTestButton = new pr32::graphics::ui::UIButton("DUAL PALETTE TEST", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&dualPaletteTestScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(dualPaletteTestButton);

    fontTestButton = new pr32::graphics::ui::UIButton("FONT TEST", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&fontTestScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(fontTestButton);

#ifdef PIXELROOT32_ENABLE_2BPP_SPRITES
    spritesDemoButton = new pr32::graphics::ui::UIButton("SPRITES DEMO", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&spritesDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    buttonLayout->addElement(spritesDemoButton);
#endif

    lblNavigate = new pr32::graphics::ui::UILabel("UP/DOWN: Navigate", 0, screenHeight - menu::NAV_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblNavigate->centerX(screenWidth);
    lblNavigate->setRenderLayer(2);
    addEntity(lblNavigate);

    lblSelect = new pr32::graphics::ui::UILabel("A: Select", 0, screenHeight - menu::SEL_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblSelect->centerX(screenWidth);
    lblSelect->setRenderLayer(2);
    addEntity(lblSelect);

    // Initialize layout selection to first button
    if (buttonLayout->getElementCount() > 0) {
        buttonLayout->setSelectedIndex(0);
    }
}

void MenuScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);
    
    // Input Handling
    auto& input = engine.getInputManager();

    // Handle layout navigation (UP/DOWN handled by layout)
    // Track previous selection for sound feedback
    static int lastSelectedIndex = -1;
    
    buttonLayout->handleInput(input);
    
    // Check if selection changed (for sound feedback)
    int newSelectedIndex = buttonLayout->getSelectedIndex();
    if (newSelectedIndex != lastSelectedIndex && newSelectedIndex >= 0) {
        // Play navigation sound
        pr32::audio::AudioEvent ev;
        ev.type = pr32::audio::WaveType::TRIANGLE;
        ev.frequency = menu::SOUND_NAV_FREQ;
        ev.duration = menu::SOUND_NAV_DUR;
        ev.volume = menu::SOUND_VOL_NAV;
        engine.getAudioEngine().playEvent(ev);
        lastSelectedIndex = newSelectedIndex;
    }

    // Sound test for SELECT button
    if (input.isButtonPressed(menu::BTN_SELECT)) {
        pr32::audio::AudioEvent ev;
        ev.type = pr32::audio::WaveType::PULSE;
        ev.frequency = menu::SOUND_BLIP_FREQ;
        ev.duration = menu::SOUND_BLIP_DUR;
        ev.volume = menu::SOUND_VOL_BLIP;
        ev.duty = 0.5f;
        engine.getAudioEngine().playEvent(ev);
    }
}

void MenuScene::draw(pixelroot32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}
