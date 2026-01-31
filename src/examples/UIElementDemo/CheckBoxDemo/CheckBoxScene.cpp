#include "CheckBoxScene.h"
#include "core/Engine.h"
#include "EngineConfig.h"
#include "Menu/MenuScene.h"
#include "graphics/Color.h"
#include "graphics/Renderer.h"
#include "graphics/ui/UICheckBox.h"

namespace pr32 = pixelroot32;
using Color = pr32::graphics::Color;

extern pr32::core::Engine engine;
extern MenuScene menuScene;

namespace checkboxdemo {

class CheckBoxScene::DemoBackground : public pr32::core::Entity {
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

CheckBoxScene::CheckBoxScene() 
    : background(nullptr), verticalLayout(nullptr), titleLabel(nullptr), instructionLabel(nullptr) {
}

CheckBoxScene::~CheckBoxScene() {
}

void CheckBoxScene::init() {
    clearEntities();

    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    
    background = new DemoBackground();
    addEntity(background);
    
    // Title
    titleLabel = new pr32::graphics::ui::UILabel("CheckBox Demo", 0, 10, Color::White, 2);
    titleLabel->centerX(DISPLAY_WIDTH);
    addEntity(titleLabel);

    // Vertical Layout
    float layoutWidth = 200.0f;
    float layoutHeight = 160.0f;
    float layoutX = (DISPLAY_WIDTH - layoutWidth) / 2.0f;
    float layoutY = 40.0f;
    
    verticalLayout = new pr32::graphics::ui::UIVerticalLayout(layoutX, layoutY, layoutWidth, layoutHeight);
    verticalLayout->setPadding(10);
    verticalLayout->setSpacing(10);
    verticalLayout->setNavigationButtons(0, 1); // UP/DOWN
    addEntity(verticalLayout);

    // Add 6 checkboxes to the vertical layout
    for (int i = 1; i <= 6; ++i) {
        std::string labelText = "Option " + std::to_string(i);
        // index 4 is usually the "A" or "Confirm" button in the engine's default mapping
        auto* checkBox = new pr32::graphics::ui::UICheckBox(labelText, 4, 0, 0, 180, 20, false, [i](bool checked) {
            (void)checked;
            // Callback logic here
        }, 1);
        verticalLayout->addElement(checkBox);
    }

    // Instructions
    instructionLabel = new pr32::graphics::ui::UILabel("UP/DOWN: Nav  A: Toggle", 0, DISPLAY_HEIGHT - 20, Color::Yellow, 1);
    instructionLabel->centerX(DISPLAY_WIDTH);
    addEntity(instructionLabel);
}

void CheckBoxScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);

    auto& input = engine.getInputManager();

    // Handle vertical layout input (UP/DOWN navigation)
    if (verticalLayout) {
        verticalLayout->handleInput(input);
    }

    // Handle back button (typically button 5 / B) to return to menu if needed
    if (input.isButtonPressed(5)) {
        engine.setScene(&menuScene);
    }
}

void CheckBoxScene::draw(pr32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}

}
