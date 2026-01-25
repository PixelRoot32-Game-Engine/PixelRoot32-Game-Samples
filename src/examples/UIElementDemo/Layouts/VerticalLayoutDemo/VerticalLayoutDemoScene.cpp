#include "VerticalLayoutDemoScene.h"
#include "Menu/MenuScene.h"
#include "core/Engine.h"
#include "graphics/Color.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UIButton.h"

namespace pr32 = pixelroot32;
using Color = pr32::graphics::Color;

extern pr32::core::Engine engine;
extern MenuScene menuScene;

namespace verticallayoutdemo {

class VerticalLayoutDemoScene::DemoBackground : public pr32::core::Entity {
public:
    DemoBackground()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);
    }
};

VerticalLayoutDemoScene::VerticalLayoutDemoScene() 
    : background(nullptr), verticalLayout(nullptr), titleLabel(nullptr), instructionLabel(nullptr) {
}

VerticalLayoutDemoScene::~VerticalLayoutDemoScene() {
}

void VerticalLayoutDemoScene::init() {
    // Clear any existing entities to prevent duplicates if init() is called multiple times
    clearEntities();

    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    
    background = new DemoBackground();
    addEntity(background);
    
    // Title
    titleLabel = new pr32::graphics::ui::UILabel("Vertical Layout", 0, 10, Color::White, 2);
    titleLabel->centerX(DISPLAY_WIDTH);
    addEntity(titleLabel);

    // Vertical Layout
    float layoutWidth = 200.0f;
    float layoutHeight = 150.0f;
    float layoutX = (DISPLAY_WIDTH - layoutWidth) / 2.0f;
    float layoutY = 40.0f;
    
    verticalLayout = new pr32::graphics::ui::UIVerticalLayout(layoutX, layoutY, layoutWidth, layoutHeight);
    verticalLayout->setPadding(10);
    verticalLayout->setSpacing(15);
    verticalLayout->setScrollEnabled(true); // Enable scroll to test vertical scrolling
    verticalLayout->setNavigationButtons(0, 1);
    verticalLayout->setButtonStyle(Color::White, Color::Cyan, Color::White, Color::Black);
    addEntity(verticalLayout);

    // Add some labels to the vertical layout (Testing Safe Mixed Layout Support)
    for (int i = 1; i <= 3; ++i) {
        std::string labelText = "Label " + std::to_string(i);
        // Now it is safe to use UILabel in UIVerticalLayout
        pr32::graphics::ui::UILabel* label = new pr32::graphics::ui::UILabel(labelText, 0, 0, Color::Green, 1);
        verticalLayout->addElement(label);
    }
    
    // Add a button in the middle to test mixing types
    pr32::graphics::ui::UIButton* btn = new pr32::graphics::ui::UIButton("BUTTON", 0, 0, 0, 100, 20, [](){}, pr32::graphics::ui::TextAlignment::CENTER, 1);
    btn->setStyle(Color::White, Color::DarkRed, true);
    verticalLayout->addElement(btn);
    
    // Add more labels
    for (int i = 4; i <= 6; ++i) {
        std::string labelText = "Label " + std::to_string(i);
        pr32::graphics::ui::UILabel* label = new pr32::graphics::ui::UILabel(labelText, 0, 0, Color::Orange, 1);
        verticalLayout->addElement(label);
    }

    // Instructions
    instructionLabel = new pr32::graphics::ui::UILabel("UP/DOWN: Scroll", 0, DISPLAY_HEIGHT - 20, Color::Yellow, 1);
    instructionLabel->centerX(DISPLAY_WIDTH);
    addEntity(instructionLabel);
}

void VerticalLayoutDemoScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);

    auto& input = engine.getInputManager();

    // Handle vertical layout input (UP/DOWN scroll)
    verticalLayout->handleInput(input);

    // Handle back button (B button, typically button 5)
    static bool wasBackPressed = false;
    bool isBackPressed = input.isButtonPressed(5); // B button
    if (isBackPressed && !wasBackPressed) {
        engine.setScene(&menuScene);
    }
    wasBackPressed = isBackPressed;
}

void VerticalLayoutDemoScene::draw(pixelroot32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}

}
