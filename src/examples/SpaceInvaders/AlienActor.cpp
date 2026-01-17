#include "AlienActor.h"
#include "GameConstants.h"
#include "AlienSprites.h"

namespace spaceinvaders {

using pixelroot32::graphics::Sprite;
using pixelroot32::graphics::MultiSprite;
using pixelroot32::graphics::SpriteAnimationFrame;

// Step-based animation tables for each alien type.
// These tables are immutable and live in flash when possible.
static const SpriteAnimationFrame SQUID_ANIM_FRAMES[] = {
    { &SQUID_F1, nullptr },
    { &SQUID_F2, nullptr }
};

static const SpriteAnimationFrame CRAB_ANIM_FRAMES[] = {
    { nullptr, &CRAB_F1_MULTI }
};

static const SpriteAnimationFrame OCTOPUS_ANIM_FRAMES[] = {
    { &OCTOPUS_F1, nullptr },
    { &OCTOPUS_F2, nullptr }
};

AlienActor::AlienActor(float x, float y, AlienType type)
    : pixelroot32::core::Actor(x, y, 0, 0), type(type), active(true) {

    // Configure sprite dimensions based on alien type.
    switch (type) {
        case AlienType::SQUID:
            width = ALIEN_SQUID_W;
            height = ALIEN_SQUID_H;
            break;
        case AlienType::CRAB:
            width = ALIEN_CRAB_W;
            height = ALIEN_CRAB_H;
            break;
        case AlienType::OCTOPUS:
            width = ALIEN_OCTOPUS_W;
            height = ALIEN_OCTOPUS_H;
            break;
    }

    // Bind the appropriate animation table.
    if (type == AlienType::SQUID) {
        animation.frames     = SQUID_ANIM_FRAMES;
        animation.frameCount = static_cast<uint8_t>(sizeof(SQUID_ANIM_FRAMES) / sizeof(SpriteAnimationFrame));
    } else if (type == AlienType::CRAB) {
        animation.frames     = CRAB_ANIM_FRAMES;
        animation.frameCount = static_cast<uint8_t>(sizeof(CRAB_ANIM_FRAMES) / sizeof(SpriteAnimationFrame));
    } else {
        animation.frames     = OCTOPUS_ANIM_FRAMES;
        animation.frameCount = static_cast<uint8_t>(sizeof(OCTOPUS_ANIM_FRAMES) / sizeof(SpriteAnimationFrame));
    }
    animation.reset();
}

void AlienActor::update(unsigned long deltaTime) {
    // Aliens don't move themselves in update(), they are moved by Scene controller
}

void AlienActor::move(float dx, float dy) {
    x += dx;
    y += dy;
    // Step-based animation: advance one frame per logical movement step.
    animation.step();
}

void AlienActor::draw(pixelroot32::graphics::Renderer& renderer) {
    if (!active) return;
    
    using Color       = pixelroot32::graphics::Color;
    using Sprite      = pixelroot32::graphics::Sprite;
    using MultiSprite = pixelroot32::graphics::MultiSprite;

    // Query current frame from the animation system.
    const Sprite*      sprite      = animation.getCurrentSprite();
    const MultiSprite* multiSprite = animation.getCurrentMultiSprite();

    const int drawX = static_cast<int>(x);
    const int drawY = static_cast<int>(y);

    if (multiSprite) {
        renderer.drawMultiSprite(*multiSprite, drawX, drawY);
    } else if (sprite) {
        renderer.drawSprite(*sprite, drawX, drawY, Color::Orange);
    }
}

pixelroot32::core::Rect AlienActor::getHitBox() {
    return {x, y, (int)width, (int)height};
}

void AlienActor::onCollision(pixelroot32::core::Actor* other) {
    // Handled mostly by Projectile, but here we can react too
}

int AlienActor::getScoreValue() const {
    switch (type) {
        case AlienType::SQUID: return 30;
        case AlienType::CRAB: return 20;
        case AlienType::OCTOPUS: return 10;
        default: return 0;
    }
}

}
