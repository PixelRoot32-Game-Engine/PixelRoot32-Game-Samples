#pragma once
#include <cstdint>

namespace menu {

    // Input Configuration (Aligned with global mapping)
    constexpr uint8_t BTN_NAV_UP = 0;
    constexpr uint8_t BTN_NAV_DOWN = 1;
    constexpr uint8_t BTN_SELECT = 4; // A Button

    // UI Dimensions & Layout
    constexpr float BTN_WIDTH = 120.0f;
    constexpr float BTN_HEIGHT = 20.0f;
    constexpr float BTN_START_Y = 60.0f;
    constexpr float BTN_GAP = 6.0f;
    
    constexpr int TITLE_Y = 25;
    constexpr int NAV_INSTR_Y_OFFSET = 40; // From bottom
    constexpr int SEL_INSTR_Y_OFFSET = 25; // From bottom

    // Font Sizes
    constexpr int BTN_FONT_SIZE = 1;
    constexpr int TITLE_FONT_SIZE = 2;
    constexpr int INSTRUCTION_FONT_SIZE = 1;

    // Audio Settings
    constexpr float SOUND_BLIP_FREQ = 440.0f;
    constexpr float SOUND_BLIP_DUR = 0.1f;
    constexpr float SOUND_NAV_FREQ = 880.0f;
    constexpr float SOUND_NAV_DUR = 0.05f;
    constexpr float SOUND_VOL_BLIP = 0.5f;
    constexpr float SOUND_VOL_NAV = 0.3f;

#ifdef PIXELROOT32_ENABLE_2BPP_SPRITES
    constexpr int GAME_COUNT = 7;
#else
    constexpr int GAME_COUNT = 6;
#endif
}
