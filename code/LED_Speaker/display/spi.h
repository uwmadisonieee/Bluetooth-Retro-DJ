#ifndef DISPLAY_SPI_H_
#define DISPLAY_SPI_H_

#include <mraa/spi.h>
#include <mraa/gpio.h>
#include <stdlib.h>

#define SPI_BUS 0
#define SPI_FREQ 32000000 // 32 MHz

#define SPI_CS 24 // using pin 24 (HIGH == LED, LOW == LCD)

uint8_t spi_lcd_or_led;

mraa_spi_context spi_c;
mraa_gpio_context cs_c;

void SPICleanup(void);
void SPISetup(void);

#endif
