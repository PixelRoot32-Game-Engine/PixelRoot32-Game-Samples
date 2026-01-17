#include "SpaceInvadersScene.h"
#include "PlayerActor.h"
#include "AlienActor.h"
#include "ProjectileActor.h"
#include "GameConstants.h"
#include "core/Engine.h"

namespace pr32 = pixelroot32;
extern pr32::core::Engine engine;

namespace spaceinvaders {

SpaceInvadersScene::SpaceInvadersScene() : player(nullptr), score(0), lives(3), gameOver(false) {
}

SpaceInvadersScene::~SpaceInvadersScene() {
    cleanup();
}

void SpaceInvadersScene::init() {
    resetGame();
}

void SpaceInvadersScene::cleanup() {
    if (player) {
        delete player;
        player = nullptr;
    }
    for (auto* alien : aliens) {
        delete alien;
    }
    aliens.clear();
    for (auto* proj : projectiles) {
        delete proj;
    }
    projectiles.clear();
}

void SpaceInvadersScene::resetGame() {
    cleanup();

    // Spawn Player
    player = new PlayerActor(PLAYER_START_X, PLAYER_START_Y);
    // Note: We don't add to base Scene::addEntity because we manage it manually 
    // to avoid MAX_ENTITIES limit issues with aliens.

    spawnAliens();

    score = 0;
    lives = 3;
    gameOver = false;
    
    stepTimer = 0;
    moveDirection = 1;
    calculateStepDelay();
}

void SpaceInvadersScene::spawnAliens() {
    for (int row = 0; row < ALIEN_ROWS; ++row) {
        AlienType type;
        if (row == 0) type = AlienType::SQUID;
        else if (row < 3) type = AlienType::CRAB;
        else type = AlienType::OCTOPUS;

        for (int col = 0; col < ALIEN_COLS; ++col) {
            float x = ALIEN_START_X + (col * ALIEN_SPACING_X);
            float y = ALIEN_START_Y + (row * ALIEN_SPACING_Y);
            
            AlienActor* alien = new AlienActor(x, y, type);
            aliens.push_back(alien);
        }
    }
}

void SpaceInvadersScene::update(unsigned long deltaTime) {
    if (gameOver) {
        if (engine.getInputManager().isButtonPressed(BTN_FIRE)) {
            resetGame();
        }
        return;
    }

    // Update Player
    if (player) {
        player->update(deltaTime);
    }

    // Update Projectiles
    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        (*it)->update(deltaTime);
        if (!(*it)->isActive()) {
            delete *it;
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }

    // Update Aliens
    updateAliens(deltaTime);
}

void SpaceInvadersScene::updateAliens(unsigned long deltaTime) {
    stepTimer += deltaTime;
    
    if (stepTimer >= stepDelay) {
        stepTimer = 0;
        
        bool edgeHit = false;
        
        // Check bounds before moving
        for (auto* alien : aliens) {
            if (!alien->isActive()) continue;
            
            if (moveDirection == 1) { // Moving Right
                if (alien->x + alien->width >= LOGICAL_WIDTH - 2) {
                    edgeHit = true;
                    break;
                }
            } else { // Moving Left
                if (alien->x <= 2) {
                    edgeHit = true;
                    break;
                }
            }
        }

        if (edgeHit) {
            moveDirection *= -1;
            for (auto* alien : aliens) {
                if (alien->isActive()) {
                    alien->move(0, ALIEN_DROP_AMOUNT);
                }
            }
        } else {
            float dx = moveDirection * ALIEN_STEP_AMOUNT_X;
            for (auto* alien : aliens) {
                if (alien->isActive()) {
                    alien->move(dx, 0);
                }
            }
        }
    }
}

int SpaceInvadersScene::getActiveAlienCount() const {
    int count = 0;
    for (auto* alien : aliens) {
        if (alien->isActive()) count++;
    }
    return count;
}

void SpaceInvadersScene::calculateStepDelay() {
    int count = getActiveAlienCount();
    if (count == 0) return;
    
    int total = ALIEN_ROWS * ALIEN_COLS;
    // Linear speed increase
    stepDelay = INITIAL_STEP_DELAY * count / total;
    
    // Clamp minimum speed (fastest)
    if (stepDelay < 50) stepDelay = 50; 
}

void SpaceInvadersScene::draw(pr32::graphics::Renderer& renderer) {
    // Clear screen
    renderer.drawFilledRectangle(0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT, pr32::graphics::Color::Black);

    if (player) {
        player->draw(renderer);
    }

    for (auto* alien : aliens) {
        alien->draw(renderer);
    }

    for (auto* proj : projectiles) {
        proj->draw(renderer);
    }

    // Simple HUD
    // ...
}

}
