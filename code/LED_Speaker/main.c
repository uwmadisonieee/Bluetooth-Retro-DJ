#include "main.h"

extern double playback_gain;
extern int lcd_play_next;
extern int lcd_speed;
extern uint8_t spi_lcd_or_led;
void BlueDataCallback( int type, char* value) {
  int val;
  double val_d;
  //  FILE *fp;

  switch(type) {
  case 0:
    fprintf(stdout, "message: %s\n", value);
    break;
  case 1:
    break;
  case 2: // play/pause
    //    val = atoi(value);
    playback_pause ^= 0x01;
    break;
  case 3: // seek
    val = atoi(value);
    TrackSeek(val);
    break;
  case 4:
    break;
  case 5:
    break;
  case 6:
    break;
  case 7:
    //between 1 - 50
    val_d = (double)(1.0f * atoi(value)) - 48.0f;
    playback_gain = pow(10.0, val_d / 20.0);
    break;
  case 8:
    lcd_play_next = 1;
  case 9:
    // between 1 - 30
    lcd_speed = (atoi(value) * 3000) + 10000;
    // get gif speed between 10000 and 100000
    break;
  default:
    fprintf(stderr, "Not a valid type [%d]\n", type);
    break;
  }
}

void* SPILoop(void* na) {

  // LCD will be on majority of time
  while(1) {
    if (spi_lcd_or_led == 0) {
      LCDLoop();
    } else {
      mraa_gpio_write(cs_c, 1); // toggle led

      LEDSend();

      mraa_gpio_write(cs_c, 0); // go back to lcd
      spi_lcd_or_led = 0;
    }
  }
}


int main(int argc, char* argv[]) {
  char debug[256];
  int debugI;
  double debugF;
  // need to give data callback
  server_on_data = BlueDataCallback;

  // audio needs to be setup before bluetooth
  PlaybackSetup();
  fprintf(stdout, "Playback Setup\n");

/*  spi_lcd_or_led = 0;  SPISetup();
  fprintf(stdout, "SPI (and MRAA) Setup\n");

  LEDSetup();
  fprintf(stdout, "LED Setup\n");

  // Send Init LED from setup
  mraa_gpio_write(cs_c, 1);
  LEDSend();
  mraa_gpio_write(cs_c, 0);
  spi_lcd_or_led = 0;

  LCDSetup();
  fprintf(stdout, "LCD Setup\n");

  spi_lcd_or_led = 0;
  pthread_create(&spi_thread, NULL, SPILoop, NULL);
*/
  pthread_create(&playback_thread, NULL, TrackPlay, NULL);

  // Bluetooth Activate!
  // ServerStart();
  fprintf(stdout, "Server Started\n");
  
  while (1) {
    //scanf("%lf", &debugF);
    //PlaybackSetGain(debugF);
    getchar();
    playback_play_sample = 1;
  }

  PlaybackCleanup();
  return 0;
}
