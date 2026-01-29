#include "MetroidvaniaScene.h"
#include "PlayerActor.h"
#include "GameConstants.h"
#include "core/Engine.h"
#include "input/InputManager.h"
#include "graphics/Renderer.h"
#include "graphics/Color.h"
#include "assets/MetroidvaniaSceneOneTileMap.h"
#include "assets/PlayerPalette.h"
#include <cstdint>

namespace {
    constexpr int MAX_PLATFORM_RECTS = metroidvaniasceneonetilemap::MAP_WIDTH * metroidvaniasceneonetilemap::MAP_HEIGHT;
}

extern pixelroot32::core::Engine engine;

namespace pr32 = pixelroot32;

namespace metroidvania {

using pr32::graphics::Color;

class BackgroundLayerEntity : public pr32::core::Entity {
public:
    BackgroundLayerEntity()
        : pr32::core::Entity(0.0f, 0.0f,
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_WIDTH * metroidvaniasceneonetilemap::TILE_SIZE),
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_HEIGHT * metroidvaniasceneonetilemap::TILE_SIZE),
                             pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);

        x = 0.0f;
        y = 0.0f;
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawTileMap(metroidvaniasceneonetilemap::background, static_cast<int>(x), static_cast<int>(y));
    }
};

class PlatformsLayerEntity : public pr32::core::Entity {
public:
    PlatformsLayerEntity()
        : pr32::core::Entity(0.0f, 0.0f,
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_WIDTH * metroidvaniasceneonetilemap::TILE_SIZE),
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_HEIGHT * metroidvaniasceneonetilemap::TILE_SIZE),
                             pr32::core::EntityType::GENERIC) {
        setRenderLayer(1);
        
        x = 0.0f;
        y = 0.0f;
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawTileMap(metroidvaniasceneonetilemap::platforms, static_cast<int>(x), static_cast<int>(y));
    }
};

class StairsLayerEntity : public pr32::core::Entity {
public:
    StairsLayerEntity()
        : pr32::core::Entity(0.0f, 0.0f,
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_WIDTH * metroidvaniasceneonetilemap::TILE_SIZE),
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_HEIGHT * metroidvaniasceneonetilemap::TILE_SIZE),
                             pr32::core::EntityType::GENERIC) {
        setRenderLayer(2);
        
        x = 0.0f;
        y = 0.0f;
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawTileMap(metroidvaniasceneonetilemap::stairs, static_cast<int>(x), static_cast<int>(y));
    }
};

void MetroidvaniaScene::init() {
    metroidvaniasceneonetilemap::init();
    
    // Set global sprite palette.
    // In this engine, 4bpp sprites use an indexed palette.
    pr32::graphics::setSpriteCustomPalette(metroidvania::PLAYER_SPRITE_PALETTE_RGB565);

    // Map layers
    addEntity(new metroidvania::BackgroundLayerEntity());
    addEntity(new metroidvania::PlatformsLayerEntity());
    addEntity(new metroidvania::StairsLayerEntity());

    // Create and add the player.
    player = new metroidvania::PlayerActor(PLAYER_START_X, PLAYER_START_Y);
    addEntity(player);

    // Pass stairs layer data to the player for internal logic.
    player->setStairs(metroidvaniasceneonetilemap::stairs.indices, 
                      metroidvaniasceneonetilemap::MAP_WIDTH, 
                      metroidvaniasceneonetilemap::MAP_HEIGHT, 
                      metroidvaniasceneonetilemap::TILE_SIZE);

    // Pass platform layer data to the player for optimized tile-based collision.
    player->setPlatformTiles(metroidvaniasceneonetilemap::platforms.indices,
                             metroidvaniasceneonetilemap::MAP_WIDTH,
                             metroidvaniasceneonetilemap::MAP_HEIGHT,
                             metroidvaniasceneonetilemap::TILE_SIZE);
}

void MetroidvaniaScene::update(unsigned long deltaTime) {
    auto& input = engine.getInputManager();
    
    // Input management: map buttons to directions.
    float moveDir = 0.0f;
    if (input.isButtonDown(3)) moveDir += 1.0f;  // Right
    if (input.isButtonDown(2)) moveDir -= 1.0f;  // Left

    float vDir = 0.0f;
    if (input.isButtonDown(0)) vDir -= 1.0f;     // Up
    if (input.isButtonDown(1)) vDir += 1.0f;     // Down

    bool jumpPressed = input.isButtonPressed(4);  // Jump (A / Space)
    
    // Send processed inputs to the player actor.
    if (player) player->setInput(moveDir, vDir, jumpPressed);

    // Update all entities in the scene.
    pixelroot32::core::Scene::update(deltaTime);
}

void MetroidvaniaScene::draw(pr32::graphics::Renderer& renderer) {
    pixelroot32::core::Scene::draw(renderer);
}

}
