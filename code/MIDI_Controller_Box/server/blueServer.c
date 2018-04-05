#include "blueServer.h"

const char BLUE_SERVER_MAC[18] = "02:00:1B:D3:2E:51";

//static int server_fd;
static int client_fd;
static struct sockaddr_rc server = {0};

void BlueMessage(int type, char* value) {
  char send_buffer[BLUE_MAX_BUFFER];
  sprintf(send_buffer, "%d:%s", type, value);
  write(client_fd, send_buffer, strlen(send_buffer));
}

void BlueMessageInt(int type, int value) {
  char send_buffer[BLUE_MAX_BUFFER];
  sprintf(send_buffer, "%d:%d", type, value);
  write(client_fd, send_buffer, strlen(send_buffer));
}

static void* BlueDaemon() {

  int status;
  char buffer[BLUE_MAX_BUFFER];
  memset(buffer, 0, sizeof(buffer));
  
  client_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  server.rc_family = AF_BLUETOOTH;
  server.rc_channel = (uint8_t) 1;
  str2ba( BLUE_SERVER_MAC, &server.rc_bdaddr );

  status = connect(client_fd, (struct sockaddr*)&server, sizeof(server));
  if (status < 0) {
    fprintf(stderr, "Error with bluetooth connection\n");
  } else {
    fprintf(stdout, "Connected to Speaker!\n");
  }

  
  fprintf(stdout, "Waiting for init bluetooth\n");
  // init data
  status = read(client_fd, buffer, sizeof(buffer));
  bluetooth_on_init(buffer);
  memset(buffer, 0, sizeof(buffer));
  
  // init data
  //  status = read(client_fd, buffer, sizeof(buffer));
  //  bluetooth_on_analysis(buffer);
  //  memset(buffer, 0, sizeof(buffer));
  sleep(1);
  fprintf(stdout, "Starting main blue daemon loop\n");
  while(1) {
    status = read(client_fd, buffer, sizeof(buffer));
    if (status > 0) {
      const char s[2] = ":";

      // 100% assuming data in in key,value form
      int type = atoi(strtok(buffer, s));
      char* value = strtok(NULL, s);
      fprintf(stdout, "Sending data %d\n", type);
      bluetooth_on_data(type,value);
      memset(buffer, 0, sizeof(buffer));
    }
  }
  
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
