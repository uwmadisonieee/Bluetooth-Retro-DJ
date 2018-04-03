/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/* mraa header */
#include <mraa/spi.h>


/* SPI declaration */
#define SPI_BUS 0

/* SPI frequency in Hz */
#define SPI_FREQ 8000000

static mraa_spi_context spi;

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

int main(int argc, char** argv)
{
    mraa_result_t status = MRAA_SUCCESS;
    
    int i, j;

    char start[4] = {0, 0, 0, 0};
    char red[4] = {0xff, 0xff, 0, 0};
    char blue[4] = {0xff,0, 0, 0xff};

   
    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

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
    mraa_spi_write_buf(spi, start, 4);

    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, red, 4);
    mraa_spi_write_buf(spi, blue, 4); 
					
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
