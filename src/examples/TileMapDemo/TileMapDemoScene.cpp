#include "TileMapDemoScene.h"
#include "graphics/Renderer.h"
#include "assets/PR32Logo.h"
#include <cstdint>

namespace pr32 = pixelroot32;

namespace tilemapdemo {

using pr32::graphics::Color;

namespace {

class TileMapDemoBackground : public pr32::core::Entity {
public:
    TileMapDemoBackground()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);
    }
    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);
    }
};

class LogoEntity : public pr32::core::Entity {
public:
    LogoEntity()
        : pr32::core::Entity(0.0f, 0.0f, 240.0f, 240.0f, pr32::core::EntityType::GENERIC) {
        setRenderLayer(1);
        
        x = -8.0f;
        y = -8.0f;
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawTileMap(pr32logo::background, static_cast<int>(x), static_cast<int>(y));
    }
};

}

void TileMapDemoScene::init() {
    pr32logo::init();

    addEntity(new TileMapDemoBackground());
    addEntity(new LogoEntity());
}

void TileMapDemoScene::update(unsigned long deltaTime) {
    pixelroot32::core::Scene::update(deltaTime);
}

void TileMapDemoScene::draw(pr32::graphics::Renderer& renderer) {
    pixelroot32::core::Scene::draw(renderer);
}

}
