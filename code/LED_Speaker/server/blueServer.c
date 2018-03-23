#include "blueServer.h"

static int socket_fd;
static int client_fd;

/*static int printError(char* message, int return_val) {
  fprintf(stderr,"%s\n", message);
  return return_val;
  }*/

static void printFatal(char* message, int id) {
  fprintf(stderr,"\n*************************************\n");
  fprintf(stderr,"Fatel Error %d:\n%s\n", id, message);
  fprintf(stderr,"*************************************\n");
  exit(id);
}

static void sendSongData() {
  char buffer[BLUE_MAX_BUFFER] = "";
  char path[32] = "./audio/tracks/";
  char temp[256];
  
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (path)) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      
      if (!strcmp(ent->d_name, ".") || !strcmp (ent->d_name, "..")) { continue; }
      memset(temp, 0, sizeof(temp));
      strncpy(temp, ent->d_name, strlen(ent->d_name) - 4); // remove .wav
      for (char* p = temp; (p = strchr(p, '_')); ++p) { // replace _ for spaces
	*p = ' ';
      }
      strcat(buffer, ";");
      strcat(buffer, temp);
    }
    closedir(dir);
  } else {
    fprintf(stderr, "Can't oppen directory!");
    exit(-2);
  }

  write(client_fd, buffer, strlen(buffer));
  
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

  //init connect
  memset(buffer, 0, sizeof(buffer));
  client_fd = accept(socket_fd, (struct sockaddr*)&client, &client_size);

  // init data
  sendSongData();
  
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

  char send_buffer[BLUE_MAX_BUFFER];
  sprintf(send_buffer, "%d:%s", type, value);
  write(client_fd, send_buffer, strlen(send_buffer));

}


void ServerStart() {
  int status;

  status = pthread_create(&server_thread, NULL, serverDaemon, NULL);

  if (status < 0) {
    printFatal("server thread failed", status);
  }
}
