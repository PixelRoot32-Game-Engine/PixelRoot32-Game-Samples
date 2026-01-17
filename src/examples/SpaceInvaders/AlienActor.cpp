#include "AlienActor.h"
#include "GameConstants.h"
#include "AlienSprites.h"

namespace spaceinvaders {

AlienActor::AlienActor(float x, float y, AlienType type)
    : pixelroot32::core::Actor(x, y, 0, 0), type(type), active(true), animationFrame(false) {
    
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
}

void AlienActor::update(unsigned long deltaTime) {
    // Aliens don't move themselves in update(), they are moved by Scene controller
}

void AlienActor::move(float dx, float dy) {
    x += dx;
    y += dy;
    animationFrame = !animationFrame;
}

void AlienActor::draw(pixelroot32::graphics::Renderer& renderer) {
    if (!active) return;
    
    using Color = pixelroot32::graphics::Color;
    Color c = Color::Orange; // Match reference image

    const uint16_t* sprite = nullptr;

    if (type == AlienType::SQUID) {
        sprite = animationFrame ? SQUID_F2 : SQUID_F1;
    } else if (type == AlienType::CRAB) {
        sprite = animationFrame ? CRAB_F2_REAL : CRAB_F1_REAL;
    } else {
        sprite = animationFrame ? OCTOPUS_F2 : OCTOPUS_F1;
    }

    // Draw sprite pixel by pixel
    int startX = (int)x;
    int startY = (int)y;

    for (int r = 0; r < 8; ++r) {
        uint16_t row = sprite[r];
        // For 8-12 width, 16 bits is enough. 
        // We assume LSB is rightmost, so bit 0 is x + width - 1?
        // Or we assume standard "bit 0 is 1", "bit 1 is 2".
        // Let's iterate from width-1 down to 0 for bits 0..width-1
        
        // Actually, if I defined 0x0018 for SQUID (00011000), 
        // and we want it centered or aligned left.
        // Let's iterate from 0 to width-1.
        // If we treat 0x8000 as left, we shift left.
        // But my hex values: 0x0018. 
        // 0001 1000. 
        // If width is 8.
        // Bit 4 and 3 are set. (0-indexed).
        // Let's iterate bit `b` from 0 to width-1.
        // If `row & (1 << b)` is set, draw pixel at `x + (width - 1 - b)`.
        
        for (int b = 0; b < width; ++b) {
             if (row & (1 << b)) {
                 renderer.drawFilledRectangle(startX + (width - 1 - b), startY + r, 1, 1, c);
             }
        }
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
