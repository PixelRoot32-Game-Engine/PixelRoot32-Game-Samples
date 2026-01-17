#include "SnakeScene.h"
#include "core/Engine.h"
#include "audio/AudioTypes.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

namespace snake {

SnakeScene::SnakeScene()
    : dir(DIR_RIGHT),
      nextDir(DIR_RIGHT),
      score(0),
      gameOver(false),
      lastMoveTime(0),
      moveInterval(INITIAL_MOVE_INTERVAL_MS) {
}

void SnakeScene::init() {
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        seeded = true;
    }
    resetGame();
}

void SnakeScene::resetGame() {
    for (auto* segment : snakeSegments) {
        removeEntity(segment);
        delete segment;
    }
    snakeSegments.clear();

    int centerX = GRID_WIDTH / 2;
    int centerY = GRID_HEIGHT / 2;

    const int initialLength = 4;
    for (int i = 0; i < initialLength; ++i) {
        int x = centerX - i;
        int y = centerY;
        bool head = (i == 0);
        auto* segment = new SnakeSegmentActor(x, y, head);
        snakeSegments.push_back(segment);
        addEntity(segment);
    }

    dir = DIR_RIGHT;
    nextDir = DIR_RIGHT;
    score = 0;
    gameOver = false;
    moveInterval = INITIAL_MOVE_INTERVAL_MS;
    lastMoveTime = millis();
    spawnFood();
}

void SnakeScene::spawnFood() {
    bool valid = false;
    while (!valid) {
        int fx = std::rand() % GRID_WIDTH;
        int fy = TOP_UI_GRID_ROWS + (std::rand() % (GRID_HEIGHT - TOP_UI_GRID_ROWS));
        valid = true;
        for (const auto* segment : snakeSegments) {
            if (segment->getCellX() == fx && segment->getCellY() == fy) {
                valid = false;
                break;
            }
        }
        if (valid) {
            food.x = fx;
            food.y = fy;
        }
    }
}

void SnakeScene::update(unsigned long deltaTime) {
    auto& input = engine.getInputManager();
    auto& audio = engine.getAudioEngine();

    if (gameOver) {
        if (input.isButtonPressed(BTN_RESTART)) {
            pr32::audio::AudioEvent ev{};
            ev.type = pr32::audio::WaveType::PULSE;
            ev.frequency = 800.0f;
            ev.duration = 0.12f;
            ev.volume = 0.6f;
            ev.duty = 0.5f;
            audio.playEvent(ev);
            resetGame();
        }
        return;
    }

    if (input.isButtonDown(BTN_UP) && dir != DIR_DOWN) {
        nextDir = DIR_UP;
    }
    if (input.isButtonDown(BTN_DOWN) && dir != DIR_UP) {
        nextDir = DIR_DOWN;
    }
    if (input.isButtonDown(BTN_LEFT) && dir != DIR_RIGHT) {
        nextDir = DIR_LEFT;
    }
    if (input.isButtonDown(BTN_RIGHT) && dir != DIR_LEFT) {
        nextDir = DIR_RIGHT;
    }

    unsigned long now = millis();
    if (now - lastMoveTime >= static_cast<unsigned long>(moveInterval)) {
        lastMoveTime = now;
        dir = nextDir;

        if (snakeSegments.empty()) {
            return;
        }

        SnakeSegmentActor* headSegment = snakeSegments[0];
        int newX = headSegment->getCellX();
        int newY = headSegment->getCellY();

        if (dir == DIR_UP) {
            newY -= 1;
        } else if (dir == DIR_DOWN) {
            newY += 1;
        } else if (dir == DIR_LEFT) {
            newX -= 1;
        } else if (dir == DIR_RIGHT) {
            newX += 1;
        }

        if (newX < 0 || newX >= GRID_WIDTH || newY < TOP_UI_GRID_ROWS || newY >= GRID_HEIGHT) {
            gameOver = true;
            pr32::audio::AudioEvent ev{};
            ev.type = pr32::audio::WaveType::NOISE;
            ev.frequency = 900.0f;
            ev.duration = 0.25f;
            ev.volume = 0.9f;
            ev.duty = 0.5f;
            audio.playEvent(ev);
            return;
        }

        bool ateFood = (newX == food.x && newY == food.y);

        if (ateFood) {
            auto* newHeadSegment = new SnakeSegmentActor(newX, newY, true);
            snakeSegments.insert(snakeSegments.begin(), newHeadSegment);
            addEntity(newHeadSegment);

            for (size_t i = 1; i < snakeSegments.size(); ++i) {
                snakeSegments[i]->setHead(false);
            }

            score += SCORE_PER_FOOD;
            spawnFood();
            pr32::audio::AudioEvent eatEv{};
            eatEv.type = pr32::audio::WaveType::PULSE;
            eatEv.frequency = 1200.0f;
            eatEv.duration = 0.12f;
            eatEv.volume = 0.8f;
            eatEv.duty = 0.5f;
            audio.playEvent(eatEv);
            if (moveInterval > MIN_MOVE_INTERVAL_MS) {
                moveInterval -= MOVE_INTERVAL_STEP_MS;
                if (moveInterval < MIN_MOVE_INTERVAL_MS) {
                    moveInterval = MIN_MOVE_INTERVAL_MS;
                }
            }
        } else {
            SnakeSegmentActor* tail = snakeSegments.back();
            snakeSegments.pop_back();
            tail->setCellPosition(newX, newY);
            snakeSegments.insert(snakeSegments.begin(), tail);

            for (size_t i = 0; i < snakeSegments.size(); ++i) {
                snakeSegments[i]->setHead(i == 0);
            }

            snakeSegments[0]->resetAlive();

            pr32::audio::AudioEvent moveEv{};
            moveEv.type = pr32::audio::WaveType::PULSE;
            moveEv.frequency = 300.0f;
            moveEv.duration = 0.04f;
            moveEv.volume = 0.4f;
            moveEv.duty = 0.5f;
            audio.playEvent(moveEv);
        }
    }

    pr32::core::Scene::update(deltaTime);

    if (!gameOver && !snakeSegments.empty()) {
        SnakeSegmentActor* headSegment = snakeSegments[0];
        if (!headSegment->isAlive()) {
            gameOver = true;
            pr32::audio::AudioEvent ev{};
            ev.type = pr32::audio::WaveType::NOISE;
            ev.frequency = 700.0f;
            ev.duration = 0.25f;
            ev.volume = 0.9f;
            ev.duty = 0.5f;
            audio.playEvent(ev);
        }
    }
}

void SnakeScene::draw(pr32::graphics::Renderer& renderer) {
    using Color = pr32::graphics::Color;

    renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);

    int playAreaX = 0;
    int playAreaY = TOP_UI_GRID_ROWS * CELL_SIZE;
    int playAreaW = GRID_WIDTH * CELL_SIZE - 1;
    int playAreaH = (GRID_HEIGHT - TOP_UI_GRID_ROWS) * CELL_SIZE - 1;
    renderer.drawRectangle(playAreaX, playAreaY, playAreaW, playAreaH, Color::DarkGreen);

    int fx = food.x * CELL_SIZE;
    int fy = food.y * CELL_SIZE;
    renderer.drawFilledRectangle(fx, fy, CELL_SIZE - 1, CELL_SIZE - 1, Color::Red);

    pr32::core::Scene::draw(renderer);

    char scoreStr[16];
    std::snprintf(scoreStr, sizeof(scoreStr), "SCORE: %d", score);
    renderer.drawText(scoreStr, 5, 5, Color::White, 1);

    if (gameOver) {
        renderer.drawTextCentered("GAME OVER", 100, Color::White, 2);
        renderer.drawTextCentered("Press A to Restart", 130, Color::White, 1);
    }
}

}
