#ifndef DISPLAY_LCD_TFT_H_
#define DISPLAY_LCD_TFT_H_

/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

/* mraa header */
#include "spi.h"
#include <mraa/gpio.h>

#include "gif.h"

#define TFT_GIF_PATH "./display/gifs/"

#define TFT_DC 31
#define TFT_HRESET 26

#define TFT_MAX_GIFS 15
#define TFT_FRAME_TIL_SWAP 150

#define TFT_SWRESET 0x01 //software reset
#define TFT_SLPOUT 0x11 //sleep out
#define TFT_DISPON 0x29 //display on
#define TFT_CASET 0x2A //column address set
#define TFT_RASET 0x2B //row address set
#define TFT_RAMWR 0x2C //RAM write
#define TFT_MADCTL 0x36//#axis control
#define TFT_COLMOD 0x3A //color mode
#define TFT_MADCTL_MY  0x80
#define TFT_MADCTL_MX  0x40
#define TFT_MADCTL_MV  0x20
#define TFT_MADCTL_ML  0x10
#define TFT_MADCTL_RGB 0x00

#define TFT_WIDTH 160
#define TFT_HEIGHT 128
#define TFT_COLOR 3 // 24 bits for RGB666 which is default color mode

void LCDLoop(void);
void LCDSetup(void);

#endif
