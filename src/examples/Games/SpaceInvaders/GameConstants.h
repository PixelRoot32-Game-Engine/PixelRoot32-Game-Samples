#pragma once
#include "Config.h"
#include <cstdint>

namespace spaceinvaders {

    // Input Constants
    constexpr uint8_t BTN_LEFT = 2;
    constexpr uint8_t BTN_RIGHT = 3;
    constexpr uint8_t BTN_FIRE = 4;

    // Scaling
    constexpr float SPRITE_SCALE = 1.25f;

    // Player
    constexpr int PLAYER_SPRITE_W = 11;
    constexpr int PLAYER_SPRITE_H = 8;
    // Scaled dimensions (using simple ceil logic: +0.99 cast to int)
    constexpr int PLAYER_WIDTH = (int)(PLAYER_SPRITE_W * SPRITE_SCALE + 0.99f);
    constexpr int PLAYER_HEIGHT = (int)(PLAYER_SPRITE_H * SPRITE_SCALE + 0.99f);
    
    constexpr float PLAYER_SPEED = 60.0f;
    constexpr float PLAYER_START_X = (DISPLAY_WIDTH - PLAYER_WIDTH) / 2.0f;
    constexpr float PLAYER_START_Y = DISPLAY_HEIGHT - PLAYER_HEIGHT - 4.0f;

    // Projectile
    constexpr int PROJECTILE_WIDTH = 1;
    constexpr int PROJECTILE_HEIGHT = 4;
    constexpr float PROJECTILE_SPEED = 120.0f;

    // Bunkers
    constexpr int BUNKER_COUNT = 4;
    constexpr int BUNKER_WIDTH = 24;
    constexpr int BUNKER_HEIGHT = 16;
    constexpr float BUNKER_Y = DISPLAY_HEIGHT - PLAYER_HEIGHT - 40.0f;

    // Aliens
    constexpr int ALIEN_ROWS = 4;
    constexpr int ALIEN_COLS = 8;
    
    constexpr int ALIEN_SQUID_SPRITE_W = 8;
    constexpr int ALIEN_SQUID_SPRITE_H = 8;
    constexpr int ALIEN_SQUID_W = (int)(ALIEN_SQUID_SPRITE_W * SPRITE_SCALE + 0.99f);
    constexpr int ALIEN_SQUID_H = (int)(ALIEN_SQUID_SPRITE_H * SPRITE_SCALE + 0.99f);

    constexpr int ALIEN_CRAB_SPRITE_W = 11;
    constexpr int ALIEN_CRAB_SPRITE_H = 8;
    constexpr int ALIEN_CRAB_W = (int)(ALIEN_CRAB_SPRITE_W * SPRITE_SCALE + 0.99f);
    constexpr int ALIEN_CRAB_H = (int)(ALIEN_CRAB_SPRITE_H * SPRITE_SCALE + 0.99f);

    constexpr int ALIEN_OCTOPUS_SPRITE_W = 12;
    constexpr int ALIEN_OCTOPUS_SPRITE_H = 8;
    constexpr int ALIEN_OCTOPUS_W = (int)(ALIEN_OCTOPUS_SPRITE_W * SPRITE_SCALE + 0.99f);
    constexpr int ALIEN_OCTOPUS_H = (int)(ALIEN_OCTOPUS_SPRITE_H * SPRITE_SCALE + 0.99f);

    constexpr float ALIEN_SPACING_X = 17.0f;
    constexpr float ALIEN_SPACING_Y = 18.0f;

    constexpr float ALIEN_GRID_WIDTH = (ALIEN_COLS - 1) * ALIEN_SPACING_X + ALIEN_OCTOPUS_W;
    constexpr float ALIEN_START_X = (DISPLAY_WIDTH - ALIEN_GRID_WIDTH) * 0.5f;
    constexpr float ALIEN_START_Y = 40.0f;

    // Movement (Stepping)
    constexpr unsigned long BASE_STEP_DELAY = 417; // 72 BPM (Corchea)
    constexpr unsigned long INITIAL_STEP_DELAY = 380; // Legacy (can be removed if unused)
    constexpr unsigned long MIN_STEP_DELAY = 150;     // Legacy
    constexpr float ALIEN_DROP_AMOUNT = 7.0f;
    constexpr float ALIEN_STEP_AMOUNT_X = 2.5f;

    // Difficulty / Music Sync
    constexpr float GAME_OVER_Y = 200.0f;
    constexpr float Y_RANGE = 160.0f; // GAME_OVER_Y - ALIEN_START_Y (200 - 40)
    constexpr float INV_Y_RANGE = 0.00625f; // 1.0 / 160.0

    // Physics / Collision Layers (using engine's CollisionTypes.h patterns)
    // We will define these usage in the Actors, but good to know:
    // Layer 1: Player
    // Layer 2: Enemy
    // Layer 3: Player Projectile
    // Layer 4: Enemy Projectile
    // Layer 5: Bunker (if added)

}
