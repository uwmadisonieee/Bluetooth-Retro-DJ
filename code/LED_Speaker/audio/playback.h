#ifndef AUDIO_PLAYBACK_H_
#define AUDIO_PLAYBACK_H_

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>

#include "../display/led.h"
#include "tracks.h"

uint8_t playback_play_sample;
uint8_t playback_new_track;
uint8_t playback_pause;

int playback_cur_track;
int playback_cur_sample;

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

// LED effect const
#define PLAY_COL_1_8 (32767 / 8) * 1
#define PLAY_COL_2_8 (32767 / 8) * 2
#define PLAY_COL_3_8 (32767 / 8) * 3
#define PLAY_COL_4_8 (32767 / 8) * 4
#define PLAY_COL_5_8 (32767 / 8) * 5 
#define PLAY_COL_6_8 (32767 / 8) * 6
#define PLAY_COL_7_8 (32767 / 8) * 7
#define PLAY_COL_8_8 (32767 / 8) * 8

// BIT PERFECT (may clip)
//#define SAMPLE_TO_S16(x)    ((int16_t) ((x * 32768.0 >= 32767.5) ? 32767 : lround(x * 32768.0)))

#define SAMPLE_TO_S16(x)    ((int16_t) lround(x * 32767.0))
#define S16_TO_SAMPLE(x)    ((double) x / 32768.0)

#endif
