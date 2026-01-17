#include "SpaceInvadersLogic.h"

namespace SpaceInvaders {

    SpaceInvadersLogic::SpaceInvadersLogic() {
        initGame();
    }

    void SpaceInvadersLogic::initGame() {
        entities.clear();

        // 1. Create Player
        Entity player;
        player.type = EntityType::PLAYER;
        player.width = 11;  // Standard sprite width
        player.height = 8;  // Standard sprite height
        player.x = (Constants::LOGICAL_WIDTH - player.width) / 2.0f;
        player.y = Constants::LOGICAL_HEIGHT - 16.0f; // Near bottom
        player.active = true;
        entities.push_back(player);

        // 2. Create Aliens (Grid)
        // Rows: 0 (Squid/Top), 1-2 (Crab/Mid), 3-4 (Octopus/Bottom)
        for (int row = 0; row < Constants::ALIEN_ROWS; ++row) {
            EntityType type;
            int scoreVal = 0;
            float w = 0, h = 8; // Height is roughly constant

            if (row == 0) {
                type = EntityType::ALIEN_SQUID;
                scoreVal = SCORE_SQUID;
                w = 8;
            } else if (row < 3) {
                type = EntityType::ALIEN_CRAB;
                scoreVal = SCORE_CRAB;
                w = 11;
            } else {
                type = EntityType::ALIEN_OCTOPUS;
                scoreVal = SCORE_OCTOPUS;
                w = 12;
            }

            for (int col = 0; col < Constants::ALIEN_COLS; ++col) {
                Entity alien;
                alien.type = type;
                alien.width = w;
                alien.height = h;
                alien.x = Constants::ALIEN_START_X + (col * Constants::ALIEN_SPACING_X);
                alien.y = Constants::ALIEN_START_Y + (row * Constants::ALIEN_SPACING_Y);
                alien.active = true;
                entities.push_back(alien);
            }
        }

        // 3. Create Bunkers (4 bunkers usually)
        // ... (To be implemented in future phases or now if needed, keeping it simple for Phase 1 structure)
    }

    void SpaceInvadersLogic::update(float deltaTime) {
        // Phase 1: Just basic structure, logic comes in Phase 2
    }

    const std::vector<Entity>& SpaceInvadersLogic::getEntities() const {
        return entities;
    }

    void SpaceInvadersLogic::setInput(bool left, bool right, bool fire) {
        inputLeft = left;
        inputRight = right;
        inputFire = fire;
    }

}
