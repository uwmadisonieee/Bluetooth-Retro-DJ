#include "lcd_tft.h"

int lcd_speed;
int lcd_play_next;

static mraa_gpio_context dc_c, hreset_c;
static uint8_t** frame_buffers;
static uint8_t gif_length[TFT_MAX_GIFS];
static uint8_t gif_count = 0;
static uint32_t frame_size;
static uint8_t* miso_buffer;

static uint8_t cur_gif = 0;
static uint8_t cur_frame = 0;
static uint32_t swap_count = 0;



static void SetAddrWindow(int x0, int x1, int y0, int y1) {
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

void LCDLoop() {
  int i;

  if (lcd_play_next == 1 || (swap_count >= TFT_FRAME_TIL_SWAP && cur_frame >= gif_length[cur_gif])) {
    // Time to switch gifs
    cur_gif++;
    if (cur_gif >= gif_count) { cur_gif = 0; }

    cur_frame = 0;
    swap_count = 0;
    lcd_play_next = 0;
  }

  while(cur_frame < gif_length[cur_gif]) {

    // a single frame #PointerGameStrong
    for (i = 0; i < frame_size; i += 2048) {
      miso_buffer = mraa_spi_write_buf(spi_c, (*(frame_buffers + cur_gif)) + (cur_frame * frame_size) + i, 2048);

      // "The Pointer return has to be free'd by the caller" ~MRAA Doc
      if (miso_buffer != NULL) { free(miso_buffer); } 
    }

    swap_count++;
    cur_frame++;

    // TODO time swap to LED to range how much delay to use
    if (spi_lcd_or_led == 1) {
      return;
    }

    usleep(lcd_speed); // delay between frame
  }

  // reset gif and let function reset above
  if (swap_count < TFT_FRAME_TIL_SWAP) {
    cur_frame = 0;
  }

}

void LCDSetup() {

  mraa_result_t status = MRAA_SUCCESS;
  frame_size = TFT_WIDTH * TFT_HEIGHT * TFT_COLOR * sizeof(uint8_t);

  DIR *dir;
  struct dirent *ent;
  char full_path[256];

  lcd_speed = 50000;
  lcd_play_next = 0;

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
    SPICleanup();
  }

  status = mraa_gpio_dir(hreset_c, MRAA_GPIO_OUT);
  if (status != MRAA_SUCCESS) {
    SPICleanup();
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
  SetAddrWindow(0, TFT_HEIGHT, 0, TFT_WIDTH);
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

      // creat path
      memset(full_path, 0, sizeof(full_path));
      strcpy(full_path,TFT_GIF_PATH);
      strcat(full_path, ent->d_name);

      // get number of frames in gif
      gif_length[gif_count] = gif_frame_count(full_path);
      if (gif_length[gif_count] <= 0) {
	fprintf(stderr, "Gif reading failed: %d\n", gif_length[gif_count]); exit(-1);
      }

      // memory to hold it
      *(frame_buffers + gif_count) = (uint8_t*)malloc(frame_size * gif_length[gif_count]);
      if (*(frame_buffers + gif_count) == NULL) {
	fprintf(stderr, "shit, allocate failed\n"); exit(-1);
      }

      // conversion
      gif_2_rgb(full_path, *(frame_buffers + gif_count), frame_size);

      gif_count++;

      if (gif_count >= TFT_MAX_GIFS) { break; }
    }
    closedir(dir);
  } else {
    fprintf(stderr, "ERROR: Can't oppen directory! %s\n", TFT_GIF_PATH);
    exit(-2);
  }

}
