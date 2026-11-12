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
  while (running){
    while (SDL_PollEvent(&event)){
      if (event.type == SDL_EVENT_QUIT)
        running = false;
      mod->handle_event(mod, &event);
    }
    mod->update(mod);
    mod->render(mod);
    r_render();
  }
  r_shutdown();
  return EXIT_SUCCESS;
}
