#ifndef DOOM_FIRE_H
#define DOOM_FIRE_H
#include <stdint.h>

#define PALETTE_ANCHORS 5

#ifndef PALETTE_STEPS
#define PALETTE_STEPS 4
#endif

#define PALETTE_SIZE ((PALETTE_ANCHORS - 1) * PALETTE_STEPS + 1)

void generate_fire_palette(uint32_t out[PALETTE_SIZE]);

#endif
