#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


int mode(int mode);

int main(int argc, char *argv[])
{
    ginit();
    screen(0);
        mode(40);
        printf("msx screen 0, mode 40\n");
    while(!kbhit());
        mode(80);
        printf("msx screen 0, mode 80\n");

    return 0;
}

int mode(int smode) {
    struct regs regs01;

    if (!(smode == 40 || smode == 80)) return 1;

    *((char *)0xf3ae) = (char)smode;      /* LINL40  = mode;    mode  =  80 or 40 */
    regs01.a = 0;                         /* screen mode 0 */
    calbio(0x005f, &regs01);              /* call CHGMOD */

    return 0;
}
