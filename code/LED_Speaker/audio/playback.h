#ifndef AUDIO_PLAYBACK_H_
#define AUDIO_PLAYBACK_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <pthread.h>

#include "tracks.h"

int play_test;

uint8_t playback_new_track;
uint8_t playback_pause;

int playback_cur_track;

pthread_t playback_thread;

void* TrackPlay(void* na);

void PlaybackSetup(void);
void TrackSeek(int amount);
void TrackChange(int index);

// if Direction is 0 or greater moves forward
// negative direction goes back
void TrackNext(int direction);

void PlaybackCleanup(void);

#define PCM_DEVICE "plughw:0,1"

// BIT PERFECT (may clip)
//#define SAMPLE_TO_S16(x)    ((int16_t) ((x * 32768.0 >= 32767.5) ? 32767 : lround(x * 32768.0)))

#define SAMPLE_TO_S16(x)    ((int16_t) lround(x * 32767.0))
#define S16_TO_SAMPLE(x)    ((double) x / 32768.0)

#endif
