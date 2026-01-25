#include "BunkerActor.h"
#include "GameConstants.h"

namespace spaceinvaders {

BunkerActor::BunkerActor(float x, float y, int w, int h, int maxHealth)
    : pixelroot32::core::Actor(x, y, w, h), health(maxHealth), maxHealth(maxHealth) {
}

void BunkerActor::update(unsigned long deltaTime) {
    (void)deltaTime;
}

void BunkerActor::draw(pixelroot32::graphics::Renderer& renderer) {
    if (health <= 0) return;

    using Color = pixelroot32::graphics::Color;
    float ratio = (maxHealth > 0) ? (float)health / (float)maxHealth : 0.0f;
    if (ratio <= 0.0f) return;

    int visibleHeight = (int)(height * ratio);
    if (visibleHeight <= 0) return;

    int drawY = (int)(y + (height - visibleHeight));

    Color c = Color::Green;
    if (ratio < 0.5f && ratio >= 0.25f) {
        c = Color::Yellow;
    } else if (ratio < 0.25f) {
        c = Color::Red;
    }

    renderer.drawFilledRectangle((int)x, drawY, width, visibleHeight, c);
}

pixelroot32::core::Rect BunkerActor::getHitBox() {
    if (health <= 0) {
        return {x, y + height, width, 0};
    }

    float ratio = (maxHealth > 0) ? (float)health / (float)maxHealth : 0.0f;
    int visibleHeight = (int)(height * ratio);
    int hitY = (int)(y + (height - visibleHeight));
    return {x, (float)hitY, width, visibleHeight};
}

void BunkerActor::onCollision(pixelroot32::core::Actor* other) {
    (void)other;
}

void BunkerActor::applyDamage(int amount) {
    health -= amount;
    if (health < 0) {
        health = 0;
    }
}

bool BunkerActor::isDestroyed() const {
    return health <= 0;
}

}
