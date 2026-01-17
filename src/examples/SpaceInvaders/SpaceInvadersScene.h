#pragma once
#include "core/Scene.h"
#include "graphics/Renderer.h"
#include <vector>

namespace spaceinvaders {

    // Forward declarations
    class PlayerActor;
    class AlienActor;
    class ProjectileActor;

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
        void cleanup();

        // Custom entity management (due to MAX_ENTITIES limit in base Scene)
        PlayerActor* player;
        std::vector<AlienActor*> aliens;
        std::vector<ProjectileActor*> projectiles;

        // Game State
        int score;
        int lives;
        bool gameOver;

        // Horde Movement State
        unsigned long stepTimer;
        unsigned long stepDelay;
        int moveDirection; // 1: Right, -1: Left

        void updateAliens(unsigned long deltaTime);
        int getActiveAlienCount() const;
        void calculateStepDelay();
    };

}
