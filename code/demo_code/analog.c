#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <ads1115.h>

#define MY_BASE 2222
#define POT_DIV 880
#define SLIDE_DIV 528

static unsigned long l_gif_irq = 0;
static unsigned long c_gif_irq = 0;

void GifButton(void) {
  c_gif_irq = millis();
  if (c_gif_irq - l_gif_irq > 150) {
    puts("gif");
    //broadcastInt("0", 0);
  }
  l_gif_irq = c_gif_irq;
}

int main ( int argc, char* argv[] ) {
  int last_slider;
  int last_pot;
  int slider_v;
  int pot_v;
  // sets up the wiringPi library
  if (wiringPiSetupGpio () < 0) {
    puts("SAD"); exit(-1);
  }

  wiringPiISR (4, INT_EDGE_FALLING, &GifButton);
  
  ads1115Setup (MY_BASE, 0x48);

  last_slider = analogRead (MY_BASE + 0) / SLIDE_DIV;
  last_pot = analogRead (MY_BASE + 2) / POT_DIV;
  // main infinite loop
  while(1) {

    // TODO - gets blocked until first jog spin
    slider_v = analogRead (MY_BASE + 0) / SLIDE_DIV;
    pot_v = analogRead (MY_BASE + 2) / POT_DIV;
    if (slider_v != last_slider) {
      last_slider = slider_v;
      printf("slider: %d\n", slider_v);
    }

    if (pot_v != last_pot) {
      last_pot = pot_v;
      printf("pot: %d\n", pot_v);
    }
    usleep(1000);
  }
  return 0;
}
