#include <stdio.h>
#include <mraa/gpio.h>
#include <unistd.h>
#include <sys/time.h>

struct timeval stop, start;
static int count = 0;
static uint64_t epochMilli;

void buttonISR(void* arg) {
 struct  timespec ts ;
  uint64_t now;
  


  clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
  now  = (uint64_t)ts.tv_sec * (uint64_t)1000 + (uint64_t)(ts.tv_nsec / 1000000L) ;
  
  if( ((uint32_t)(now - epochMilli)) < 2500 ) {
    puts("DUD");
    } else {
     printf("PRESSED: %d\n", count++); 
    }
}

int main( ) {
  int status;
  mraa_gpio_context button;
  struct timespec ts;
    
  clock_gettime (CLOCK_MONOTONIC_RAW, &ts) ;
  epochMilli = (uint64_t)ts.tv_sec * (uint64_t)1000    + (uint64_t)(ts.tv_nsec / 1000000L) ;

  button = mraa_gpio_init(33);
  if (button == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", 33);
    mraa_deinit();
    return -1;
  }

  status = mraa_gpio_dir(button, MRAA_GPIO_IN);
  if (status != MRAA_SUCCESS) {
    puts("Status bad for input");
  }

  status =  mraa_gpio_isr(button, MRAA_GPIO_EDGE_FALLING, &buttonISR, NULL);
  if (status != MRAA_SUCCESS) {
    puts("Status bad for isr");
  }

  while(1) {
    sleep(1);
  }
}
