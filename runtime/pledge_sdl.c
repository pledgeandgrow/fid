// pledge_sdl.c — SDL2 helper functions for Fides FFI
//
// Implements pledge_sdl.h: functions that wrap SDL2 calls requiring
// pointer passing (event polling, texture drawing).
//
// Build: gcc -c pledge_sdl.c -o pledge_sdl.o $(sdl2-config --cflags)
// Link: -lSDL2

#include <SDL2/SDL.h>
#include "pledge_sdl.h"
#include <string.h>

// ─── Texture Drawing ─────────────────────────────────────────────────────

void pledge_sdl_draw_texture(long long ren, long long tex,
                              long long x, long long y,
                              long long w, long long h) {
    SDL_Renderer* renderer = (SDL_Renderer*)ren;
    SDL_Texture* texture = (SDL_Texture*)tex;
    if (!renderer || !texture) return;

    SDL_Rect dst;
    dst.x = (int)x;
    dst.y = (int)y;
    dst.w = (int)w;
    dst.h = (int)h;

    SDL_RenderCopy(renderer, texture, NULL, &dst);
}

// pledge_sdl_poll_events is implemented in pledge_input.c
// (it shares the FidesInputState layout with the input backend)
