#include "include/render.h"
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

int main() {
  r_init(WIDTH, HEIGHT, 0, "Particley");

  uint8_t running = 1;
  SDL_Event event;
  while (running){
    while (SDL_PollEvent(&event)){
      if (event.type == SDL_EVENT_QUIT)
        running = 0;
    }
    r_clear(0x0);
    r_put_pixel(100, 100, 0xFFFFFF);
    r_render();
  }

  r_shutdown();
  return EXIT_SUCCESS;
}
