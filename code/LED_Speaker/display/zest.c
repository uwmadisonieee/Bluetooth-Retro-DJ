#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <mraa/spi.h>
#include <mraa/gpio.h>

#define SPI_BUS 0
#define SPI_FREQ 8000000 // 32 MHz

#define LED_COUNT 10
#define LED_MAX_BRIGHTNESS 31

typedef struct led_frame {
  uint8_t head;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} led_frame;

#define CS 24
static mraa_spi_context spi_c;
static mraa_gpio_context cs;

static uint8_t* led_buffer;
static uint8_t* led_start_frame;
static uint8_t* led_end_frame;

static uint8_t led_f_red[4] = {0xff, 0xff, 0x00, 0x00};

int main(int argc, char* argv[]) {

  if (argc < 5) { puts("./zest brightness red gree blue"); }
  uint8_t brightness = (uint8_t)atoi(argv[1]);
  uint8_t red = (uint8_t)atoi(argv[2]);
  uint8_t green = (uint8_t)atoi(argv[3]);
  uint8_t blue = (uint8_t)atoi(argv[4]);

  mraa_result_t status = MRAA_SUCCESS;
  
  int i, j;

  if (brightness > LED_MAX_BRIGHTNESS) { brightness = LED_MAX_BRIGHTNESS; }
  brightness = 0xE0 | (brightness & 0x1F);


  uint8_t test_buffer[88] = {
    0, 0, 0, 0,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,
    brightness, red, blue, green,

    0, 0, 0, 0
  };
  mraa_init();
  
  /* initialize GPIO pin */
  cs = mraa_gpio_init(CS);
  if (cs == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", CS);
    mraa_deinit();
    return EXIT_FAILURE;
  }

  /* set GPIO to output */
  status = mraa_gpio_dir(cs, MRAA_GPIO_OUT);
  if (status != MRAA_SUCCESS) {
    goto err_exit;
  }

  mraa_gpio_write(cs, 1);

  //! [Interesting] 
  /* initialize SPI bus */
  spi_c = mraa_spi_init(SPI_BUS);
  if (spi_c == NULL) {
    fprintf(stderr, "Failed to initialize SPI\n");
    mraa_deinit();
    return EXIT_FAILURE;
  }

  /* set SPI frequency */
  status = mraa_spi_frequency(spi_c, SPI_FREQ);
  if (status != MRAA_SUCCESS)
    goto err_exit;

  /* set big endian mode */
  status = mraa_spi_lsbmode(spi_c, 0);
  if (status != MRAA_SUCCESS) {
    goto err_exit;
  }

  /* MAX7219/21 chip needs the data in word size */
  status = mraa_spi_bit_per_word(spi_c, 8);
  if (status != MRAA_SUCCESS) {
    fprintf(stdout, "Failed to set SPI Device to 16Bit mode\n");
    goto err_exit;
  }

  led_buffer = (uint8_t*)malloc(2048);
  if (led_buffer == NULL) {
    fprintf(stderr, "failed to allocated LED\n"); exit(-1);
  }
  
  memset(led_buffer, 0, 4);
  for (i = 0; i < 10; i++){
    memcpy(led_buffer + 4 + (i * 4), led_f_red, 4);
  }
  memset(led_buffer + (4 * 11), 0, 4);

  mraa_spi_write_buf(spi_c, test_buffer, 88);
  puts("sent");

  /* stop spi */
  mraa_spi_stop(spi_c);

  //! [Interesting]
  /* deinitialize mraa for the platform (not needed most of the times) */
  mraa_deinit();

  return EXIT_SUCCESS;

 err_exit:
  mraa_result_print(status);

  /* stop spi */
  mraa_spi_stop(spi_c);

  /* deinitialize mraa for the platform (not needed most of the times) */
  mraa_deinit();

  return EXIT_FAILURE;
}
