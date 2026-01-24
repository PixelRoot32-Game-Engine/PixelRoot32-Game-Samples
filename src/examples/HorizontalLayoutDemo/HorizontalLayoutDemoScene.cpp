#include "HorizontalLayoutDemoScene.h"
#include "Menu/MenuScene.h"
#include "core/Engine.h"
#include "graphics/Color.h"
#include "graphics/Renderer.h"

namespace pr32 = pixelroot32;
using Color = pr32::graphics::Color;

extern pr32::core::Engine engine;
extern MenuScene menuScene;

namespace horizontallayoutdemo {

class HorizontalLayoutDemoScene::DemoBackground : public pr32::core::Entity {
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

HorizontalLayoutDemoScene::HorizontalLayoutDemoScene() 
    : background(nullptr), horizontalLayout(nullptr), titleLabel(nullptr), instructionLabel(nullptr) {
}

HorizontalLayoutDemoScene::~HorizontalLayoutDemoScene() {
}

void HorizontalLayoutDemoScene::init() {
    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    
    background = new DemoBackground();
    addEntity(background);
    
    // Title
    titleLabel = new pr32::graphics::ui::UILabel("Horizontal Layout", 0, 10, Color::White, 2);
    titleLabel->centerX(DISPLAY_WIDTH);
    addEntity(titleLabel);

    // Horizontal Layout
    float layoutY = 60.0f;
    float layoutHeight = 100.0f;
    horizontalLayout = new pr32::graphics::ui::UIHorizontalLayout(10, layoutY, DISPLAY_WIDTH - 20, layoutHeight);
    horizontalLayout->setPadding(10);
    horizontalLayout->setSpacing(15);
    horizontalLayout->setScrollEnabled(true);
    addEntity(horizontalLayout);

    // Add some labels to the horizontal layout
    for (int i = 1; i <= 6; ++i) {
        std::string labelText = "Label " + std::to_string(i);
        pr32::graphics::ui::UILabel* label = new pr32::graphics::ui::UILabel(labelText, 0, 0, Color::Cyan, 1);
        horizontalLayout->addElement(label);
    }

    // Instructions
    instructionLabel = new pr32::graphics::ui::UILabel("LEFT/RIGHT: Scroll", 0, DISPLAY_HEIGHT - 30, Color::Yellow, 1);
    instructionLabel->centerX(DISPLAY_WIDTH);
    addEntity(instructionLabel);
}

void HorizontalLayoutDemoScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);

    auto& input = engine.getInputManager();

    // Handle horizontal layout input (LEFT/RIGHT scroll)
    if (horizontalLayout) {
        horizontalLayout->handleInput(input);
    }

    // Handle back button (B button, typically button 5)
    static bool wasBackPressed = false;
    bool isBackPressed = input.isButtonPressed(5); // B button
    if (isBackPressed && !wasBackPressed) {
        engine.setScene(&menuScene);
    }
    wasBackPressed = isBackPressed;
}

void HorizontalLayoutDemoScene::draw(pixelroot32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}

}
