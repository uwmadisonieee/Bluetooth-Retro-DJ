#include "led.h"

static uint8_t* led_buffer;

static led_frame led_f_red = {0xff, 0xff, 0x00, 0x00};
static led_frame led_f_green = {0xff, 0x00, 0xff, 0x00};
static led_frame led_f_blue = {0xff, 0x00, 0x00, 0xff};

void LEDSetup() {

  led_buffer = (uint8_t*)malloc((sizeof(led_frame) * LED_COUNT) + 8);
  if (led_buffer == NULL) {
    fprintf(stderr, "failed to allocated LED\n"); exit(-1);
  }
  
  memset(led_buffer, 0, LED_END_OFF);
  memset(led_buffer + LED_END_OFF, 0xff, 4);

  led_effect = 0;
  
  // init set to red
  LEDSetAll(led_f_red);
}

void LEDSend() {
  mraa_spi_write_buf(spi_c, led_buffer, LED_BUF_LEN);
}

led_frame LEDMakeFrame(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue) {
  if (brightness > LED_MAX_BRIGHTNESS) { brightness = LED_MAX_BRIGHTNESS; }

  uint8_t start = 0xE0 | (brightness & 0x1F);
  led_frame ret = {start, red, green, blue};
  return ret;
}

void LEDSetSide(uint8_t side, uint8_t red, uint8_t green, uint8_t blue) {
  int i;
  led_frame frame = {0xFA, red, green, blue};
  if (side > 3) { side = 3; }

  if (side == 0) {
    memcpy(led_buffer + 4 + (4 * 0), &frame, 4);
    memcpy(led_buffer + 4 + (4 * 1), &frame, 4);
    memcpy(led_buffer + 4 + (4 * 2), &frame, 4);
    memcpy(led_buffer + 4 + (4 * 18), &frame, 4);
    memcpy(led_buffer + 4 + (4 * 19), &frame, 4);
  } else {
    for (i = 0; i < 5; i++) {
      // idk anymore, it works
      memcpy(led_buffer + 4+ (4 * ( (5 * (side-1)) + 3 + i) ), &frame, 4);
    }
  }
}

void LEDSetSideBrightness(uint8_t side, uint8_t brightness) {
  int i;
  if (side > 3) { side = 3; }
  if (brightness > LED_MAX_BRIGHTNESS) { brightness = LED_MAX_BRIGHTNESS; }
  uint8_t start = 0xE0 | (brightness & 0x1F);

  if (side == 0) {
    memcpy(led_buffer + 4 + (4 * 0), &start, 1);
    memcpy(led_buffer + 4 + (4 * 1), &start, 1);
    memcpy(led_buffer + 4 + (4 * 2), &start, 1);
    memcpy(led_buffer + 4 + (4 * 18), &start, 1);
    memcpy(led_buffer + 4 + (4 * 19), &start, 1);
  } else {
    for (i = 0; i < 5; i++) {
      // idk anymore, it works
      memcpy(led_buffer + 4 + (4 * ( (5 * (side-1)) + 3 + i) ), &start, 1);
    }
  }
}

void LEDSetAll(led_frame frame) {
  int i;
  for (i = 0; i < LED_COUNT; i++) {
    memcpy(led_buffer + 4 + (4 * i), &frame, 4);
  }
}

void LEDSetAllRed() {
  LEDSetAll(led_f_red);
}
void LEDSetAllGreen() {
  LEDSetAll(led_f_green);
}
void LEDSetAllBlue() {
  LEDSetAll(led_f_blue);
}

static int snake_count = 0;
void LEDSnake(uint8_t red, uint8_t green, uint8_t blue) {
  int i;
  int brite = 5; // snake brightness gores from 5 - 25
  led_frame snake_frame = {0xE5, red, green, blue}; 
  snake_count++;
  if (snake_count >= LED_COUNT) { snake_count = 0; }


  for (i = snake_count; i < LED_COUNT; i++) {
    snake_frame.head = 0xE0 | ((brite) & 0x1F);
    memcpy(led_buffer + 4 + (4 * i), &snake_frame, 4);
    brite++;
  }
  for (i = 0; i < snake_count; i++) {
    snake_frame.head = 0xE0 | ((brite) & 0x1F);
    memcpy(led_buffer + 4 + (4 * i), &snake_frame, 4);
    brite++;
  }

  snake_count++;
  
}

void LEDSetAllBrightness(uint8_t brightness) {
  int i;
  if (brightness > LED_MAX_BRIGHTNESS) { brightness = LED_MAX_BRIGHTNESS; }
  uint8_t start = 0xE0 | (brightness & 0x1F);

   for (i = 0; i < LED_COUNT; i++) {
    memcpy(led_buffer + 4 + (4 * i), &start, 1);
  }
}

void LEDSetFrame(uint8_t led, led_frame frame) {
  if (led >= LED_COUNT) { led = LED_COUNT - 1; }

  memcpy(led_buffer + 4 + (4 * led), &frame, 4);
}
