#pragma once
#include "core/PhysicsActor.h"
#include "graphics/Renderer.h"

namespace spaceinvaders {

enum class ProjectileType {
    PLAYER_BULLET,
    ENEMY_BULLET
};

class ProjectileActor : public pixelroot32::core::PhysicsActor {
public:
    ProjectileActor(float x, float y, ProjectileType type);
    
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer);
    void reset(float x, float y, ProjectileType type);
    
    pixelroot32::core::Rect getHitBox() override;
    void onCollision(pixelroot32::core::Actor* other) override;
    
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    ProjectileType getType() const { return type; }

private:
    ProjectileType type;
    bool active;
};

}
