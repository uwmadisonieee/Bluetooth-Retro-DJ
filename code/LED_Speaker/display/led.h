#ifndef DISPLAY_LED_H_
#define DISPLAY_LED_H_

#include "spi.h"
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define LED_COUNT 20
#define LED_MAX_BRIGHTNESS 31

typedef struct led_frame {
  uint8_t head;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} led_frame;

void LEDSetup(void);

void LEDSend(void);

led_frame LEDMakeFrame(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue);

void LEDSetSide(uint8_t side, led_frame frame);

void LEDSetSideBrightness(uint8_t side, uint8_t brightness);

void LEDSetAll(led_frame frame);

void LEDSetAllBrightness(uint8_t brightness);

void LEDSetFrame(uint8_t led, led_frame frame);

#endif
