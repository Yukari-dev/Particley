#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600
#define TARGET_FPS 1 / 60.0

#define ERROR(msg)                                                             \
  {                                                                            \
    printf("\033[31mERROR: %s.\033[0m\n", msg);                                \
  }

#define WARNING(msg)                                                           \
  {                                                                            \
    printf("\033[33mWARNING: %s.\033[0m\n", msg);                              \
  }

void put_pixel(int x, int y, uint32_t color, uint32_t *fb) {
  if ((unsigned)x >= (unsigned)WIDTH || (unsigned)y >= (unsigned)HEIGHT)
    return;
  fb[y * WIDTH + x] = color;
}

void put_rectangle(int x, int y, int w, int h, uint32_t color, uint32_t *fb) {
  for (uint8_t dy = y; dy <= y + h; dy++)
    for (uint8_t dx = x; dx <= x + w; dx++)
      put_pixel(dx, dy, color, fb);
}

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    ERROR("Could not initialize SDL");
    exit(EXIT_FAILURE);
  }

  SDL_Window *window = SDL_CreateWindow("Particley", WIDTH, HEIGHT, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  SDL_Texture *render_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  SDL_SetTextureScaleMode(render_texture, SDL_SCALEMODE_NEAREST);

  uint32_t framebuffer[WIDTH * HEIGHT];
  bzero(framebuffer, sizeof(framebuffer));
  SDL_Event event;
  uint8_t is_running = 1;
  put_rectangle(100, 100, 100, 100, 0xFFFFFF, framebuffer);
  while (is_running) {
    uint64_t start = SDL_GetPerformanceCounter();
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        is_running = 0;
    }

    SDL_UpdateTexture(render_texture, NULL, framebuffer,
                      (uint32_t)WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, render_texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    uint64_t end = SDL_GetPerformanceCounter();

    double elapsed =
        (double)(end - start) / (double)SDL_GetPerformanceFrequency();
    printf("%f\n", 1 / elapsed);
    if (elapsed < TARGET_FPS) {
      SDL_Delay((TARGET_FPS - elapsed) * 1000.f);
    }
  }

  SDL_DestroyTexture(render_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
