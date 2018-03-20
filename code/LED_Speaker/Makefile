CC = gcc
CFLAGS = -Wall -Werror
BLUEAPP = ../common/blueApp.c -lbluetooth

gif:
	gcc gif.c lcd_tft.c -lgif -lmraa -o test

blue:
	$(CC) $(BLUEAPP) main.c -o LED_Base
