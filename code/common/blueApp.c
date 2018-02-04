#include "blueApp.h"

void* blueAppClient(void* info)
{
  // detaches thread
  pthread_detach(pthread_self());
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    
  blue_app_t* info_t = (blue_app_t*)info;

  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  
  int status;

  // allocatte socket
  info->socket_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

  status = connect(info->socket_fd, (struct sockaddr*) &(info->blue_addr), sizeof(info->blue_addr));

  if (status == 0) {
    status = write(info->socket_fd, "Hello World", 11); 
  }

  if (status < 0) { printError("Failed to connect", -1); }

  close(info->socket_fd);
  return 0;  
}
