#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


int main(int argc, char *argv[]) {
	screen(1);
	puts("SCREEN 1\n");
	getch();
	screen(0);
	puts("SCREEN 0\n");

	return 0;
}
