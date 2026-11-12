#include "../../include/render.h"
#include "../module.h"
#include "doom-fire.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint8_t *fire_grid;
  size_t fire_size;
  uint32_t w, h;
  uint32_t palette[PALETTE_SIZE];
} FireState;

typedef struct {
  const char *name;
  uint8_t r, g, b;
} Anchor;

static const Anchor ANCHORS[PALETTE_ANCHORS] = {
  {"black", 0x00, 0x00, 0x00},
  {"red", 0xFF, 0x00, 0x00},
  {"orange", 0xFF, 0x80, 0x00},
  {"yellow", 0xFF, 0xFF, 0x00},
  {"white", 0xFF, 0xFF, 0xFF},
};

static inline uint8_t lerp_u8(uint8_t a, uint8_t b, float t){
  return (uint8_t)(a + (b - a) * t + 0.5f);
}

static inline uint32_t u8_to_u32_color(uint8_t r, uint8_t g, uint8_t b){
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void generate_fire_palette(uint32_t out[PALETTE_SIZE]){
  out[0] = u8_to_u32_color(ANCHORS[0].r, ANCHORS[0].g, ANCHORS[0].b);
  int idx = 1;
  for(int seg = 0; seg < PALETTE_ANCHORS - 1; seg++){
    const Anchor *a = &ANCHORS[seg];
    const Anchor *b = &ANCHORS[seg + 1];

    for(int step = 0; step < PALETTE_STEPS; step++){
      float t = ((float)step + 0.5f) / (float)PALETTE_STEPS;

      uint8_t r = lerp_u8(a->r, b->r, t);
      uint8_t g = lerp_u8(a->g, b->g, t);
      uint8_t bl = lerp_u8(a->b, b->b, t);

      out[idx++] = u8_to_u32_color(r, g, bl);
    }
  }
}

void init(Module *self){
  FireState *fs = calloc(1, sizeof(FireState));
  if (!fs)
    return;
  fs->w = self->w;
  fs->h = self->h;
  fs->fire_size = fs->w * fs->h;
  fs->fire_grid = calloc(fs->fire_size, sizeof(uint32_t));
  generate_fire_palette(fs->palette);

  for(uint32_t x = 0; x < fs->w; x++)
    fs->fire_grid[(fs->h - 1) * fs->w + x] = PALETTE_SIZE - 1;

  self->state = fs;
}

void update(Module *self){
  FireState *fs = self->state;
  for(uint32_t x = 0; x < fs->w; x++){
    for(uint32_t y = 1; y < fs->h; y++){
      uint32_t src = y * fs->w + x;
      uint8_t src_val = fs->fire_grid[src];

      int32_t shift = (int32_t)(rand() % 5) - 2;
      int32_t dst_x = ((int32_t)x + shift + (int32_t)fs->w) % (int32_t)fs->w;
      uint32_t dst = (y - 1) * fs->w + (uint32_t)dst_x;
      uint8_t cool = ((uint32_t)rand() % 100 < 15) ? 1 : 0;
      fs->fire_grid[dst] = (src_val > cool) ? (src_val - cool) : 0;
    }
  }
  const uint32_t SEED_ROWS = 3;
  for (uint32_t r = 0; r < SEED_ROWS; r++) {
    uint32_t y = fs->h - 1 - r;
    for (uint32_t x = 0; x < fs->w; x++)
      fs->fire_grid[(size_t)y * fs->w + x] = PALETTE_SIZE - 1;
  }
}

void render(Module *self){
  FireState *fs = self->state;

  r_clear(0x000000);

  for(uint32_t y = 0; y < fs->h; y++){
    for(uint32_t x = 0; x < fs->w; x++){
      uint32_t idx = fs->fire_grid[y * fs->w + x];
      uint32_t c = fs->palette[idx];
      if (c)
        r_put_pixel(x, y, c);
    }
  }
}

void handle_event(Module *self, const SDL_Event *event){
  (void)self;
  (void)event;
}

void destroy(Module *self){
  free(self->state);
}

Module *create_module(uint32_t width, uint32_t height){
  Module *m = calloc(1, sizeof(Module));
  if (!m)
    return NULL;
  m->name = "doom-fire";
  m->w = width;
  m->h = height;

  m->init = init;
  m->update = update;
  m->render = render;
  m->handle_event = handle_event;

  m->destroy = destroy;
  return m;
}
