#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1200
#define HEIGHT 720
#define FIRE_W WIDTH
#define FIRE_H HEIGHT
#define FIRE_PALETTE_SIZE 37
#define TARGET_FPS 1 / 60.0

#define ERROR(msg)                                                             \
  {                                                                            \
    fprintf(stderr, "\033[31mERROR: %s.\033[0m\n", msg);                       \
  }

#define WARNING(msg)                                                           \
  {                                                                            \
    fprintf(stderr, "\033[33mWARNING: %s.\033[0m\n", msg);                     \
  }

void put_pixel(int x, int y, uint32_t color, uint32_t *fb) {
  if ((unsigned)x >= (unsigned)WIDTH || (unsigned)y >= (unsigned)HEIGHT)
    return;
  fb[y * WIDTH + x] = color;
}

void put_rectangle(int x, int y, int w, int h, uint32_t color, uint32_t *fb) {
  for (uint8_t dy = 0; dy < h; dy++)
    for (uint8_t dx = 0; dx < w; dx++)
      put_pixel(dx + x, dy + y, color, fb);
}

void clear(uint32_t color, uint32_t *fb) {
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    fb[i] = color;
  }
}

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    ERROR("Could not initialize SDL");
    exit(EXIT_FAILURE);
  }

  SDL_Window *window = SDL_CreateWindow("Particley", WIDTH, HEIGHT, 0);
  if (!window) {
    ERROR("Could not Create SDL Window");
    exit(EXIT_SUCCESS);
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  if (!renderer) {
    ERROR("Could not Create SDL Renderer");
    exit(EXIT_SUCCESS);
  }
  SDL_SetRenderVSync(renderer, 1);
  SDL_Texture *render_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (!render_texture) {
    ERROR("Could not Create SDL Render Texture");
    exit(EXIT_SUCCESS);
  }
  SDL_SetTextureScaleMode(render_texture, SDL_SCALEMODE_NEAREST);

  static uint32_t framebuffer[WIDTH * HEIGHT];

  static uint8_t fire_array[FIRE_W * FIRE_H];
  static const uint32_t fire_palette[FIRE_PALETTE_SIZE] = {
      0x070707, 0x1F0707, 0x2F0F07, 0x470F07, 0x571707, 0x671F07, 0x771F07,
      0x8F2707, 0x9F2F07, 0xAF3F07, 0xBF4707, 0xC74707, 0xDF4F07, 0xDF5707,
      0xDF5707, 0xD75F07, 0xD75F07, 0xD7670F, 0xCF6F0F, 0xCF770F, 0xCF7F0F,
      0xCF8717, 0xC78717, 0xC78F17, 0xC7971F, 0xBF9F1F, 0xBF9F1F, 0xBFA727,
      0xBFA727, 0xBFAF2F, 0xB7AF2F, 0xB7B72F, 0xB7B737, 0xCFCF6F, 0xDFDF9F,
      0xEFEFC7, 0xFFFFFF};

  for (int x = 0; x < FIRE_W; x++)
    fire_array[(FIRE_H - 1) * FIRE_W + x] = FIRE_PALETTE_SIZE - 1;

  SDL_Event event;
  uint8_t is_running = 1;
  uint32_t frame = 1;
  static double fps_accum = 0;
  static int fps_frames = 0;
  while (is_running) {
    uint32_t start = SDL_GetPerformanceCounter();
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        is_running = 0;
    }

    clear(0x000000, framebuffer);
    for (int x = 0; x < FIRE_W; x++) {
      for (int y = 1; y < FIRE_H; y++) {
        uint32_t src_index = y * FIRE_W + x;
        uint32_t rand_index = rand() % 4;
        int32_t dst = (int32_t)src_index - FIRE_W;
        int32_t dst_x = (int32_t)(src_index % FIRE_W) - (int32_t)rand_index + 1;
        if (dst < 0 || dst_x < 0 || dst_x >= FIRE_W)
          continue;
        else {
          dst = dst - (int32_t)(src_index % FIRE_W) + dst_x;
          uint8_t src_val = fire_array[src_index];
          uint8_t decay = rand_index & 1;
          fire_array[dst] = (src_val > decay) ? (src_val - decay) : 0;
        }
      }
    }

    for (int y = 0; y < FIRE_H; y++) {
      for (int x = 0; x < FIRE_W; x++) {
        uint8_t idx = fire_array[y * FIRE_W + x];
        put_pixel(x, y, fire_palette[idx], framebuffer);
      }
    }

    SDL_UpdateTexture(render_texture, NULL, framebuffer,
                      (uint32_t)WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, render_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    uint32_t end = SDL_GetPerformanceCounter();
    double elapsed = (double)(end - start) / SDL_GetPerformanceFrequency();
    fps_accum += elapsed;
    fps_frames++;
    if (fps_accum >= 0.5) {
      fprintf(stderr, "FPS: %.1f\n", fps_frames / fps_accum);
      fps_accum = 0;
      fps_frames = 0;
    }
    frame++;
  }

  SDL_DestroyTexture(render_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return EXIT_SUCCESS;
}
