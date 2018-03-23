#ifndef SERVER_BLUE_SERVER_H_
#define SERVER_BLUE_SERVER_H_

#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>

#define BLUE_MAX_BUFFER 4096

typedef void (*callbackIntStr)(int, char*);
typedef void (*callbackIntConstStr)(int, const char*);

pthread_t server_thread;

callbackIntStr server_on_data;

void ServerStart( void );

void ServerMessage(int type, char* value);

#endif
