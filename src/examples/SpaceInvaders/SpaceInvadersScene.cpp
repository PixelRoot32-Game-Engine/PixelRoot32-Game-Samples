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
using pixelroot32::graphics::TileMap;
using pixelroot32::audio::AudioEvent;
using pixelroot32::audio::WaveType;
using pixelroot32::audio::MusicNote;
using pixelroot32::audio::MusicTrack;
using pixelroot32::audio::InstrumentPreset;
using pixelroot32::audio::INSTR_PULSE_BASS;
using pixelroot32::audio::Note;

#ifdef PIXELROOT32_ENABLE_SCENE_ARENA
using pixelroot32::core::SceneArena;

static unsigned char SPACE_INVADERS_SCENE_ARENA_BUFFER[8192];

template<typename T, typename... Args>
T* arenaNew(SceneArena& arena, Args&&... args) {
    void* mem = arena.allocate(sizeof(T), alignof(T));
    if (!mem) {
        return nullptr;
    }
    return new (mem) T(static_cast<Args&&>(args)...);
}
#endif

static constexpr int TILE_SIZE = 8;
static constexpr int TILEMAP_WIDTH = DISPLAY_WIDTH / TILE_SIZE;
static constexpr int TILEMAP_HEIGHT = DISPLAY_HEIGHT / TILE_SIZE;

static const uint16_t TILE_EMPTY_BITS[] = {
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000
};

static const uint16_t TILE_STAR_BITS[] = {
    0x0000,
    0x0000,
    0x0100,
    0x0380,
    0x0100,
    0x0000,
    0x0000,
    0x0000
};

static const Sprite TILE_SPRITES[] = {
    { TILE_EMPTY_BITS, TILE_SIZE, TILE_SIZE },
    { TILE_STAR_BITS,  TILE_SIZE, TILE_SIZE }
};

static uint8_t STARFIELD_INDICES[TILEMAP_WIDTH * TILEMAP_HEIGHT];

static TileMap STARFIELD_MAP = {
    STARFIELD_INDICES,
    static_cast<uint8_t>(TILEMAP_WIDTH),
    static_cast<uint8_t>(TILEMAP_HEIGHT),
    TILE_SPRITES,
    TILE_SIZE,
    TILE_SIZE,
    static_cast<uint16_t>(sizeof(TILE_SPRITES) / sizeof(Sprite))
};

static void initStarfield() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    int total = TILEMAP_WIDTH * TILEMAP_HEIGHT;
    for (int i = 0; i < total; ++i) {
        int x = i % TILEMAP_WIDTH;
        int y = i / TILEMAP_WIDTH;
        int pattern = (x * 3 + y * 5);
        if (pattern % 11 == 0) {
            STARFIELD_INDICES[i] = 1;
        } else {
            STARFIELD_INDICES[i] = 0;
        }
    }
}

class TilemapBackground : public pr32::core::Entity {
public:
    TilemapBackground(const TileMap& map)
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC),
          tilemap(map) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        renderer.drawTileMap(tilemap, 0, 0, pr32::graphics::Color::DarkBlue);
    }

private:
    const TileMap& tilemap;
};

// Base four-note bass pattern: "tu tu tu tu"
static const InstrumentPreset BASS_INSTRUMENT = INSTR_PULSE_BASS;

static const MusicNote BGM_SLOW_NOTES[] = {
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.21f),
    pixelroot32::audio::makeRest(0.207f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.21f),
    pixelroot32::audio::makeRest(0.207f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.21f),
    pixelroot32::audio::makeRest(0.207f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.21f),
    pixelroot32::audio::makeRest(0.207f),
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

// --- WIN / GAME OVER MUSIC ---

static const MusicNote WIN_NOTES[] = {
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.15f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::E, 0.15f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::G, 0.15f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.4f), // C High ideally, but using C for safety if C_High undefined
    pixelroot32::audio::makeRest(0.1f)
};

static const MusicTrack WIN_TRACK = {
    WIN_NOTES,
    sizeof(WIN_NOTES) / sizeof(MusicNote),
    false, // No loop
    WaveType::PULSE,
    BASS_INSTRUMENT.duty
};

static const MusicNote GAME_OVER_NOTES[] = {
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::G, 0.2f),
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::E, 0.2f), // Using E instead of Eb if Eb undefined, checking later
    pixelroot32::audio::makeNote(BASS_INSTRUMENT, Note::C, 0.4f),
    pixelroot32::audio::makeRest(0.1f)
};

static const MusicTrack GAME_OVER_TRACK = {
    GAME_OVER_NOTES,
    sizeof(GAME_OVER_NOTES) / sizeof(MusicNote),
    false, // No loop
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
    : background(nullptr),
      player(nullptr),
      score(0),
      lives(3),
      gameOver(false),
      gameWon(false),
      stepTimer(0.0f),
      stepDelay(BASE_STEP_DELAY),
      moveDirection(1),
      isPaused(false),
      fireInputReady(false),
      currentMusicTempoFactor(1.0f) {

    initStarfield();
    background = new TilemapBackground(STARFIELD_MAP);
    addEntity(background);

    for (int i = 0; i < MaxEnemyExplosions; ++i) {
        enemyExplosions[i].active = false;
        enemyExplosions[i].x = 0.0f;
        enemyExplosions[i].y = 0.0f;
        enemyExplosions[i].remainingMs = 0;
    }
}

SpaceInvadersScene::~SpaceInvadersScene() {
    cleanup();
    if (background) {
        delete background;
        background = nullptr;
    }
}

void SpaceInvadersScene::init() {
 #ifdef PIXELROOT32_ENABLE_SCENE_ARENA
    arena.init(SPACE_INVADERS_SCENE_ARENA_BUFFER, sizeof(SPACE_INVADERS_SCENE_ARENA_BUFFER));
#endif
    resetGame();

    engine.getMusicPlayer().play(BGM_SLOW_TRACK);
    currentMusicTempoFactor = 1.0f;
    engine.getMusicPlayer().setTempoFactor(currentMusicTempoFactor);
}

void SpaceInvadersScene::cleanup() {
    clearEntities();

#ifndef PIXELROOT32_ENABLE_SCENE_ARENA
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
#else
    player = nullptr;
    aliens.clear();
    projectiles.clear();
    bunkers.clear();
#endif
}

void SpaceInvadersScene::resetGame() {
    cleanup();

#ifdef PIXELROOT32_ENABLE_SCENE_ARENA
    arena.reset();
#endif

    if (background) {
        addEntity(background);
    }

 #ifdef PIXELROOT32_ENABLE_SCENE_ARENA
    player = arenaNew<PlayerActor>(arena, PLAYER_START_X, PLAYER_START_Y);
#else
    player = new PlayerActor(PLAYER_START_X, PLAYER_START_Y);
#endif
    addEntity(player);

    spawnAliens();
    spawnBunkers();

    projectiles.reserve(MaxProjectiles);
    for (int i = 0; i < MaxProjectiles; ++i) {
 #ifdef PIXELROOT32_ENABLE_SCENE_ARENA
        ProjectileActor* projectile = arenaNew<ProjectileActor>(arena, 0.0f, -PROJECTILE_HEIGHT, ProjectileType::PLAYER_BULLET);
#else
        ProjectileActor* projectile = new ProjectileActor(0.0f, -PROJECTILE_HEIGHT, ProjectileType::PLAYER_BULLET);
#endif
        if (!projectile) {
            continue;
        }
        projectile->deactivate();
        projectiles.push_back(projectile);
        addEntity(projectile);
    }

    score = 0;
    lives = 3;
    gameOver = false;
    gameWon = false;
    isPaused = false;
    fireInputReady = false;

    // Clear any pending visual effects.
    for (int i = 0; i < MaxEnemyExplosions; ++i) {
        enemyExplosions[i].active = false;
        enemyExplosions[i].remainingMs = 0;
    }
    
    stepTimer = 0.0f;
    moveDirection = 1;
    stepDelay = BASE_STEP_DELAY;
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
            
 #ifdef PIXELROOT32_ENABLE_SCENE_ARENA
            AlienActor* alien = arenaNew<AlienActor>(arena, x, y, type);
#else
            AlienActor* alien = new AlienActor(x, y, type);
#endif
            if (!alien) {
                continue;
            }
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
    float gap = (DISPLAY_WIDTH - totalBunkersWidth) / (BUNKER_COUNT + 1);

    for (int i = 0; i < BUNKER_COUNT; ++i) {
        float x = gap + i * (BUNKER_WIDTH + gap);
        float y = BUNKER_Y - BUNKER_HEIGHT;
 #ifdef PIXELROOT32_ENABLE_SCENE_ARENA
        BunkerActor* bunker = arenaNew<BunkerActor>(arena, x, y, BUNKER_WIDTH, BUNKER_HEIGHT, 4);
#else
        BunkerActor* bunker = new BunkerActor(x, y, BUNKER_WIDTH, BUNKER_HEIGHT, 4);
#endif
        if (!bunker) {
            continue;
        }
        bunkers.push_back(bunker);
        addEntity(bunker);
    }
}

void SpaceInvadersScene::update(unsigned long deltaTime) {
    if (gameOver) {
        if (engine.getInputManager().isButtonPressed(BTN_FIRE)) {
            resetGame();
            engine.getMusicPlayer().play(BGM_SLOW_TRACK);
            currentMusicTempoFactor = 1.0f;
            engine.getMusicPlayer().setTempoFactor(currentMusicTempoFactor);
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

                for (auto* proj : projectiles) {
                    if (!proj->isActive()) {
                        proj->reset(px, py, ProjectileType::PLAYER_BULLET);

                        AudioEvent event{};
                        event.type = WaveType::PULSE;
                        event.frequency = 880.0f;
                        event.duration = 0.08f;
                        event.volume = 0.4f;
                        event.duty = 0.5f;
                        engine.getAudioEngine().playEvent(event);
                        break;
                    }
                }
            }
        }
    }

    updateAliens(deltaTime);

    handleCollisions();

    // Update enemy hit explosions while gameplay is running.
    updateEnemyExplosions(deltaTime);

    updateMusicTempo();
}

void SpaceInvadersScene::updateAliens(unsigned long deltaTime) {
    // Scaled time accumulation for movement sync
    float scaledDelta = static_cast<float>(deltaTime) * currentMusicTempoFactor;
    stepTimer += scaledDelta;
    
    if (stepTimer >= stepDelay) {
        stepTimer = 0.0f;
        
        bool edgeHit = false;
        
        // Check bounds before moving
        for (auto* alien : aliens) {
            if (!alien->isActive()) continue;
            
            if (moveDirection == 1) { // Moving Right
                if (alien->x + alien->width >= DISPLAY_WIDTH - 2) {
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
                    engine.getMusicPlayer().setTempoFactor(1.0f);
                    engine.getMusicPlayer().play(GAME_OVER_TRACK);
                    break;
                }
            }
        }
    }
}

void SpaceInvadersScene::handleCollisions() {
    using pixelroot32::physics::Circle;
    using pixelroot32::physics::sweepCircleVsRect;

    for (auto* proj : projectiles) {
        if (!proj->isActive()) {
            continue;
        }
        if (proj->getType() == ProjectileType::PLAYER_BULLET) {
            float radius = PROJECTILE_WIDTH * 0.5f;

            Circle startCircle;
            startCircle.x = proj->getPreviousX() + radius;
            startCircle.y = proj->getPreviousY() + PROJECTILE_HEIGHT * 0.5f;
            startCircle.radius = radius;

            Circle endCircle;
            endCircle.x = proj->x + radius;
            endCircle.y = proj->y + PROJECTILE_HEIGHT * 0.5f;
            endCircle.radius = radius;

            bool hitResolved = false;

            for (auto* alien : aliens) {
                if (!alien->isActive()) {
                    continue;
                }
                float tHit = 0.0f;
                pixelroot32::core::Rect targetBox = alien->getHitBox();
                if (sweepCircleVsRect(startCircle, endCircle, targetBox, tHit) ||
                    proj->getHitBox().intersects(targetBox)) {
                    proj->deactivate();
                    alien->kill();
                    score += alien->getScoreValue();

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

                    if (getActiveAlienCount() == 0) {
                        gameOver = true;
                        gameWon = true;
                        engine.getMusicPlayer().setTempoFactor(1.0f);
                        engine.getMusicPlayer().play(WIN_TRACK);
                    }

                    hitResolved = true;
                    break;
                }
            }
            if (!hitResolved && proj->isActive()) {
                for (auto* bunker : bunkers) {
                    if (bunker->isDestroyed()) {
                        continue;
                    }
                    float tHit = 0.0f;
                    pixelroot32::core::Rect bunkerBox = bunker->getHitBox();
                    if (sweepCircleVsRect(startCircle, endCircle, bunkerBox, tHit) ||
                        proj->getHitBox().intersects(bunkerBox)) {
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
            float radius = PROJECTILE_WIDTH * 0.5f;

            Circle startCircle;
            startCircle.x = proj->getPreviousX() + radius;
            startCircle.y = proj->getPreviousY() + PROJECTILE_HEIGHT * 0.5f;
            startCircle.radius = radius;

            Circle endCircle;
            endCircle.x = proj->x + radius;
            endCircle.y = proj->y + PROJECTILE_HEIGHT * 0.5f;
            endCircle.radius = radius;

            pixelroot32::core::Rect eBox = proj->getHitBox();
            bool handled = false;
            for (auto* bunker : bunkers) {
                if (bunker->isDestroyed()) {
                    continue;
                }
                pixelroot32::core::Rect bunkerBox = bunker->getHitBox();
                float tHit = 0.0f;
                if (sweepCircleVsRect(startCircle, endCircle, bunkerBox, tHit) ||
                    eBox.intersects(bunkerBox)) {
                    proj->deactivate();
                    bunker->applyDamage(1);
                    handled = true;
                    break;
                }
            }
            if (handled) {
                continue;
            }
            float tHitPlayer = 0.0f;
            if (sweepCircleVsRect(startCircle, endCircle, playerBox, tHitPlayer) ||
                eBox.intersects(playerBox)) {
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

    for (auto* proj : projectiles) {
        if (!proj->isActive()) {
            proj->reset(sx, sy, ProjectileType::ENEMY_BULLET);
            break;
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

void SpaceInvadersScene::draw(pr32::graphics::Renderer& renderer) {
    renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::graphics::Color::Black);
    Scene::draw(renderer);

    // Draw enemy explosions and player explosion on top of entities.
    drawEnemyExplosions(renderer);
    playerExplosion.draw(renderer);

    char buffer[32];

    std::snprintf(buffer, sizeof(buffer), "SCORE %04d", score);
    renderer.drawText(buffer, 4, 4, pr32::graphics::Color::White, 1);

    std::snprintf(buffer, sizeof(buffer), "LIVES %d", lives);
    renderer.drawText(buffer, DISPLAY_WIDTH - 70, 4, pr32::graphics::Color::White, 1);

    if (gameOver) {
        if (gameWon) {
            std::snprintf(buffer, sizeof(buffer), "YOU WIN!");
            int textY = DISPLAY_HEIGHT / 2 - 8;
            renderer.drawTextCentered(buffer, textY, pr32::graphics::Color::Green, 2);
        } else {
            std::snprintf(buffer, sizeof(buffer), "GAME OVER");
            int textY = DISPLAY_HEIGHT / 2 - 8;
            renderer.drawTextCentered(buffer, textY, pr32::graphics::Color::Red, 2);
        }

        std::snprintf(buffer, sizeof(buffer), "PRESS FIRE");
        int textY = DISPLAY_HEIGHT / 2 - 8;
        renderer.drawTextCentered(buffer, textY + 20, pr32::graphics::Color::White, 1);
    }
}

void SpaceInvadersScene::updateMusicTempo() {
    if (gameOver) return;

    float lowestY = ALIEN_START_Y;
    bool found = false;

    // Find the lowest active alien
    for (auto* alien : aliens) {
        if (alien->isActive()) {
            float y = alien->y + alien->height;
            if (y > lowestY) {
                lowestY = y;
            }
            found = true;
        }
    }

    if (!found) return;

    // Optimized Threat Factor Calculation (Zero divisions)
    // threat = (currentY - startY) * invRange
    float threatFactor = (lowestY - ALIEN_START_Y) * INV_Y_RANGE;
    
    // Clamp 0.0 - 1.0
    if (threatFactor < 0.0f) threatFactor = 0.0f;
    if (threatFactor > 1.0f) threatFactor = 1.0f;

    // Target Tempo: 1.0 (Base) to ~1.9 (Max Speed)
    float targetTempo = 1.0f + (threatFactor * 0.9f);

    // Smoothing (LERP)
    currentMusicTempoFactor += (targetTempo - currentMusicTempoFactor) * 0.05f;

    // Apply global sync factor
    engine.getMusicPlayer().setTempoFactor(currentMusicTempoFactor);
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
        if (hx < DISPLAY_WIDTH && hw > 0) {
            if (hx + hw > DISPLAY_WIDTH) {
                hw = DISPLAY_WIDTH - hx;
            }
            if (hw > 0 && hy >= 0 && hy < DISPLAY_HEIGHT) {
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
        if (vy < DISPLAY_HEIGHT && vh > 0) {
            if (vy + vh > DISPLAY_HEIGHT) {
                vh = DISPLAY_HEIGHT - vy;
            }
            if (vh > 0 && vx >= 0 && vx < DISPLAY_WIDTH) {
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
        engine.getMusicPlayer().setTempoFactor(1.0f);
        engine.getMusicPlayer().play(GAME_OVER_TRACK);
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
