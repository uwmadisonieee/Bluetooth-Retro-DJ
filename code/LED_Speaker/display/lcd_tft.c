#include "lcd_tft.h"

static mraa_gpio_context dc_c, hreset_c;
static uint8_t** frame_buffers;
static uint8_t gif_length[TFT_MAX_GIFS];
static int gif_count = 0;

static void set_addr_window(int x0, int x1, int y0, int y1) {
  mraa_gpio_write(dc_c, 0);
  mraa_spi_write(spi_c, TFT_RASET);
  mraa_gpio_write(dc_c, 1);

  mraa_spi_write(spi_c, x0 >> 8);
  mraa_spi_write(spi_c, x0 & 0xFF);
  mraa_spi_write(spi_c, x1 >> 8);
  mraa_spi_write(spi_c, x1 & 0xFF);

  mraa_gpio_write(dc_c, 0);
  mraa_spi_write(spi_c, TFT_CASET);
  mraa_gpio_write(dc_c, 1);

  mraa_spi_write(spi_c, y0 >> 8);
  mraa_spi_write(spi_c, y0 & 0xFF);
  mraa_spi_write(spi_c, y1 >> 8);
  mraa_spi_write(spi_c, y1 & 0xFF);
}

void lcd_setup() {

  mraa_result_t status = MRAA_SUCCESS;
  int frame_size = TFT_WIDTH * TFT_HEIGHT * TFT_COLOR * sizeof(uint8_t);

  DIR *dir;
  struct dirent *ent;

  /* initialize GPIO pin */
  dc_c = mraa_gpio_init(TFT_DC);
  if (dc_c == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", TFT_DC);
    mraa_deinit();
    exit(-1);
  }

  hreset_c = mraa_gpio_init(TFT_HRESET);
  if (dc_c == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", TFT_HRESET);
    mraa_deinit();
    exit(-1);
  }

  /* set GPIO to output */
  status = mraa_gpio_dir(dc_c, MRAA_GPIO_OUT);
  if (status != MRAA_SUCCESS) {
    spi_cleanup();
  }

  status = mraa_gpio_dir(hreset_c, MRAA_GPIO_OUT);
  if (status != MRAA_SUCCESS) {
    spi_cleanup();
  }

  // Reseting screen
  mraa_gpio_write(hreset_c, 1);
  mraa_gpio_write(hreset_c, 0);
  mraa_gpio_write(hreset_c, 1);
  mraa_gpio_write(dc_c, 0);
  mraa_spi_write(spi_c, TFT_SLPOUT);
  usleep(500000);
  mraa_spi_write(spi_c, TFT_DISPON);
  usleep(200000);

  // turn screen to long width
  mraa_gpio_write(dc_c, 0);
  mraa_spi_write(spi_c, TFT_MADCTL);
  mraa_gpio_write(dc_c, 1);
  mraa_spi_write(spi_c, TFT_MADCTL_MY | TFT_MADCTL_MV | TFT_MADCTL_RGB);

  // sets screen to be full screen
  set_addr_window(0, TFT_HEIGHT, 0, TFT_WIDTH);
  mraa_gpio_write(dc_c, 0);
  mraa_spi_write(spi_c, TFT_RAMWR);
  mraa_gpio_write(dc_c, 1);

  // time to allocate some gif memory
  frame_buffers = (uint8_t**)malloc(sizeof(uint8_t*) * TFT_MAX_GIFS);
  if (frame_buffers == NULL) {
    puts("Can't malloc frame_buffer"); exit(-1);
  }

  gif_count = 0;
  if ((dir = opendir (TFT_GIF_PATH)) != NULL) {
    while ((ent = readdir (dir)) != NULL) {

      // ignore ./ and ../ inode
      if (!strcmp(ent->d_name, ".") || !strcmp (ent->d_name, "..")) { continue; }

      // get number of frames in gif
      gif_length[gif_count] = gif_frame_count(ent->d_name);
      if (gif_length[gif_count] <= 0) {
	fprintf(stderr, "Gif reading failed: %d\n", gif_length[gif_count]); exit(-1);
      }

      // memory to hold it
      *(frame_buffers + gif_count) = (uint8_t*)malloc(frame_size * gif_length[gif_count]);
      if (*(frame_buffers + gif_count) == NULL) {
	fprintf(stderr, "shit, allocate failed\n"); exit(-1);
      }

      // conversion
      gif_2_rgb(ent->d_name, *(frame_buffers + gif_count), frame_size);

      gif_count++;

      if (gif_count >= TFT_MAX_GIFS) { break; }
    }
    closedir(dir);
  } else {
    fprintf(stderr, "ERROR: Can't oppen directory!");
    exit(-2);
  }

}


/*
  for (i = 0; i < gif_length; i++) {
    for (j = 0; j < frame_size; j += 2048) {
      mraa_spi_write_buf(spi_c, frame_buffers + (i * frame_size) + j, 2048);
    }
    usleep(80000); // delay between frame
  }
*/
