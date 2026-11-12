#include "../../include/render.h"
#include "../module.h"
#include "starfield.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  float x, y, z;
} Star;

typedef struct {
  Star *stars;
  int n;
  uint32_t w, h;
  int32_t cx, cy;
  float speed, warp;
  int burst_frames;
  int mouse_x, mouse_y;
  uint32_t rng;
} StarfieldState;

static inline uint32_t xorshift32(uint32_t *s) {
    uint32_t x = *s;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return *s = x;
}

static inline float frand(uint32_t *s, float lo, float hi) {
  return lo + (hi - lo) * ((float)xorshift32(s) / (float)UINT32_MAX);
}

static void star_respawn(Star *s, uint32_t *rng) {
  s->x = frand(rng, -STARFIELD_SPREAD, STARFIELD_SPREAD);
  s->y = frand(rng, -STARFIELD_SPREAD, STARFIELD_SPREAD);
  s->z = frand(rng, STARFIELD_Z_NEAR + 10.0f, STARFIELD_Z_FAR);
}

static inline uint8_t star_brightness(float z, float z_far) {
  if (z <= 0.0f) return 0;
  float b = 1.0f - z / z_far;
  if (b < 0.0f) b = 0.0f;
  if (b > 1.0f) b = 1.0f;
  return (uint8_t)(b * 255.0f);
}

static inline void star_color(float z, float z_far, uint8_t *r, uint8_t *g, uint8_t *b) {
  uint8_t br = star_brightness(z, z_far);
  float t = 1.0f - (float)br / 255.0f;
  *r = (uint8_t)(255 - 215 * t);
  *g = (uint8_t)(255 - 175 * t);
  *b = (uint8_t)(255 -  55 * t);
}

static inline uint32_t u8_to_u32_color(uint8_t r, uint8_t g, uint8_t b){
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void init(Module *self){
  StarfieldState *ss = calloc(1, sizeof(StarfieldState));
  if (!ss) return;
  ss->w = self->w;
  ss->h = self->h;
  ss->cx = (int32_t)ss->w / 2;
  ss->cy = (int32_t)ss->h / 2;
  ss->n = STARFIELD_N;
  ss->stars = calloc((size_t)ss->n, sizeof(Star));
  ss->speed = STARFIELD_SPEED;
  ss->warp = 1.0f;

  ss->rng = 0x12345u;
  for(int i = 0; i < ss->n; i++)
    star_respawn(&ss->stars[i], &ss->rng);
  self->state = ss;
}

void update(Module *self){
  StarfieldState *s = self->state;

  float speed = s->speed * s->warp;
  for(int i = 0; i < s->n; i++){
    Star *star = &s->stars[i];
    if (!star)
      return;
    star->z -= speed;
    if (star->z < STARFIELD_Z_NEAR){
      star->x = frand(&s->rng, -STARFIELD_SPREAD, STARFIELD_SPREAD);
      star->y = frand(&s->rng, -STARFIELD_SPREAD, STARFIELD_SPREAD);
      star->z += STARFIELD_Z_FAR;
    }
  }
  if (s->burst_frames > 0) {
    s->burst_frames--;
    s->warp = 1.0f + 3.0f * ((float)s->burst_frames / 30.0f);
  } else {
    s->warp = 1.0f;
  }
}

void render(Module *self){
  StarfieldState *s = self->state;

  r_fade(100);

  for(int i = 0; i < s->n; i++){
    Star *star = &s->stars[i];

    if(star->z <= STARFIELD_Z_NEAR) continue;

    float inv = STARFIELD_FOCAL / star->z;
    int sx = s->cx + (int)(star->x * inv);
    int sy = s->cy + (int)(star->y * inv);
    if ((unsigned)sx >= (unsigned)s->w || (unsigned)sy >= (unsigned)s->h)
      continue;
    uint8_t r, g, b;
    star_color(star->z, STARFIELD_Z_FAR, &r, &g, &b);
    r_put_pixel((uint32_t)sx, (uint32_t)sy, u8_to_u32_color(r, g, b));
  }
}

static void handle_event(Module *m, const SDL_Event *e) {
  StarfieldState *s = m->state;
  switch (e->type) {
    case SDL_EVENT_KEY_DOWN:
      if (e->key.key == SDLK_SPACE) {
        s->burst_frames = 30;
      } else if (e->key.key == SDLK_UP) {
        s->speed += 1.0f;
      } else if (e->key.key == SDLK_DOWN) {
        s->speed -= 1.0f;
        if (s->speed < 0.5f) s->speed = 0.5f;
      }
      break;
    default:
      break;
  }
}

void destroy(Module *self){
  free(self->state);
}

Module *create_module(uint32_t width, uint32_t height){
  Module *m = calloc(1, sizeof(Module));
  m->name = "Starfield";
  m->w = width;
  m->h = height;
  m->init = init;
  m->update = update;
  m->render = render;
  m->handle_event = handle_event;
  m->destroy = destroy;
  return m;
}
