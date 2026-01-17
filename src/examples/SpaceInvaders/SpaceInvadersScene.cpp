#include "SpaceInvadersScene.h"
#include "PlayerActor.h"
#include "AlienActor.h"
#include "ProjectileActor.h"
#include "BunkerActor.h"
#include "GameConstants.h"
#include "core/Engine.h"
#include "audio/AudioTypes.h"
#include "audio/AudioMusicTypes.h"
#include <cstdlib>
#include <cstdio>

namespace pr32 = pixelroot32;
extern pr32::core::Engine engine;

namespace spaceinvaders {

using pixelroot32::graphics::Sprite;
using pixelroot32::graphics::SpriteAnimationFrame;
using pixelroot32::audio::AudioEvent;
using pixelroot32::audio::WaveType;
using pixelroot32::audio::MusicNote;
using pixelroot32::audio::MusicTrack;
using pixelroot32::audio::InstrumentPreset;
using pixelroot32::audio::INSTR_PULSE_BASS;
using pixelroot32::audio::Note;

// Base four-note bass pattern: "tu tu tu tu"
static const InstrumentPreset BASS_INSTRUMENT = INSTR_PULSE_BASS;

static const MusicNote BGM_SLOW_NOTES[] = {
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.18f),
    pixelroot32::audio::makeRest(0.10f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.18f),
    pixelroot32::audio::makeRest(0.10f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.18f),
    pixelroot32::audio::makeRest(0.10f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.18f),
    pixelroot32::audio::makeRest(0.10f),
};

static const MusicNote BGM_MEDIUM_NOTES[] = {
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.12f),
    pixelroot32::audio::makeRest(0.06f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.12f),
    pixelroot32::audio::makeRest(0.06f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.12f),
    pixelroot32::audio::makeRest(0.06f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.12f),
    pixelroot32::audio::makeRest(0.06f),
};

static const MusicNote BGM_FAST_NOTES[] = {
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.08f),
    pixelroot32::audio::makeRest(0.04f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.08f),
    pixelroot32::audio::makeRest(0.04f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.08f),
    pixelroot32::audio::makeRest(0.04f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.08f),
    pixelroot32::audio::makeRest(0.04f),
};

static const MusicTrack BGM_SLOW_TRACK = {
    BGM_SLOW_NOTES,
    sizeof(BGM_SLOW_NOTES) / sizeof(MusicNote),
    true,
    WaveType::PULSE,
    BASS_INSTRUMENT.duty
};

static const MusicTrack BGM_MEDIUM_TRACK = {
    BGM_MEDIUM_NOTES,
    sizeof(BGM_MEDIUM_NOTES) / sizeof(MusicNote),
    true,
    WaveType::PULSE,
    BASS_INSTRUMENT.duty
};

static const MusicTrack BGM_FAST_TRACK = {
    BGM_FAST_NOTES,
    sizeof(BGM_FAST_NOTES) / sizeof(MusicNote),
    true,
    WaveType::PULSE,
    BASS_INSTRUMENT.duty
};

// Simple 8x8 explosion sprites for the player explosion animation.
// Frames are intentionally small and cheap to draw on ESP32.
static const uint16_t PLAYER_EXPLOSION_F1_BITS[] = {
    0b00011000,
    0b00111100,
    0b01111110,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,
    0b00011000
};

static const uint16_t PLAYER_EXPLOSION_F2_BITS[] = {
    0b00100100,
    0b01011010,
    0b10111101,
    0b11111111,
    0b11111111,
    0b10111101,
    0b01011010,
    0b00100100
};

static const uint16_t PLAYER_EXPLOSION_F3_BITS[] = {
    0b00000000,
    0b00100100,
    0b01000010,
    0b10011001,
    0b10011001,
    0b01000010,
    0b00100100,
    0b00000000
};

static const Sprite PLAYER_EXPLOSION_F1 = { PLAYER_EXPLOSION_F1_BITS, 8, 8 };
static const Sprite PLAYER_EXPLOSION_F2 = { PLAYER_EXPLOSION_F2_BITS, 8, 8 };
static const Sprite PLAYER_EXPLOSION_F3 = { PLAYER_EXPLOSION_F3_BITS, 8, 8 };

static const SpriteAnimationFrame PLAYER_EXPLOSION_FRAMES[] = {
    { &PLAYER_EXPLOSION_F1, nullptr },
    { &PLAYER_EXPLOSION_F2, nullptr },
    { &PLAYER_EXPLOSION_F3, nullptr }
};

ExplosionAnimation::ExplosionAnimation()
    : active(false), x(0.0f), y(0.0f), timeAccumulator(0), stepsDone(0) {
    animation.frames = PLAYER_EXPLOSION_FRAMES;
    animation.frameCount = static_cast<uint8_t>(sizeof(PLAYER_EXPLOSION_FRAMES) / sizeof(SpriteAnimationFrame));
    animation.current = 0;
}

void ExplosionAnimation::start(float startX, float startY) {
    // Begin playing the explosion animation from the first frame.
    x = startX;
    y = startY;
    timeAccumulator = 0;
    stepsDone = 0;
    animation.reset();
    active = true;
}

void ExplosionAnimation::update(unsigned long deltaTime) {
    if (!active) {
        return;
    }

    // Advance animation frames using a small fixed timestep.
    const unsigned long frameTimeMs = 60;
    timeAccumulator += deltaTime;

    while (timeAccumulator >= frameTimeMs && active) {
        timeAccumulator -= frameTimeMs;

        // Step only while there are remaining frames; do not loop.
        if (stepsDone + 1 < animation.frameCount) {
            animation.step();
            stepsDone++;
        } else {
            active = false;
        }
    }
}

void ExplosionAnimation::draw(pr32::graphics::Renderer& renderer) {
    if (!active) {
        return;
    }

    const Sprite* sprite = animation.getCurrentSprite();
    if (!sprite) {
        return;
    }

    const int drawX = static_cast<int>(x);
    const int drawY = static_cast<int>(y);

    renderer.drawSprite(*sprite, drawX, drawY, pr32::graphics::Color::White);
}

bool ExplosionAnimation::isActive() const {
    return active;
}

SpaceInvadersScene::SpaceInvadersScene()
    : player(nullptr),
      score(0),
      lives(3),
      gameOver(false),
      stepTimer(0),
      stepDelay(0),
      moveDirection(1),
      isPaused(false),
      fireInputReady(false),
      currentMusicTempoFactor(0.0f) {

    // Initialize enemy explosion slots as inactive.
    for (int i = 0; i < MaxEnemyExplosions; ++i) {
        enemyExplosions[i].active = false;
        enemyExplosions[i].x = 0.0f;
        enemyExplosions[i].y = 0.0f;
        enemyExplosions[i].remainingMs = 0;
    }
}

SpaceInvadersScene::~SpaceInvadersScene() {
    cleanup();
}

void SpaceInvadersScene::init() {
    resetGame();

    engine.getMusicPlayer().play(BGM_SLOW_TRACK);
    currentMusicTempoFactor = 0.0f;
}

void SpaceInvadersScene::cleanup() {
    clearEntities();

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
    for (auto* bunker : bunkers) {
        delete bunker;
    }
    bunkers.clear();
}

void SpaceInvadersScene::resetGame() {
    cleanup();

    // Spawn Player
    player = new PlayerActor(PLAYER_START_X, PLAYER_START_Y);
    addEntity(player);

    spawnAliens();
    spawnBunkers();

    score = 0;
    lives = 3;
    gameOver = false;
    isPaused = false;
    fireInputReady = false;

    // Clear any pending visual effects.
    for (int i = 0; i < MaxEnemyExplosions; ++i) {
        enemyExplosions[i].active = false;
        enemyExplosions[i].remainingMs = 0;
    }
    
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
            addEntity(alien);
        }
    }
}

void SpaceInvadersScene::spawnBunkers() {
    if (BUNKER_COUNT <= 0) {
        return;
    }

    float totalBunkersWidth = BUNKER_COUNT * BUNKER_WIDTH;
    float gap = (LOGICAL_WIDTH - totalBunkersWidth) / (BUNKER_COUNT + 1);

    for (int i = 0; i < BUNKER_COUNT; ++i) {
        float x = gap + i * (BUNKER_WIDTH + gap);
        float y = BUNKER_Y - BUNKER_HEIGHT;
        BunkerActor* bunker = new BunkerActor(x, y, BUNKER_WIDTH, BUNKER_HEIGHT, 4);
        bunkers.push_back(bunker);
        addEntity(bunker);
    }
}

void SpaceInvadersScene::update(unsigned long deltaTime) {
    if (gameOver) {
        if (engine.getInputManager().isButtonPressed(BTN_FIRE)) {
            resetGame();
            engine.getMusicPlayer().play(BGM_SLOW_TRACK);
            currentMusicTempoFactor = 0.0f;
        }
        return;
    }

    if (isPaused) {
        // While paused, only update visual effects (explosion, enemy impacts).
        playerExplosion.update(deltaTime);
        updateEnemyExplosions(deltaTime);

        // When the player explosion completes, resume gameplay and respawn.
        if (!playerExplosion.isActive()) {
            respawnPlayerUnderBunker();
            isPaused = false;
        }
        return;
    }

    Scene::update(deltaTime);

    if (player) {
        if (!fireInputReady) {
            if (!player->isFireDown()) {
                fireInputReady = true;
            }
        }

        if (fireInputReady && player->wantsToShoot()) {
            bool hasPlayerBullet = false;
            for (auto* proj : projectiles) {
                if (proj->isActive() && proj->getType() == ProjectileType::PLAYER_BULLET) {
                    hasPlayerBullet = true;
                    break;
                }
            }
            if (!hasPlayerBullet) {
                float px = player->x + (PLAYER_WIDTH - PROJECTILE_WIDTH) / 2.0f;
                float py = player->y - PROJECTILE_HEIGHT;
                ProjectileActor* bullet = new ProjectileActor(px, py, ProjectileType::PLAYER_BULLET);
                projectiles.push_back(bullet);
                addEntity(bullet);

                AudioEvent event{};
                event.type = WaveType::PULSE;
                event.frequency = 880.0f;
                event.duration = 0.08f;
                event.volume = 0.4f;
                event.duty = 0.5f;
                engine.getAudioEngine().playEvent(event);
            }
        }
    }

    for (auto it = projectiles.begin(); it != projectiles.end(); ) {
        ProjectileActor* proj = *it;
        if (!proj->isActive()) {
            removeEntity(proj);
            delete proj;
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }

    updateAliens(deltaTime);

    handleCollisions();

    // Update enemy hit explosions while gameplay is running.
    updateEnemyExplosions(deltaTime);

    updateMusicTempo();
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

        enemyShoot();

        if (!gameOver && player) {
            for (auto* alien : aliens) {
                if (!alien->isActive()) {
                    continue;
                }
                float bottom = alien->y + alien->height;
                if (bottom >= player->y) {
                    lives = 0;
                    gameOver = true;
                    break;
                }
            }
        }
    }
}

void SpaceInvadersScene::handleCollisions() {
    for (auto* proj : projectiles) {
        if (!proj->isActive()) {
            continue;
        }
        if (proj->getType() == ProjectileType::PLAYER_BULLET) {
            pixelroot32::core::Rect pBox = proj->getHitBox();
            for (auto* alien : aliens) {
                if (!alien->isActive()) {
                    continue;
                }
                if (pBox.intersects(alien->getHitBox())) {
                    proj->deactivate();
                    alien->kill();
                    score += alien->getScoreValue();
                    calculateStepDelay();
                    float ex = alien->x + alien->width * 0.5f;
                    float ey = alien->y + alien->height * 0.5f;
                    spawnEnemyExplosion(ex, ey);

                    AudioEvent event{};
                    event.type = WaveType::NOISE;
                    event.frequency = 600.0f;
                    event.duration = 0.12f;
                    event.volume = 0.6f;
                    event.duty = 0.5f;
                    engine.getAudioEngine().playEvent(event);

                    break;
                }
            }
            if (proj->isActive()) {
                for (auto* bunker : bunkers) {
                    if (bunker->isDestroyed()) {
                        continue;
                    }
                    if (pBox.intersects(bunker->getHitBox())) {
                        proj->deactivate();
                        bunker->applyDamage(1);
                        break;
                    }
                }
            }
        }
    }

    if (!player) {
        return;
    }

    pixelroot32::core::Rect playerBox = player->getHitBox();
    for (auto* proj : projectiles) {
        if (!proj->isActive()) {
            continue;
        }
        if (proj->getType() == ProjectileType::ENEMY_BULLET) {
            pixelroot32::core::Rect eBox = proj->getHitBox();
            bool handled = false;
            for (auto* bunker : bunkers) {
                if (bunker->isDestroyed()) {
                    continue;
                }
                if (eBox.intersects(bunker->getHitBox())) {
                    proj->deactivate();
                    bunker->applyDamage(1);
                    handled = true;
                    break;
                }
            }
            if (handled) {
                continue;
            }
            if (eBox.intersects(playerBox)) {
                proj->deactivate();
                handlePlayerHit();
                break;
            }
        }
    }
}

void SpaceInvadersScene::enemyShoot() {
    std::vector<AlienActor*> bottomAliens;
    for (auto* candidate : aliens) {
        if (!candidate->isActive()) {
            continue;
        }
        bool hasBelow = false;
        for (auto* other : aliens) {
            if (!other->isActive()) {
                continue;
            }
            if (other->x == candidate->x && other->y > candidate->y) {
                hasBelow = true;
                break;
            }
        }
        if (!hasBelow) {
            bottomAliens.push_back(candidate);
        }
    }

    if (bottomAliens.empty()) {
        return;
    }

    int activeEnemyBullets = 0;
    for (auto* proj : projectiles) {
        if (proj->isActive() && proj->getType() == ProjectileType::ENEMY_BULLET) {
            activeEnemyBullets++;
        }
    }
    if (activeEnemyBullets >= 4) {
        return;
    }

    int total = ALIEN_ROWS * ALIEN_COLS;
    int alive = getActiveAlienCount();
    if (alive <= 0) {
        return;
    }

    float t = 1.0f - static_cast<float>(alive) / static_cast<float>(total);
    int minChance = 8;
    int maxChance = 30;
    int chance = minChance + static_cast<int>(t * (maxChance - minChance));
    if (chance < minChance) chance = minChance;
    if (chance > maxChance) chance = maxChance;

    int roll = std::rand() % 100;
    if (roll >= chance) {
        return;
    }

    int r = std::rand();
    if (r < 0) {
        r = -r;
    }
    std::size_t index = static_cast<std::size_t>(r) % bottomAliens.size();
    AlienActor* shooter = bottomAliens[index];

    float sx = shooter->x + shooter->width / 2.0f;
    float sy = shooter->y + shooter->height;

    ProjectileActor* bullet = new ProjectileActor(sx, sy, ProjectileType::ENEMY_BULLET);
    projectiles.push_back(bullet);
    addEntity(bullet);
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
    stepDelay = INITIAL_STEP_DELAY * count / total;
    if (stepDelay < MIN_STEP_DELAY) stepDelay = MIN_STEP_DELAY; 
}

void SpaceInvadersScene::draw(pr32::graphics::Renderer& renderer) {
    renderer.drawFilledRectangle(0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT, pr32::graphics::Color::Black);
    Scene::draw(renderer);

    // Draw enemy explosions and player explosion on top of entities.
    drawEnemyExplosions(renderer);
    playerExplosion.draw(renderer);

    char buffer[32];

    std::snprintf(buffer, sizeof(buffer), "SCORE %04d", score);
    renderer.drawText(buffer, 4, 4, pr32::graphics::Color::White, 1);

    std::snprintf(buffer, sizeof(buffer), "LIVES %d", lives);
    renderer.drawText(buffer, LOGICAL_WIDTH - 70, 4, pr32::graphics::Color::White, 1);

    if (gameOver) {
        std::snprintf(buffer, sizeof(buffer), "GAME OVER");
        int textY = LOGICAL_HEIGHT / 2 - 8;
        renderer.drawTextCentered(buffer, textY, pr32::graphics::Color::Red, 2);

        std::snprintf(buffer, sizeof(buffer), "PRESS FIRE");
        renderer.drawTextCentered(buffer, textY + 20, pr32::graphics::Color::White, 1);
    }
}

void SpaceInvadersScene::updateMusicTempo() {
    int total = ALIEN_ROWS * ALIEN_COLS;
    int alive = getActiveAlienCount();
    if (alive <= 0) {
        engine.getMusicPlayer().stop();
        return;
    }

    float t = 1.0f - static_cast<float>(alive) / static_cast<float>(total);

    float newFactor;
    if (t < 0.33f) {
        newFactor = 0.0f;
    } else if (t < 0.66f) {
        newFactor = 1.0f;
    } else {
        newFactor = 2.0f;
    }

    if (newFactor == currentMusicTempoFactor) {
        return;
    }

    currentMusicTempoFactor = newFactor;

    if (currentMusicTempoFactor == 0.0f) {
        engine.getMusicPlayer().play(BGM_SLOW_TRACK);
    } else if (currentMusicTempoFactor == 1.0f) {
        engine.getMusicPlayer().play(BGM_MEDIUM_TRACK);
    } else {
        engine.getMusicPlayer().play(BGM_FAST_TRACK);
    }
}

void SpaceInvadersScene::updateEnemyExplosions(unsigned long deltaTime) {
    for (int i = 0; i < MaxEnemyExplosions; ++i) {
        EnemyExplosion& e = enemyExplosions[i];
        if (!e.active) {
            continue;
        }

        if (deltaTime >= e.remainingMs) {
            e.active = false;
            e.remainingMs = 0;
        } else {
            e.remainingMs -= deltaTime;
        }
    }
}

void SpaceInvadersScene::drawEnemyExplosions(pr32::graphics::Renderer& renderer) {
    using Color = pr32::graphics::Color;

    for (int i = 0; i < MaxEnemyExplosions; ++i) {
        const EnemyExplosion& e = enemyExplosions[i];
        if (!e.active) {
            continue;
        }

        int cx = static_cast<int>(e.x);
        int cy = static_cast<int>(e.y);

        int hx = cx - 2;
        int hw = 5;
        int hy = cy;

        if (hx < 0) {
            hw += hx;
            hx = 0;
        }
        if (hx < LOGICAL_WIDTH && hw > 0) {
            if (hx + hw > LOGICAL_WIDTH) {
                hw = LOGICAL_WIDTH - hx;
            }
            if (hw > 0 && hy >= 0 && hy < LOGICAL_HEIGHT) {
                renderer.drawFilledRectangle(hx, hy, hw, 1, Color::White);
            }
        }

        int vx = cx;
        int vy = cy - 2;
        int vh = 5;

        if (vy < 0) {
            vh += vy;
            vy = 0;
        }
        if (vy < LOGICAL_HEIGHT && vh > 0) {
            if (vy + vh > LOGICAL_HEIGHT) {
                vh = LOGICAL_HEIGHT - vy;
            }
            if (vh > 0 && vx >= 0 && vx < LOGICAL_WIDTH) {
                renderer.drawFilledRectangle(vx, vy, 1, vh, Color::White);
            }
        }
    }
}

void SpaceInvadersScene::spawnEnemyExplosion(float x, float y) {
    // Reuse the first available slot to avoid dynamic allocations.
    for (int i = 0; i < MaxEnemyExplosions; ++i) {
        EnemyExplosion& e = enemyExplosions[i];
        if (!e.active) {
            e.active = true;
            e.x = x;
            e.y = y;
            e.remainingMs = 200;
            return;
        }
    }
}

void SpaceInvadersScene::handlePlayerHit() {
    if (lives > 0) {
        lives -= 1;
    }

    AudioEvent event{};
    event.type = WaveType::NOISE;
    event.frequency = 400.0f;
    event.duration = 0.18f;
    event.volume = 0.7f;
    event.duty = 0.5f;
    engine.getAudioEngine().playEvent(event);

    if (lives <= 0) {
        gameOver = true;
        return;
    }

    if (!player) {
        return;
    }

    // Start the explosion animation at the player's current position.
    float ex = player->x;
    float ey = player->y;
    playerExplosion.start(ex, ey);

    // Hide the player while the explosion is playing.
    player->setVisible(false);

    // Pause gameplay logic while keeping rendering active.
    isPaused = true;
}

void SpaceInvadersScene::respawnPlayerUnderBunker() {
    if (!player) {
        return;
    }

    // Choose the first non-destroyed bunker as a respawn anchor.
    BunkerActor* targetBunker = nullptr;
    for (auto* bunker : bunkers) {
        if (!bunker->isDestroyed()) {
            targetBunker = bunker;
            break;
        }
    }

    float newX = PLAYER_START_X;

    if (targetBunker) {
        newX = targetBunker->x + (targetBunker->width - PLAYER_WIDTH) * 0.5f;
    }

    // Reset player position and make it visible again.
    player->x = newX;
    player->setVisible(true);
}

}
