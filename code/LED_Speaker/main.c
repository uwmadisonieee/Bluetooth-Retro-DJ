#include "main.h"

void BlueDataCallback( int type, char* value) {
  //  int val;
  //  FILE *fp;

  switch(type) {
  case 0:
    fprintf(stdout, "message: %s\n", value);
    break;

  default:
    fprintf(stderr, "Not a valid type [%d]\n", type);
    break;
  }
}

void HardwareSetup() {
}

int main(int argc, char* argv[]) {

  // need to give data callback
  server_on_data = BlueDataCallback;

  // MRAA time
  HardwareSetup();

  // Bluetooth Activate!
  ServerStart();

  while (1) {
  }
  
  return 0;
}
