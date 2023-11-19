CC: gcc
GFLAGS: -g -Wall -pthread

all: client server

client: src/client.c
	$(CC) $(GFLAGS) -o bin/client src/client.c

server: src/server.c
	$(CC) $(GFLAGS) -o bin/server src/server.c

clean:
	rm -f bin/*

.DEFAULT_GOAL := all