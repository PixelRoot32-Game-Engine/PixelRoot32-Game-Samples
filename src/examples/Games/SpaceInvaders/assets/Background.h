#ifndef BACKGROUND_ASSETS_H
#define BACKGROUND_ASSETS_H

#include <stdint.h>

/**
 * Space Invaders starfield background.
 * Pattern generated in code (no Tilemap Editor).
 */

namespace background_assets {

    /** Number of star positions in the pattern */
    extern const int STAR_COUNT;

    /** Star X coordinates (0..DISPLAY_WIDTH-1) */
    extern const uint8_t STAR_X[];

    /** Star Y coordinates (0..DISPLAY_HEIGHT-1) */
    extern const uint8_t STAR_Y[];

    /**
     * Optional init. Kept for compatibility; no-op.
     */
    void init();

} // namespace background_assets

#endif // BACKGROUND_ASSETS_H
