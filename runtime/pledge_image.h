// pledge_image.h — Image loading helpers for Fides FFI
//
// Wraps SDL2_image functions for loading PNG/JPG/BMP files.

#ifndef PLEDGE_IMAGE_H
#define PLEDGE_IMAGE_H

// Load a PNG image. Writes width, height, channels to the out pointers
// and pixel data to out_data (a Fides Vec<i64>).
// Returns 0 on success, -1 on failure.
long long pledge_image_load_png(char* path,
                                  long long* out_w, long long* out_h,
                                  long long* out_channels, void* out_data);

// Load a JPEG image. Same interface as pledge_image_load_png.
long long pledge_image_load_jpg(char* path,
                                 long long* out_w, long long* out_h,
                                 long long* out_channels, void* out_data);

// Convert raw pixel data to an SDL texture.
// Returns texture handle (i64) or 0 on failure.
long long pledge_image_to_texture(long long ren,
                                    long long width, long long height,
                                    long long channels, void* data);

#endif // PLEDGE_IMAGE_H
