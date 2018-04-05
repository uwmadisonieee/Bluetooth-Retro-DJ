#ifndef AUDIO_TRACKS_H_
#define AUDIO_TRACKS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <time.h>

#define TRACKS_NAME_SIZE 128
#define TRACKS_MAX_COUNT 30
#define SAMPLE_MAX_COUNT 30
#define TRACKS_PATH "/home/linaro/Bluetooth-Retro-DJ/code/LED_Speaker/audio/tracks/"
#define SAMPLES_PATH "/home/linaro/Bluetooth-Retro-DJ/code/LED_Speaker/audio/samples/"
#define TRACKS_RATE 44100
#define TRACKS_ANALYSIS_SIZE 150

#define WAV_HEADER_SIZE 44

typedef struct track_t {
  char name[TRACKS_NAME_SIZE];
  uint32_t size; // number of buffer segments
  uint32_t file_size; // not including 44 byte header
  uint32_t time; // ms
  void* buffer;
  char analysis[TRACKS_ANALYSIS_SIZE * 10];
} track_t;

typedef struct sample_t {
  char name[TRACKS_NAME_SIZE];
  uint32_t size; // number of buffer segments
  int16_t* buffer;
} sample_t;

int tracks_count;
int samples_count;

uint32_t buffer_size;

// All the tracks and samples!
track_t* tracks;
sample_t* samples_x;

void TracksReadFiles();

int TrackLoad(char* file, track_t* track);

int SampleLoad(char* file, sample_t* sample);

// Assumes buffer is empty string and can hold all of it
void TracksAsString(char* buffer);

// sends backs offset before buffer of TRACKS_ANALYSIS_SIZE
int TracksInfo(char* buffer, int index);

void TracksAnalysis(track_t* track);

#endif
