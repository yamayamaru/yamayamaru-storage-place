#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


void plot(int x, int y, int ch)
{
    printf("\33Y%c%c%c\n", y+32, x+32, ch);
}

int main(int argc, char *argv[])
{
    int x, y, c;

    srnd();
    puts("\33x5\f");

    for (c = 'A'; c <= 'Z'; ++c) {
        x = rnd(40);
        y = rnd(22);
        do {
            plot(x, y, ' ');
            x += sgn(20 - x);
            y += sgn(10 - y);
            plot(x, y, c);
        } while (x!= 20 || y != 10);
    }

	return 0;
}

