#include <stdio.h>
#include "msxalib.h"
#include "msxclib.h"

int main(int argc, char *argv[])
{
    int i,j,k;

    ginit();
    srnd();
    screen(8);

    j = 0;
	k = 1;
    while (!kbhit()) {
         boxfil(j, j, 256-j, 212-j, 255-j, 0);
         for (i = 0; i < 8000; ++i);
		 if(j > 212) k = -1;
		 if(j < 0) k = 1;
		 j = j + k;
    }
    screen(0);

	return 0;
}

