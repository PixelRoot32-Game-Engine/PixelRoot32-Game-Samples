#include "PongScene.h"
#include "core/Engine.h"
#include "core/PhysicsActor.h"
#include "audio/AudioTypes.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

namespace pong {

using Color = pr32::graphics::Color;

class PongBackground : public pr32::core::Entity {
public:
    PongBackground(int top, int bottom)
        : pr32::core::Entity(0.0f, 0.0f, 0, 0, pr32::core::EntityType::GENERIC),
          playAreaTop(top),
          playAreaBottom(bottom) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        int screenWidth = renderer.getWidth();
        int screenHeight = renderer.getHeight();

        renderer.drawFilledRectangle(0, 0, screenWidth, playAreaTop, Color::DarkGray);
        renderer.drawFilledRectangle(0, playAreaBottom, screenWidth, screenHeight - playAreaBottom, Color::DarkGray);

        renderer.drawLine(0, playAreaTop, screenWidth, playAreaTop, Color::White);
        renderer.drawLine(0, playAreaBottom, screenWidth, playAreaBottom, Color::White);

        int16_t centerX = screenWidth / 2;
        int16_t dashHeight = 10;
        int16_t dashGap = 5;

        for (int16_t y = 0; y < screenHeight; y += dashHeight + dashGap) {
            int16_t dashEnd = y + dashHeight;
            if (dashEnd > screenHeight) {
                dashEnd = screenHeight;
            }
            renderer.drawLine(centerX, y, centerX, dashEnd, Color::LightGray);
        }
    }

private:
    int playAreaTop;
    int playAreaBottom;
};

void PongScene::init() {
    pr32::graphics::setPalette(pr32::graphics::PaletteType::PICO8);
    int screenWidth = engine.getRenderer().getWidth();
    int screenHeight = engine.getRenderer().getHeight();

    // Calculate play area
    playAreaTop = (screenHeight - PONG_PLAY_AREA_HEIGHT) / 2;
    playAreaBottom = playAreaTop + PONG_PLAY_AREA_HEIGHT;

    leftScore = 0;
    rightScore = 0;
    gameOver = false;

    addEntity(new PongBackground(playAreaTop, playAreaBottom));

    leftPaddle = new PaddleActor(0, screenHeight/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT, false);
    leftPaddle->setTopLimit(playAreaTop);
    leftPaddle->setBottomLimit(playAreaBottom);
    
    rightPaddle = new PaddleActor(screenWidth - PADDLE_WIDTH, screenHeight/2 - PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT, true);
    rightPaddle->setTopLimit(playAreaTop);
    rightPaddle->setBottomLimit(playAreaBottom);  
    
    ball = new BallActor(screenWidth/2, screenHeight/2, BALL_SPEED, BALL_RADIUS);
    ball->setWorldSize(screenWidth, screenHeight);
    ball->setLimits(pr32::core::LimitRect(-1, playAreaTop + BALL_RADIUS, -1, playAreaBottom + BALL_RADIUS));
    ball->reset();

    addEntity(leftPaddle);
    addEntity(rightPaddle);
    addEntity(ball);
}

void PongScene::update(unsigned long deltaTime) {
    // 1. Input Processing
    if (!gameOver) {
        leftPaddle->velocity = 0;
        if (engine.getInputManager().isButtonDown(BTN_UP)) leftPaddle->velocity = -100.0f;
        if (engine.getInputManager().isButtonDown(BTN_DOWN)) leftPaddle->velocity = 100.0f;
    } else {
        if (engine.getInputManager().isButtonPressed(BTN_START)) resetGame();
    }

    // 2. Physics & Entity Update (Always call this!)
    Scene::update(deltaTime);

    // 3. Game Logic
    if (!gameOver) {
        // --- Check if ball is out of bounds ---
        if (ball->isActive && ball->getWorldCollisionInfo().left) {
            rightScore++;
            
            // Score sound (bad for player)
            pr32::audio::AudioEvent scoreEv{};
            scoreEv.type = pr32::audio::WaveType::NOISE; // Changed from SAWTOOTH (not supported) to NOISE
            scoreEv.frequency = 200.0f;
            scoreEv.duration = 0.3f;
            scoreEv.volume = 0.7f;
            engine.getAudioEngine().playEvent(scoreEv);

            ball->reset();
        }
        if (ball->isActive && ball->getWorldCollisionInfo().right) {
            leftScore++;

            pr32::audio::AudioEvent coinEv{};
            coinEv.type = pr32::audio::WaveType::PULSE;
            coinEv.frequency = 900.0f;
            coinEv.duration = 0.2f;
            coinEv.volume = 0.7f;
            coinEv.duty = 0.5f;
            engine.getAudioEngine().playEvent(coinEv);

            ball->reset();
        }

        // --- Game Over ---
        if (leftScore >= SCORE_TO_WIN || rightScore >= SCORE_TO_WIN) {
            gameOver = true;

            if (leftScore >= SCORE_TO_WIN) {
                // Player Wins
                pr32::audio::AudioEvent winEv{};
                winEv.type = pr32::audio::WaveType::PULSE;
                winEv.frequency = 600.0f;
                winEv.duration = 0.6f;
                winEv.volume = 0.8f;
                engine.getAudioEngine().playEvent(winEv);
            } else {
                // CPU Wins
                pr32::audio::AudioEvent loseEv{};
                loseEv.type = pr32::audio::WaveType::NOISE;
                loseEv.frequency = 200.0f;
                loseEv.duration = 0.6f;
                loseEv.volume = 0.8f;
                engine.getAudioEngine().playEvent(loseEv);
            }
        }

    }
}

void PongScene::draw(pr32::graphics::Renderer& renderer) {
    Scene::draw(renderer);

    char scoreStr[16];
    snprintf(scoreStr, sizeof(scoreStr), "%d : %d", leftScore, rightScore);
    int16_t scoreY = playAreaTop / 2 - 8;
    renderer.drawTextCentered(scoreStr, scoreY, Color::Black, 2);

    if (gameOver) {
        renderer.drawTextCentered("GAME OVER", 120, Color::White, 2);
        renderer.drawTextCentered("PRESS A TO START", 150, Color::White, 1);
    }
}

void PongScene::resetGame() {
    leftScore = 0;
    rightScore = 0;
    gameOver = false;
    ball->reset();
}

}
