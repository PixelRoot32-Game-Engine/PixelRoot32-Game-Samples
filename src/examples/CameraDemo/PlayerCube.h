#pragma once
#include "core/PhysicsActor.h"
#include "graphics/Renderer.h"

namespace camerademo {

struct PlatformRect {
    float x;
    float y;
    float w;
    float h;
};

class PlayerCube : public pixelroot32::core::PhysicsActor {
public:
    PlayerCube(float x,
               float y,
               float w,
               float h);

    using pixelroot32::core::PhysicsActor::update;

    void setInput(float dir, bool jumpPressed);

    pixelroot32::core::Rect getHitBox();

    void update(unsigned long deltaTime,
                const PlatformRect* platforms,
                int platformCount);

    void draw(pixelroot32::graphics::Renderer& renderer);

    void reset(float newX, float newY);

private:
    float gravity;
    float moveSpeed;
    float jumpVelocity;

    float moveDir;
    bool wantsJump;
    bool onGround;
};

}
