#pragma once
#include "core/PhysicsActor.h"
#include "GameConstants.h"
#include "GameLayers.h"

namespace metroidvania {

/**
 * @brief Possible player states.
 * IDLE: Not moving.
 * RUN: Moving horizontally on ground.
 * JUMP: In the air (jumping or falling).
 * CLIMBING: Moving up or down ladders.
 */
enum class PlayerState {
    IDLE,
    RUN,
    JUMP,
    CLIMBING
};

/**
 * @brief PlayerActor class representing the main protagonist.
 * Inherits from PhysicsActor to utilize basic physics properties.
 */
class PlayerActor : public pixelroot32::core::PhysicsActor {
public:
    PlayerActor(float x, float y);

    /** @brief Updates player logic every frame. */
    void update(unsigned long deltaTime) override;

    /** @brief Draws the current state sprite. */
    void draw(pixelroot32::graphics::Renderer& renderer) override;

    /** @brief Defines the player's collision area. */
    pixelroot32::core::Rect getHitBox() override;

    /** @brief Handles collisions with other actors (enemies, items, etc). */
    void onCollision(pixelroot32::core::Actor* other) override;

    /** @brief Updates input state received from the scene. */
    void setInput(float dir, float vDir, bool jumpPressed);

    /** @brief Assigns the list of platform collision rectangles. */
    void setPlatforms(const PlatformRect* list, int count);

    /** @brief Assigns stairs layer data from the map. */
    void setStairs(const uint8_t* indices, int width, int height, int tileSize);

    /** @brief Debug utility to visualize platform hitboxes. */
    void drawDebugPlatformHitboxes(pixelroot32::graphics::Renderer& renderer);

private:
    unsigned long timeAccumulator = 0;   // Accumulator for animation timing
    uint8_t currentFrame = 0;           // Current animation frame
    PlayerState currentState = PlayerState::IDLE;

    float moveDir = 0.0f;               // Horizontal direction (-1, 0, 1)
    float verticalDir = 0.0f;           // Vertical direction (-1, 0, 1) for ladders
    bool wantsJump = false;             // Jump intent flag
    bool onGround = false;              // Ground contact flag
    bool facingLeft = false;            // Sprite orientation

    // Environment collision data (manual, not managed by actor CollisionSystem)
    const PlatformRect* platforms = nullptr;
    int platformCount = 0;

    // Stairs layer data
    const uint8_t* stairsIndices = nullptr;
    int stairsWidth = 0;
    int stairsHeight = 0;
    int stairsTileSize = 0;

    /** @brief Checks if the player is overlapping a stairs area. */
    bool isOverlappingStairs() const;

    /** @brief Returns the number of frames for the current state. */
    int getNumberOfFramesByState() const;

    /** @brief Returns the sprite for the current state and frame. */
    pixelroot32::graphics::Sprite4bpp getSpriteByState() const;

    /** @brief Changes player state and resets animation if needed. */
    void changeState(PlayerState newState);
};

} // namespace metroidvania
