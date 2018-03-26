#include "main.h"

void BlueDataCallback( int type, char* value) {
  int val;
  double val_d;
  //  FILE *fp;

  switch(type) {
  case 0:
    fprintf(stdout, "message: %s\n", value);
    break;

  case 2: // play/pause
    val = atoi(value);
    playback_pause = val;
    break;
  case 3: // seek
    val = atoi(value);
    TrackSeek(val);
    break;
  case 4: // gain
    val_d = atof(value);
    PlaybackSetGain(val_d);
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
  fprintf(stdout, "Hardware Setup\n");

  // audio needs to be setup before bluetooth
  PlaybackSetup();
  fprintf(stdout, "Playback Setup\n");

  // Bluetooth Activate!
  ServerStart();
  fprintf(stdout, "Server Started\n");

  while (1) {
  }

  PlaybackCleanup();
  return 0;
}
