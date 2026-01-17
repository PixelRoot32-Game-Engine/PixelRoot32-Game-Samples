#pragma once
#include "core/Actor.h"
#include "graphics/Renderer.h"

namespace spaceinvaders {

class BunkerActor : public pixelroot32::core::Actor {
public:
    BunkerActor(float x, float y, int w, int h, int maxHealth);

    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

    pixelroot32::core::Rect getHitBox() override;
    void onCollision(pixelroot32::core::Actor* other) override;

    void applyDamage(int amount);
    bool isDestroyed() const;

private:
    int health;
    int maxHealth;
};

}
