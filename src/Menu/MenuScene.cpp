#include "MenuScene.h"
#include "MenuConstants.h"
#include "core/Engine.h"
#include "Config.h"
#include "graphics/Renderer.h"

#include "examples/Games/Pong/PongScene.h"
#include "examples/Games/BrickBreaker/BrickBreakerScene.h"
#include "examples/Games/TicTacToe/TicTacToeScene.h"
#include "examples/Games/Snake/SnakeScene.h"
#include "examples/Games/SpaceInvaders/SpaceInvadersScene.h"
#include "examples/CameraDemo/CameraDemoScene.h"
#include "examples/DualPaletteTest/DualPaletteTestScene.h"
#include "examples/FontTest/FontTestScene.h"
#include "examples/SpritesDemo/SpritesDemoScene.h"
#include "examples/TileMapDemo/TileMapDemoScene.h"
#include "examples/UIElementDemo/CheckBoxDemo/CheckBoxScene.h"
#include "examples/UIElementDemo/ButtonDemo/ButtonScene.h"
#include "examples/UIElementDemo/LabelDemo/LabelScene.h"
#include "examples/UIElementDemo/Layouts/VerticalLayoutDemo/VerticalLayoutDemoScene.h"
#include "examples/UIElementDemo/Layouts/HorizontalLayoutDemo/HorizontalLayoutDemoScene.h"
#include "examples/UIElementDemo/Layouts/GridLayoutDemo/GridLayoutDemoScene.h"
#include "examples/UIElementDemo/Layouts/AnchorLayoutDemo/AnchorLayoutDemoScene.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

// Define instances here
pong::PongScene pongScene;
brickbreaker::BrickBreakerScene brickScene;
tictactoe::TicTacToeScene tttScene;
snake::SnakeScene snakeScene;
spaceinvaders::SpaceInvadersScene spaceInvadersScene;
camerademo::CameraDemoScene cameraDemoScene;
dualpalettetest::DualPaletteTestScene dualPaletteTestScene;
fonttest::FontTestScene fontTestScene;
checkboxdemo::CheckBoxScene checkBoxScene;
buttondemo::ButtonScene buttonScene;
labeldemo::LabelScene labelScene;
verticallayoutdemo::VerticalLayoutDemoScene verticalLayoutDemoScene;
horizontallayoutdemo::HorizontalLayoutDemoScene horizontalLayoutDemoScene;
gridlayoutdemo::GridLayoutDemoScene gridLayoutDemoScene;
anchorlayoutdemo::AnchorLayoutDemoScene anchorLayoutDemoScene;
spritesdemo::SpritesDemoScene spritesDemoScene;
tilemapdemo::TileMapDemoScene tileMapDemoScene;

using Color = pr32::graphics::Color;

void MenuScene::init() {
    // Clear any existing entities to prevent duplicates if init() is called multiple times
    clearEntities();

    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    int screenWidth = engine.getRenderer().getWidth();
    int screenHeight = engine.getRenderer().getHeight();

    // Create title label
    titleLabel = new pr32::graphics::ui::UILabel("Examples", 0, menu::TITLE_Y, Color::White, menu::TITLE_FONT_SIZE);
    titleLabel->centerX(screenWidth);
    titleLabel->setRenderLayer(2);
    addEntity(titleLabel);

    // Create vertical layout for buttons
    float btnW = menu::BTN_WIDTH;
    float cx = screenWidth / 2.0f;
    float btnX = cx - (btnW / 2.0f);
    float startY = menu::BTN_START_Y;
    float layoutHeight = screenHeight - startY - menu::NAV_INSTR_Y_OFFSET - 10;
    
    buttonLayout = new pr32::graphics::ui::UIVerticalLayout(btnX, startY, btnW, layoutHeight);
    buttonLayout->setPadding(0);
    buttonLayout->setSpacing(menu::BTN_GAP);
    buttonLayout->setScrollEnabled(true);
    buttonLayout->setNavigationButtons(menu::BTN_NAV_UP, menu::BTN_NAV_DOWN);
    buttonLayout->setButtonStyle(Color::White, Color::Cyan, Color::White, Color::Black);
    buttonLayout->setRenderLayer(2);
    addEntity(buttonLayout);

    // Create all buttons (will be shown/hidden based on menu state)
    setupMainMenu();
    setupGamesMenu();
    setupUIElementsMenu();
    setupLayoutsMenu();

    // Navigation labels
    lblNavigate = new pr32::graphics::ui::UILabel("UP/DOWN: Navigate", 0, screenHeight - menu::NAV_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblNavigate->centerX(screenWidth);
    lblNavigate->setRenderLayer(2);
    addEntity(lblNavigate);

    lblSelect = new pr32::graphics::ui::UILabel("A: Select", 0, screenHeight - menu::SEL_INSTR_Y_OFFSET, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblSelect->centerX(screenWidth);
    lblSelect->setRenderLayer(2);
    addEntity(lblSelect);

    lblBack = new pr32::graphics::ui::UILabel("B: Back", 0, screenHeight - 15, Color::Cyan, menu::INSTRUCTION_FONT_SIZE);
    lblBack->centerX(screenWidth);
    lblBack->setRenderLayer(2);
    addEntity(lblBack);

    // Show main menu initially
    currentState = MenuState::MAIN;
    showMenu(MenuState::MAIN);
}

void MenuScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);
    
    // Input Handling
    auto& input = engine.getInputManager();

    // Handle back button (B button, typically button 5)
    static bool wasBackPressed = false;
    bool isBackPressed = input.isButtonPressed(5); // B button
    if (isBackPressed && !wasBackPressed) {
        goBack();
        // Play back sound
        pr32::audio::AudioEvent ev;
        ev.type = pr32::audio::WaveType::TRIANGLE;
        ev.frequency = menu::SOUND_NAV_FREQ;
        ev.duration = menu::SOUND_NAV_DUR;
        ev.volume = menu::SOUND_VOL_NAV;
        engine.getAudioEngine().playEvent(ev);
    }
    wasBackPressed = isBackPressed;

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

void MenuScene::setupMainMenu() {
    float btnW = menu::BTN_WIDTH;
    float btnH = menu::BTN_HEIGHT;
    
    gamesButton = new pr32::graphics::ui::UIButton("GAMES", menu::BTN_SELECT, 0, 0, btnW, btnH, [this]() {
        showMenu(MenuState::GAMES);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    cameraDemoButton = new pr32::graphics::ui::UIButton("CAMERA DEMO", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&cameraDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    spritesDemoButton = new pr32::graphics::ui::UIButton("SPRITES DEMO", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&spritesDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    tileMapDemoButton = new pr32::graphics::ui::UIButton("TILEMAP DEMO", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&tileMapDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    fontTestButton = new pr32::graphics::ui::UIButton("FONT TEST", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&fontTestScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    dualPaletteTestButton = new pr32::graphics::ui::UIButton("DUAL PALETTE TEST", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&dualPaletteTestScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    uiElementsButton = new pr32::graphics::ui::UIButton("UIELEMENTS", menu::BTN_SELECT, 0, 0, btnW, btnH, [this]() {
        showMenu(MenuState::UIELEMENTS);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
}

void MenuScene::setupGamesMenu() {
    float btnW = menu::BTN_WIDTH;
    float btnH = menu::BTN_HEIGHT;
    
    pongButton = new pr32::graphics::ui::UIButton("PONG", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&pongScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);

    brickBrackeButton = new pr32::graphics::ui::UIButton("BRICK BRACKE", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&brickScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    snakeButton = new pr32::graphics::ui::UIButton("SNAKE", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&snakeScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    spaceInvadersButton = new pr32::graphics::ui::UIButton("SPACE INVADERS", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&spaceInvadersScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    tttButton = new pr32::graphics::ui::UIButton("TIC TAC TOE", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&tttScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
}

void MenuScene::setupUIElementsMenu() {
    float btnW = menu::BTN_WIDTH;
    float btnH = menu::BTN_HEIGHT;

    buttonsButton = new pr32::graphics::ui::UIButton("BUTTONS", menu::BTN_SELECT, 0, 0, btnW, btnH, [this]() {
        showMenu(MenuState::BUTTONS);
        engine.setScene(&buttonScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    labelsButton = new pr32::graphics::ui::UIButton("LABELS", menu::BTN_SELECT, 0, 0, btnW, btnH, [this]() {
        showMenu(MenuState::LABELS);
        engine.setScene(&labelScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    checkboxesButton = new pr32::graphics::ui::UIButton("CHECKBOXES", menu::BTN_SELECT, 0, 0, btnW, btnH, [this]() {
        showMenu(MenuState::CHECKBOXES);
        engine.setScene(&checkBoxScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    layoutsButton = new pr32::graphics::ui::UIButton("LAYOUTS", menu::BTN_SELECT, 0, 0, btnW, btnH, [this]() {
        showMenu(MenuState::LAYOUTS);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
}

void MenuScene::setupLayoutsMenu() {
    float btnW = menu::BTN_WIDTH;
    float btnH = menu::BTN_HEIGHT;
    
    verticalLayoutButton = new pr32::graphics::ui::UIButton("VERTICAL LAYOUT", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&verticalLayoutDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    horizontalLayoutButton = new pr32::graphics::ui::UIButton("HORIZONTAL LAYOUT", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&horizontalLayoutDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    gridLayoutButton = new pr32::graphics::ui::UIButton("GRID LAYOUT", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&gridLayoutDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
    
    anchorLayoutButton = new pr32::graphics::ui::UIButton("ANCHOR LAYOUT", menu::BTN_SELECT, 0, 0, btnW, btnH, []() {
        engine.setScene(&anchorLayoutDemoScene);
    }, pr32::graphics::ui::TextAlignment::CENTER, menu::BTN_FONT_SIZE);
}

void MenuScene::showMenu(MenuState state) {
    currentState = state;
    
    // Clear all elements from layout
    buttonLayout->clearElements();
    
    // Update title
    int screenWidth = engine.getRenderer().getWidth();
    
    switch (state) {
        case MenuState::MAIN:
            titleLabel->setText("Main");
            buttonLayout->addElement(gamesButton);
            buttonLayout->addElement(cameraDemoButton);
#ifdef PIXELROOT32_ENABLE_4BPP_SPRITES
            buttonLayout->addElement(spritesDemoButton);
            buttonLayout->addElement(tileMapDemoButton);
#endif
            buttonLayout->addElement(fontTestButton);
            buttonLayout->addElement(dualPaletteTestButton);
            buttonLayout->addElement(uiElementsButton);
            break;
            
        case MenuState::GAMES:
            titleLabel->setText("Games");
            buttonLayout->addElement(pongButton);
            buttonLayout->addElement(brickBrackeButton);
            buttonLayout->addElement(snakeButton);
            buttonLayout->addElement(spaceInvadersButton);
            buttonLayout->addElement(tttButton);
            break;
        
        case MenuState::UIELEMENTS: 
            titleLabel->setText("UIElements");
            buttonLayout->addElement(buttonsButton);
            buttonLayout->addElement(labelsButton);
            buttonLayout->addElement(checkboxesButton);
            buttonLayout->addElement(layoutsButton);
            break;
            
        case MenuState::LAYOUTS:
            titleLabel->setText("Layouts");
            buttonLayout->addElement(verticalLayoutButton);
            buttonLayout->addElement(horizontalLayoutButton);
            buttonLayout->addElement(gridLayoutButton);
            buttonLayout->addElement(anchorLayoutButton);
            break;
    }
    
    titleLabel->centerX(screenWidth);
}

void MenuScene::goBack() {
    switch (currentState) {
        case MenuState::MAIN:
            // Already at main menu, nothing to do
            break;
        case MenuState::GAMES:
            showMenu(MenuState::MAIN);
            break;
        case MenuState::UIELEMENTS:
            showMenu(MenuState::MAIN);
            break;
        case MenuState::BUTTONS:
        case MenuState::LABELS:
        case MenuState::CHECKBOXES:
        case MenuState::LAYOUTS:
            showMenu(MenuState::UIELEMENTS);
            break;

    }
}
