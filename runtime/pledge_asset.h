// pledge_asset.h — Asset loading helpers for Fides FFI (stdlib/asset.fd)
//
// Thin wrappers over the other runtime helpers: SDL2_image for textures,
// pledge_audio (SDL2_mixer) for sounds, pledge_font (stb_truetype) for
// fonts. Link pledge_asset.c together with pledge_audio.c and
// pledge_font.c, and SDL2, SDL2_image and SDL2_mixer.

#ifndef PLEDGE_ASSET_H
#define PLEDGE_ASSET_H

// Load an image file (PNG, JPG, BMP, ...) as an SDL_Surface: its handle
// (> 0), or 0 on failure. The asset manager has no renderer, so this is
// the decoded image, not a GPU texture; pledge_asset_unload frees it.
long long pledge_asset_load_texture(char* path);

// Load a sound through pledge_audio_load: its index in pledge_audio's
// sound cache (0 or more), or -1 on failure (also when pledge_audio_init
// was not called). The cache owns the sound until pledge_audio_shutdown.
long long pledge_asset_load_sound(char* path);

// Load a TTF/OTF font through pledge_font_load: its handle (> 0), or 0 on
// failure; pledge_asset_unload frees it.
long long pledge_asset_load_font(char* path, double size);

// Meshes have no loader in the runtime yet: always 0 (failure), with a
// message on stderr.
long long pledge_asset_load_mesh(char* path);

// Give back what the matching pledge_asset_load_* returned, by asset type
// (stdlib/asset.fd: 0 texture, 1 sound, 2 font, 3 mesh, 4 text, 5 data).
// A sound stays in pledge_audio's cache (it has no per-sound free); text
// and data assets have no handle.
void pledge_asset_unload(long long handle, long long asset_type);

// The file's last modification time (seconds since the epoch), or 0 when
// it cannot be read.
long long pledge_asset_mtime(char* path);

#endif // PLEDGE_ASSET_H
