#include <stdio.h>
#include <msxalib.h>
#include <msxclib.h>
#include <conio.h>

char Ptn[4+13*20] = { 13, 0, 19, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4,15, 4, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4,15, 4, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4,15, 4, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4,15, 4, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4,15, 4, 0, 0,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     0, 0, 0,11,11, 8, 8, 8,11,11, 0, 0, 0,
     0, 4, 4, 4,11,11,11,11,11, 4, 4, 4, 0,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     4, 4, 4, 4, 4, 4,11, 4, 4, 4, 4, 4, 4,
    11,11, 4, 4, 4, 4, 4, 4, 4, 4, 4,11,11,
    11,11, 4,11, 4,11,11,11, 4,11, 4,11,11,
     0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
     0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0,
     0, 8, 8, 0,11,11, 0,11,11, 0, 8, 8, 0,
     8, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8,
     8, 8, 8, 8, 8, 8, 0, 8, 8, 8, 8, 8, 8 };

int main(int argc, char *argv[])
{
    int i, j;

    ginit();
    color(15, 1, 1);
    screen(5);
    cpym2v(Ptn, 0, 10, 10, 1, 0);

    while (!kbhit()) {
        for (j = 0; j < 3000; ++j);
        cpyv2v(10, 10, 22, 29, 1, 100, 100, 0, 0);
        for (i = 0; i < 20000; ++i);
        for (i = 101; i <= 108; ++i) {
            cpyv2v(10, 10, 22, 18, 1, 100, i, 0, 0);
            for (j = 0; j < 2000; ++j);
        }
    }
    screen(0);
}

