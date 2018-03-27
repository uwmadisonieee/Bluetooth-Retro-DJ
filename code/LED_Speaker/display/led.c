#include "led.h"

static uint8_t* led_buffer;
static uint8_t* led_start_frame;
static uint8_t* led_end_frame;

static led_frame led_f_red = {0xff, 0xff, 0x00, 0x00};
//static led_frame led_f_green = {0xff, 0xff, 0x00, 0x00};
//static led_frame led_f_blue = {0xff, 0xff, 0x00, 0x00};

void led_setup() {

  led_buffer = (uint8_t*)malloc(sizeof(led_frame) * LED_COUNT);
  if (led_buffer == NULL) {
    fprintf(stderr, "failed to allocated LED\n"); exit(-1);
  }
  led_start_frame = (uint8_t*)malloc(4);
  if (led_start_frame == NULL) {
    fprintf(stderr, "failed to allocated LED\n"); exit(-1);
  }
  led_end_frame = (uint8_t*)malloc(4);
  if (led_end_frame == NULL) {
    fprintf(stderr, "failed to allocated LED\n"); exit(-1);
  }

  memset(led_start_frame, 0, 4);
  memset(led_end_frame, 0, 4);

  // init set to red
  led_set_all(led_f_red);

  led_send();
}

void led_send() {
  mraa_gpio_write(cs_c, 1); // need to start somewhere

  mraa_spi_write_buf(spi_c, led_start_frame, 4);
  mraa_spi_write_buf(spi_c, led_buffer, sizeof(led_frame) * LED_COUNT);
  mraa_spi_write_buf(spi_c, led_end_frame, 4);
}

led_frame led_make_frame(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue) {
  if (brightness > LED_MAX_BRIGHTNESS) { brightness = LED_MAX_BRIGHTNESS; }

  uint8_t start = 0xE0 | (brightness & 0x1F);
  led_frame ret = {start, red, green, blue};
  return ret;
}

void led_set_side(uint8_t side, led_frame frame) {
  int i;
  if (side > 3) { side = 3; }

  if (side == 0) {
    memcpy(led_buffer + (4 * 0), &frame, 4);
    memcpy(led_buffer + (4 * 1), &frame, 4);
    memcpy(led_buffer + (4 * 2), &frame, 4);
    memcpy(led_buffer + (4 * 18), &frame, 4);
    memcpy(led_buffer + (4 * 19), &frame, 4);
  } else {
    for (i = 0; i < 5; i++) {
      // idk anymore, it works
      memcpy(led_buffer + (4 * ( (5 * (side-1)) + 3 + i) ), &frame, 4);
    }
  }
}

void led_set_side_brightness(uint8_t side, uint8_t brightness) {
  int i;
  if (side > 3) { side = 3; }
  if (brightness > LED_MAX_BRIGHTNESS) { brightness = LED_MAX_BRIGHTNESS; }
  uint8_t start = 0xE0 | (brightness & 0x1F);

  if (side == 0) {
    memcpy(led_buffer + (4 * 0), &start, 1);
    memcpy(led_buffer + (4 * 1), &start, 1);
    memcpy(led_buffer + (4 * 2), &start, 1);
    memcpy(led_buffer + (4 * 18), &start, 1);
    memcpy(led_buffer + (4 * 19), &start, 1);
  } else {
    for (i = 0; i < 5; i++) {
      // idk anymore, it works
      memcpy(led_buffer + (4 * ( (5 * (side-1)) + 3 + i) ), &start, 1);
    }
  }
}

void led_set_all(led_frame frame) {
  int i;
  for (i = 0; i < LED_COUNT; i++) {
    memcpy(led_buffer + (4 * i), &frame, 4);
  }
}

void led_set_all_brightness(uint8_t brightness) {
  int i;
  if (brightness > LED_MAX_BRIGHTNESS) { brightness = LED_MAX_BRIGHTNESS; }
  uint8_t start = 0xE0 | (brightness & 0x1F);

   for (i = 0; i < LED_COUNT; i++) {
    memcpy(led_buffer + (4 * i), &start, 1);
  }
}

void led_set_frame(uint8_t led, led_frame frame) {
  if (led >= LED_COUNT) { led = LED_COUNT - 1; }

  memcpy(led_buffer + (4 * led), &frame, 4);
}
