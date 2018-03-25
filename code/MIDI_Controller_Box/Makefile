CC = gcc
CFLAGS = -Wall -Werror -c
SHARED = -lpthread -lbluetooth -lwiringPi
OBJ_SERVER = server/blueServer.o server/util.o server/server.o server/websocket.o server/http.o server/sha1.o server/base64.o

main: server
	$(CC) $(OBJ_SERVER) main.c $(SHARED) -o MIDI_Controller

server: $(OBJ_SERVER)

server/%.o: server/%.c
	$(CC) $(CFLAGS) -lpthread -lbluetooth -o $@ $<

clean:
	rm server/*.o
	rm MIDI_Controller





