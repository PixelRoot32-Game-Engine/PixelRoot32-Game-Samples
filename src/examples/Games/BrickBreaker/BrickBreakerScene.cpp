#include "BrickBreakerScene.h"
#include "EngineConfig.h"
#include "core/Engine.h"
#include "graphics/Color.h"
#include "graphics/particles/ParticlePresets.h"
#include "GameLayers.h"
#include "GameConstants.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

namespace brickbreaker {

using Color = pr32::graphics::Color;
using namespace pr32::audio;

// Define the Atari-style background music (a simple 4-note loop)
static const MusicNote ATARI_MELODY[] = {
    { Note::A, 3, 0.25f, 0.3f },
    { Note::C, 4, 0.25f, 0.3f },
    { Note::E, 4, 0.25f, 0.3f },
    { Note::G, 4, 0.25f, 0.3f }
};

BrickBreakerScene::BrickBreakerScene() {
    musicPlayer = new MusicPlayer(engine.getAudioEngine());
}

BrickBreakerScene::~BrickBreakerScene() {
    delete musicPlayer;
}

void BrickBreakerScene::setupMusic() {
    bgmTrack.notes = ATARI_MELODY;
    bgmTrack.count = sizeof(ATARI_MELODY) / sizeof(MusicNote);
    bgmTrack.loop = true;
    bgmTrack.channelType = WaveType::TRIANGLE; // Triangle wave for a softer Atari feel
    bgmTrack.duty = 0.5f;
    
    musicPlayer->play(bgmTrack);
    musicPlayer->setTempoFactor(1.2f);
}

void BrickBreakerScene::init() {
    pr32::graphics::setPalette(pr32::graphics::PaletteType::GBC);

    clearEntities(); 

    int sw = engine.getRenderer().getWidth();
    int sh = engine.getRenderer().getHeight();

    paddle = new PaddleActor(sw/2.0f - PADDLE_W/2.0f, sh - 20.0f, PADDLE_W, PADDLE_H, sw);

    ball = new BallActor(sw/2.0f, sh - 30.0f, 0.0f, BALL_SIZE);
    ball->setWorldSize(sw, sh);
    ball->setLimits(pr32::core::LimitRect(0, (int)BORDER_TOP, sw, sh));
    ball->attachTo(paddle);
    
    explosionEffect = new pr32::graphics::particles::ParticleEmitter(100,100, pr32::graphics::particles::ParticlePresets::Explosion);

    addEntity(paddle);
    addEntity(ball);
    addEntity(explosionEffect);

    lblGameOver = new pr32::graphics::ui::UILabel("GAME OVER", 0, 120, Color::White, 2);
    lblGameOver->centerX(sw); 
    lblGameOver->setVisible(false);
    addEntity(lblGameOver);

    lblStartMessage = new pr32::graphics::ui::UILabel("PRESS START", 0, 150, Color::White, 1);
    lblStartMessage->centerX(sw);
    lblStartMessage->setVisible(true);
    addEntity(lblStartMessage);

    score = 0;
    lives = 3;
    currentLevel = 1;
    gameStarted = false;
    gameOver = false;

    loadLevel(currentLevel);
    resetBall();
    setupMusic();
}

void BrickBreakerScene::addScore(int score) {
    this->score += score;
}

void BrickBreakerScene::loadLevel(int level) {
    for(auto* b : bricks) {
        removeEntity(b);
    }
    bricks.clear(); 

    int cols = 7;
    int spacingX = 32;
    int spacingY = 14;
    int offsetX = (engine.getRenderer().getWidth() - (cols * spacingX)) / 2 + 2;

    int rows = 3 + (level / 2); 
    if (rows > 7) rows = 7;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            
            bool shouldCreate = true;
            
            if (level % 2 == 0 && (row + col) % 2 == 0) shouldCreate = false;
            
            if (level >= 3 && col >= 2 && col <= 4 && row == 1) shouldCreate = false;

            if (shouldCreate) {
                int posX = offsetX + (col * spacingX);
                int posY = 40 + (row * spacingY);
                
                int brickHP = 1;

                if (level == 1) {
                    brickHP = 1;
                } else {
                    int baseHP = (rows - row); 
                    brickHP = baseHP + (level / 2) - 1;
                }

                if (brickHP > 4) brickHP = 4;
                if (brickHP < 1) brickHP = 1;

                BrickActor* b = new BrickActor(posX, posY, brickHP);
                bricks.push_back(b);
                addEntity(b);
            }
        }
    }
}

void BrickBreakerScene::resetBall() {
    ball->reset(paddle);
    gameStarted = false;
}

void BrickBreakerScene::update(unsigned long deltaTime) {
    // 1. Input Processing
    if (gameOver) {
        if (engine.getInputManager().isButtonPressed(BTN_START)) {
            engine.getAudioEngine().playEvent(sfx::START_GAME);
            init();
        }
        lblGameOver->setVisible(true);
        lblStartMessage->setText("PRESS START TO RETRY");
        lblStartMessage->centerX(engine.getRenderer().getWidth());
        lblStartMessage->setVisible(true);
        
        if (musicPlayer->isPlaying()) musicPlayer->stop();
    } else if (!gameStarted) {
        if (engine.getInputManager().isButtonPressed(BTN_START)) {
            gameStarted = true;
            ball->launch(120.0f, -120.0f);
            engine.getAudioEngine().playEvent(sfx::START_GAME);
        }
        lblStartMessage->setVisible(true);
        lblGameOver->setVisible(false);
    } else {
        lblStartMessage->setVisible(false);
        lblGameOver->setVisible(false);
    }

    // 2. Physics & Entity Update
    Scene::update(deltaTime);

    // 3. Game Logic
    if (!gameOver && gameStarted) {
        bool levelCleared = true;
        for (auto& b : bricks) {
            if (b->active) {
                 levelCleared = false; 
                 break; 
            }
        }

        if (levelCleared) {
            currentLevel++;
            loadLevel(currentLevel);
            resetBall();
            engine.getAudioEngine().playEvent(sfx::START_GAME);
        }

        if (ball->isLaunched && ball->getWorldCollisionInfo().bottom) {
            lives--;
            engine.getAudioEngine().playEvent(sfx::LIFE_LOST);
            
            if (lives <= 0) {
                gameOver = true;
            }

            resetBall();
        }
    }
}

void BrickBreakerScene::draw(pr32::graphics::Renderer& renderer) {
    // Draw HUD first
    char scoreStr[16];
    snprintf(scoreStr, sizeof(scoreStr), "SCORE %d", score);
    renderer.drawText(scoreStr, 10, 5, Color::White, 1);

    char levelStr[16];
    snprintf(levelStr, sizeof(levelStr), "LVL %d", currentLevel);
    renderer.drawText(levelStr, 10, 15, Color::White, 1);

    const int rectSize = 8;
    const int spacing = 4;      
    const int marginRight = 6; 
    const int posY = 6;
    
    int currentWidth = renderer.getWidth(); 

    for (int i = 0; i < lives; i++) {
        int x = currentWidth - marginRight - (i + 1) * (rectSize + spacing);
        
        renderer.drawFilledRectangle(x, posY, rectSize, rectSize, Color::Red);
    }

    // Delegate entity drawing to Scene (which calls entity->draw)
    // This fixes the antipattern of manually drawing bricks here
    Scene::draw(renderer);
}

}
