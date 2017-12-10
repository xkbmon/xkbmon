xkbmon: main.c
	gcc -Wall -O2 -std=c99 $< -o $@ -lX11
