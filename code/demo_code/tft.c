/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* mraa header */
#include <mraa/spi.h>
#include <mraa/gpio.h>

/* SPI declaration */
#define SPI_BUS 0

#define CS 24
#define DC 31
#define HRESET 26

#define SWRESET 0x01 //software reset
#define SLPOUT 0x11 //sleep out
#define DISPON 0x29 //display on
#define CASET 0x2A //column address set
#define RASET 0x2B //row address set
#define RAMWR 0x2C //RAM write
#define MADCTL 0x36//#axis control
#define COLMOD 0x3A //color mode

#define S_WIDTH 160
#define S_HEIGHT 128
#define S_COLOR 3 // 24 bits for RGB666 which is default color mode

/* SPI frequency in Hz */
#define SPI_FREQ 500000

static mraa_spi_context spi;
static mraa_gpio_context cs, dc, hreset;


volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}


void set_addr_window(int x0, int x1, int y0, int y1) {
  mraa_gpio_write(dc, 0);
  mraa_spi_write(spi, RASET);
  mraa_gpio_write(dc, 1);
 
  mraa_spi_write(spi, x0 >> 8);
  mraa_spi_write(spi, x0 & 0xFF);
  mraa_spi_write(spi, x1 >> 8);
  mraa_spi_write(spi, x1 & 0xFF); 

  mraa_gpio_write(dc, 0);
  mraa_spi_write(spi, CASET);
  mraa_gpio_write(dc, 1);
 
  mraa_spi_write(spi, y0 >> 8);
  mraa_spi_write(spi, y0 & 0xFF);
  mraa_spi_write(spi, y1 >> 8);
  mraa_spi_write(spi, y1 & 0xFF); 
    
}

int main(int argc, char** argv)
{
    mraa_result_t status = MRAA_SUCCESS;
    
    int i, j;

    uint8_t *frame_buffer_0;
    uint8_t *frame_buffer_1;
    int frame_size = S_WIDTH * S_HEIGHT * S_COLOR * sizeof(uint8_t);

    char red[3] = {0xff, 0, 0};
    char blue[3] = {0, 0, 0xff};

    frame_buffer_0 = (uint8_t*)malloc(frame_size);
    if (frame_buffer_0 == NULL) {
      puts("Can't malloc frame_buffer");
      return -1;
    }
    frame_buffer_1 = (uint8_t*)malloc(frame_size);
    if (frame_buffer_1 == NULL) {
      puts("Can't malloc frame_buffer");
      return -1;
    }

    // set FB_0 to all blue and FB_1 to all red
    for (i = 0; i < frame_size; i += S_COLOR) {
      memcpy(frame_buffer_0 + i, blue, 3);
      memcpy(frame_buffer_1 + i, red, 3);
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

    mraa_gpio_write(hreset, 1);
    mraa_gpio_write(hreset, 0);
    mraa_gpio_write(hreset, 1);
    
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

    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, SWRESET);
    usleep(500000);
    mraa_spi_write(spi, SLPOUT);
    usleep(500000);
    mraa_spi_write(spi, DISPON);
    usleep(200000);

 
    set_addr_window(0, S_WIDTH, 0, S_HEIGHT); 
    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, RAMWR);     
    mraa_gpio_write(dc, 1);
    for (i = 0; i < frame_size; i += 2048) {
      mraa_spi_write_buf(spi, frame_buffer_0 + i, 2048);
    }

    //    set_addr_window(0, S_WIDTH, 0, S_HEIGHT); 
    //    mraa_gpio_write(dc, 0);
    //    mraa_spi_write(spi, RAMWR);     
    //    mraa_gpio_write(dc, 1);
    for (i = 0; i < frame_size; i += 1024) {
      mraa_spi_write_buf(spi, frame_buffer_1 + i, 1024);
    }

    mraa_spi_write_buf(spi, frame_buffer_0, 2048);
    
    // fill green
    
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
