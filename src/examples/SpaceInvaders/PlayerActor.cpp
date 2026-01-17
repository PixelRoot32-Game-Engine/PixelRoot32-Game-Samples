#include "PlayerActor.h"
#include "GameConstants.h"
#include "core/Engine.h"

namespace pr32 = pixelroot32;
extern pr32::core::Engine engine;

namespace spaceinvaders {

PlayerActor::PlayerActor(float x, float y)
    : PhysicsActor(x, y, PLAYER_WIDTH, PLAYER_HEIGHT), isAlive(true) {
    // Set collision layer (example: Layer 1 for Player)
    // We assume standard layers are available or we define them.
    // For now, we'll just set it. 
    // TODO: Define layers properly in GameConstants or use Engine defaults.
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

void PlayerActor::draw(pr32::graphics::Renderer& renderer) {
    if (!isAlive) return;
    
    // Simple green cannon
    using Color = pr32::graphics::Color;
    renderer.drawFilledRectangle((int)x, (int)y + 4, (int)width, 4, Color::Green); // Base
    renderer.drawFilledRectangle((int)x + 4, (int)y, 3, 4, Color::Green); // Turret
}

pr32::core::Rect PlayerActor::getHitBox() {
    return {x, y, (int)width, (int)height};
}

void PlayerActor::onCollision(pr32::core::Actor* other) {
    // Handle collision (e.g., with enemy projectile)
}

}
