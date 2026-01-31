#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/Color.h"
#include "EngineConfig.h"
#include "assets/Sprites.h"
#include "assets/SpritesPopup.h"

namespace spritesdemo {

class SpritesDemoScene : public pixelroot32::core::Scene {
public:
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;
};

}