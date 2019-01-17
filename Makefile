CC ?= gcc
CFLAGS += -Wall -O2 -std=c99

xkbmon: main.c
	$(CC) $(CFLAGS) $< -o $@ -lX11
