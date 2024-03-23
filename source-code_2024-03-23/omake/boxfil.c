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
    int i;

    ginit();
    srnd();
    screen(8);

    while (!kbhit()) {
         boxfil(rnd(256), rnd(212),
                rnd(256), rnd(212), rnd(256), 0);
         for (i = 0; i < 8000; ++i);
    }
    screen(0);

	return 0;
}

