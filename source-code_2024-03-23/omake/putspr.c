#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


char Ptspr[32] = {
    0x07, 0x1f, 0x37, 0x7f, 0x6f, 0xdf, 0xdf, 0xdf,
    0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x1f, 0x07,
    0xe0, 0xf8, 0xfc, 0xfe, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xf8, 0xe0 };

int main(int argc, char *argv[])
{
    int x, y, dx, dy, i;

    ginit();
    color(15, 1, 1);
    screen(2);
    inispr(2);
    sprite(0, Ptspr);

	putspr(0, 0, 0, 7, 0);
	putspr(1, 0, 32, 7, 0);

    for (x=0, y=4, dx=3, dy=0; y<175 || dy; ++dy) {
        putspr(0, x, y, 7, 0);
        putspr(1, 240-x, y, 8, 0);
        if ((x += dx) == 240 || x == 0) dx = -dx;
        if ((y += dy) == 175) dy = -dy;
        for (i = 0; i < 1000; ++i);
    }

    getch();
    screen(0);

	return  0;
}

