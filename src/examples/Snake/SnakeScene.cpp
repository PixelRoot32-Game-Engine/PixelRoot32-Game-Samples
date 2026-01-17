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
    snake.clear();

    int centerX = GRID_WIDTH / 2;
    int centerY = GRID_HEIGHT / 2;

    snake.push_back({centerX, centerY});
    snake.push_back({centerX - 1, centerY});
    snake.push_back({centerX - 2, centerY});

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
        int fy = std::rand() % GRID_HEIGHT;
        valid = true;
        for (const auto& p : snake) {
            if (p.x == fx && p.y == fy) {
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
    pr32::core::Scene::update(deltaTime);

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

        Point newHead = snake[0];
        if (dir == DIR_UP) {
            newHead.y -= 1;
        } else if (dir == DIR_DOWN) {
            newHead.y += 1;
        } else if (dir == DIR_LEFT) {
            newHead.x -= 1;
        } else if (dir == DIR_RIGHT) {
            newHead.x += 1;
        }

        if (newHead.x < 0 || newHead.x >= GRID_WIDTH || newHead.y < 0 || newHead.y >= GRID_HEIGHT) {
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

        for (const auto& p : snake) {
            if (p.x == newHead.x && p.y == newHead.y) {
                gameOver = true;
                pr32::audio::AudioEvent ev{};
                ev.type = pr32::audio::WaveType::NOISE;
                ev.frequency = 700.0f;
                ev.duration = 0.25f;
                ev.volume = 0.9f;
                ev.duty = 0.5f;
                audio.playEvent(ev);
                return;
            }
        }

        snake.insert(snake.begin(), newHead);

        if (newHead.x == food.x && newHead.y == food.y) {
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
            snake.pop_back();
            pr32::audio::AudioEvent moveEv{};
            moveEv.type = pr32::audio::WaveType::PULSE;
            moveEv.frequency = 300.0f;
            moveEv.duration = 0.04f;
            moveEv.volume = 0.4f;
            moveEv.duty = 0.5f;
            audio.playEvent(moveEv);
        }
    }
}

void SnakeScene::draw(pr32::graphics::Renderer& renderer) {
    using Color = pr32::graphics::Color;

    renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);

    int fx = food.x * CELL_SIZE;
    int fy = food.y * CELL_SIZE;
    renderer.drawFilledRectangle(fx, fy, CELL_SIZE - 1, CELL_SIZE - 1, Color::Red);

    for (int i = 0; i < static_cast<int>(snake.size()); ++i) {
        int sx = snake[i].x * CELL_SIZE;
        int sy = snake[i].y * CELL_SIZE;
        Color color = (i == 0) ? Color::LightGreen : Color::DarkGreen;
        renderer.drawFilledRectangle(sx, sy, CELL_SIZE - 1, CELL_SIZE - 1, color);
    }

    char scoreStr[16];
    std::snprintf(scoreStr, sizeof(scoreStr), "SCORE: %d", score);
    renderer.drawText(scoreStr, 5, 5, Color::White, 2);

    if (gameOver) {
        renderer.drawTextCentered("GAME OVER", 100, Color::White, 2);
        renderer.drawTextCentered("Press A to Restart", 130, Color::White, 1);
    }
}

}
