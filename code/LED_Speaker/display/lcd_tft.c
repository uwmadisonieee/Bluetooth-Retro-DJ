#include "lcd_tft.h"

static mraa_spi_context spi;
static mraa_gpio_context cs, dc, hreset;

static void set_addr_window(int x0, int x1, int y0, int y1) {
  mraa_gpio_write(dc, 0);
  mraa_spi_write(spi, TFT_RASET);
  mraa_gpio_write(dc, 1);

  mraa_spi_write(spi, x0 >> 8);
  mraa_spi_write(spi, x0 & 0xFF);
  mraa_spi_write(spi, x1 >> 8);
  mraa_spi_write(spi, x1 & 0xFF);

  mraa_gpio_write(dc, 0);
  mraa_spi_write(spi, TFT_CASET);
  mraa_gpio_write(dc, 1);

  mraa_spi_write(spi, y0 >> 8);
  mraa_spi_write(spi, y0 & 0xFF);
  mraa_spi_write(spi, y1 >> 8);
  mraa_spi_write(spi, y1 & 0xFF);
}

int LCD_Start(int index) {

  mraa_result_t status = MRAA_SUCCESS;

    int i, j;

    int gif_length;
    uint8_t *frame_buffers;
    int frame_size = TFT_WIDTH * TFT_HEIGHT * TFT_COLOR * sizeof(uint8_t);

    frame_buffers = (uint8_t*)malloc(frame_size * TFT_MAX_FB);
    if (frame_buffers == NULL) {
      puts("Can't malloc frame_buffer");
      return -1;
    }


    gif_length = GIF2RGB("gifs/1.gif", frame_buffers, TFT_MAX_FB, frame_size);
    if (gif_length <= 0) {
      printf("Gif_length failed: %d\n", gif_length);
      return -1;
    }

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    /* initialize GPIO pin */
    cs = mraa_gpio_init(CS);
    if (cs == NULL) {
      fprintf(stderr, "Failed to initialize GPIO %d\n", CS);
      mraa_deinit();
      return EXIT_FAILURE;
    }

    /* initialize GPIO pin */
    dc = mraa_gpio_init(DC);
    if (dc == NULL) {
      fprintf(stderr, "Failed to initialize GPIO %d\n", DC);
      mraa_deinit();
      return EXIT_FAILURE;
    }

    hreset = mraa_gpio_init(HRESET);
    if (dc == NULL) {
      fprintf(stderr, "Failed to initialize GPIO %d\n", HRESET);
      mraa_deinit();
      return EXIT_FAILURE;
    }
    /* set GPIO to output */
    status = mraa_gpio_dir(cs, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
      goto err_exit;
    }

    /* set GPIO to output */
    status = mraa_gpio_dir(dc, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
      goto err_exit;
    }

    status = mraa_gpio_dir(hreset, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
      goto err_exit;
    }

    mraa_gpio_write(cs, 0);

    //! [Interesting]
    /* initialize SPI bus */
    spi = mraa_spi_init(SPI_BUS);
    if (spi == NULL) {
        fprintf(stderr, "Failed to initialize SPI\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set SPI frequency */
    status = mraa_spi_frequency(spi, SPI_FREQ);
    if (status != MRAA_SUCCESS)
        goto err_exit;

    /* set big endian mode */
    status = mraa_spi_lsbmode(spi, 0);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* MAX7219/21 chip needs the data in word size */
    status = mraa_spi_bit_per_word(spi, 8);
    if (status != MRAA_SUCCESS) {
        fprintf(stdout, "Failed to set SPI Device to 16Bit mode\n");
        goto err_exit;
    }

    //mraa_spi_write_buf(spi, pixel_test_8, 16);

    mraa_gpio_write(hreset, 1);
    mraa_gpio_write(hreset, 0);
    mraa_gpio_write(hreset, 1);

    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, TFT_SWRESET);
    usleep(500000);
    mraa_spi_write(spi, TFT_SLPOUT);
    usleep(500000);
    mraa_spi_write(spi, TFT_DISPON);
    usleep(200000);


    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, TFT_MADCTL);
    mraa_gpio_write(dc, 1);
    mraa_spi_write(spi, TFT_MADCTL_MY | TFT_MADCTL_MV | TFT_MADCTL_RGB);

    //    set_addr_window(0, TFT_WIDTH, 0, TFT_HEIGHT);
    set_addr_window(0, TFT_HEIGHT, 0, TFT_WIDTH);
    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, TFT_RAMWR);
    mraa_gpio_write(dc, 1);


    for (i = 0; i < gif_length; i++) {
      for (j = 0; j < frame_size; j += 2048) {
	mraa_spi_write_buf(spi, frame_buffers + (i * frame_size) + j, 2048);
      }
      usleep(80000); // delay between frame
    }

    /* stop spi */
    mraa_spi_stop(spi);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* stop spi */
    mraa_spi_stop(spi);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
