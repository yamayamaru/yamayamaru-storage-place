
#include <stdio.h>
#include <stdlib.h>
#include <msxalib.h>

void screen(int md)
{
    struct regs r;

    r.a = (char)md;
    calbio(0x005f, &r);
}

void color(int fg, int bg, int bd)
{
    struct regs r;

    *((char *)0xf3e9) = (char)fg;
    *((char *)0xf3ea) = (char)bg;
    *((char *)0xf3eb) = (char)bd;
    calbio(0x0062, &r);
}

void inispr(int sz)
{
    struct regs r;

    r.bc = (int)((*((char *)0xf3e0) & 0xfc) | sz) * 256 + 1;
    calbio(0x0047, &r);
    if (*((char *)0xfaf8) != 0) {
        r.bc = (int)(*((char *)0xffe7) & 0xfd) * 256 + 8;
        calbio(0x0047, &r);
    }
    calbio(0x0069, &r);
}

void sprite(int no, char *dt)
{
    if (*((char *)0xf3e0) & 2)
        ldirvm(*((int *)0xf926) + no * 8, dt, 32);
    else
        ldirvm(*((int *)0xf926) + no * 8, dt, 8);
}

void putspr(int pl, int x, int y, int cl, int pt)
{
    char dt[4];

    dt[0] = (char)y;
    dt[1] = (char)x;
    dt[2] = (char)pt;
    dt[3] = (char)cl;
    ldirvm(*((int *)0xf928) + pl * 4, dt, 4);
}

void colspr(int pl, char *dt)
{
    if (*((char *)0xf3e0) & 2)
        ldirvm(*((int *)0xf928) - 512 + pl * 16, dt, 16);
    else
        ldirvm(*((int *)0xf928) - 512 + pl * 16, dt, 8);
}


unsigned char gtstck(int no)
{
    struct regs r;

    r.a = (char)no;
    calbio(0x00d5, &r);
    return (r.a);
}

unsigned char gttrig(int no)
{
    struct regs r;

    r.a = (char)no;
    calbio(0x00d8, &r);
    return (r.a != 0);
}

int gtpad(int no)
{
    struct regs r;

    r.a = (char)no;
    calbio(0x00db, &r);
    if (r.a & 0x80)
        return (int)(r.a) - 256;
    else
        return (int)(r.a);
}
unsigned char snsmat(int mt)
{
    struct regs r;

    r.a = (char)mt;
    calbio(0x0141, &r);
    return (r.a);
}

void kilbuf()
{
    struct regs r;

    calbio(0x0156, &r);
}

void gicini()
{
    struct regs r;

    calbio(0x0090, &r);
}

void sound(int rg, int dt)
{
    struct regs r;

    r.a = (char)rg;
    r.de = dt;
    calbio(0x0093,&r);
}

int sgn(int vl)
{
    if (vl < 0)
        return -1;
    if (vl != 0)
        return 1;
    return 0;
}

void srnd()
{
    extern int rnd_m_rndspd01[2];
	int *p1;

    //rnd_m_rndspd01[1] = *((int *)0xfca2);
	p1 = (int *)0xfca2;
    rnd_m_rndspd01[1] = *p1;
}


