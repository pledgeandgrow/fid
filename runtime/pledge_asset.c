// pledge_asset.c — Asset loading helpers for Fides FFI (stdlib/asset.fd)
//
// See pledge_asset.h. Each loader delegates to the runtime helper that
// already exists for its kind; nothing here keeps state of its own.

#include "pledge_asset.h"
#include "pledge_audio.h"
#include "pledge_font.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

// stdlib/asset.fd's ASSET_* constants
#define PLEDGE_ASSET_TEXTURE 0
#define PLEDGE_ASSET_SOUND   1
#define PLEDGE_ASSET_FONT    2
#define PLEDGE_ASSET_MESH    3

long long pledge_asset_load_texture(char* path) {
    if (!path) return 0;
    SDL_Surface* s = IMG_Load(path);
    if (!s) {
        fprintf(stderr, "pledge_asset_load_texture: IMG_Load('%s') failed: %s\n", path, IMG_GetError());
        return 0;
    }
    return (long long)(intptr_t)s;
}

long long pledge_asset_load_sound(char* path) {
    if (!path) return -1;
    return pledge_audio_load(path);
}

long long pledge_asset_load_font(char* path, double size) {
    if (!path) return 0;
    long long f = pledge_font_load(path, size, NULL);
    return f > 0 ? f : 0;
}

long long pledge_asset_load_mesh(char* path) {
    fprintf(stderr, "pledge_asset_load_mesh: no mesh loader in the runtime ('%s' not loaded)\n", path ? path : "(null)");
    return 0;
}

void pledge_asset_unload(long long handle, long long asset_type) {
    switch (asset_type) {
        case PLEDGE_ASSET_TEXTURE:
            if (handle > 0) SDL_FreeSurface((SDL_Surface*)(intptr_t)handle);
            break;
        case PLEDGE_ASSET_FONT:
            if (handle > 0) pledge_font_free((void*)(intptr_t)handle);
            break;
        default:
            // sounds stay in pledge_audio's cache (freed by
            // pledge_audio_shutdown); meshes are never loaded; text and
            // data assets have no handle
            break;
    }
}

long long pledge_asset_mtime(char* path) {
    if (!path) return 0;
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return (long long)st.st_mtime;
}
