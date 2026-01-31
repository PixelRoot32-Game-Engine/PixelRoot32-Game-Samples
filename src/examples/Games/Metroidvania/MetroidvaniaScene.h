#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "graphics/Color.h"
#include "EngineConfig.h"

namespace metroidvania {

class PlayerActor;

class MetroidvaniaScene : public pixelroot32::core::Scene {
public:
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    PlayerActor* player = nullptr;
};

}
