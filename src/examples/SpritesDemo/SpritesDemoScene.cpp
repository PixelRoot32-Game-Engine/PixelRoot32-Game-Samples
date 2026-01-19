#include "SpritesDemoScene.h"
#include "graphics/Renderer.h"
#include <cstdint>

namespace pr32 = pixelroot32;

namespace spritesdemo {

using pr32::graphics::Color;
using pr32::graphics::Sprite2bpp;
using pr32::graphics::SpriteAnimation;
using pr32::graphics::SpriteAnimationFrame;

namespace {

template <unsigned int N>
constexpr uint8_t spriteHeight(const uint16_t (&)[N]) {
    return static_cast<uint8_t>(N);
}

static constexpr uint8_t SPRITE_WIDTH = 16;
static constexpr uint8_t SPRITE_HEIGHT = spriteHeight(SPRITE_0_LAYER_0);

static constexpr uint8_t SPRITE_2BPP_ROW_BYTES = static_cast<uint8_t>((SPRITE_WIDTH * 2 + 7) / 8);

static uint8_t SPRITE_0_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_1_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_2_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_3_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_4_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_5_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_6_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_7_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};
static uint8_t SPRITE_8_2BPP_DATA[SPRITE_HEIGHT * SPRITE_2BPP_ROW_BYTES] = {};

static const Color SPRITES_2BPP_PALETTE[] = {
    Color::Transparent,
    Color::Black,
    Color::LightBlue,
    Color::White
};

static Sprite2bpp SPRITE_0_2BPP = { SPRITE_0_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_1_2BPP = { SPRITE_1_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_2_2BPP = { SPRITE_2_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_3_2BPP = { SPRITE_3_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_4_2BPP = { SPRITE_4_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_5_2BPP = { SPRITE_5_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_6_2BPP = { SPRITE_6_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_7_2BPP = { SPRITE_7_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static Sprite2bpp SPRITE_8_2BPP = { SPRITE_8_2BPP_DATA, SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };

static Sprite2bpp* SPRITES_2BPP[] = {
    &SPRITE_0_2BPP,
    &SPRITE_1_2BPP,
    &SPRITE_2_2BPP,
    &SPRITE_3_2BPP,
    &SPRITE_4_2BPP,
    &SPRITE_5_2BPP,
    &SPRITE_6_2BPP,
    &SPRITE_7_2BPP,
    &SPRITE_8_2BPP
};

static void packSpriteTo2bpp(const uint16_t* layer0,
                             const uint16_t* layer1,
                             const uint16_t* layer3,
                             uint8_t* outData) {
    for (uint8_t row = 0; row < SPRITE_HEIGHT; ++row) {
        uint8_t* rowData = outData + row * SPRITE_2BPP_ROW_BYTES;

        for (uint8_t col = 0; col < SPRITE_WIDTH; ++col) {
            bool bit0 = layer0 && ((layer0[row] & (static_cast<uint16_t>(1u) << col)) != 0);
            bool bit1 = layer1 && ((layer1[row] & (static_cast<uint16_t>(1u) << col)) != 0);
            bool bit3 = layer3 && ((layer3[row] & (static_cast<uint16_t>(1u) << col)) != 0);

            uint8_t index = 0;
            if (bit3) {
                index = 3;
            } else if (bit1) {
                index = 2;
            } else if (bit0) {
                index = 1;
            }

            uint8_t bitIndex = static_cast<uint8_t>(col * 2);
            uint8_t byteIndex = static_cast<uint8_t>(bitIndex >> 3);
            uint8_t shift = static_cast<uint8_t>(bitIndex & 7);

            rowData[byteIndex] = static_cast<uint8_t>(
                (rowData[byteIndex] & static_cast<uint8_t>(~(0x3u << shift))) |
                static_cast<uint8_t>((index & 0x3u) << shift)
            );
        }
    }
}

static void initSprites2bpp() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    packSpriteTo2bpp(SPRITE_0_LAYER_0, SPRITE_0_LAYER_1, SPRITE_0_LAYER_3, SPRITE_0_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_1_LAYER_0, SPRITE_1_LAYER_1, SPRITE_1_LAYER_3, SPRITE_1_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_2_LAYER_0, SPRITE_2_LAYER_1, SPRITE_2_LAYER_3, SPRITE_2_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_3_LAYER_0, SPRITE_3_LAYER_1, SPRITE_3_LAYER_3, SPRITE_3_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_4_LAYER_0, SPRITE_4_LAYER_1, SPRITE_4_LAYER_3, SPRITE_4_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_5_LAYER_0, SPRITE_5_LAYER_1, SPRITE_5_LAYER_3, SPRITE_5_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_6_LAYER_0, SPRITE_6_LAYER_1, SPRITE_6_LAYER_3, SPRITE_6_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_7_LAYER_0, SPRITE_7_LAYER_1, SPRITE_7_LAYER_3, SPRITE_7_2BPP_DATA);
    packSpriteTo2bpp(SPRITE_8_LAYER_0, SPRITE_8_LAYER_1, SPRITE_8_LAYER_3, SPRITE_8_2BPP_DATA);
}

// --- Animation Groups ---
// Note: We use empty frames here because we manually map the animation index 
// to our Sprite2bpp array in the draw() method. The engine's SpriteAnimation 
// logic is only used here to track the current frame index and timing.

// Animation 0: Sprites 0-2
static const SpriteAnimationFrame ANIM_0_FRAMES[] = {
    { nullptr, nullptr },
    { nullptr, nullptr },
    { nullptr, nullptr }
};

// Animation 1: Sprites 3-4
static const SpriteAnimationFrame ANIM_1_FRAMES[] = {
    { nullptr, nullptr },
    { nullptr, nullptr }
};

// Animation 2: Sprites 5-6
static const SpriteAnimationFrame ANIM_2_FRAMES[] = {
    { nullptr, nullptr },
    { nullptr, nullptr }
};

// Animation 3: Sprites 7-8
static const SpriteAnimationFrame ANIM_3_FRAMES[] = {
    { nullptr, nullptr },
    { nullptr, nullptr }
};


class SpritesDemoBackground : public pr32::core::Entity {
public:
    SpritesDemoBackground()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);
    }
};

class SpritesDemoActor : public pr32::core::Entity {
public:
    SpritesDemoActor(float px, float py)
        : pr32::core::Entity(px, py, SPRITE_WIDTH, SPRITE_HEIGHT, pr32::core::EntityType::GENERIC),
          timeAccumulator(0),
          switchTimer(0),
          currentAnimGroup(0) {
        setRenderLayer(1);
        setAnimationGroup(0);
    }

    void update(unsigned long deltaTime) override {
        // Animation Step Logic
        const unsigned long frameTimeMs = 150; // Slower for visibility
        timeAccumulator += deltaTime;
        while (timeAccumulator >= frameTimeMs) {
            timeAccumulator -= frameTimeMs;
            animation.step();
        }

        // Animation Group Switching Logic (every 2 seconds)
        switchTimer += deltaTime;
        if (switchTimer >= 2000) {
            switchTimer = 0;
            currentAnimGroup = (currentAnimGroup + 1) % 4; // 0 -> 1 -> 2 -> 3 -> 0
            setAnimationGroup(currentAnimGroup);
        }
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        int drawX = static_cast<int>(x);
        int drawY = static_cast<int>(y);

        int spriteIndex = 0;
        switch (currentAnimGroup) {
            case 0:
                spriteIndex = animation.current;
                break;
            case 1:
                spriteIndex = 3 + animation.current;
                break;
            case 2:
                spriteIndex = 5 + animation.current;
                break;
            case 3:
                spriteIndex = 7 + animation.current;
                break;
            default:
                spriteIndex = 0;
                break;
        }

        if (spriteIndex >= 0 && spriteIndex < static_cast<int>(sizeof(SPRITES_2BPP) / sizeof(SPRITES_2BPP[0]))) {
            Sprite2bpp* sprite = SPRITES_2BPP[spriteIndex];
            renderer.drawSprite(*sprite, drawX, drawY, false);
        }
    }

private:
    SpriteAnimation animation;
    unsigned long timeAccumulator;
    unsigned long switchTimer;
    int currentAnimGroup;

    void setAnimationGroup(int group) {
        switch(group) {
            case 0:
                animation.frames = ANIM_0_FRAMES;
                animation.frameCount = sizeof(ANIM_0_FRAMES) / sizeof(SpriteAnimationFrame);
                break;
            case 1:
                animation.frames = ANIM_1_FRAMES;
                animation.frameCount = sizeof(ANIM_1_FRAMES) / sizeof(SpriteAnimationFrame);
                break;
            case 2:
                animation.frames = ANIM_2_FRAMES;
                animation.frameCount = sizeof(ANIM_2_FRAMES) / sizeof(SpriteAnimationFrame);
                break;
            case 3:
                animation.frames = ANIM_3_FRAMES;
                animation.frameCount = sizeof(ANIM_3_FRAMES) / sizeof(SpriteAnimationFrame);
                break;
        }
        animation.reset();
    }
};

}

void SpritesDemoScene::init() {
    initSprites2bpp();

    float px = (DISPLAY_WIDTH - SPRITE_WIDTH) * 0.5f;
    float py = (DISPLAY_HEIGHT - SPRITE_HEIGHT) * 0.5f;

    addEntity(new SpritesDemoBackground());
    addEntity(new SpritesDemoActor(px, py));
}

void SpritesDemoScene::update(unsigned long deltaTime) {
    pixelroot32::core::Scene::update(deltaTime);
}

void SpritesDemoScene::draw(pr32::graphics::Renderer& renderer) {
    pixelroot32::core::Scene::draw(renderer);
}

}
