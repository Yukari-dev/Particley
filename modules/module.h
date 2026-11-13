#ifndef MODULES_H
#define MODULES_H
#include <SDL3/SDL_events.h>

typedef struct Module{
  const char *name;
  void (*init)(struct Module *self);
  void (*update)(struct Module *self, double dt);
  void (*render)(struct Module *self);
  void (*handle_event)(struct Module *self, const SDL_Event *event);
  void (*destroy)(struct Module *self);
  void *state;
  uint32_t w, h;
} Module;

Module *create_module(uint32_t width, uint32_t height);

#endif
