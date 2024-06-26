#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>

#define T32NAM *((int *)0xf3bd)

int main(int argc, char *argv[])
{
    ginit();
    color(15, 0, 0);
    screen(1);
        filvrm(T32NAM, 32, '*');
        filvrm(T32NAM+32, 32, '?');
        filvrm(T32NAM+64, 32, 'Z');
        while(!kbhit());
    screen(0);
}
