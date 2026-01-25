#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include "Config.h"
#include "GameConstants.h"
#include "SnakeSegmentActor.h"
#include <vector>

namespace snake {

struct Point {
    int x;
    int y;
};

enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

class SnakeScene : public pixelroot32::core::Scene {
public:
    SnakeScene();
    ~SnakeScene();
    void init() override;
    void update(unsigned long deltaTime) override;
    void draw(pixelroot32::graphics::Renderer& renderer) override;

private:
    static constexpr int MaxSnakeSegments = GRID_WIDTH * GRID_HEIGHT;
    class SnakeBackground;
    SnakeBackground* background;
    std::vector<SnakeSegmentActor*> snakeSegments;
    std::vector<SnakeSegmentActor*> segmentPool;
    Point food;
    Direction dir;
    Direction nextDir;
    int score;
    bool gameOver;
    unsigned long lastMoveTime;
    int moveInterval;

    void resetGame();
    void spawnFood();
};

}
