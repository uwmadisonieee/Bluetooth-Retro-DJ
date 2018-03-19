#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <mraa/gpio.h>

#define  GPIO_PIN_1  30
#define  GPIO_PIN_2  32

static volatile int globalCounter = 0 ;

unsigned char flag;
unsigned char Last_RoB_Status;
unsigned char Current_RoB_Status;

int main(void)
{

  int status;
  mraa_gpio_context encA;
  mraa_gpio_context encB;

  /* initialize GPIO pin */
  encA = mraa_gpio_init(GPIO_PIN_1);
  if (encA == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", GPIO_PIN_1);
    mraa_deinit();
    return EXIT_FAILURE;
  }
  /* initialize GPIO pin */
  encB = mraa_gpio_init(GPIO_PIN_2);
  if (encB == NULL) {
    fprintf(stderr, "Failed to initialize GPIO %d\n", GPIO_PIN_2);
    mraa_deinit();
    return EXIT_FAILURE;
  }
  /* set GPIO to input */
  status = mraa_gpio_dir(encA, MRAA_GPIO_IN);
  if (status != MRAA_SUCCESS) {
    printf("FAILED encA - %d", status);
    return -1;
  }
  /* set GPIO to input */
  status = mraa_gpio_dir(encB, MRAA_GPIO_IN);
  if (status != MRAA_SUCCESS) {
    printf("FAILED encB - %d", status);
    return -1;
  }
  
  while(1){
    Last_RoB_Status = mraa_gpio_read(encB);

    while(!mraa_gpio_read(encA)){
      Current_RoB_Status = mraa_gpio_read(encB);
      flag = 1;
    }

    if(flag == 1){
      flag = 0;
      if((Last_RoB_Status == 0)&&(Current_RoB_Status == 1)){
	globalCounter ++;
	printf("globalCounter : %d\n",globalCounter);
      }
      if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
	globalCounter --;
	printf("globalCounter : %d\n",globalCounter);
      }

    }

  }

  return 0;
}
