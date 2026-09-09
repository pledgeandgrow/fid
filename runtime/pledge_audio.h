// pledge_audio.h — Audio backend for Fides
//
// Audio playback via SDL2_mixer. Implements the FFI functions
// declared in stdlib/audio.fd.

#ifndef PLEDGE_AUDIO_H
#define PLEDGE_AUDIO_H

long long pledge_audio_init(void);
void pledge_audio_shutdown(void);
long long pledge_audio_load(char* path);
long long pledge_audio_play(long long sound, double volume);
void pledge_audio_stop(long long channel);
void pledge_audio_set_volume(long long channel, double volume);
void pledge_audio_pause(long long channel);
void pledge_audio_resume(long long channel);
long long pledge_audio_is_playing(long long channel);
void pledge_audio_set_loop(long long channel, long long looping);
void pledge_audio_set_pan(long long channel, double pan);
void pledge_audio_stop_all(void);
void pledge_audio_master_volume(double volume);

// WAV file parser (fallback)
int pledge_parse_wav(const char* path, void** out_data, unsigned int* out_size,
                     int* out_channels, int* out_sample_rate, int* out_bits);

#endif // PLEDGE_AUDIO_H
