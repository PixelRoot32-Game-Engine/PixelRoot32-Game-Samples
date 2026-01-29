#pragma once
#include "core/PhysicsActor.h"
#include "GameConstants.h"
#include "GameLayers.h"

namespace metroidvania {

enum class PlayerState {
    IDLE,
    RUN,
    JUMP
};

class PlayerActor : public pixelroot32::core::PhysicsActor {
public:
    PlayerActor(float x, float y);

    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

    pixelroot32::core::Rect getHitBox() override;
    void onCollision(pixelroot32::core::Actor* other) override;

    void setInput(float dir, bool jumpPressed);
    void setPlatforms(const PlatformRect* list, int count);

    /** Debug only: draws red rectangles over each platform hitbox. Remove when not needed. */
    void drawDebugPlatformHitboxes(pixelroot32::graphics::Renderer& renderer);

private:
    unsigned long timeAccumulator = 0;
    uint8_t currentFrame = 0;
    PlayerState currentState = PlayerState::IDLE;

    float moveDir = 0.0f;
    bool wantsJump = false;
    bool onGround = false;
    bool facingLeft = false;
    const PlatformRect* platforms = nullptr;
    int platformCount = 0;

    int getNumberOfFramesByState() const;
    pixelroot32::graphics::Sprite4bpp getSpriteByState() const;
    void changeState(PlayerState newState);
};

} // namespace metroidvania
