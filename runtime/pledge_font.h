// pledge_font.h — Font rendering helpers for Fides FFI
//
// Wraps stb_truetype functions for loading TTF/OTF fonts and
// rasterizing text to bitmaps.

#ifndef PLEDGE_FONT_H
#define PLEDGE_FONT_H

// Load a TTF/OTF font from a file path at the given pixel size.
// Writes font info to out_font (a Fides Font struct pointer).
// Returns 0 on success, -1 on failure.
long long pledge_font_load(char* path, double pixel_size, void* out_font);

// Load a TTF/OTF font from raw bytes in memory.
// Returns 0 on success, -1 on failure.
long long pledge_font_load_mem(void* data, long long data_len,
                                 double pixel_size, void* out_font);

// Get a glyph bitmap for a codepoint.
// Writes width, height, xoff, yoff, advance to out pointers.
// Writes grayscale bitmap to out_bitmap (width * height bytes).
// Returns 0 on success, -1 if glyph not found.
long long pledge_font_get_glyph(void* font, long long codepoint,
                                  long long* out_w, long long* out_h,
                                  long long* out_xoff, long long* out_yoff,
                                  double* out_advance, void* out_bitmap);

// Get kerning advance between two codepoints (in pixels).
double pledge_font_kern(void* font, long long ch1, long long ch2);

// Measure the width of a text string in pixels.
long long pledge_font_text_width(void* font, char* text);

// Free a font's resources.
void pledge_font_free(void* font);

#endif // PLEDGE_FONT_H
