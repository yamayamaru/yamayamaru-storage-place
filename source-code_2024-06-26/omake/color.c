#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


int main(int argc, char *argv[])
{
    int  i, j, k;

    screen(1);

    for (i = 1; i <= 15; ++i) {
        for (j = 1; j <= 15; ++j) {
            color(15, j, i);
            for (k = 0; k < 18000; ++k);
        }
    }

   color(15, 1, 1);
   screen(0);

   return 0;
}

