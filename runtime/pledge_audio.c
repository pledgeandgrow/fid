// pledge_audio.c — Audio backend for Fides
//
// Audio playback via SDL2_mixer. Implements the FFI functions declared
// in stdlib/audio.fd.
//
// Build: gcc -c pledge_audio.c -o pledge_audio.o $(sdl2-config --cflags)
// Link: -lSDL2 -lSDL2_mixer
//
// Alternative (no SDL2): use ALSA directly (libasound2-dev).
// The SDL2_mixer path is recommended for cross-platform compatibility.

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ─── Constants ───────────────────────────────────────────────────────────

#define MAX_CHANNELS 32
#define MAX_SOUNDS 256

// ─── State ───────────────────────────────────────────────────────────────

static int initialized = 0;

// Sound cache: loaded WAV/OGG data
static Mix_Chunk* sounds[MAX_SOUNDS];
static int num_sounds = 0;

// Channel state
typedef struct {
    int sound_ref;      // index into sounds[] (-1 = free)
    double volume;      // 0.0 to 1.0
    double pan;         // -1.0 (left) to 1.0 (right)
    int looping;        // 0 = no loop, 1 = loop
    int playing;        // 0 = stopped, 1 = playing
} AudioChannel;

static AudioChannel channels[MAX_CHANNELS];

// ─── pledge_audio.h API (used by stdlib/audio.fd) ────────────────────────

long long pledge_audio_init(void) {
    if (initialized) return 0;

    // Initialize SDL_mixer: 44100 Hz, 16-bit, stereo, 1024 chunk size
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
        fprintf(stderr, "pledge_audio_init: Mix_OpenAudio failed: %s\n", Mix_GetError());
        return -1;
    }

    // Allocate 32 mixing channels
    Mix_AllocateChannels(MAX_CHANNELS);

    // Initialize channel state
    int i;
    for (i = 0; i < MAX_CHANNELS; i++) {
        channels[i].sound_ref = -1;
        channels[i].volume = 1.0;
        channels[i].pan = 0.0;
        channels[i].looping = 0;
        channels[i].playing = 0;
    }

    // Clear sound cache
    memset(sounds, 0, sizeof(sounds));
    num_sounds = 0;

    initialized = 1;
    return 0;
}

void pledge_audio_shutdown(void) {
    if (!initialized) return;

    // Stop all channels
    Mix_HaltChannel(-1);

    // Free all sounds
    int i;
    for (i = 0; i < num_sounds; i++) {
        if (sounds[i]) {
            Mix_FreeChunk(sounds[i]);
            sounds[i] = NULL;
        }
    }

    Mix_CloseAudio();
    initialized = 0;
}

long long pledge_audio_load(char* path) {
    if (!initialized || !path) return -1;

    if (num_sounds >= MAX_SOUNDS) {
        fprintf(stderr, "pledge_audio_load: sound cache full (%d sounds)\n", MAX_SOUNDS);
        return -1;
    }

    Mix_Chunk* chunk = Mix_LoadWAV(path);
    if (!chunk) {
        fprintf(stderr, "pledge_audio_load: Mix_LoadWAV('%s') failed: %s\n", path, Mix_GetError());
        return -1;
    }

    int index = num_sounds;
    sounds[index] = chunk;
    num_sounds++;
    return (long long)index;
}

long long pledge_audio_play(long long sound, double volume) {
    if (!initialized || sound < 0 || sound >= num_sounds) return -1;
    if (!sounds[sound]) return -1;

    // Find a free channel
    int ch = Mix_GroupAvailable(-1);
    if (ch < 0) {
        // No free channel, use channel 0 (oldest sound gets cut)
        ch = 0;
    }

    // Set volume (0-128)
    int vol = (int)(volume * 128.0);
    if (vol < 0) vol = 0;
    if (vol > 128) vol = 128;
    Mix_Volume(ch, vol);

    // Play the sound (0 loops = play once)
    if (Mix_PlayChannel(ch, sounds[sound], 0) < 0) {
        fprintf(stderr, "pledge_audio_play: Mix_PlayChannel failed: %s\n", Mix_GetError());
        return -1;
    }

    channels[ch].sound_ref = (int)sound;
    channels[ch].volume = volume;
    channels[ch].looping = 0;
    channels[ch].playing = 1;

    return (long long)ch;
}

void pledge_audio_stop(long long channel) {
    if (!initialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;

    Mix_HaltChannel((int)channel);
    channels[channel].playing = 0;
    channels[channel].sound_ref = -1;
}

void pledge_audio_set_volume(long long channel, double volume) {
    if (!initialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;

    int vol = (int)(volume * 128.0);
    if (vol < 0) vol = 0;
    if (vol > 128) vol = 128;
    Mix_Volume((int)channel, vol);
    channels[channel].volume = volume;
}

void pledge_audio_pause(long long channel) {
    if (!initialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    Mix_Pause((int)channel);
}

void pledge_audio_resume(long long channel) {
    if (!initialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    Mix_Resume((int)channel);
}

long long pledge_audio_is_playing(long long channel) {
    if (!initialized) return 0;
    if (channel < 0 || channel >= MAX_CHANNELS) return 0;
    return (long long)Mix_Playing((int)channel);
}

void pledge_audio_set_loop(long long channel, long long looping) {
    if (!initialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    channels[channel].looping = (int)looping;

    // SDL_mixer doesn't support changing loop status after playback starts.
    // To loop, use pledge_audio_play_loop instead.
    // This is stored for informational purposes.
}

void pledge_audio_set_pan(long long channel, double pan) {
    if (!initialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;

    // SDL_mixer pan: 0 = left, 128 = center, 255 = right
    // Our pan: -1.0 = left, 0.0 = center, 1.0 = right
    int sdl_pan = (int)((pan + 1.0) * 127.5);
    if (sdl_pan < 0) sdl_pan = 0;
    if (sdl_pan > 255) sdl_pan = 255;

    // Mix_SetPanning(channel, left_vol, right_vol)
    // For simplicity, use left/right volumes based on pan
    Uint8 left = (Uint8)(255 - sdl_pan);
    Uint8 right = (Uint8)sdl_pan;
    Mix_SetPanning((int)channel, left, right);
    channels[channel].pan = pan;
}

void pledge_audio_stop_all(void) {
    if (!initialized) return;
    Mix_HaltChannel(-1);
    int i;
    for (i = 0; i < MAX_CHANNELS; i++) {
        channels[i].playing = 0;
        channels[i].sound_ref = -1;
    }
}

void pledge_audio_master_volume(double volume) {
    if (!initialized) return;
    int vol = (int)(volume * 128.0);
    if (vol < 0) vol = 0;
    if (vol > 128) vol = 128;
    Mix_Volume(-1, vol);  // -1 = all channels
}

// ─── WAV File Parser (fallback if SDL_mixer not available) ────────────────
//
// A minimal WAV parser that extracts PCM data from a RIFF/WAVE file.
// This is used when SDL_mixer is not linked, as a fallback.

#pragma pack(push, 1)
typedef struct {
    char riff[4];        // "RIFF"
    unsigned int size;   // file size - 8
    char wave[4];        // "WAVE"
} WavHeader;

typedef struct {
    char id[4];          // "fmt "
    unsigned int size;   // chunk size (16 for PCM)
    short format;        // 1 = PCM
    short channels;      // 1 or 2
    unsigned int sample_rate;
    unsigned int byte_rate;
    short block_align;
    short bits_per_sample;
} FmtChunk;

typedef struct {
    char id[4];          // "data"
    unsigned int size;   // data size
} DataChunk;
#pragma pack(pop)

// Parse a WAV file and return PCM data.
// Returns 0 on success, -1 on failure.
// On success: *out_data points to malloc'd PCM data, *out_size = data size.
int pledge_parse_wav(const char* path, void** out_data, unsigned int* out_size,
                     int* out_channels, int* out_sample_rate, int* out_bits) {
    FILE* f = fopen(path, "rb");
    if (!f) return -1;

    WavHeader hdr;
    if (fread(&hdr, sizeof(WavHeader), 1, f) != 1) { fclose(f); return -1; }
    if (memcmp(hdr.riff, "RIFF", 4) != 0 || memcmp(hdr.wave, "WAVE", 4) != 0) {
        fclose(f);
        return -1;
    }

    FmtChunk fmt;
    DataChunk data;
    int found_fmt = 0;
    int found_data = 0;

    // Read chunks until we find fmt and data
    while (!found_fmt || !found_data) {
        char chunk_id[4];
        unsigned int chunk_size;
        if (fread(chunk_id, 4, 1, f) != 1) break;
        if (fread(&chunk_size, 4, 1, f) != 1) break;

        if (memcmp(chunk_id, "fmt ", 4) == 0) {
            if (fread(&fmt, sizeof(FmtChunk), 1, f) != 1) { fclose(f); return -1; }
            // Skip any extra fmt bytes
            if (chunk_size > sizeof(FmtChunk)) {
                fseek(f, chunk_size - sizeof(FmtChunk), SEEK_CUR);
            }
            found_fmt = 1;
        } else if (memcmp(chunk_id, "data", 4) == 0) {
            data.size = chunk_size;
            void* pcm = malloc(chunk_size);
            if (!pcm) { fclose(f); return -1; }
            if (fread(pcm, 1, chunk_size, f) != chunk_size) {
                free(pcm);
                fclose(f);
                return -1;
            }
            *out_data = pcm;
            *out_size = chunk_size;
            found_data = 1;
        } else {
            // Skip unknown chunk
            fseek(f, chunk_size, SEEK_CUR);
        }
    }

    fclose(f);

    if (!found_fmt || !found_data) return -1;

    if (out_channels) *out_channels = fmt.channels;
    if (out_sample_rate) *out_sample_rate = fmt.sample_rate;
    if (out_bits) *out_bits = fmt.bits_per_sample;

    return 0;
}
