#include "main.h"

extern server_t* g_server;

void WebDataCallback(int type, const char* value) {
}

void BlueDataCallback(int type, char* value) {
}

void HardwareSetup() {
  if (wiringPiSetupGpio() < 0) {
    fprintf(stderr, "Failed to start wiringPi\n");
    exit(-1);
  }
}

int main(int argc, char* argv[]) {

  HardwareSetup();
  
  g_server = (server_t*)malloc(sizeof(server_t));
  g_server->port = 6419;
  g_server->onSocketMessage = WebDataCallback;

  startServer();

  bluetooth_on_data = BlueDataCallback;

  BlueStart();

  while (1) {
    
  }
  return 0;
}
