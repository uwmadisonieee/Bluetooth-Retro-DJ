#ifndef DISPLAY_LED_H_
#define DISPLAY_LED_H_

#include "spi.h"
#include <stdint.h>
#include <unistd.h>

#define LED_COUNT 20
#define LED_MAX_BRIGHTNESS 31

typedef struct led_frame {
  uint8_t head;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} led_frame;

void led_setup(void);

void led_send(void);

led_frame led_make_frame(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue);

void led_set_side(uint8_t side, led_frame frame);

void led_set_side_brightness(uint8_t side, uint8_t brightness);

void led_set_all(led_frame frame);

void led_set_all_brightness(uint8_t brightness);

void led_set_frame(uint8_t led, led_frame frame);

#endif
