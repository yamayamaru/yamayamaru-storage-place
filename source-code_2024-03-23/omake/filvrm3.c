#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>

#define T32NAM *((int *)0xf3b3)


int mode(int smode);

int main(int argc, char *argv[])
{
    ginit();
    color(15, 0, 0);
    screen(0);
    mode(40);
    filvrm(T32NAM, 40, '*');
    filvrm(T32NAM+40, 40, '?');
    filvrm(T32NAM+80, 40, 'Z');
    while(!kbhit());
    screen(0);
    mode(80);
}

int mode(int smode) {
    struct regs regs01;

    if (!(smode == 40 || smode == 80)) return 1;

    *((char *)0xf3ae) = (char)smode;      /* LINL40  = mode;    mode  =  80 or 40 */
    regs01.a = 0;                         /* screen mode 0 */
    calbio(0x005f, &regs01);              /* call CHGMOD */

    return 0;
}
