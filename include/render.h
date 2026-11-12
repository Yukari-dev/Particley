#ifndef RENDER_H
#define RENDER_H
#include <SDL3/SDL.h>
#include <stdint.h>

typedef SDL_WindowFlags r_WindowFlags;
typedef uint32_t r_Color;

int r_init(uint32_t width, uint32_t height, r_WindowFlags flags, const char *title);
void r_put_pixel(uint32_t x, uint32_t y, r_Color color);
void r_clear(r_Color color);
void r_render();
void r_shutdown();

#endif
