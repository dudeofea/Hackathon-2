all:
	gcc -Wno-implicit-function-declaration -o PS2 -std=c99 PS2.c `pkg-config --libs --cflags libusb-1.0`