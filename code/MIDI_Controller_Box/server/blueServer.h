#ifndef SERVER_BLUE_SERVER_H_
#define SERVER_BLUE_SERVER_H_

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>

#define BLUE_MAX_BUFFER 4096

pthread_t bluetooth_thread;

typedef void (*callbackIntStr)(int, char*);
typedef void (*callbackStr)(char*);
callbackIntStr bluetooth_on_data;
callbackStr bluetooth_on_init;

void BlueMessage(int type, char* value);

void BlueStart( void );

#endif


