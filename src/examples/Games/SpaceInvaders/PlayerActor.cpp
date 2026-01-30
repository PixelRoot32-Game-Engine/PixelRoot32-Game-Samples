#include "PlayerActor.h"
#include "EngineConfig.h"
#include "GameConstants.h"
#include "core/Engine.h"

namespace pr32 = pixelroot32;
extern pr32::core::Engine engine;

namespace spaceinvaders {

using Sprite = pr32::graphics::Sprite;

static const uint16_t PLAYER_SHIP_BITS[] = {
    0x0020,
    0x0070,
    0x00F8,
    0x01FC,
    0x03DE,
    0x03FE,
    0x0124,
    0x0124
};

static const Sprite PLAYER_SHIP_SPRITE = { PLAYER_SHIP_BITS, PLAYER_SPRITE_W, PLAYER_SPRITE_H };

PlayerActor::PlayerActor(float x, float y)
    : PhysicsActor(x, y, PLAYER_WIDTH, PLAYER_HEIGHT), isAlive(true) {
    setWorldSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    setLimits(pr32::core::LimitRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT));
}

void PlayerActor::update(unsigned long deltaTime) {
    handleInput();
    PhysicsActor::update(deltaTime); // Apply velocity and limits
}

void PlayerActor::handleInput() {
    auto& input = engine.getInputManager();
    
    vx = 0; // Reset velocity
    
    if (input.isButtonDown(BTN_LEFT)) {
        vx = -PLAYER_SPEED;
    }
    if (input.isButtonDown(BTN_RIGHT)) {
        vx = PLAYER_SPEED;
    }
}

bool PlayerActor::isFireDown() const {
    auto& input = engine.getInputManager();
    return input.isButtonDown(BTN_FIRE);
}

bool PlayerActor::wantsToShoot() const {
    auto& input = engine.getInputManager();
    return input.isButtonPressed(BTN_FIRE);
}

void PlayerActor::draw(pr32::graphics::Renderer& renderer) {
    if (!isAlive) return;
    
    using Color = pr32::graphics::Color;
    renderer.drawSprite(PLAYER_SHIP_SPRITE,
                        static_cast<int>(x),
                        static_cast<int>(y),
                        SPRITE_SCALE,
                        SPRITE_SCALE,
                        Color::Yellow);
}

pr32::core::Rect PlayerActor::getHitBox() {
    return {x, y, (int)width, (int)height};
}

void PlayerActor::onCollision(pr32::core::Actor* other) {
    (void)other; // Unused parameter
    // Handle collision (e.g., with enemy projectile)
}

}
