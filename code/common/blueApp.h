#ifndef BLUE_APP_H_
#define BLUE_APP_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h> // socket, accept, send, recv
#include <sys/socket.h>

#include <signal.h>
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "util.h"

// Hardcoded for this project :)
const char DEVICE_MCB[18] = "02:00:1B:D3:2E:51";
const char DEVICE_LED_BASE[18] = "B8:27:EB:DF:33:A1";
const char DEVICE_LED_PCB[18] = "B8:27:EB:56:69:24";
const uint8_t MCB_TO_BASE_CHAN = 1;
const uint8_t BASE_TO_PCB_CHAN = 8;

// function callbacks on receive
typedef void (*callbackIntConstStr)(int, const char*);

typedef struct blue_app_t {
  int                 port;
  callbackIntConstStr onMessage;
  sockaddr_rc         blue_addr;
  int                 socket_fd;
  pthread_mutex_t     blue_mutex;
  pthread_cond_t      blue_cond; 
};

// assumes blue_addr is already configured
void* blueAppClient(void* info);

void* blueAppServer(void* info);


#endif // BLUE_APP_H_
