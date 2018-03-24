#ifndef AUDIO_TRACKS_H_
#define AUDIO_TRACKS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define TRACKS_NAME_SIZE 128
#define TRACKS_MAX_COUNT 10
#define TRACKS_PATH "./audio/tracks/"

int tracks_count;

void TracksReadFiles();

char* TracksAt(int index);

// Assumes buffer is empty string and can hold all of it
void TracksAsString(char* buffer);


#endif
