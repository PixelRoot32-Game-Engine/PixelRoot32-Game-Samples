#include "ProjectileActor.h"
#include "GameConstants.h"

namespace spaceinvaders {

ProjectileActor::ProjectileActor(float x, float y, ProjectileType type)
    : pixelroot32::core::PhysicsActor(x, y, PROJECTILE_WIDTH, PROJECTILE_HEIGHT), type(type), active(true) {
    
    float speed = (type == ProjectileType::PLAYER_BULLET) ? -PROJECTILE_SPEED : PROJECTILE_SPEED;
    setVelocity(0, speed);
}

void ProjectileActor::update(unsigned long deltaTime) {
    if (!active) return;

    float dt = deltaTime * 0.001f;
    x += vx * dt;
    y += vy * dt;

    if (y < -PROJECTILE_HEIGHT || y > DISPLAY_HEIGHT) {
        active = false;
    }
}

void ProjectileActor::draw(pixelroot32::graphics::Renderer& renderer) {
    if (!active) return;
    
    using Color = pixelroot32::graphics::Color;
    Color c = (type == ProjectileType::PLAYER_BULLET) ? Color::White : Color::Red;
    
    renderer.drawFilledRectangle((int)x, (int)y, (int)width, (int)height, c);
}

pixelroot32::core::Rect ProjectileActor::getHitBox() {
    return {x, y, (int)width, (int)height};
}

void ProjectileActor::onCollision(pixelroot32::core::Actor* other) {
    (void)other;

    // Basic collision handling, deactivate self
    // Logic for destroying other entity should be handled in Scene or via callbacks
    active = false; 
}

}
