#include "blueServer.h"

static int socket_fd;
static int client_fd;
static char* send_buffer;
static int connected = 0;
static void* track_analysis_buffer;


static void printFatal(char* message, int id) {
  fprintf(stderr,"\n*************************************\n");
  fprintf(stderr,"Fatel Error %d:\n%s\n", id, message);
  fprintf(stderr,"*************************************\n");
  exit(id);
}

static void sendSongData() {
  //  int i;
  
  // Sure hope max buffer is less then string lol #ThugLife
  memset(send_buffer, 0, BLUE_MAX_BUFFER);
  TracksAsString(send_buffer);
  write(client_fd, send_buffer, strlen(send_buffer));
  fprintf(stdout, "\tfirst init data sent\n");
  usleep(100000);
  
  //  i = TracksPackAnalysis(track_analysis_buffer);
  //  write(client_fd, send_buffer, i);
  //fprintf(stdout, "\tsecond init data sent\n");
}

static void* serverDaemon() {

  struct sockaddr_rc server = {0};
  struct sockaddr_rc client = {0};
  char buffer[BLUE_MAX_BUFFER] = {0};
  int status;

  socklen_t client_size = sizeof(client);

  // allocate socket
  socket_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  server.rc_family = AF_BLUETOOTH;
  server.rc_bdaddr = *BDADDR_ANY;
  server.rc_channel = (uint8_t) 1;

  status = bind(socket_fd, (struct sockaddr*)&server, sizeof(server));
  if (status < 0) {
    printFatal("Unable to bind server", status);
  }

  listen(socket_fd, 1);

  fprintf(stdout,"Waiting for client to connect\n");
  //init connect
  memset(buffer, 0, sizeof(buffer));
  client_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);
  fprintf(stdout,"Connected to client\n");
  // init data
  sendSongData();

  connected = 1;
  
  while(1) {

    status = read(client_fd, buffer, sizeof(buffer));
    if (status > 0) {
      const char s[2] = ":";

      // 100% assuming data in in key,value form
      int type = atoi(strtok(buffer, s));
      char* value = strtok(NULL, s);

      server_on_data(type,value);
    }

    // get ready for next connection
    memset(buffer, 0, sizeof(buffer));
  }

  return NULL;
  
}

void ServerMessage(int type, char* value) {
  if (connected == 0) { return; }
  sprintf(send_buffer, "%d:%s", type, value);
  write(client_fd, send_buffer, strlen(send_buffer));
}


void ServerStart() {
  int status;

  send_buffer = (char*) malloc(BLUE_MAX_BUFFER);
  if (send_buffer == NULL) { printFatal("ERROR: Allocate send_buffer", 0); }

  track_analysis_buffer = malloc(TRACKS_ANALYSIS_SIZE * TRACKS_MAX_COUNT);
  if (track_analysis_buffer == NULL) {
    fprintf(stderr,"ERROR: malloc T_A_B\n");
  }

  
  status = pthread_create(&server_thread, NULL, serverDaemon, NULL);

  if (status < 0) {
    printFatal("server thread failed", status);
  }
}
