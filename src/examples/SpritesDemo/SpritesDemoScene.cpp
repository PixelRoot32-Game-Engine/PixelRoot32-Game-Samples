#include "SpritesDemoScene.h"
#include "graphics/Renderer.h"
#include <cstdint>

namespace pr32 = pixelroot32;

namespace spritesdemo {

using pr32::graphics::Color;
using pr32::graphics::SpriteLayer;
using pr32::graphics::MultiSprite;
using pr32::graphics::SpriteAnimation;
using pr32::graphics::SpriteAnimationFrame;

namespace {

template <unsigned int N>
constexpr uint8_t spriteHeight(const uint16_t (&)[N]) {
    return static_cast<uint8_t>(N);
}

static constexpr uint8_t SPRITE_WIDTH = 16;
static constexpr uint8_t SPRITE_HEIGHT = spriteHeight(SPRITE_0_LAYER_0);

// --- Sprite Definitions ---

// Sprite 0
static const SpriteLayer SPRITE_0_LAYERS[] = {
    { SPRITE_0_LAYER_0, Color::Black },
    { SPRITE_0_LAYER_1, Color::LightBlue },
    { SPRITE_0_LAYER_2, Color::Yellow },
    { SPRITE_0_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_0 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_0_LAYERS, 4 };

// Sprite 1
static const SpriteLayer SPRITE_1_LAYERS[] = {
    { SPRITE_1_LAYER_0, Color::Black },
    { SPRITE_1_LAYER_1, Color::LightBlue },
    { SPRITE_1_LAYER_2, Color::Yellow },
    { SPRITE_1_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_1 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_1_LAYERS, 4 };

// Sprite 2
static const SpriteLayer SPRITE_2_LAYERS[] = {
    { SPRITE_2_LAYER_0, Color::Black },
    { SPRITE_2_LAYER_1, Color::LightBlue },
    { SPRITE_2_LAYER_2, Color::Yellow },
    { SPRITE_2_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_2 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_2_LAYERS, 4 };

// Sprite 3
static const SpriteLayer SPRITE_3_LAYERS[] = {
    { SPRITE_3_LAYER_0, Color::Black },
    { SPRITE_3_LAYER_1, Color::LightBlue },
    { SPRITE_3_LAYER_2, Color::Yellow },
    { SPRITE_3_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_3 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_3_LAYERS, 4 };

// Sprite 4
static const SpriteLayer SPRITE_4_LAYERS[] = {
    { SPRITE_4_LAYER_0, Color::Black },
    { SPRITE_4_LAYER_1, Color::LightBlue },
    { SPRITE_4_LAYER_2, Color::Yellow },
    { SPRITE_4_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_4 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_4_LAYERS, 4 };

// Sprite 5
static const SpriteLayer SPRITE_5_LAYERS[] = {
    { SPRITE_5_LAYER_0, Color::Black },
    { SPRITE_5_LAYER_1, Color::LightBlue },
    { SPRITE_5_LAYER_2, Color::Yellow },
    { SPRITE_5_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_5 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_5_LAYERS, 4 };

// Sprite 6
static const SpriteLayer SPRITE_6_LAYERS[] = {
    { SPRITE_6_LAYER_0, Color::Black },
    { SPRITE_6_LAYER_1, Color::LightBlue },
    { SPRITE_6_LAYER_2, Color::Yellow },
    { SPRITE_6_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_6 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_6_LAYERS, 4 };

// Sprite 7
static const SpriteLayer SPRITE_7_LAYERS[] = {
    { SPRITE_7_LAYER_0, Color::Black },
    { SPRITE_7_LAYER_1, Color::LightBlue },
    { SPRITE_7_LAYER_2, Color::Yellow },
    { SPRITE_7_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_7 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_7_LAYERS, 4 };

// Sprite 8
static const SpriteLayer SPRITE_8_LAYERS[] = {
    { SPRITE_8_LAYER_0, Color::Black },
    { SPRITE_8_LAYER_1, Color::LightBlue },
    { SPRITE_8_LAYER_2, Color::Yellow },
    { SPRITE_8_LAYER_3, Color::White }
};
static const MultiSprite SPRITE_MULTI_8 = { SPRITE_WIDTH, SPRITE_HEIGHT, SPRITE_8_LAYERS, 4 };


// --- Animation Groups ---

// Animation 0: Sprites 0-2
static const SpriteAnimationFrame ANIM_0_FRAMES[] = {
    { nullptr, &SPRITE_MULTI_0 },
    { nullptr, &SPRITE_MULTI_1 },
    { nullptr, &SPRITE_MULTI_2 }
};

// Animation 1: Sprites 3-4
static const SpriteAnimationFrame ANIM_1_FRAMES[] = {
    { nullptr, &SPRITE_MULTI_3 },
    { nullptr, &SPRITE_MULTI_4 }
};

// Animation 2: Sprites 5-6
static const SpriteAnimationFrame ANIM_2_FRAMES[] = {
    { nullptr, &SPRITE_MULTI_5 },
    { nullptr, &SPRITE_MULTI_6 }
};

// Animation 3: Sprites 7-8
static const SpriteAnimationFrame ANIM_3_FRAMES[] = {
    { nullptr, &SPRITE_MULTI_7 },
    { nullptr, &SPRITE_MULTI_8 }
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
        
        const MultiSprite* current = animation.getCurrentMultiSprite();
        if (current) {
            renderer.drawMultiSprite(*current, drawX, drawY);
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
