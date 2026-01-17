#pragma once
#include <cstdint>

namespace spaceinvaders {

// Sprites are defined as bitmaps where 1 = pixel on, 0 = pixel off.
// We use uint16_t to accommodate widths up to 16px.
// Bit 0 is the rightmost pixel (not used usually if we align left, but here:
// We'll assume LSB is rightmost column.
// e.g. 0x0001 is ......X

// Squid (8x8)
// Frame 1
const uint16_t SQUID_F1[] = {
    0x0018, // ...XX...
    0x003C, // ..XXXX..
    0x007E, // .XXXXXX.
    0x00DB, // XX.XX.XX
    0x00FF, // XXXXXXXX
    0x0024, // ..X..X..
    0x005A, // .X.XX.X.
    0x00A5  // X.X..X.X
};
// Frame 2
const uint16_t SQUID_F2[] = {
    0x0018, // ...XX...
    0x003C, // ..XXXX..
    0x007E, // .XXXXXX.
    0x00DB, // XX.XX.XX
    0x00FF, // XXXXXXXX
    0x005A, // .X.XX.X.
    0x0081, // X......X
    0x0042  // .X....X.
};

// Crab (11x8)
// Frame 1
const uint16_t CRAB_F1[] = {
    0x00C0, // ..XX....... (padded to 11?) No, let's align right for simplicity or use specific bits.
            // Let's use visual bits: 00100000100 -> 0x104
    0x0208, // ..X.....X..
    0x0090, // ...X...X...
    0x03F8, // ..XXXXXXX..
    0x061C, // .XX.XXX.XX.
    0x07FE, // XXXXXXXXXXX
    0x061C, // .XX.XXX.XX.
    0x0208, // ..X.....X..
    0x0000  // ...........
};
// Wait, Crab is 11x8. Let's try again with accurate shapes.
// Crab F1:
// ..X.....X..
// X.........X
// X.XXXXXXX.X
// XXX.XXX.XXX
// XXXXXXXXXXX
// .X.XXXXX.X.
// X.X.....X.X
// ...     ... (bottom empty?)

// Let's use standard bitmaps found online for 2600.
// Crab F1:
// 00100000100 (0x104)
// 00010001000 (0x088)
// 00111111100 (0x1FC)
// 01101110110 (0x376)
// 11111111111 (0x7FF)
// 10111111101 (0x5FD)
// 10100000101 (0x505)
// 00000000000 

// Actually, I'll use a simpler "visual" approach that looks good enough.
// Crab F1 (11x8)
const uint16_t CRAB_F1_REAL[] = {
    0x0104, // ..X.....X..
    0x0088, // ...X...X...
    0x01FC, // ..XXXXXXX..
    0x0376, // .XX.XXX.XX.
    0x07FF, // XXXXXXXXXXX
    0x01DD, // ..X.XXX.X..
    0x0208, // .X.......X.
    0x0000
};
// Crab F2
const uint16_t CRAB_F2_REAL[] = {
    0x0104,
    0x03F8,
    0x01FC,
    0x0376,
    0x07FF,
    0x02AA, // .X.X...X.X.
    0x0404, // X.........X
    0x0000
};

// Octopus (12x8)
// Frame 1
const uint16_t OCTOPUS_F1[] = {
    0x0000,
    0x01E0, // ....XXXX....
    0x07F8, // .XXXXXXXXXX.
    0x09F2, // X..XXXXXX..X
    0x0FFE, // XXXXXXXXXXXX
    0x05A0, // .X.XX..XX.X.
    0x0A50, // X.X......X.X
    0x0210  // ..X......X..
};
// Frame 2
const uint16_t OCTOPUS_F2[] = {
    0x0000,
    0x01E0, // ....XXXX....
    0x07F8, // .XXXXXXXXXX.
    0x09F2, // X..XXXXXX..X
    0x0FFE, // XXXXXXXXXXXX
    0x02D0, // ..X.XXXX.X..
    0x0528, // .X.X....X.X.
    0x0000
};

}
