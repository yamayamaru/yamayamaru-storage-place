#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>

char Ypos[5];

char Ptspr0[32] = {
    0x01,0x13,0x13,0x37,0x37,0x76,0x74,0x54,
    0xfc,0xf4,0xb4,0xb4,0xbe,0xf7,0xf7,0x63,
    0x80,0xc8,0xc8,0xec,0xec,0x6e,0x2e,0xaa,
    0xbf,0xaf,0x2d,0xad,0x7d,0xef,0xef,0xc6 };

char Ptspr1[32] = {
    0x01,0x03,0x03,0x03,0x03,0x03,0x01,0x02,
    0x01,0x02,0x01,0x00,0x01,0x00,0x01,0x00,
    0x80,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,0xc0,
    0x00,0xc0,0x00,0x80,0x00,0x80,0x00,0x80 };

void beam()
{
    int i;

    for (i = 0; i <= 4; ++i) {
        if (Ypos[i] != 255) {
            Ypos[i] -= 2;
            if (Ypos[i] < 212)
                putspr(i, 120, Ypos[i]-15, 10, 4);
            else {
                Ypos[i] = 255;
                putspr(i, 0, 217, 0, 0);
            }
        }
    }
}

void shot()
{
    int i;
    static int wait = 0;

    if ((gttrig(0) | gttrig(1)) == 0) {
        wait -= 4;
        if (wait < 0)
            wait = 0;
    } else {
        --wait;
        if (wait <= 0) {
            wait = 36;
            for (i = 0; i <= 4; ++i) {
                if (Ypos[i] == 255) {
                    Ypos[i] = 182;
                    break;
                }
            }
        }
    }
}

int main()
{
    int i;

    color(15, 1, 1);
    screen(2);
    inispr(2);
    sprite(0, Ptspr0);
    sprite(4, Ptspr1);
    putspr(5, 120, 170, 15, 0);
    for (i = 0; i <= 4; ++i)
        Ypos[i] = 255;

    while ((snsmat(7) & 0x80) != 0) {
        shot();
        beam();
        for (i = 0; i < 150; ++i);
    }

    screen(0);
    kilbuf();

	return 0;
}

