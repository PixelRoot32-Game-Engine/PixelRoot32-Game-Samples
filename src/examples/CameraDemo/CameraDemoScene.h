#pragma once
#include "core/Scene.h"
#include "graphics/Camera2D.h"

namespace camerademo {

class CameraDemoScene : public pixelroot32::core::Scene {
public:
    CameraDemoScene();
    ~CameraDemoScene() override;

    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    pixelroot32::graphics::Camera2D camera;
    float levelWidth;
    bool jumpInputReady = false;
};

}

