#pragma once
#include <cstdint>

namespace spaceinvaders {

    // Input Constants
    constexpr uint8_t BTN_LEFT = 2;
    constexpr uint8_t BTN_RIGHT = 3;
    constexpr uint8_t BTN_FIRE = 4;

    // Game Constants
    constexpr int LOGICAL_WIDTH = 160;
    constexpr int LOGICAL_HEIGHT = 192; // Atari resolution roughly

    // Player
    constexpr int PLAYER_WIDTH = 11;
    constexpr int PLAYER_HEIGHT = 8;
    constexpr float PLAYER_SPEED = 60.0f;
    constexpr float PLAYER_START_X = (LOGICAL_WIDTH - PLAYER_WIDTH) / 2.0f;
    constexpr float PLAYER_START_Y = LOGICAL_HEIGHT - 20.0f;

    // Projectile
    constexpr int PROJECTILE_WIDTH = 1;
    constexpr int PROJECTILE_HEIGHT = 4;
    constexpr float PROJECTILE_SPEED = 120.0f;

    // Aliens
    constexpr int ALIEN_ROWS = 5;
    constexpr int ALIEN_COLS = 11;
    constexpr float ALIEN_START_X = 9.0f;
    constexpr float ALIEN_START_Y = 40.0f;
    constexpr float ALIEN_SPACING_X = 13.0f;
    constexpr float ALIEN_SPACING_Y = 14.0f; // More spacing for better look
    
    constexpr int ALIEN_SQUID_W = 8;
    constexpr int ALIEN_SQUID_H = 8;
    constexpr int ALIEN_CRAB_W = 11;
    constexpr int ALIEN_CRAB_H = 8;
    constexpr int ALIEN_OCTOPUS_W = 12;
    constexpr int ALIEN_OCTOPUS_H = 8;

    // Movement (Stepping)
    constexpr unsigned long INITIAL_STEP_DELAY = 200; // ms (Faster for testing)
    constexpr float ALIEN_DROP_AMOUNT = 8.0f;
    constexpr float ALIEN_STEP_AMOUNT_X = 4.0f;

    // Physics / Collision Layers (using engine's CollisionTypes.h patterns)
    // We will define these usage in the Actors, but good to know:
    // Layer 1: Player
    // Layer 2: Enemy
    // Layer 3: Player Projectile
    // Layer 4: Enemy Projectile
    // Layer 5: Bunker (if added)

}
