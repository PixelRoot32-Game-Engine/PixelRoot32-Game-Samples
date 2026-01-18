#include "PlayerCube.h"
#include "GameConstants.h"

namespace pr32 = pixelroot32;

namespace camerademo {

using pr32::graphics::Color;

PlayerCube::PlayerCube(float x,
                       float y,
                       float w,
                       float h)
    : PhysicsActor(x, y, w, h)
    , gravity(PLAYER_GRAVITY)
    , moveSpeed(PLAYER_MOVE_SPEED)
    , jumpVelocity(PLAYER_JUMP_VELOCITY)
    , moveDir(0.0f)
    , wantsJump(false)
    , onGround(false) {
    setRestitution(0.0f);
    setFriction(0.0f);
}

void PlayerCube::setInput(float dir, bool jumpPressed) {
    moveDir = dir;
    if (jumpPressed && onGround) {
        wantsJump = true;
    }
}

pr32::core::Rect PlayerCube::getHitBox() {
    pr32::core::Rect r;
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    return r;
}

void PlayerCube::update(unsigned long deltaTime,
                        const PlatformRect* platforms,
                        int platformCount) {
    float dt = deltaTime * 0.001f;

    float prevY = y;

    vx = moveDir * moveSpeed;
    vy += gravity * dt;

    if (wantsJump && onGround) {
        vy = -jumpVelocity;
        wantsJump = false;
        onGround = false;
    }

    integrate(dt);
    resolveWorldBounds();

    onGround = worldCollisionInfo.bottom;

    float bottomPrev = prevY + static_cast<float>(height);
    float bottomNow = y + static_cast<float>(height);

    if (vy >= 0.0f && platforms && platformCount > 0) {
        for (int i = 0; i < platformCount; ++i) {
            const PlatformRect& p = platforms[i];

            float px0 = x;
            float px1 = x + static_cast<float>(width);
            float platLeft = p.x;
            float platRight = p.x + p.w;

            bool horizontalOverlap = px1 > platLeft && px0 < platRight;
            if (!horizontalOverlap) {
                continue;
            }

            float platTop = p.y;

            if (bottomPrev <= platTop && bottomNow >= platTop) {
                y = platTop - static_cast<float>(height);
                vy = 0.0f;
                onGround = true;
                break;
            }
        }
    }
}

void PlayerCube::draw(pr32::graphics::Renderer& renderer) {
    renderer.drawFilledRectangle(static_cast<int>(x),
                                 static_cast<int>(y),
                                 width,
                                 height,
                                 Color::Cyan);
}

void PlayerCube::reset(float newX, float newY) {
    x = newX;
    y = newY;
    vx = 0.0f;
    vy = 0.0f;
    moveDir = 0.0f;
    wantsJump = false;
    onGround = false;
}

}

