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
    int i, r;

    for (i = 0; i < 10; ++i) {
        r = rnd(1000);
        printf("%3d ", r);
    }

	return 0;
}

