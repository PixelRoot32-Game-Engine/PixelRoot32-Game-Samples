#include "LabelScene.h"
#include "core/Engine.h"
#include "Menu/MenuScene.h"
#include "graphics/Color.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UILabel.h"

namespace pr32 = pixelroot32;
using Color = pr32::graphics::Color;

extern pr32::core::Engine engine;
extern MenuScene menuScene;

namespace labeldemo {

class LabelScene::DemoBackground : public pr32::core::Entity {
public:
    DemoBackground()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {}

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);
    }
};

LabelScene::LabelScene() 
    : background(nullptr), verticalLayout(nullptr), titleLabel(nullptr), instructionLabel(nullptr) {
}

LabelScene::~LabelScene() {
}

void LabelScene::init() {
    clearEntities();

    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    
    background = new DemoBackground();
    addEntity(background);
    
    // Title
    titleLabel = new pr32::graphics::ui::UILabel("Label Demo", 0, 10, Color::White, 2);
    titleLabel->centerX(DISPLAY_WIDTH);
    addEntity(titleLabel);

    // Vertical Layout
    float layoutWidth = 160.0f;
    float layoutHeight = 160.0f;
    float layoutX = (DISPLAY_WIDTH - layoutWidth) / 2.0f;
    float layoutY = 40.0f;
    
    verticalLayout = new pr32::graphics::ui::UIVerticalLayout(layoutX, layoutY, layoutWidth, layoutHeight);
    verticalLayout->setPadding(10);
    verticalLayout->setSpacing(10);
    // Labels are typically not focusable, so navigation might not be visible 
    // but the layout can still contain them.
    addEntity(verticalLayout);

    // Add 5 labels to the vertical layout
    Color colors[] = { Color::Red, Color::Green, Color::Blue, Color::Yellow, Color::Cyan };
    for (int i = 1; i <= 5; ++i) {
        std::string labelText = "Label Number " + std::to_string(i);
        auto* label = new pr32::graphics::ui::UILabel(labelText, 0, 0, colors[i-1], 1);
        verticalLayout->addElement(label);
    }

    // Instructions
    instructionLabel = new pr32::graphics::ui::UILabel("Static Labels in Layout", 0, DISPLAY_HEIGHT - 20, Color::White, 1);
    instructionLabel->centerX(DISPLAY_WIDTH);
    addEntity(instructionLabel);
}

void LabelScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);

    auto& input = engine.getInputManager();

    // Handle back button (typically button 5 / B) to return to menu if needed
    if (input.isButtonPressed(5)) {
        engine.setScene(&menuScene);
    }
}

void LabelScene::draw(pr32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}

}
