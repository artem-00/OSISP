CC=gcc
CFLAGS=-pthread -g -ggdb -pedantic -std=c11 -W -Wall -Wextra
BIN_DIR=bin

all: main

main: src/main.c src/ring.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^

clean:
	rm -rf $(BIN_DIR)