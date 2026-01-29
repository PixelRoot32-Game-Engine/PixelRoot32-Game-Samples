#pragma once
#include "physics/CollisionTypes.h"

namespace metroidvania {

namespace pr = pixelroot32;

/**
 * Collision layers for Actor-vs-Actor collisions (used by the engine CollisionSystem).
 * Platform collision is handled manually via PlatformRect list because the platform
 * layer is a tilemap, not Actors; the engine only detects collisions between Actors.
 */
namespace Layers {
    const pr::physics::CollisionLayer PLAYER = 1 << 0;
    const pr::physics::CollisionLayer ENEMY  = 1 << 1;  // for future use (enemies, projectiles, pickups)
}

} // namespace metroidvania
