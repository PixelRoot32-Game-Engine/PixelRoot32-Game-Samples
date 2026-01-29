#include "PlayerActor.h"
#include "assets/PlayerIdleSprites.h"
#include "assets/PlayerRunSprites.h"
#include "assets/PlayerJumSprites.h"
#include "assets/PlayerPalette.h"
#include "Config.h"
#include "graphics/Renderer.h"
#include "graphics/Color.h"
#if defined(ESP32) || defined(ESP8266)
#include <pgmspace.h>
#endif

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
    setRenderLayer(2);                  // Layer above background and platforms
    setCollisionLayer(Layers::PLAYER);  // Identifies this actor as player
    setCollisionMask(Layers::ENEMY);    // Defines which other actors it can collide with (e.g., enemies)
    setRestitution(0.0f);               // No bounce
    setFriction(0.0f);                  // No friction (movement is direct by input)
}

void PlayerActor::setInput(float dir, float vDir, bool jumpPressed) {
    moveDir = dir;
    verticalDir = vDir;
    if (jumpPressed) {
        // If climbing, jump releases the ladder
        if (currentState == PlayerState::CLIMBING) {
            changeState(PlayerState::JUMP);
            vy = -PLAYER_JUMP_VELOCITY * 0.8f; // Slightly shorter jump when releasing
        } else if (onGround) {
            wantsJump = true;
        }
    }
}

void PlayerActor::setPlatforms(const PlatformRect* list, int count) {
    platforms = list;
    platformCount = count;
}

void PlayerActor::setStairs(const uint8_t* indices, int width, int height, int tileSize) {
    stairsIndices = indices;
    stairsWidth = width;
    stairsHeight = height;
    stairsTileSize = tileSize;
}

void PlayerActor::setPlatformTiles(const uint8_t* indices, int width, int height, int tileSize) {
    platformIndices = indices;
    platformWidth = width;
    platformHeight = height;
    platformTileSize = tileSize;
}

void PlayerActor::buildStairsCache() {
    if (!stairsIndices || stairsWidth <= 0 || stairsHeight <= 0) return;
    const int totalCells = stairsWidth * stairsHeight;
    if (totalCells > STAIRS_CACHE_MAX_BYTES * 8) return;

    for (int i = 0; i < STAIRS_CACHE_MAX_BYTES; ++i) stairsMask[i] = 0;

    for (int r = 0; r < stairsHeight; ++r) {
        for (int c = 0; c < stairsWidth; ++c) {
            const int idx = c + r * stairsWidth;
#if defined(ESP32) || defined(ESP8266)
            uint8_t v = pgm_read_byte(stairsIndices + idx);
#else
            uint8_t v = stairsIndices[idx];
#endif
            if (v != 0) stairsMask[idx >> 3] |= (1u << (idx & 7));
        }
    }
    stairsMaskReady = true;
}

/**
 * @brief Checks if the player's center overlaps a stairs tile.
 * Multiple vertical points are checked to allow entering the stairs from top or bottom.
 */
bool PlayerActor::isOverlappingStairs() const {
    if (!stairsIndices && !stairsMaskReady) return false;
    
    float centerX = x + width / 2.0f;
    int col = static_cast<int>(centerX) / stairsTileSize;
    
    if (col < 0 || col >= stairsWidth) return false;

    // Check points: head, center, feet and slightly below feet.
    float yPoints[] = { y, y + height / 2.0f, y + height - 1.0f, y + height + 2.0f };
    for (float py : yPoints) {
        int row = static_cast<int>(py) / stairsTileSize;
        if (row >= 0 && row < stairsHeight) {
            const int idx = col + row * stairsWidth;
            if (stairsMaskReady) {
                if (stairsMask[idx >> 3] & (1u << (idx & 7))) return true;
            } else {
                if (stairsIndices[idx] != 0) return true;
            }
        }
    }
    
    return false;
}

void PlayerActor::update(unsigned long deltaTime) {
    float dt = deltaTime * 0.001f;
    if (dt > 0.05f) dt = 0.05f; // Cap to avoid large jumps due to lag

    bool overlappingStairs = isOverlappingStairs();

    // State and movement logic
    if (currentState == PlayerState::CLIMBING) {
        vx = moveDir * PLAYER_MOVE_SPEED * 0.7f; // Reduced horizontal speed on ladder
        vy = verticalDir * PLAYER_CLIMB_SPEED;
        
        // If we leave the stairs area, return to normal state
        if (!overlappingStairs) {
            changeState(PlayerState::IDLE);
        }
    } else {
        vx = moveDir * PLAYER_MOVE_SPEED;
        vy += PLAYER_GRAVITY * dt; // Apply gravity
        
        // Try to enter climbing mode
        if (overlappingStairs && verticalDir != 0.0f) {
            bool canClimbDown = (verticalDir > 0 && onGround); // Climb down from a platform
            bool canClimbUp = (verticalDir < 0);               // Climb up from ground or air

            if (canClimbUp || canClimbDown) {
                changeState(PlayerState::CLIMBING);
                vx = 0;
                vy = 0;
                // Auto-center player on the ladder
                int col = static_cast<int>(x + width / 2.0f) / stairsTileSize;
                x = col * stairsTileSize + (stairsTileSize - width) / 2.0f;
                
                // If climbing down, give a small nudge to pass through the initial ground
                if (canClimbDown) {
                    y += 4.0f; 
                    onGround = false;
                }
            }
        }

        if (wantsJump && onGround) {
            vy = -PLAYER_JUMP_VELOCITY;
            wantsJump = false;
            onGround = false;
        }
    }

    // Sprite orientation
    if (moveDir > 0.0f) facingLeft = false;
    else if (moveDir < 0.0f) facingLeft = true;

    // Lazy initialization of world size (if not defined)
    if (worldHeight == 0) {
        setWorldSize(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }

    // --- TILE-BASED ENVIRONMENT COLLISION RESOLUTION ---
    // Note: Separated into X and Y for more precise response.
    // This is much more efficient (O(1)) than iterating over all platform rects (O(N)).

    // 1. Horizontal movement and collision
    x += vx * dt;
    if (platformIndices) {
        // Check collision at 4 points (top-left, top-right, bottom-left, bottom-right)
        // or more simply, check columns around the player's bounding box.
        int startCol = static_cast<int>(x) / platformTileSize;
        int endCol = static_cast<int>(x + width - 1) / platformTileSize;
        int startRow = static_cast<int>(y) / platformTileSize;
        int endRow = static_cast<int>(y + height - 1) / platformTileSize;

        for (int r = startRow; r <= endRow; ++r) {
            for (int c = startCol; c <= endCol; ++c) {
                if (r >= 0 && r < platformHeight && c >= 0 && c < platformWidth) {
                    if (platformIndices[c + r * platformWidth] != 0) {
                        // Collision detected
                        if (vx > 0) x = c * platformTileSize - width;
                        else if (vx < 0) x = (c + 1) * platformTileSize;
                        vx = 0;
                        break;
                    }
                }
            }
        }
    }

    // 2. Vertical movement and collision
    y += vy * dt;
    bool wasOnGround = onGround;
    onGround = false;
    if (platformIndices) {
        int startCol = static_cast<int>(x) / platformTileSize;
        int endCol = static_cast<int>(x + width - 1) / platformTileSize;
        int startRow = static_cast<int>(y) / platformTileSize;
        int endRow = static_cast<int>(y + height - 1) / platformTileSize;

        for (int r = startRow; r <= endRow; ++r) {
            bool collisionFound = false;
            for (int c = startCol; c <= endCol; ++c) {
                if (r >= 0 && r < platformHeight && c >= 0 && c < platformWidth) {
                    if (platformIndices[c + r * platformWidth] != 0) {
                        
                        // Check if there is a ladder at this collision point (use RAM cache on ESP32)
                        bool hasLadder = (stairsMaskReady || stairsIndices != nullptr) &&
                                         (c >= 0 && c < stairsWidth && r >= 0 && r < stairsHeight);
                        if (hasLadder) {
                            const int sidx = c + r * stairsWidth;
                            hasLadder = stairsMaskReady
                                ? (stairsMask[sidx >> 3] & (1u << (sidx & 7))) != 0
                                : (stairsIndices[sidx] != 0);
                        }

                        // EXCEPTION: If there's a ladder, allow passing through the platform in certain conditions
                        if (hasLadder) {
                            if (vy < 0) continue; // Moving UP: always pass through
                            if (currentState == PlayerState::CLIMBING) {
                                if (verticalDir > 0 && vy > 0) continue; // Actively climbing down
                                if (verticalDir == 0) {
                                    // If no input, land on top if falling onto it
                                    if (vy > 0 && y + height <= r * platformTileSize + 4.0f) {
                                        // Let it collide below
                                    } else {
                                        continue;
                                    }
                                }
                            }
                        }

                        // Standard collision resolution (floor/ceiling)
                        if (vy > 0) { // Falling
                            y = r * platformTileSize - height;
                            onGround = true;
                        }
                        else if (vy < 0) { // Jumping
                            y = (r + 1) * platformTileSize;
                        }
                        vy = 0;
                        collisionFound = true;
                        break;
                    }
                }
            }
            if (collisionFound) break;
        }
    }

    // World bounds (screen edges)
    resolveWorldBounds();
    if (worldCollisionInfo.bottom) onGround = true;

    // Fix: If we were on ground and we are not moving upwards, and we haven't detected a collision yet,
    // check if there's still ground exactly below us to maintain onGround state.
    if (!onGround && wasOnGround && vy >= 0) {
        float checkY = y + height + 1.0f;
        int startCol = static_cast<int>(x) / platformTileSize;
        int endCol = static_cast<int>(x + width - 1) / platformTileSize;
        int r = static_cast<int>(checkY) / platformTileSize;

        if (r >= 0 && r < platformHeight) {
            for (int c = startCol; c <= endCol; ++c) {
                if (c >= 0 && c < platformWidth && platformIndices[c + r * platformWidth] != 0) {
                    onGround = true;
                    break;
                }
            }
        }
    }

    // State machine for animations
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
        case PlayerState::CLIMBING:
            // CLIMBING state is managed by ladder presence and vertical input
            break;
    }
    if (nextState != currentState) changeState(nextState);

    // Animation frame update
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
        case PlayerState::IDLE:     return NUM_IDLE_FRAMES;
        case PlayerState::RUN:      return NUM_RUN_FRAMES;
        case PlayerState::JUMP:     return NUM_JUM_FRAMES;
        case PlayerState::CLIMBING: return NUM_RUN_FRAMES;
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
        case PlayerState::CLIMBING:
            return RUN_FRAMES[currentFrame < NUM_RUN_FRAMES ? currentFrame : 0];
    }
    return IDLE_FRAMES[0];
}

} // namespace metroidvania
