#pragma once
#include "core/Actor.h"
#include "graphics/Renderer.h"

namespace spaceinvaders {

enum class AlienType {
    SQUID,
    CRAB,
    OCTOPUS
};

class AlienActor : public pixelroot32::core::Actor {
public:
    AlienActor(float x, float y, AlienType type);
    
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer);
    
    // Custom movement (called by Scene)
    void move(float dx, float dy);
    
    pixelroot32::core::Rect getHitBox() override;
    void onCollision(pixelroot32::core::Actor* other) override;

    bool isActive() const { return active; }
    void kill() { active = false; }
    int getScoreValue() const;

private:
    AlienType type;
    bool active;
    pixelroot32::graphics::SpriteAnimation animation;
};

}
