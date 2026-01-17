#pragma once
#include "core/PhysicsActor.h"
#include "graphics/Renderer.h"

namespace spaceinvaders {

class PlayerActor : public pixelroot32::core::PhysicsActor {
public:
    PlayerActor(float x, float y);
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer);
    
    // PhysicsActor overrides
    pixelroot32::core::Rect getHitBox() override;
    void onCollision(pixelroot32::core::Actor* other) override;

private:
    void handleInput();
    bool isAlive;
};

}
