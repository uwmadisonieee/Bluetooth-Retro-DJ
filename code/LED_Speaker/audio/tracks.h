#ifndef AUDIO_TRACKS_H_
#define AUDIO_TRACKS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>

#define TRACKS_NAME_SIZE 128
#define TRACKS_MAX_COUNT 30
#define TRACKS_PATH "./audio/tracks/"
#define TRACKS_RATE 44100

typedef struct track_t {
  char name[TRACKS_NAME_SIZE];
  uint32_t size; // number of buffer segments
  uint32_t time; // ms
  void* buffer;
  uint8_t analysis[512];
} track_t;

int tracks_count;

// All the tracks!
track_t* tracks;

void TracksReadFiles();

// Assumes buffer is empty string and can hold all of it
void TracksAsString(char* buffer);

void TracksAnalysis(void* buffer, uint32_t size, int index);

#endif
