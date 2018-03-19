/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

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


/* SPI frequency in Hz */
#define SPI_FREQ 500000

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

int
main(int argc, char** argv)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_spi_context spi;
    mraa_gpio_context cs, dc, hreset;
    int width = 128;
    int height = 160;
    int i, j;

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

    // set addre window
    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, CASET);
    mraa_gpio_write(dc, 1);
    mraa_spi_write(spi, 0);
    mraa_spi_write(spi, 0);
    mraa_spi_write(spi, 0);
    mraa_spi_write(spi, 127);

    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, RASET);
    mraa_gpio_write(dc, 1);
    mraa_spi_write(spi, 0);
    mraa_spi_write(spi, 0);
    mraa_spi_write(spi, 0);
    mraa_spi_write(spi, 159);
    
    mraa_gpio_write(dc, 0);
    mraa_spi_write(spi, RAMWR);

    mraa_gpio_write(dc, 1);

    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {	
	mraa_spi_write(spi, 0xFF);
	mraa_spi_write(spi, 0x00);
	mraa_spi_write(spi, 0x00);
      }
    }
    
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
