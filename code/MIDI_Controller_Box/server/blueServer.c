#include "blueServer.h"

const char BLUE_SERVER_MAC[18] = "02:00:1B:D3:2E:51";

//static int server_fd;
static int client_fd;
static struct sockaddr_rc server = {0};

void BlueMessage(int type, char* value) {

  char send_buffer[1024];
  sprintf(send_buffer, "%d:%s", type, value);
  write(client_fd, send_buffer, strlen(send_buffer));
  
}

static void* BlueDaemon() {

  int status;

  client_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  server.rc_family = AF_BLUETOOTH;
  server.rc_channel = (uint8_t) 1;
  str2ba( BLUE_SERVER_MAC, &server.rc_bdaddr );

  status = connect(client_fd, (struct sockaddr*)&server, sizeof(server));
  if (status < 0) {
    fprintf(stderr, "Error with bluetooth connection\n");
  }

  BlueMessage(0, "Please work first take first try");
  
  return NULL;
}

void BlueStart() {
  int status;

  status = pthread_create(&bluetooth_thread, NULL, BlueDaemon, NULL);

  if (status < 0) {
    fprintf(stderr, "Failed to start bluetooth thread");
    exit(-1);
  }
}
