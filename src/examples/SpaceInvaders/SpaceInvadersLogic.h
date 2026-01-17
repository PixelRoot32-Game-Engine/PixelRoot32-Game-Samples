#pragma once

#include <vector>
#include <cstdint>

// Phase 1: Data Architecture and Constants

namespace SpaceInvaders {

    namespace Constants {
        constexpr int LOGICAL_WIDTH = 240;
        constexpr int LOGICAL_HEIGHT = 240;
        
        // Base movement speeds (pixels per second)
        constexpr float BASE_PLAYER_SPEED = 60.0f;
        constexpr float PROJECTILE_SPEED = 120.0f;
        
        // Grid configuration
        constexpr int ALIEN_ROWS = 5;
        constexpr int ALIEN_COLS = 11;
        constexpr float ALIEN_SPACING_X = 10.0f;
        constexpr float ALIEN_SPACING_Y = 12.0f;
        constexpr float ALIEN_GRID_WIDTH = (ALIEN_COLS - 1) * ALIEN_SPACING_X + 12.0f;
        constexpr float ALIEN_START_X = (LOGICAL_WIDTH - ALIEN_GRID_WIDTH) * 0.5f;
        constexpr float ALIEN_START_Y = 40.0f;
    }

    enum class EntityType {
        PLAYER,
        ALIEN_SQUID,    // Top row (30 points)
        ALIEN_CRAB,     // Middle 2 rows (20 points)
        ALIEN_OCTOPUS,  // Bottom 2 rows (10 points)
        PROJECTILE_PLAYER,
        PROJECTILE_ENEMY,
        BUNKER,
        UFO             // Mystery ship
    };

    struct Entity {
        float x;
        float y;
        float width;
        float height;
        EntityType type;
        bool active;

        // Helper for simple AABB collision
        bool intersects(const Entity& other) const {
            if (!active || !other.active) return false;
            return (x < other.x + other.width &&
                    x + width > other.x &&
                    y < other.y + other.height &&
                    y + height > other.y);
        }
    };

    class SpaceInvadersLogic {
    public:
        SpaceInvadersLogic();
        ~SpaceInvadersLogic() = default;

        // Main integration interface
        void update(float deltaTime);
        const std::vector<Entity>& getEntities() const;

        // Input handling (Pure logic flags)
        void setInput(bool left, bool right, bool fire);

    private:
        // Game State
        std::vector<Entity> entities;
        
        // Input State
        bool inputLeft = false;
        bool inputRight = false;
        bool inputFire = false;

        // Score constants
        static constexpr int SCORE_SQUID = 30;
        static constexpr int SCORE_CRAB = 20;
        static constexpr int SCORE_OCTOPUS = 10;
        
        // Internal initialization
        void initGame();
    };

}
