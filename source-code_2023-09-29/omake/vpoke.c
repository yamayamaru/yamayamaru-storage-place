#include <stdio.h>
#include "msxalib.h"
#include "msxclib.h"

#define T32NAM *((int *)0xf3bd)

void fill(char ch)
{
    int i, j, k;
    int ad = T32NAM -1;
    int x = 32;
    int y = 23;

    for (i = 0; i <= 11; ++i) {
        for (j = 0; j < x; ++j) {
            vpoke(++ad, ch);
            for (k = 0; k < 1000; ++k);
        }
        --x;
        for (j = 0; j < y; ++j) {
            vpoke(ad += 32, ch);
            for (k = 0; k < 1000; ++k);
        }
        --y;
        for (j = 0; j < x; ++j) {
            vpoke(--ad, ch);
            for (k = 0; k < 1000; ++k);
        }
        --x;
        for (j = 0; j < y; ++j) {
            vpoke(ad -= 32, ch);
            for (k = 0; k < 1000; ++k);
        }
        --y;
    }
}


int main(int argc, char *argv[])
{
    int i;

    ginit();
    color(15, 0, 0);
    screen(1);
    fill('?');
    fill(' ');
    screen(0);

	return 0;
}

