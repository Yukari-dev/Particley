#include "include/logger.h"
#include "include/render.h"
#include "modules/module.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

int main() {
  r_init(WIDTH, HEIGHT, 0, "Particley");

  Module *mod = create_module(WIDTH, HEIGHT);
  char mod_name[64];
  snprintf(mod_name, sizeof(mod_name), "%s", mod->name);
  INFO(mod_name);
  mod->init(mod);

  uint8_t running = 1;
  SDL_Event event;
  double dt = 0.0;
  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;
  double frequency = SDL_GetPerformanceFrequency();
  while (running){
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    dt = (double)(NOW - LAST) / frequency;
    if (dt > 0.25)
      dt = 0.25;
    printf("dt: %f, fps: %.0f\n", dt, 1 / dt);
    while (SDL_PollEvent(&event)){
      if (event.type == SDL_EVENT_QUIT)
        running = false;
      mod->handle_event(mod, &event);
    }
    mod->update(mod, dt);
    mod->render(mod);
    r_render();
  }
  r_shutdown();
  return EXIT_SUCCESS;
}
