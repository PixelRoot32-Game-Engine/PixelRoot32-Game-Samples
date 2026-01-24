#include "GridLayoutDemoScene.h"
#include "Menu/MenuScene.h"
#include "core/Engine.h"
#include "graphics/Color.h"
#include "graphics/Renderer.h"

namespace pr32 = pixelroot32;
using Color = pr32::graphics::Color;

extern pr32::core::Engine engine;
extern MenuScene menuScene;

namespace gridlayoutdemo {

class GridLayoutDemoScene::DemoBackground : public pr32::core::Entity {
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

GridLayoutDemoScene::GridLayoutDemoScene() 
    : background(nullptr), gridLayout(nullptr), paddingContainer(nullptr), 
      titleLabel(nullptr), instructionLabel(nullptr), infoLabel(nullptr) {
}

GridLayoutDemoScene::~GridLayoutDemoScene() {
}

void GridLayoutDemoScene::init() {
    clearEntities();
    
    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    
    background = new DemoBackground();
    addEntity(background);
    
    // Title
    titleLabel = new pr32::graphics::ui::UILabel("Grid Layout Demo", 0, 8, Color::White, 2);
    titleLabel->centerX(DISPLAY_WIDTH);
    addEntity(titleLabel);

    // Create a PaddingContainer to wrap the GridLayout
    float containerX = 10.0f;
    float containerY = 35.0f;
    float containerW = DISPLAY_WIDTH - 20.0f;
    float containerH = 160.0f;
    
    paddingContainer = new pr32::graphics::ui::UIPaddingContainer(containerX, containerY, containerW, containerH);
    paddingContainer->setPadding(5); // 5px padding relative to the container
    // We can also set asymmetric padding: paddingContainer->setPadding(5, 5, 10, 10);
    addEntity(paddingContainer);

    // Create GridLayout inside the container
    // Note: The position (0,0) and size (containerW, containerH) will be adjusted by PaddingContainer logic 
    // if we added logic to resize child, but currently PaddingContainer just offsets.
    // So we should size the grid to fit inside the padded area if we want it precise, 
    // or rely on the grid positioning itself?
    // Let's size it to fill the padded area.
    float gridW = containerW - 10.0f; // containerW - padding*2
    float gridH = containerH - 10.0f; // containerH - padding*2
    
    gridLayout = new pr32::graphics::ui::UIGridLayout(0, 0, gridW, gridH);
    gridLayout->setColumns(3);
    gridLayout->setPadding(2);
    gridLayout->setSpacing(4);
    // Since we are inside a container, position is relative to it? 
    // No, currently UI system uses absolute positions. 
    // UIPaddingContainer::setChild() typically just calls setPosition on the child.
    // So we don't need to manually set X/Y here if we use setChild correctly.
    
    paddingContainer->setChild(gridLayout);
    
    // Populate Grid
    int totalItems = 9;
    for (int i = 0; i < totalItems; ++i) {
        if (i == 0) {
            // Center element is a Label to test Type Safety
            pr32::graphics::ui::UILabel* label = new pr32::graphics::ui::UILabel("CTR", 0, 0, Color::Yellow, 1);
            gridLayout->addElement(label);
        } else {
            // Others are Buttons
            std::string text = "Btn " + std::to_string(i + 1);
            pr32::graphics::ui::UIButton* btn = new pr32::graphics::ui::UIButton(
                text, 0, 0, 0, 50, 30, // Pos/Size handled by layout
                [this, i]() {
                    // Button action
                    infoLabel->setText("Clicked: " + std::to_string(i + 1));
                    infoLabel->centerX(DISPLAY_WIDTH);
                },
                pr32::graphics::ui::TextAlignment::CENTER, 1
            );
            btn->setStyle(Color::White, Color::Blue, true);
            gridLayout->addElement(btn);
        }
    }
    
    // Explicitly add grid to entities AFTER adding to container might not be needed if container manages it?
    // Currently Engine requires all entities to be added to Scene to receive update/draw.
    // UIPaddingContainer does NOT implement draw/update for its child automatically unless we subclass strictly.
    // Looking at UIPaddingContainer source (assumed), usually it's just a positioner. 
    // But since it's an Entity, maybe it delegates?
    // Safer to add gridLayout to entities as well, but maybe with higher render layer?
    // Actually, Scene::draw calls draw on all entities.
    // If UIPaddingContainer draws its child, adding both will double draw.
    // Standard pattern: Add both, but maybe child is part of hierarchy?
    // Let's assume we need to add it.
    addEntity(gridLayout);

    // Info Label
    infoLabel = new pr32::graphics::ui::UILabel("Select an item", 0, 205, Color::Cyan, 1);
    infoLabel->centerX(DISPLAY_WIDTH);
    addEntity(infoLabel);

    // Instructions
    instructionLabel = new pr32::graphics::ui::UILabel("D-PAD: Navigate | A: Select", 0, 225, Color::Gray, 1);
    instructionLabel->centerX(DISPLAY_WIDTH);
    addEntity(instructionLabel);
}

void GridLayoutDemoScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);

    auto& input = engine.getInputManager();

    // Handle grid input
    if (gridLayout) {
        gridLayout->handleInput(input);
    }

    // Handle back button
    static bool wasBackPressed = false;
    if (input.isButtonPressed(5) && !wasBackPressed) {
        engine.setScene(&menuScene);
    }
    wasBackPressed = input.isButtonPressed(5);
}

void GridLayoutDemoScene::draw(pixelroot32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
    
    // Draw container border for visualization
    if (paddingContainer) {
        renderer.drawRectangle(
            static_cast<int>(paddingContainer->x), 
            static_cast<int>(paddingContainer->y), 
            paddingContainer->width, 
            paddingContainer->height, 
            Color::DarkGray
        );
    }
}

}
