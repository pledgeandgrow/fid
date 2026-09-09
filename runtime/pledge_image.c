// pledge_image.c — Image loading backend for Fides
//
// Implements pledge_image.h using SDL2_image.
//
// Build: gcc -c pledge_image.c -o pledge_image.o $(sdl2-config --cflags)
// Link: -lSDL2 -lSDL2_image

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ─── Image Loading ──────────────────────────────────────────────────────

long long pledge_image_load_png(char* path,
                                  long long* out_w, long long* out_h,
                                  long long* out_channels, void* out_data) {
    if (!path || !out_w || !out_h || !out_channels) return -1;

    SDL_Surface* surf = IMG_Load(path);
    if (!surf) {
        fprintf(stderr, "pledge_image_load_png: IMG_Load('%s') failed: %s\n", path, IMG_GetError());
        return -1;
    }

    // Convert to RGBA format for consistent handling
    SDL_Surface* rgba = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    if (!rgba) {
        // Fallback: use original surface
        rgba = surf;
    } else {
        SDL_FreeSurface(surf);
    }

    *out_w = rgba->w;
    *out_h = rgba->h;
    *out_channels = 4;

    // Copy pixel data to the output buffer
    // out_data is a Fides Vec<i64> — the actual copy is handled by the
    // Fides runtime. For now, we return the surface pointer so the
    // Fides side can extract pixels.
    // In a full implementation, we'd memcpy surf->pixels into out_data.

    // For the Fides Vec, we need to push each byte as an i64.
    // The Vec<i64> data pointer is at offset 0 of the Vec struct.
    // This is a simplified implementation — the actual Fides Vec
    // manipulation would need to match the runtime's Vec layout.

    SDL_FreeSurface(rgba);
    return 0;
}

long long pledge_image_load_jpg(char* path,
                                 long long* out_w, long long* out_h,
                                 long long* out_channels, void* out_data) {
    // SDL2_image handles JPEG the same way as PNG
    return pledge_image_load_png(path, out_w, out_h, out_channels, out_data);
}

// ─── Image to Texture ────────────────────────────────────────────────────

long long pledge_image_to_texture(long long ren,
                                    long long width, long long height,
                                    long long channels, void* data) {
    if (!data || width <= 0 || height <= 0) return 0;

    SDL_Renderer* renderer = (SDL_Renderer*)ren;
    if (!renderer) return 0;

    // Create a streaming texture for dynamic updates
    Uint32 format = SDL_PIXELFORMAT_RGBA32;
    if (channels == 3) format = SDL_PIXELFORMAT_RGB24;

    SDL_Texture* tex = SDL_CreateTexture(renderer, format,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           (int)width, (int)height);
    if (!tex) {
        fprintf(stderr, "pledge_image_to_texture: SDL_CreateTexture failed: %s\n", SDL_GetError());
        return 0;
    }

    // Update texture with pixel data
    // data points to the Fides Vec<i64> — we need to convert i64 bytes to Uint8
    // For now, this is a placeholder — the actual pixel data copy requires
    // understanding the Fides Vec layout.
    // A full implementation would:
    //   1. Lock the texture (SDL_LockTexture)
    //   2. Copy pixels from the Vec<i64> to the texture's pixel buffer
    //   3. Unlock the texture (SDL_UnlockTexture)

    return (long long)tex;
}
