#ifndef DISPLAY_GIF_H_
#define DISPLAY_GIF_H_

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdint.h>

#include <gif_lib.h>

#define GIF_MESSAGE(Msg) fprintf(stderr, "\ngiflib: %s\n", Msg)

// Pass in file and buffer to store gif
// assume gif dimensions match buffer
// returns number of frames gif needed on success
int GIF2RGB(char *FileName, uint8_t *Buffer, int MaxFrameLen, int FrameSize );

#endif
