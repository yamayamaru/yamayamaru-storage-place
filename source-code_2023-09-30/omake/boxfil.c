#include <stdio.h>
#include <msxalib.h>
#include <msxclib.h>
#include <conio.h>

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
}

