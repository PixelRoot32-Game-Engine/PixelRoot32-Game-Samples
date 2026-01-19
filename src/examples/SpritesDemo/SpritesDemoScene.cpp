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

static constexpr uint8_t SPRITE_WIDTH = 16;
static constexpr uint8_t SPRITE_HEIGHT = 32;

static const Color SPRITES_2BPP_PALETTE[] = {
    Color::Transparent,
    Color::Black,
    Color::LightBlue,
    Color::White
};

static const Sprite2bpp SPRITE_0_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_0_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_1_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_1_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_2_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_2_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_3_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_3_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_4_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_4_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_5_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_5_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_6_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_6_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_7_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_7_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };
static const Sprite2bpp SPRITE_8_2BPP_STRUCT = { reinterpret_cast<const uint8_t*>(SPRITE_8_2BPP), SPRITES_2BPP_PALETTE, SPRITE_WIDTH, SPRITE_HEIGHT, 4 };

static const Sprite2bpp* SPRITES_2BPP[] = {
    &SPRITE_0_2BPP_STRUCT,
    &SPRITE_1_2BPP_STRUCT,
    &SPRITE_2_2BPP_STRUCT,
    &SPRITE_3_2BPP_STRUCT,
    &SPRITE_4_2BPP_STRUCT,
    &SPRITE_5_2BPP_STRUCT,
    &SPRITE_6_2BPP_STRUCT,
    &SPRITE_7_2BPP_STRUCT,
    &SPRITE_8_2BPP_STRUCT
};


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
            const Sprite2bpp* sprite = SPRITES_2BPP[spriteIndex];
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
