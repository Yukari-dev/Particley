#ifndef RENDER_H
#define RENDER_H
#include <SDL3/SDL.h>
#include <stdint.h>

typedef SDL_WindowFlags WindowFlags;

int r_init(uint32_t width, uint32_t height, WindowFlags flags, const char *title);
void r_render();
void r_shutdown();

#endif
