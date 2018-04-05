#include "main.h"
#include <ads1115.h>

extern server_t* g_server;
int server_connected = 0;

void WebDataCallback(int type, char* value) {

  switch(type) {
  case 0:
    server_connected = 1;
    break;
  case 1:
    BlueMessage(0, value);
    break;
  case 2:
    BlueMessageInt(2, play_pause);
  case 3:
    BlueMessage(3, value);
    break;
  case 4:    
    BlueMessage(4, value);
    break;
  case 5:
    BlueMessage(5, value);
    break;
    
  default:
    fprintf(stderr, "Not a valid type [%d]\n", type);
    break;
  }
}

void BlueDataInit(char* value) {
  //  const char s[2] = ";";
  //  char *token;
  //  char send_buffer[4096];

  //  token = strtok(value, s);
  //  while (token != NULL) {
  fprintf(stdout, "bluedatainit\n");
    broadcastString("0", value);
    fprintf(stdout, "bluedatainit done\n");
    //    token = strtok(NULL, s);
    //  }

}

void BlueDataAnalysis(char* value) {
  fprintf(stdout, "bluedataAnalysis\n");
  broadcastString("1", value);
  fprintf(stdout, "bluedataAnalysis done\n");
}

void BlueDataCallback(int type, char* value) {
  
  switch(type) {
  case 0:
    broadcastString("1", value);
    break;

  default:
    fprintf(stderr, "Not a valid type [%d]\n", type);
    break;
  }  
}

static unsigned long l_red_irq = 0;
static unsigned long c_red_irq = 0;
static unsigned long l_green_irq = 0;
static unsigned long c_green_irq = 0;
static unsigned long l_blue_irq = 0;
static unsigned long c_blue_irq = 0;
static unsigned long l_gif_irq = 0;
static unsigned long c_gif_irq = 0;

void RedButton(void) {
  c_red_irq = millis();
  if (c_red_irq - l_red_irq > 20) {
    play_pause ^= 1;
    broadcastInt("2", play_pause);
  }
  l_red_irq = c_red_irq;
}
void GreenButton(void) {
  c_green_irq = millis();
  if (c_green_irq - l_green_irq > 20) {
    broadcastInt("3", 0);
  }
  l_green_irq = c_green_irq;
}
void BlueButton(void) {
  c_blue_irq = millis();
  if (c_blue_irq - l_blue_irq > 20) {
    broadcastInt("4", 0);
  }
  l_blue_irq = c_blue_irq;
}
void GifButton(void) {
  c_gif_irq = millis();
  if (c_gif_irq - l_gif_irq > 20) {
   BlueMessage(8, "-");
  }
  l_gif_irq = c_gif_irq;
}

unsigned char flag;
unsigned char Last_RoB_Status;
unsigned char Current_RoB_Status;
int globalCounter = 0 ;
void rotaryDeal(void)
{
  Last_RoB_Status = digitalRead(JOG_DATA_B);

  while(!digitalRead(JOG_DATA_A)){
    Current_RoB_Status = digitalRead(JOG_DATA_B);
    flag = 1;
  }

  if(flag == 1){
    flag = 0;
    if((Last_RoB_Status == 0)&&(Current_RoB_Status == 1)){
      broadcastInt("5", 1);
      //      globalCounter ++;
      //printf("globalCounter : %d\n",globalCounter);

    }
    if((Last_RoB_Status == 1)&&(Current_RoB_Status == 0)){
      broadcastInt("5", -1);
      //globalCounter --;
      //printf("globalCounter : %d\n",globalCounter);
    }

  }
}

static int last_slider;
static int last_pot;
static int slider_v;
static int pot_v;

void HardwareSetup() {
  if (wiringPiSetupGpio() < 0) {
    fprintf(stderr, "Failed to start wiringP\n");
    exit(-1);
  }

  pinMode(BLUE_BUT, INPUT);
  pinMode(GREEN_BUT, INPUT);
  pinMode(RED_BUT, INPUT);
  pinMode(GIF_BUT, INPUT);

  wiringPiISR (BLUE_BUT, INT_EDGE_FALLING, &BlueButton);
  wiringPiISR (GREEN_BUT, INT_EDGE_FALLING, &GreenButton);
  wiringPiISR (RED_BUT, INT_EDGE_FALLING, &RedButton);
  wiringPiISR (GIF_BUT, INT_EDGE_FALLING, &GifButton);

  pinMode(JOG_DATA_A, INPUT);
  pinMode(JOG_DATA_B, INPUT);

  pullUpDnControl(JOG_DATA_A, PUD_UP);
  pullUpDnControl(JOG_DATA_B, PUD_UP);

  ads1115Setup (ADC_BASE, 0x48);

  last_slider = analogRead (ADC_BASE + SLIDER_BASE) / SLIDER_DIV;
  last_pot = analogRead (ADC_BASE + POT_BASE) / POT_DIV;
}

int main(int argc, char* argv[]) {

  char val[16];
  
  HardwareSetup();
  
  g_server = (server_t*)malloc(sizeof(server_t));
  g_server->port = 6419;
  g_server->onSocketMessage = WebDataCallback;

  startServer();

  bluetooth_on_init = BlueDataInit;
  bluetooth_on_analysis = BlueDataAnalysis;
  bluetooth_on_data = BlueDataCallback;

  BlueStart();

  while(server_connected == 0) {
  /*  while(1){
    slider_v = analogRead (ADC_BASE + SLIDER_BASE) / SLIDER_DIV;;
    pot_v = analogRead (ADC_BASE + POT_BASE) / POT_DIV;
    printf("TEST: S: %d\tP: %d\n", slider_v, pot_v); usleep(100000); continue;
    //    rotaryDeal();
    usleep(500000);*/
  }
  broadcastInt("6", last_slider * 2);
  broadcastInt("7", last_pot * 3);

  // main infinite loop
  while(1) {

    // TODO - gets blocked until first jog spin
    slider_v = analogRead (ADC_BASE + SLIDER_BASE) / SLIDER_DIV;
    pot_v = analogRead (ADC_BASE + 2) / POT_DIV;

    if (slider_v != last_slider) {
      last_slider = slider_v;
       sprintf(val, "%d", slider_v);
      broadcastInt("6", last_slider * 2);
      BlueMessage(7, val);
      //printf("slider: %d\n", slider_v);
    }

    if (pot_v != last_pot) {
      last_pot = pot_v;
      BlueMessage(9, val);
      broadcastInt("7", last_pot * 3);
      sprintf(val, "%d", pot_v);
      printf("pot: %d\n", pot_v);
    }
    rotaryDeal();

    /*    if (globalCounter > 0) {
      broadcastInt("5", 1);
      //      BlueMessage(3, "5");
    } else if (globalCounter < 0) {
      broadcastInt("5", -1);
      //      BlueMessage(3, "-5");
    }
    */
    usleep(1000);
    
  }

  return 0;
}
