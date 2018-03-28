#ifndef SPEAKER_MAIN_H
#define SPEAKER_MAIN_H

#include "audio/playback.h"
#include "server/blueServer.h"
#include "display/spi.h"
#include "display/led.h"
#include "display/lcd_tft.h"

pthread_t spi_thread;

void* SPILoop(void* na);

void BlueDataCallback( int type, char* value);

#endif
