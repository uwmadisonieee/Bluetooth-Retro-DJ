CC = gcc
CFLAGS = -Wall -Werror -c
SHARED = -lpthread -lasound -lbluetooth -lmraa -lgif
OBJ_AUDIO = audio/playback.o
OBJ_DISPLAY = display/lcd_tft.o display/gif.o display/led.o

main: audio display
	$(CC) $(OBJ_AUDIO) $(OBJ_DISPLAY) main.c $(SHARED) -o LED_Speaker

audio: $(OBJ_AUDIO)

audio/%.o: audio/%.c
	$(CC) $(CFLAGS) -lasound -o $@ $<

display: $(OBJ_DISPLAY)

display/%.0: display/%.c
	$(CC) $(CFLAGS) -lmraa -lgif -o $@ $<

clean:
	rm audio/*.o
	rm display/*.o
	rm LED_Speaker
