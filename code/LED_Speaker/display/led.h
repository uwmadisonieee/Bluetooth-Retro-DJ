#ifndef DISPLAY_LED_H_
#define DISPLAY_LED_H_

#include "spi.h"
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define LED_COUNT 20
#define LED_MAX_BRIGHTNESS 31
#define LED_END_OFF 84
#define LED_OFFSET 4 // from start frame
#define LED_DATA_LEN 80
#define LED_BUF_LEN 88

#define LED_MAX_EFFECTS 6
int led_effect;

#define LED_CORRECTION 6.21

typedef struct led_frame {
  uint8_t head;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} led_frame;

void LEDSetup(void);

void LEDSend(void);

led_frame LEDMakeFrame(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue);

void LEDSetSide(uint8_t side, uint8_t red, uint8_t green, uint8_t blue);

void LEDSetSideBrightness(uint8_t side, uint8_t brightness);

void LEDSetAll(led_frame frame);

void LEDSetAllRed(void);
void LEDSetAllGreen(void);
void LEDSetAllBlue(void);

void LEDSnake(uint8_t red, uint8_t green, uint8_t blue);
void LEDSetAllBrightness(uint8_t brightness);

void LEDSetFrame(uint8_t led, led_frame frame);

#endif
