#pragma once
#include <cstdint>

namespace menu {

    // Input Configuration (Aligned with global mapping)
    constexpr uint8_t BTN_NAV_UP = 0;
    constexpr uint8_t BTN_NAV_DOWN = 1;
    constexpr uint8_t BTN_SELECT = 4; // A Button

    // UI Dimensions & Layout
    constexpr float BTN_WIDTH = 120.0f;
    constexpr float BTN_HEIGHT = 25.0f;
    constexpr float BTN_START_Y = 80.0f;
    constexpr float BTN_GAP = 0.0f;
    
    constexpr int TITLE_Y = 30;
    constexpr int NAV_INSTR_Y_OFFSET = 40; // From bottom
    constexpr int SEL_INSTR_Y_OFFSET = 25; // From bottom

    // Font Sizes
    constexpr int TITLE_FONT_SIZE = 2;
    constexpr int INSTRUCTION_FONT_SIZE = 1;

    // Audio Settings
    constexpr float SOUND_BLIP_FREQ = 440.0f;
    constexpr float SOUND_BLIP_DUR = 0.1f;
    constexpr float SOUND_NAV_FREQ = 880.0f;
    constexpr float SOUND_NAV_DUR = 0.05f;
    constexpr float SOUND_VOL_BLIP = 0.5f;
    constexpr float SOUND_VOL_NAV = 0.3f;

    constexpr int GAME_COUNT = 5;
}
