#include "AnchorLayoutDemoScene.h"
#include "Menu/MenuScene.h"
#include "core/Engine.h"
#include "graphics/Color.h"
#include "graphics/Renderer.h"

namespace pr32 = pixelroot32;
using Color = pr32::graphics::Color;

extern pr32::core::Engine engine;
extern MenuScene menuScene;

namespace anchorlayoutdemo {

class AnchorLayoutDemoScene::DemoBackground : public pr32::core::Entity {
public:
    DemoBackground()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        // Draw a grid pattern to make anchors more obvious
        renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);
        renderer.drawRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::DarkGray);
        renderer.drawLine(DISPLAY_WIDTH / 2, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, Color::DarkGray);
        renderer.drawLine(0, DISPLAY_HEIGHT / 2, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2, Color::DarkGray);
    }
};

AnchorLayoutDemoScene::AnchorLayoutDemoScene() 
    : background(nullptr), anchorLayout(nullptr), 
      scoreLabel(nullptr), livesLabel(nullptr), fpsLabel(nullptr), 
      weaponLabel(nullptr), centerLabel(nullptr), startButton(nullptr) {
}

AnchorLayoutDemoScene::~AnchorLayoutDemoScene() {
}

void AnchorLayoutDemoScene::init() {
    clearEntities();
    
    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    
    background = new DemoBackground();
    addEntity(background);
    
    // Create Anchor Layout covering the whole screen
    anchorLayout = new pr32::graphics::ui::UIAnchorLayout(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    anchorLayout->setScreenSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    addEntity(anchorLayout);
    
    // HUD Elements
    
    // TOP_LEFT
    scoreLabel = new pr32::graphics::ui::UILabel("Score: 1250", 0, 0, Color::White, 1);
    anchorLayout->addElement(scoreLabel, pr32::graphics::ui::Anchor::TOP_LEFT);
    
    // TOP_RIGHT
    livesLabel = new pr32::graphics::ui::UILabel("Lives: 3", 0, 0, Color::Red, 1);
    anchorLayout->addElement(livesLabel, pr32::graphics::ui::Anchor::TOP_RIGHT);
    
    // BOTTOM_LEFT
    fpsLabel = new pr32::graphics::ui::UILabel("FPS: 60", 0, 0, Color::Yellow, 1);
    anchorLayout->addElement(fpsLabel, pr32::graphics::ui::Anchor::BOTTOM_LEFT);
    
    // BOTTOM_RIGHT
    weaponLabel = new pr32::graphics::ui::UILabel("Weapon: Plasmacaster", 0, 0, Color::Cyan, 1);
    anchorLayout->addElement(weaponLabel, pr32::graphics::ui::Anchor::BOTTOM_RIGHT);
    
    // CENTER
    centerLabel = new pr32::graphics::ui::UILabel("PAUSED", 0, 0, Color::Orange, 2);
    // Blink effect handled in update? Or just static.
    anchorLayout->addElement(centerLabel, pr32::graphics::ui::Anchor::CENTER);
    
    // BOTTOM_CENTER (Button)
    startButton = new pr32::graphics::ui::UIButton(
        "RESUME GAME", 0, 0, 0, 100, 20, 
        [this]() {
            centerLabel->setText(centerLabel->getType() == pr32::graphics::ui::UIElement::UIElementType::LABEL ? "RESUME PRESSED" : "ERROR");
            // Re-layout needed if text changes size? Anchor layout usually updates on adding/screen size set. 
            // If element size changes, we might need to trigger layout update.
            // UIAnchorLayout currently doesn't auto-update on child content change unless explicitly told.
            // Ideally setScreenSize again or simple updateLayout.
            anchorLayout->setScreenSize(DISPLAY_WIDTH, DISPLAY_HEIGHT); 
        },
        pr32::graphics::ui::TextAlignment::CENTER, 1
    );
    startButton->setStyle(Color::White, Color::Blue, true);
    startButton->setSelected(true); // Pre-select for demo
    anchorLayout->addElement(startButton, pr32::graphics::ui::Anchor::BOTTOM_CENTER);
    
    // Note: UIAnchorLayout handles positioning, but input for button needs to be passed.
    // If we have multiple buttons we'd need navigation logic, but for this demo only one button is interactive.
}

void AnchorLayoutDemoScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);

    auto& input = engine.getInputManager();

    // Handle button input
    if (startButton) {
        startButton->handleInput(input);
    }

    // Handle back button
    static bool wasBackPressed = false;
    if (input.isButtonPressed(5) && !wasBackPressed) {
        engine.setScene(&menuScene);
    }
    wasBackPressed = input.isButtonPressed(5);
}

void AnchorLayoutDemoScene::draw(pixelroot32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}

}
