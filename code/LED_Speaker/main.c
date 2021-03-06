#include "main.h"

extern double playback_gain;
extern int lcd_play_next;
extern int lcd_speed;
extern uint8_t spi_lcd_or_led;
void BlueDataCallback( int type, char* value) {
  int val, i;
  double val_d;
  //  FILE *fp;
  fprintf(stdout,"Type: %d\t\tValue: %s\n", type, value);
  switch(type) {
  case 0:
    fprintf(stdout, "message: %s\n", value);
    break;
  case 1:
    break;
  case 2: // play/pause
    val = atoi(value);
    playback_pause = val;
    break;
  case 3: // seek
    val = atoi(value);
    TrackSeek(val);
    break;
  case 4: // new track
    val = atoi(value);
    TrackChange(val);
    ServerMessage(1, tracks[playback_cur_track].analysis);
    break;
  case 5: // new sample
    val = atoi(value);
    playback_cur_sample = val;
    break;
  case 6: // play sample
    playback_play_sample = 1;
    break;
  case 7:
    //between 0 - 50
    val_d = (double)(1.0f * atoi(value)) - 48.0f;
    playback_gain = pow(10.0, val_d / 20.0);
    break;
  case 8:
    lcd_play_next = 1;
  case 9:
    // between 1 - 34
    lcd_speed = ((34-atoi(value)) * 3000) + 10000;
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

void buttonISR(void* arg) {
  led_effect++;

  if (led_effect > LED_MAX_EFFECTS) { led_effect = 0;}

  switch(led_effect) {
  case 0:
    LEDSetAllRed();
    break;
  case 1:
    LEDSetAllGreen();
    break;
  case 2:
    LEDSetAllBlue();
    break;
  }
  
  fprintf(stdout, "pressed : %d\n", led_effect);
}

int main(int argc, char* argv[]) {
  char debug[256];
  int debugI;
  double debugF;
  mraa_gpio_context button;
  int status;
  // need to give data callback
  server_on_data = BlueDataCallback;

  // audio needs to be setup before bluetooth
  PlaybackSetup();
  fprintf(stdout, "Playback Setup\n");

  SPISetup();
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

  button = mraa_gpio_init(33);
  if (button == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", 33);
    mraa_deinit();
    return -1;
  }

  status = mraa_gpio_dir(button, MRAA_GPIO_IN);
  if (status != MRAA_SUCCESS) {
    puts("Status bad for input");
  }

  status =  mraa_gpio_isr(button, MRAA_GPIO_EDGE_FALLING, &buttonISR, NULL);
  if (status != MRAA_SUCCESS) {
    puts("Status bad for isr");
  }
  
  spi_lcd_or_led = 0;
  pthread_create(&spi_thread, NULL, SPILoop, NULL);
  pthread_create(&playback_thread, NULL, TrackPlay, NULL);

  // Bluetooth Activate!
  ServerStart();
  fprintf(stdout, "Server Started\n");
  
  while (1) {
    sleep(5);
  }

  PlaybackCleanup();
  return 0;
}
