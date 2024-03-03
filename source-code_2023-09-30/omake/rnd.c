#include <stdio.h>
#include <msxalib.h>

int main(int argc, char *argv[])
{
    int i, r;

    for (i = 0; i < 10; ++i) {
        r = rnd(1000);
        printf("%3d ", r);
    }
}

