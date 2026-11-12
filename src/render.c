#include "../include/render.h"
#include "../include/logger.h"
#include <stdlib.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *render_texture;
  uint32_t width, height, size;
  uint32_t *framebuffer;
  double fps;
} RenderContext;

static RenderContext *render_context = NULL;

int r_init(uint32_t width, uint32_t height, r_WindowFlags flags, const char *title){
  if (!SDL_Init(SDL_INIT_VIDEO)){
    ERROR("Failed to Initialize SDL");
    return 1;
  }
  INFO("SDL is Initialized successfully");

  render_context = (RenderContext*)malloc(sizeof(RenderContext));
  render_context->width = width;
  render_context->height = height;
  render_context->size = width * height;
  render_context->framebuffer = malloc(sizeof(uint32_t) * (width * height));
  render_context->fps = 0;
  render_context->window = SDL_CreateWindow(title, (int)width, (int)height, flags);
  if (!render_context->window){
    ERROR("Failed to create SDL Window");
    return 1;
  }
  INFO("SDL Window is Created successfully");
  render_context->renderer = SDL_CreateRenderer(render_context->window, NULL);
  if (!render_context->renderer){
    ERROR("Failed to create SDL Renderer");
    return 1;
  }
  INFO("SDL Renderer is Created successfully");
  render_context->render_texture = SDL_CreateTexture(render_context->renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, (int)width, (int)height);
  if (!render_context->render_texture){
    ERROR("Failed to create SDL Render Texture");
    return 1;
  }
  INFO("SDL Render Texture is Created successfully");

  return 0;
}

void r_put_pixel(uint32_t x, uint32_t y, r_Color color){
  if (x >= render_context->width || y >= render_context->height)
    return;
  render_context->framebuffer[y * render_context->width + x] = color;
}

void r_fade(uint8_t factor){
  size_t n = (size_t)render_context->size;
  for (size_t i = 0; i < n; i++) {
    uint32_t c = render_context->framebuffer[i];
    uint32_t r = (((c >> 16) & 0xFF) * factor) >> 8;
    uint32_t g = (((c >>  8) & 0xFF) * factor) >> 8;
    uint32_t b = (( c        & 0xFF) * factor) >> 8;
    render_context->framebuffer[i] = 0xFF000000u | (r << 16) | (g << 8) | b;
  }
}

void r_clear(r_Color color){
  for(uint32_t i = 0; i < render_context->size; i++)
    render_context->framebuffer[i] = color;
}

void r_render(){
  SDL_UpdateTexture(render_context->render_texture, NULL, render_context->framebuffer, (int)(render_context->width * sizeof(uint32_t)));
  SDL_RenderClear(render_context->renderer);
  SDL_RenderTexture(render_context->renderer, render_context->render_texture, NULL, NULL);
  SDL_RenderPresent(render_context->renderer);
}

void r_shutdown(){
  SDL_DestroyWindow(render_context->window);
  free(render_context->framebuffer);
  free(render_context);
  SDL_Quit();
}
