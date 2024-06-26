#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


#define T32CGP *((int *)0xf3c1)

int main()
{
    char data[8];
    int i, j;

    ginit();
    screen(1);
    ldirmv(data, T32CGP + 'A' * 8, 8);

    for (i = 0; i < 8; ++i) {
        for (j = 0; j < 8; ++j) {
             if ((data[i] & 0x80) == 0)
                 putchar(' ');
             else
                 putchar('A');
             data[i] <<= 1;
        }
        puts("\r\n");
    }

    getch();
    screen(0);

	return 0;
}

