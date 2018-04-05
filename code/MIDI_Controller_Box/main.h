#ifndef MIDI_MAIN_H_
#define MIDI_MAIN_H_

#include "server/server.h"
#include "server/blueServer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <ads1115.h>
#include <pthread.h>

#define BLUE_BUT 19
#define GREEN_BUT 13
#define RED_BUT 13
#define GIF_BUT 6
#define JOG_DATA_A 17
#define JOG_DATA_B 22

#define ADC_BASE 2222
#define SLIDER_BASE 3
#define POT_BASE 1

#define POT_DIV 880
#define SLIDER_DIV 528

uint8_t play_pause = 0;

#endif
