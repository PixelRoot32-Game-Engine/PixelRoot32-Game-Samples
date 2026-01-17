#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include <vector>

namespace spaceinvaders {

    // Forward declarations
    class PlayerActor;
    class AlienActor;
    class ProjectileActor;
    class BunkerActor;

    struct EnemyExplosion {
        bool active;
        float x;
        float y;
        unsigned long remainingMs;
    };

    class ExplosionAnimation {
    public:
        ExplosionAnimation();

        // Starts the explosion animation at the given world position.
        void start(float startX, float startY);

        // Advances the animation based on elapsed time.
        void update(unsigned long deltaTime);

        // Draws the current frame if the animation is active.
        void draw(pixelroot32::graphics::Renderer& renderer);

        // Returns true while the animation is still playing.
        bool isActive() const;

    private:
        bool active;
        float x;
        float y;
        unsigned long timeAccumulator;
        unsigned char stepsDone;
        pixelroot32::graphics::SpriteAnimation animation;
    };

    class SpaceInvadersScene : public pixelroot32::core::Scene {
    public:
        SpaceInvadersScene();
        virtual ~SpaceInvadersScene();

        void init() override;
        void update(unsigned long deltaTime) override;
        void draw(pixelroot32::graphics::Renderer& renderer) override;

    private:
        void resetGame();
        void spawnAliens();
        void spawnBunkers();
        void cleanup();

        // Custom entity management (due to MAX_ENTITIES limit in base Scene)
        PlayerActor* player;
        std::vector<AlienActor*> aliens;
        std::vector<ProjectileActor*> projectiles;
        std::vector<BunkerActor*> bunkers;

        // Game State
        int score;
        int lives;
        bool gameOver;
        bool gameWon;

        // Horde Movement State
        float stepTimer;
        unsigned long stepDelay;
        int moveDirection; // 1: Right, -1: Left

        // Explosion and pause state
        static constexpr int MaxEnemyExplosions = 8;
        EnemyExplosion enemyExplosions[MaxEnemyExplosions];
        ExplosionAnimation playerExplosion;
        bool isPaused;

        bool fireInputReady;

        // Background music tempo state
        float currentMusicTempoFactor;

        void updateAliens(unsigned long deltaTime);
        void handleCollisions();
        void enemyShoot();
        int getActiveAlienCount() const;
        void updateMusicTempo();

        // Effect helpers
        void updateEnemyExplosions(unsigned long deltaTime);
        void drawEnemyExplosions(pixelroot32::graphics::Renderer& renderer);
        void spawnEnemyExplosion(float x, float y);

        // Player hit and respawn workflow
        void handlePlayerHit();
        void respawnPlayerUnderBunker();

        // Music helpers
    };

}
