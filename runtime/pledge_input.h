// pledge_input.h — Input backend for Fides
//
// Keyboard, mouse, and gamepad input via SDL2.
// Implements the FFI functions declared in stdlib/input.fd.

#ifndef PLEDGE_INPUT_H
#define PLEDGE_INPUT_H

long long pledge_input_init(void);
void pledge_input_poll(unsigned char* keys, long long key_count);
void pledge_input_mouse(unsigned char* x, unsigned char* y,
                        unsigned char* buttons, unsigned char* wheel);
void pledge_input_gamepad(unsigned char* connected, unsigned char* buttons,
                          unsigned char* left_x, unsigned char* left_y,
                          unsigned char* right_x, unsigned char* right_y,
                          unsigned char* lt, unsigned char* rt);

#endif // PLEDGE_INPUT_H
