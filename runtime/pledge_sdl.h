// pledge_sdl.h — SDL2 helper functions for Fides FFI
//
// These functions wrap SDL2 calls that require pointer passing
// (event polling, keyboard state, texture drawing) which is
// difficult to do directly from Fides FFI declarations.

#ifndef PLEDGE_SDL_H
#define PLEDGE_SDL_H

// Poll SDL events and update the Fides SdlInputState struct.
// The state_ptr points to a Fides SdlInputState struct.
void pledge_sdl_poll_events(void* state_ptr);

// Draw a texture at the given position with the given size.
void pledge_sdl_draw_texture(long long ren, long long tex,
                              long long x, long long y,
                              long long w, long long h);

#endif // PLEDGE_SDL_H
