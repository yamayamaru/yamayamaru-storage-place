#include <stdio.h>
#include <stdlib.h>
#include <msxclib.h>
#include <conio.h>

int main(int argc, char *argv[]) {
	screen(1);
	puts("SCREEN 1\n");
	getch();
	screen(0);
	puts("SCREEN 0\n");
}
