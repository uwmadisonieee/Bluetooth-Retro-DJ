#include "main.h"
#include <ads1115.h>

#define MY_BASE 2222
#define POT_DIV 1320
#define SLIDE_DIV 528

extern server_t* g_server;
int server_connected = 0;

void WebDataCallback(int type, char* value) {
  switch(type) {
  case 0:
    server_connected = 1;
    break;
  case 1:
    BlueMessage(0, value);
    break;

  default:
    fprintf(stderr, "Not a valid type [%d]\n", type);
    break;
  }
}

void BlueDataInit(char* value) {
  const char s[2] = ";";
  char *token;
  char send_buffer[4096];

  token = strtok(value, s);
  while (token != NULL) {
    broadcastString("0", token);
    token = strtok(NULL, s);
  }

}

void BlueDataCallback(int type, char* value) {
  
  switch(type) {
  case 0:
    broadcastString("1", value);
    break;

  default:
    fprintf(stderr, "Not a valid type [%d]\n", type);
    break;
  }  
}

void HardwareSetup() {
  if (wiringPiSetupGpio() < 0) {
    fprintf(stderr, "Failed to start wiringP\n");
    exit(-1);
  }
}

int main(int argc, char* argv[]) {

  int last_slider;
  int last_pot;
  int slider_v;
  int pot_v;
  char val[16];
  
  HardwareSetup();
  
  g_server = (server_t*)malloc(sizeof(server_t));
  g_server->port = 6419;
  g_server->onSocketMessage = WebDataCallback;

  startServer();

  bluetooth_on_init = BlueDataInit;
  bluetooth_on_data = BlueDataCallback;

  BlueStart();

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
      sprintf(val, "%d", slider_v);
      BlueMessage(8, val);
      //printf("slider: %d\n", slider_v);
    }

    if (pot_v != last_pot) {
      last_pot = pot_v;
      BlueMessage(9, val);
      sprintf(val, "%d", pot_v);
      //printf("pot: %d\n", pot_v);
    }
    usleep(1000);
  }

  return 0;
}
