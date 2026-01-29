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

// Debug: draw red rectangles over platform hitboxes. Comment out to disable.
// #define METROIDVANIA_DEBUG_PLATFORM_HITBOXES

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

class DetailsLayerEntity : public pr32::core::Entity {
public:
    DetailsLayerEntity()
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
        renderer.drawTileMap(metroidvaniasceneonetilemap::details, static_cast<int>(x), static_cast<int>(y));
    }
};

class StairsLayerEntity : public pr32::core::Entity {
public:
    StairsLayerEntity()
        : pr32::core::Entity(0.0f, 0.0f,
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_WIDTH * metroidvaniasceneonetilemap::TILE_SIZE),
                             static_cast<float>(metroidvaniasceneonetilemap::MAP_HEIGHT * metroidvaniasceneonetilemap::TILE_SIZE),
                             pr32::core::EntityType::GENERIC) {
        setRenderLayer(3);
        
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
    // Sprite palette with original RGB565 from export (idle/run/jump)
    pr32::graphics::setSpriteCustomPalette(metroidvania::PLAYER_SPRITE_PALETTE_RGB565);

    addEntity(new metroidvania::BackgroundLayerEntity());
    addEntity(new metroidvania::PlatformsLayerEntity());
    addEntity(new metroidvania::DetailsLayerEntity());
    addEntity(new metroidvania::StairsLayerEntity());

    player = new metroidvania::PlayerActor(PLAYER_START_X, PLAYER_START_Y);
    addEntity(player);

    // Build platform rect list from tilemap for collision.
    // The engine CollisionSystem is Actor-vs-Actor only; the platform layer is a tilemap, not Actors,
    // so we resolve platform collision manually. Collision layer/mask on the player are for Actor-vs-Actor (e.g. enemies).
    static metroidvania::PlatformRect platformRects[MAX_PLATFORM_RECTS];
    int platformCount = 0;
    const int tw = metroidvaniasceneonetilemap::TILE_SIZE;
    const int mw = metroidvaniasceneonetilemap::MAP_WIDTH;
    const int mh = metroidvaniasceneonetilemap::MAP_HEIGHT;
    for (int row = 0; row < mh && platformCount < MAX_PLATFORM_RECTS; ++row) {
        for (int col = 0; col < mw && platformCount < MAX_PLATFORM_RECTS; ++col) {
            int idx = col + row * mw;
            if (metroidvaniasceneonetilemap::platforms.indices[idx] != 0) {
                platformRects[platformCount].x = static_cast<float>(col * tw);
                platformRects[platformCount].y = static_cast<float>(row * tw);
                platformRects[platformCount].w = static_cast<float>(tw);
                platformRects[platformCount].h = static_cast<float>(tw);
                ++platformCount;
            }
        }
    }
    player->setPlatforms(platformRects, platformCount);
}

void MetroidvaniaScene::update(unsigned long deltaTime) {
    auto& input = engine.getInputManager();
    float moveDir = 0.0f;
    if (input.isButtonDown(3)) moveDir += 1.0f;  // Right
    if (input.isButtonDown(2)) moveDir -= 1.0f;  // Left
    bool jumpPressed = input.isButtonPressed(4);  // Jump (A/Space)
    if (player) player->setInput(moveDir, jumpPressed);
    pixelroot32::core::Scene::update(deltaTime);
}

void MetroidvaniaScene::draw(pr32::graphics::Renderer& renderer) {
    pixelroot32::core::Scene::draw(renderer);
#ifdef METROIDVANIA_DEBUG_PLATFORM_HITBOXES
    if (player) player->drawDebugPlatformHitboxes(renderer);
#endif
}

}
