#include "main.h"

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

  bluetooth_on_init = BlueDataInit;
  bluetooth_on_data = BlueDataCallback;

  while(server_connected == 0) {
    usleep(100000); // 100ms wait unti web page loads
  }

  BlueStart();
  
  while (1) {
    
  }
  return 0;
}
