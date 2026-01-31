#pragma once
#include "core/Scene.h"
#include "PaddleActor.h"
#include "BallActor.h"
#include "graphics/Color.h"
#include "EngineConfig.h"
#include "GameConstants.h"

namespace pong {

class PongScene : public pixelroot32::core::Scene {
public:
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

    float getBallX() { return ball->x; }
    float getBallY() { return ball->y; }

private:
    PaddleActor* leftPaddle;
    PaddleActor* rightPaddle;
    BallActor* ball;

    int leftScore, rightScore;
    bool gameOver;
    
    // Play area bounds
    int playAreaTop;
    int playAreaBottom;

    void resetGame();
};

}
