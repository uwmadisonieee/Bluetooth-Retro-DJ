#include "spi.h"

void spi_cleanup() {
  mraa_result_print(status);

  /* stop spi */
  mraa_spi_stop(spi_c);

  /* deinitialize mraa for the platform (not needed most of the times) */
  mraa_deinit();

  exit(-1);
}

void spi_setup() {
  mraa_result_t status = MRAA_SUCCESS;

  /* initialize mraa for the platform (not needed most of the times) */
  mraa_init();

    /* initialize GPIO pin */
  cs_c = mraa_gpio_init(SPI_CS);
  if (cs_c == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", SPI_CS);
    mraa_deinit();
    return EXIT_FAILURE;
  }

  /* set GPIO to output */
  status = mraa_gpio_dir(cs_c, MRAA_GPIO_OUT);
  if (status != MRAA_SUCCESS) {
    spi_cleanup();
  }

  mraa_gpio_write(cs_c, 0); // need to start somewhere

  /* initialize SPI bus */
  spi_c = mraa_spi_init(SPI_BUS);
  if (spi_c == NULL) {
    fprintf(stderr, "Failed to initialize SPI\n");
    mraa_deinit();
    return EXIT_FAILURE;
  }

  /* set SPI frequency */
  status = mraa_spi_frequency(spi_c, SPI_FREQ);
  if (status != MRAA_SUCCESS) {
    spi_cleanup();
  }

  /* set big endian mode */
  status = mraa_spi_lsbmode(spi_c, 0);
  if (status != MRAA_SUCCESS) {
    spi_cleanup();
  }

  /* MAX7219/21 chip needs the data in word size */
  status = mraa_spi_bit_per_word(spi_c, 8);
  if (status != MRAA_SUCCESS) {
    fprintf(stdout, "Failed to set SPI Device to 16Bit mode\n");
    spi_cleanup();
  }


}
