#include "../../include/render.h"
#include "../module.h"
#include "n-body.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t n;
  float *px, *py;
  float *vx, *vy;
  float *ax, *ay;
  float *mass;
  float *radius;
  r_Color *color;
} NBodyState;

static inline void spawn_body(NBodyState *s, size_t i, uint32_t cx, uint32_t cy, double radius){
  double angle = ((double)rand() / RAND_MAX) * 2 * M_PI;
  double random_radius = sqrt(((double)rand() / RAND_MAX) * (radius * radius));

  s->px[i] = cx + random_radius * cos(angle);
  s->py[i] = cy + random_radius * sin(angle);
  double dx = s->px[i] - cx;
  double dy = s->py[i] - cy;
  double v = ORBIT_SPEED / sqrt(random_radius + 10.0);

  s->vx[i] = -dy * v;
  s->vy[i] = dx * v;
  s->radius[i] = BODY_RADIUS;
  s->color[i] = 0xFFFFFF;
}

static inline uint32_t u8_to_u32_color(uint8_t r, uint8_t g, uint8_t b){
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void init(Module *self){
  NBodyState *s = calloc(1, sizeof(*s));
  if (!s) { self->state = NULL; return; }

  s->n = BODIES_COUNT;

  s->px = calloc(s->n, sizeof(*s->px));
  s->py = calloc(s->n, sizeof(*s->py));
  s->vx = calloc(s->n, sizeof(*s->vx));
  s->vy = calloc(s->n, sizeof(*s->vy));
  s->ax = calloc(s->n, sizeof(*s->ax));
  s->ay = calloc(s->n, sizeof(*s->ay));
  s->mass = calloc(s->n, sizeof(*s->mass));
  s->radius = calloc(s->n, sizeof(*s->radius));
  s->color = calloc(s->n, sizeof(*s->color));

  if (!s->px || !s->py || !s->vx || !s->vy || !s->ax ||
    !s->ay || !s->mass || !s->radius || !s->color)
    goto fail;

  for (size_t i = 0; i < s->n; i++)
    spawn_body(s, i, self->w / 2, self->h / 2, SPAWN_RADIUS);

  self->state = s;
  return;
fail:
  free(s->px);
  free(s->py);
  free(s->vx);
  free(s->vy);
  free(s->ax);
  free(s->ay);
  free(s->mass);
  free(s->radius);
  free(s->color);
  free(s);
  self->state = NULL;
}

void update(Module *self, double dt){
  NBodyState *s = self->state;
  const size_t n = s->n;
  const float dtf = (float)dt;
  const float G_C = G;

  for (size_t i = 0; i < n; i++) { s->ax[i] = 0; s->ay[i] = 0; }

  for(size_t i = 0; i < n; i++){
    float xi = s->px[i];
    float yi = s->py[i];
    float axi = 0, ayi = 0;
    for(size_t j = i + 1; j < n; j++){
      float dx = s->px[j] - xi;
      float dy = s->py[j] - yi;
      float r2 = dx*dx + dy*dy + SOFT2;
      float inv = G_C / (r2 * sqrtf(r2));
      float fx = dx * inv;
      float fy = dy * inv;
      axi += fx;
      ayi += fy;
      s->ax[j] -= fx;
      s->ay[j] -= fy;
    }
    s->ax[i] += axi;
    s->ay[i] += ayi;
  }

  for(size_t i = 0; i < n; i++){
    s->vx[i] += s->ax[i] * dtf;
    s->vy[i] += s->ay[i] * dtf;
    s->px[i] += s->vx[i] * dtf;
    s->py[i] += s->vy[i] * dtf;
  }
}

void render(Module *self){
  NBodyState *s = self->state;
  r_fade(230);
  // r_clear(0x000000);

  for(size_t i = 0; i < s->n; i++){
    float speed = sqrtf(s->vx[i] * s->vx[i] + s->vy[i] * s->vy[i]);
    float t = speed / 100.f;
    if (t > 1.0f) t = 1.0f;
    uint8_t r = (uint8_t)(100 + 155 * t);
    uint8_t g = (uint8_t)(100 * (1 - t));
    uint8_t b = (uint8_t)(255 * (1 - t));
    r_put_circle((uint32_t)s->px[i], (uint32_t)s->py[i], s->radius[i], u8_to_u32_color(r, g, b));
  }
}

void handle_event(Module *self, const SDL_Event *event){
  (void)self;
  (void)event;
}

void destroy(Module *self){
  NBodyState *s = self->state;
  free(s->px);
  free(s->py);
  free(s->vx);
  free(s->vy);
  free(s->ax);
  free(s->ay);
  free(s->mass);
  free(s->radius);
  free(s->color);
  free(s);
}

Module *create_module(uint32_t width, uint32_t height){
  Module *m = calloc(1, sizeof(Module));
  m->name = "n-body";
  m->w = width;
  m->h = height;
  m->init = init;
  m->update = update;
  m->render = render;
  m->handle_event = handle_event;
  m->destroy = destroy;
  return m;
}
