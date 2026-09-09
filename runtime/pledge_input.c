// pledge_input.c — Linux input backend for Fides
//
// Provides keyboard, mouse, and gamepad input via SDL2.
// This file implements the FFI functions declared in stdlib/input.fd
// and the SDL input helpers in stdlib/sdl.fd.
//
// Build: gcc -c pledge_input.c -o pledge_input.o $(sdl2-config --cflags)
// Link: -lSDL2
//
// Alternative (no SDL2): use /dev/input/event* (evdev) or /dev/tty (console).
// The SDL2 path is recommended for cross-platform compatibility.

#include <SDL2/SDL.h>
#include <string.h>
#include <stdio.h>

// ─── State ───────────────────────────────────────────────────────────────

static const unsigned char* key_state = NULL;
static int num_keys = 0;
static unsigned char prev_key_state[512];

static int mouse_x = 0;
static int mouse_y = 0;
static unsigned int mouse_buttons = 0;
static unsigned int prev_mouse_buttons = 0;

static int initialized = 0;

// ─── pledge_input.h API (used by stdlib/input.fd) ────────────────────────

int pledge_input_init(void) {
    if (initialized) return 0;
    // SDL must already be initialized with SDL_INIT_VIDEO or SDL_INIT_EVENTS
    // We don't call SDL_Init here to avoid conflicting with the main SDL init.
    memset(prev_key_state, 0, sizeof(prev_key_state));
    initialized = 1;
    return 0;
}

void pledge_input_poll(unsigned char* keys, long long key_count) {
    if (!initialized || keys == NULL) return;

    // Save previous state
    memcpy(prev_key_state, keys, key_count > 512 ? 512 : (size_t)key_count);

    // Get current keyboard state from SDL
    key_state = SDL_GetKeyboardState(&num_keys);
    if (key_state && num_keys > 0) {
        size_t copy_count = (size_t)num_keys;
        if (copy_count > (size_t)key_count) copy_count = (size_t)key_count;
        if (copy_count > 512) copy_count = 512;
        memcpy(keys, key_state, copy_count);
    }
}

void pledge_input_mouse(unsigned char* x, unsigned char* y,
                        unsigned char* buttons, unsigned char* wheel) {
    prev_mouse_buttons = mouse_buttons;
    mouse_buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

    if (x) *x = (unsigned char)(mouse_x & 0xFF);
    if (y) *y = (unsigned char)(mouse_y & 0xFF);
    if (buttons) *buttons = (unsigned char)(mouse_buttons & 0xFF);
    if (wheel) *wheel = 0;  // wheel handled via SDL_EVENT in poll
}

void pledge_input_gamepad(unsigned char* connected, unsigned char* buttons,
                          unsigned char* left_x, unsigned char* left_y,
                          unsigned char* right_x, unsigned char* right_y,
                          unsigned char* lt, unsigned char* rt) {
    // Gamepad support via SDL_GameController
    // Open first connected gamepad
    static SDL_GameController* pad = NULL;

    if (!pad) {
        int num_joysticks = SDL_NumJoysticks();
        int i;
        for (i = 0; i < num_joysticks; i++) {
            if (SDL_IsGameController(i)) {
                pad = SDL_GameControllerOpen(i);
                break;
            }
        }
    }

    if (!pad || !SDL_GameControllerGetAttached(pad)) {
        if (connected) *connected = 0;
        if (buttons) *buttons = 0;
        if (left_x) *left_x = 128;
        if (left_y) *left_y = 128;
        if (right_x) *right_x = 128;
        if (right_y) *right_y = 128;
        if (lt) *lt = 0;
        if (rt) *rt = 0;
        return;
    }

    if (connected) *connected = 1;

    // Buttons (14 buttons, packed as bitmask)
    unsigned char btn = 0;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A)) btn |= 1;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_B)) btn |= 2;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_X)) btn |= 4;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_Y)) btn |= 8;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK)) btn |= 16;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START)) btn |= 32;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) btn |= 64;
    if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) btn |= 128;
    if (buttons) *buttons = btn;

    // Analog sticks (normalize -32768..32767 to 0..255)
    if (left_x) *left_x = (unsigned char)((SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX) + 32768) / 257);
    if (left_y) *left_y = (unsigned char)((SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY) + 32768) / 257);
    if (right_x) *right_x = (unsigned char)((SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_RIGHTX) + 32768) / 257);
    if (right_y) *right_y = (unsigned char)((SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_RIGHTY) + 32768) / 257);

    // Triggers (0..32767 to 0..255)
    if (lt) *lt = (unsigned char)(SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 128);
    if (rt) *rt = (unsigned char)(SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 128);
}

// ─── pledge_sdl.h API (used by stdlib/sdl.fd) ─────────────────────────────

// SdlInputState layout (must match stdlib/sdl.fd):
//   offset 0:  Vec keys (data_ptr, len, cap) = 24 bytes
//   offset 24: Vec prev_keys (data_ptr, len, cap) = 24 bytes
//   offset 48: i64 mouse_x
//   offset 56: i64 mouse_y
//   offset 64: i64 mouse_buttons
//   offset 72: i64 prev_mouse_buttons
//   offset 80: bool quit_requested (i64 for alignment)
//
// The Vec layout is: { data_ptr: *i64, len: i64, cap: i64 }
// In the Fides runtime, Vec is a heap-allocated struct with this layout.

typedef struct {
    long long* keys_data;     // Vec<i64> data pointer
    long long keys_len;       // Vec<i64> length
    long long keys_cap;       // Vec<i64> capacity
    long long* prev_keys_data;
    long long prev_keys_len;
    long long prev_keys_cap;
    long long mouse_x;
    long long mouse_y;
    long long mouse_buttons;
    long long prev_mouse_buttons;
    long long quit_requested;  // bool stored as i64
} FidesInputState;

void pledge_sdl_poll_events(void* state_ptr) {
    if (!state_ptr) return;
    FidesInputState* state = (FidesInputState*)state_ptr;

    // Save previous keyboard state
    if (state->prev_keys_data && state->prev_keys_len >= 512 &&
        state->keys_data && state->keys_len >= 512) {
        memcpy(state->prev_keys_data, state->keys_data, 512 * sizeof(long long));
    }

    // Save previous mouse buttons
    state->prev_mouse_buttons = state->mouse_buttons;

    // Poll SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            state->quit_requested = 1;
        }
        // Key events are handled via SDL_GetKeyboardState below
    }

    // Update keyboard state from SDL
    int num_keys = 0;
    const unsigned char* keys = SDL_GetKeyboardState(&num_keys);
    if (keys && state->keys_data && state->keys_len >= 512) {
        int i;
        for (i = 0; i < 512 && i < num_keys; i++) {
            state->keys_data[i] = (long long)keys[i];
        }
    }

    // Update mouse state
    int mx, my;
    state->mouse_buttons = (long long)SDL_GetMouseState(&mx, &my);
    state->mouse_x = (long long)mx;
    state->mouse_y = (long long)my;
}
