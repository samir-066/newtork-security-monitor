CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -Iinclude
TARGET = network-monitor

SRC = src/main.c src/scanner.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)