#include "PlayerActor.h"
#include "assets/PlayerIdleSprites.h"
#include "assets/PlayerRunSprites.h"
#include "assets/PlayerJumSprites.h"
#include "assets/PlayerPalette.h"
#include "Config.h"
#include "graphics/Renderer.h"
#include "graphics/Color.h"

namespace metroidvania {

namespace pr32 = pixelroot32;
using pr32::graphics::Sprite4bpp;

static const int NUM_IDLE_FRAMES = 2;
static const int NUM_RUN_FRAMES = 4;
static const int NUM_JUM_FRAMES = 5;

static const Sprite4bpp IDLE_FRAMES[] = {
    { reinterpret_cast<const uint8_t*>(PLAYER_IDLE_SPRITE_0_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_IDLE_SPRITE_1_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
};

static const Sprite4bpp RUN_FRAMES[] = {
    { reinterpret_cast<const uint8_t*>(PLAYER_RUN_SPRITE_0_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_RUN_SPRITE_1_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_RUN_SPRITE_2_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_RUN_SPRITE_3_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
};

static const Sprite4bpp JUM_FRAMES[] = {
    { reinterpret_cast<const uint8_t*>(PLAYER_JUM_SPRITE_0_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_JUM_SPRITE_1_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_JUM_SPRITE_2_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_JUM_SPRITE_3_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
    { reinterpret_cast<const uint8_t*>(PLAYER_JUM_SPRITE_4_4BPP), metroidvania::PLAYER_PALETTE_MAPPING, PLAYER_WIDTH, PLAYER_HEIGHT, 8 },
};

PlayerActor::PlayerActor(float x, float y)
    : PhysicsActor(x, y, static_cast<float>(PLAYER_WIDTH), static_cast<float>(PLAYER_HEIGHT)) {
    setRenderLayer(2);
    setCollisionLayer(Layers::PLAYER);
    setCollisionMask(Layers::ENEMY);  // future: enemies, projectiles, pickups
    setRestitution(0.0f);
    setFriction(0.0f);
}

void PlayerActor::setInput(float dir, bool jumpPressed) {
    moveDir = dir;
    if (jumpPressed && onGround) {
        wantsJump = true;
    }
}

void PlayerActor::setPlatforms(const PlatformRect* list, int count) {
    platforms = list;
    platformCount = count;
}

void PlayerActor::update(unsigned long deltaTime) {
    float dt = deltaTime * 0.001f;
    if (dt > 0.05f) dt = 0.05f;

    vx = moveDir * PLAYER_MOVE_SPEED;
    vy += PLAYER_GRAVITY * dt;

    if (moveDir > 0.0f) facingLeft = false;
    else if (moveDir < 0.0f) facingLeft = true;

    if (wantsJump && onGround) {
        vy = -PLAYER_JUMP_VELOCITY;
        wantsJump = false;
        onGround = false;
    }

    if (worldHeight == 0) {
        setWorldSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }

    // Horizontal movement and collision
    x += vx * dt;
    if (platforms && platformCount > 0) {
        for (int i = 0; i < platformCount; ++i) {
            const PlatformRect& p = platforms[i];
            if (x < p.x + p.w && x + width > p.x && y < p.y + p.h && y + height > p.y) {
                if (vx > 0) x = p.x - width;
                else if (vx < 0) x = p.x + p.w;
                vx = 0;
            }
        }
    }

    // Vertical movement and collision
    y += vy * dt;
    onGround = false;
    if (platforms && platformCount > 0) {
        for (int i = 0; i < platformCount; ++i) {
            const PlatformRect& p = platforms[i];
            if (x < p.x + p.w && x + width > p.x && y < p.y + p.h && y + height > p.y) {
                if (vy > 0) {
                    y = p.y - height;
                    onGround = true;
                }
                else if (vy < 0) {
                    y = p.y + p.h;
                }
                vy = 0;
            }
        }
    }

    resolveWorldBounds();
    if (worldCollisionInfo.bottom) onGround = true;

    PlayerState nextState = currentState;
    switch (currentState) {
        case PlayerState::IDLE:
            if (!onGround) nextState = PlayerState::JUMP;
            else if (moveDir != 0.0f) nextState = PlayerState::RUN;
            break;
        case PlayerState::RUN:
            if (!onGround) nextState = PlayerState::JUMP;
            else if (moveDir == 0.0f) nextState = PlayerState::IDLE;
            break;
        case PlayerState::JUMP:
            if (onGround) {
                nextState = (moveDir != 0.0f) ? PlayerState::RUN : PlayerState::IDLE;
            }
            break;
    }
    if (nextState != currentState) changeState(nextState);

    timeAccumulator += deltaTime;
    while (timeAccumulator >= ANIMATION_FRAME_TIME_MS) {
        timeAccumulator -= ANIMATION_FRAME_TIME_MS;
        ++currentFrame;
        int numFrames = getNumberOfFramesByState();
        if (currentFrame >= numFrames) currentFrame = 0;
    }
}

void PlayerActor::draw(pr32::graphics::Renderer& renderer) {
    const auto& sprite = getSpriteByState();
    renderer.drawSprite(sprite, static_cast<int>(x), static_cast<int>(y), facingLeft);
}

void PlayerActor::drawDebugPlatformHitboxes(pr32::graphics::Renderer& renderer) {
    if (!platforms || platformCount <= 0) return;
    for (int i = 0; i < platformCount; ++i) {
        const auto& p = platforms[i];
        renderer.drawRectangle(
            static_cast<int>(p.x),
            static_cast<int>(p.y),
            static_cast<int>(p.w),
            static_cast<int>(p.h),
            pr32::graphics::Color::Red
        );
    }
}

pr32::core::Rect PlayerActor::getHitBox() {
    return { x, y, width, height };
}

void PlayerActor::onCollision(pr32::core::Actor* other) {
    (void)other;
}

void PlayerActor::changeState(PlayerState newState) {
    if (currentState != newState) {
        currentState = newState;
        currentFrame = 0;
        timeAccumulator = 0;
    }
}

int PlayerActor::getNumberOfFramesByState() const {
    switch (currentState) {
        case PlayerState::IDLE: return NUM_IDLE_FRAMES;
        case PlayerState::RUN:  return NUM_RUN_FRAMES;
        case PlayerState::JUMP: return NUM_JUM_FRAMES;
    }
    return NUM_IDLE_FRAMES;
}

Sprite4bpp PlayerActor::getSpriteByState() const {
    switch (currentState) {
        case PlayerState::IDLE:
            return IDLE_FRAMES[currentFrame < NUM_IDLE_FRAMES ? currentFrame : 0];
        case PlayerState::RUN:
            return RUN_FRAMES[currentFrame < NUM_RUN_FRAMES ? currentFrame : 0];
        case PlayerState::JUMP:
            return JUM_FRAMES[currentFrame < NUM_JUM_FRAMES ? currentFrame : 0];
    }
    return IDLE_FRAMES[0];
}

} // namespace metroidvania
