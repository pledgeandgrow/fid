// pledge_font.c — Font rendering backend for Fides
//
// Implements pledge_font.h using stb_truetype.
// stb_truetype.h must be in the include path.
//
// Build: gcc -c pledge_font.c -o pledge_font.o
// Link: none (stb_truetype is header-only)
//
// Download stb_truetype.h:
//   wget https://raw.githubusercontent.com/nothings/stb/master/stb_truetype.h

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ─── Font Info Structure ─────────────────────────────────────────────────
//
// The Fides Font struct has these fields:
//   info: i64 (opaque pointer to stbtt_fontinfo)
//   data: Vec<i64> (TTF file bytes)
//   scale: f64
//   ascent: f64
//   descent: f64
//   line_gap: f64
//   line_height: f64
//
// We store the stbtt_fontinfo in a malloc'd struct and pass the pointer
// back to Fides as an i64.

typedef struct {
    stbtt_fontinfo info;
    unsigned char* ttf_data;     // raw TTF file bytes
    int ttf_size;
    float scale;
    float ascent;
    float descent;
    float line_gap;
} PledgeFont;

// ─── Font Loading ────────────────────────────────────────────────────────

long long pledge_font_load(char* path, double pixel_size, void* out_font) {
    if (!path) return -1;

    // Read TTF file
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "pledge_font_load: cannot open '%s'\n", path);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* ttf_data = (unsigned char*)malloc(size);
    if (!ttf_data) { fclose(f); return -1; }

    if (fread(ttf_data, 1, size, f) != (size_t)size) {
        free(ttf_data);
        fclose(f);
        return -1;
    }
    fclose(f);

    // Initialize stb_truetype
    PledgeFont* pf = (PledgeFont*)malloc(sizeof(PledgeFont));
    if (!pf) { free(ttf_data); return -1; }

    pf->ttf_data = ttf_data;
    pf->ttf_size = (int)size;

    if (!stbtt_InitFont(&pf->info, ttf_data, stbtt_GetFontOffsetForIndex(ttf_data, 0))) {
        fprintf(stderr, "pledge_font_load: stbtt_InitFont failed\n");
        free(ttf_data);
        free(pf);
        return -1;
    }

    // Calculate metrics
    pf->scale = stbtt_ScaleForPixelHeight(&pf->info, (float)pixel_size);
    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&pf->info, &ascent, &descent, &line_gap);
    pf->ascent = ascent * pf->scale;
    pf->descent = descent * pf->scale;
    pf->line_gap = line_gap * pf->scale;

    return (long long)pf;
}

long long pledge_font_load_mem(void* data, long long data_len,
                                 double pixel_size, void* out_font) {
    if (!data || data_len <= 0) return -1;

    unsigned char* ttf_data = (unsigned char*)malloc(data_len);
    if (!ttf_data) return -1;
    memcpy(ttf_data, data, data_len);

    PledgeFont* pf = (PledgeFont*)malloc(sizeof(PledgeFont));
    if (!pf) { free(ttf_data); return -1; }

    pf->ttf_data = ttf_data;
    pf->ttf_size = (int)data_len;

    if (!stbtt_InitFont(&pf->info, ttf_data, stbtt_GetFontOffsetForIndex(ttf_data, 0))) {
        free(ttf_data);
        free(pf);
        return -1;
    }

    pf->scale = stbtt_ScaleForPixelHeight(&pf->info, (float)pixel_size);
    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&pf->info, &ascent, &descent, &line_gap);
    pf->ascent = ascent * pf->scale;
    pf->descent = descent * pf->scale;
    pf->line_gap = line_gap * pf->scale;

    return (long long)pf;
}

// ─── Glyph Rasterization ─────────────────────────────────────────────────

long long pledge_font_get_glyph(void* font, long long codepoint,
                                  long long* out_w, long long* out_h,
                                  long long* out_xoff, long long* out_yoff,
                                  double* out_advance, void* out_bitmap) {
    if (!font) return -1;
    PledgeFont* pf = (PledgeFont*)font;

    int w, h, xoff, yoff;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &pf->info, pf->scale, pf->scale, (int)codepoint,
        &w, &h, &xoff, &yoff);

    if (!bitmap && (w == 0 || h == 0)) {
        // Space or missing glyph — advance only
        int advance;
        int lsb;
        stbtt_GetCodepointHMetrics(&pf->info, (int)codepoint, &advance, &lsb);
        if (out_w) *out_w = 0;
        if (out_h) *out_h = 0;
        if (out_xoff) *out_xoff = 0;
        if (out_yoff) *out_yoff = 0;
        if (out_advance) *out_advance = advance * pf->scale;
        return 0;
    }

    if (out_w) *out_w = w;
    if (out_h) *out_h = h;
    if (out_xoff) *out_xoff = xoff;
    if (out_yoff) *out_yoff = yoff;

    int advance;
    int lsb;
    stbtt_GetCodepointHMetrics(&pf->info, (int)codepoint, &advance, &lsb);
    if (out_advance) *out_advance = advance * pf->scale;

    // Copy bitmap to output buffer (out_bitmap is a Fides Vec<i64>)
    // Each byte becomes an i64 in the Vec
    if (out_bitmap && bitmap) {
        long long* vec_data = (long long*)out_bitmap;
        // The Vec<i64> data pointer is at offset 0
        // We'd need to push each byte as an i64 — this requires
        // understanding the Fides Vec layout.
        // For now, copy to a temporary buffer.
        // A full implementation would call the Fides Vec push method.
        int i;
        for (i = 0; i < w * h; i++) {
            vec_data[i] = (long long)bitmap[i];
        }
    }

    if (bitmap) stbtt_FreeBitmap(bitmap, NULL);
    return 0;
}

double pledge_font_kern(void* font, long long ch1, long long ch2) {
    if (!font) return 0.0;
    PledgeFont* pf = (PledgeFont*)font;
    return stbtt_GetCodepointKernAdvance(&pf->info, (int)ch1, (int)ch2) * pf->scale;
}

long long pledge_font_text_width(void* font, char* text) {
    if (!font || !text) return 0;
    PledgeFont* pf = (PledgeFont*)font;

    float width = 0.0f;
    int len = (int)strlen(text);
    int i;
    for (i = 0; i < len; i++) {
        int advance;
        int lsb;
        stbtt_GetCodepointHMetrics(&pf->info, (unsigned char)text[i], &advance, &lsb);
        width += advance * pf->scale;
        if (i > 0) {
            width += stbtt_GetCodepointKernAdvance(&pf->info,
                (unsigned char)text[i-1], (unsigned char)text[i]) * pf->scale;
        }
    }

    return (long long)width;
}

void pledge_font_free(void* font) {
    if (!font) return;
    PledgeFont* pf = (PledgeFont*)font;
    if (pf->ttf_data) free(pf->ttf_data);
    free(pf);
}
