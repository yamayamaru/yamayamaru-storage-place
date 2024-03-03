#include <stdio.h>
#include "msxalib.h"

extern int RNDSPD[2];

int main(int argc, char *argv[])
{
    int i, r;

    printf("RNDSPD[0] = 0x%04x, RNDSPD[1] = 0x%04x\r\n", RNDSPD[0], RNDSPD[1]);
    for (i = 0; i < 10; ++i) {
        r = rnd(1000);
        printf("%3d ", r);
    }

	return 0;
}

