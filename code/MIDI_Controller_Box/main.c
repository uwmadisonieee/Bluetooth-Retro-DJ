#include <stdio.h>
#include "../common/blueApp.h"
#include "../common/util.h"

int main(int argc, char* argv[]) {

  int status;
  
  pthread_t blue_app_thread;
  //  pthread_t http_server_thread;
  
  struct blue_app_t* blue_info = (blue_app_t*)malloc(sizeof(blue_app_t));

  if ( blue_info == NULL ) { printFatal("Failed to alloc blue_info", -1); }	      
  
  blue_info->blue_addr.rc_family = AF_BLUETOOTH;
  blue_info->blue_addr.rc_channel = MCB_TO_BASE_CHAN;
  str2ba( DEVICE_LED_BASE, &(blue_info->blue_addr.rc_bdaddr));

  status = pthread_create(&blue_app_thread, NULL, blueAppClient, (void*) blue_info);

  if (status < 0) { printFatal("Failed to create blue app thread", -1); }

  pthread_detach(blue_app_thread);
  
  // cleanup
  free(blue_info);
  
}
