CC ?= gcc
CFLAGS += -Wall -Wextra -Werror -Wconversion -pedantic -std=c99 -O2

xkbmon: main.c
	$(CC) $(CFLAGS) $^ -o $@ -lxcb -lxcb-xkb
