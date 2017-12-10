xkbmon: main.c
	gcc -Wall -O2 $< -o $@ -lX11
