#pragma once
#include <cstdint>
#include <audio/AudioTypes.h>

namespace brickbreaker {

    // Input Constants
    constexpr uint8_t BTN_START = 4;
    constexpr uint8_t BTN_RIGHT = 3;
    constexpr uint8_t BTN_LEFT  = 2;

    // Game Constants
    constexpr int BRICK_WIDTH = 30;
    constexpr int BRICK_HEIGHT = 12;
    constexpr int PADDLE_W = 40;
    constexpr int PADDLE_H = 8;
    constexpr int BALL_SIZE = 6;
    constexpr float BORDER_TOP = 20.0f;

    // Audio Constants (Pong-like frequencies)
    namespace sfx {
        const pixelroot32::audio::AudioEvent PADDLE_HIT = { pixelroot32::audio::WaveType::PULSE, 459.0f, 0.1f, 0.5f, 0.5f };
        const pixelroot32::audio::AudioEvent WALL_HIT   = { pixelroot32::audio::WaveType::PULSE, 226.0f, 0.1f, 0.5f, 0.5f };
        const pixelroot32::audio::AudioEvent BRICK_HIT  = { pixelroot32::audio::WaveType::PULSE, 490.0f, 0.1f, 0.5f, 0.5f };
        const pixelroot32::audio::AudioEvent LIFE_LOST  = { pixelroot32::audio::WaveType::PULSE, 113.0f, 0.5f, 0.6f, 0.5f };
        const pixelroot32::audio::AudioEvent START_GAME = { pixelroot32::audio::WaveType::PULSE, 880.0f, 0.2f, 0.5f, 0.5f };
    }

}
