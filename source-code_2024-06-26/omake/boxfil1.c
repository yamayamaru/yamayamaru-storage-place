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
    int i,j,k;

    ginit();
    srnd();
    screen(8);

    j = 0;
	k = 1;
    while (!kbhit()) {
         boxfil(j, j, 256-j-1, 212-j-1, 255-j, 0);
         for (i = 0; i < 8000; ++i);
		 if(j > 212) k = -1;
		 if(j < 0) k = 1;
		 j = j + k;
    }
    screen(0);

	return 0;
}

