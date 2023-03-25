#include <stdio.h>
#include <stdlib.h>
#include <i86.h>
#include <dos.h>
#include <conio.h>
#include <time.h>

#define VRAM0 0xa8000
#define VRAM1 0xb0000

void gstart();
void screen256_pack();
void fill0(char a);
void fill1(char a);
void pset(int x, int y, int color);
void set_palette(int num, int colr, int colg, int colb);

double second();

void mandelbrot(int width, int height);
void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth);
int mand(double a1, double b1, int depth1);
void set_param(void);
struct _param01 {
    double xcorner;
    double ycorner;
    double length;
    int    depth;
} param01[28];

 
int main(int argc, char *argv[]) {
    int c, i;

    gstart();
    screen256_pack();

    for (i = 1; i < 64; i++) {
        set_palette(i, i * 2 + 129, i * 4 + 3, i * 4 + 3);
    }

    fill0(0x00);

    set_param();

    mandelbrot(640, 400);

    c = getch();

    fill0(0x00);

    return 0;
}

void set_param(void) {
    param01[0].xcorner =  -2.4;
    param01[0].ycorner =  -1.7;
    param01[0].length  =   3.2;
    param01[0].depth   =  1000;

    param01[1].xcorner = -6.1099999999999965e-001;
    param01[1].ycorner =  6e-001;
    param01[1].length  =  1.0000000000000001e-001;
    param01[1].depth   =  1000;

    param01[2].xcorner = -7.512120844523107e-001;
    param01[2].ycorner = 2.939359283447132e-002;
    param01[2].length = 2.441406250000000e-005;
    param01[2].depth = 1000;

    param01[3].xcorner = -1.7878664118448890e+000;
    param01[3].ycorner = -4.6994043985999932e-003;
    param01[3].length  =  1.5258789062500001e-006;
    param01[3].depth   =  1000;

    param01[4].xcorner = -1.255058807618605e+000;
    param01[4].ycorner =  3.834129844820908e-001;
    param01[4].length  =  5.960464477539063e-009;
    param01[4].depth   =  1000;

    param01[5].xcorner = -7.424999999999979e-002;
    param01[5].ycorner = -6.523749999999998e-001;
    param01[5].length = 3.125000000000000e-003;
    param01[5].depth   =  1000;

    param01[6].xcorner = -7.366145833333310e-002;
    param01[6].ycorner = -6.500052083333332e-001;
    param01[6].length = 3.125000000000000e-003;
    param01[6].depth   =  1000;

    param01[7].xcorner = -7.451562499999977e-002;
    param01[7].ycorner = -6.500117187500000e-001;
    param01[7].length = 7.812500000000000e-004;
    param01[7].depth   =  1000;

    param01[8].xcorner = -7.409765624999977e-002;
    param01[8].ycorner = -6.494752604166667e-001;
    param01[8].length = 1.953125000000000e-004;
    param01[8].depth   =  1000;

    param01[9].xcorner = -1.408903645833333e+000;
    param01[9].ycorner = -1.342989908854166e-001;
    param01[9].length = 2.441406250000000e-005;
    param01[9].depth   =  1000;

    param01[10].xcorner = -1.023473307291662e-001;
    param01[10].ycorner = 9.571370442708340e-001;
    param01[10].length = 4.882812500000000e-005;
    param01[10].depth   =  1000;

    param01[11].xcorner = -1.023286539713537e-001;
    param01[11].ycorner = 9.571538899739589e-001;
    param01[11].length = 1.220703125000000e-005;
    param01[11].depth   =  1000;

    param01[12].xcorner = -7.424999999999979e-002;
    param01[12].ycorner = -6.523749999999998e-001;
    param01[12].length = 3.125000000000000e-003;
    param01[12].depth   =  1000;

    param01[13].xcorner = -7.366145833333310e-002;
    param01[13].ycorner = -6.500052083333332e-001;
    param01[13].length = 3.125000000000000e-003;
    param01[13].depth   =  1000;

    param01[14].xcorner = -7.451562499999977e-002;
    param01[14].ycorner = -6.500117187500000e-001;
    param01[14].length = 7.812500000000000e-004;
    param01[14].depth   =  1000;

    param01[15].xcorner = -7.409765624999977e-002;
    param01[15].ycorner = -6.494752604166667e-001;
    param01[15].length = 1.953125000000000e-004;
    param01[15].depth   =  1000;

    param01[16].xcorner = -1.408903645833333e+000;
    param01[16].ycorner = -1.342989908854166e-001;
    param01[16].length = 2.441406250000000e-005;
    param01[16].depth   =  1000;

    param01[17].xcorner = -1.408903645833333e+000;
    param01[17].ycorner = -1.342989908854166e-001;
    param01[17].length = 2.441406250000000e-005;
    param01[17].depth   =  1000;

    param01[18].xcorner = -1.408886164585749e+000;
    param01[18].ycorner = -1.342802622318267e-001;
    param01[18].length = 1.192092895507813e-008;
    param01[18].depth   =  1000;

    param01[19].xcorner = -1.408886160294215e+000;
    param01[19].ycorner = -1.342802576224008e-001;
    param01[19].length = 2.980232238769531e-009;
    param01[19].depth   =  1000;

    param01[20].xcorner = -1.023473307291662e-001;
    param01[20].ycorner = 9.571370442708340e-001;
    param01[20].length = 4.882812500000000e-005;
    param01[20].depth   =  1000;

    param01[21].xcorner = -1.023286539713537e-001;
    param01[21].ycorner = 9.571538899739589e-001;
    param01[21].length = 1.220703125000000e-005;
    param01[21].depth   =  1000;

    param01[22].xcorner = -1.165292968750000e+000;
    param01[22].ycorner = 2.393867187500003e-001;
    param01[22].length = 3.906250000000000e-004;
    param01[22].depth   =  1000;

    param01[23].xcorner = -1.164973597208659e+000;
    param01[23].ycorner = 2.394546101888024e-001;
    param01[23].length = 1.525878906250000e-006;
    param01[23].depth   =  1000;

    param01[24].xcorner = -6.703997395833329e-001;
    param01[24].ycorner = -4.582591145833326e-001;
    param01[24].length = 3.906250000000000e-004;
    param01[24].depth   =  1000;

    param01[25].xcorner = -6.702213948567705e-001;
    param01[25].ycorner = -4.580732421874992e-001;
    param01[25].length = 2.441406250000000e-005;
    param01[25].depth   =  1000;

    param01[26].xcorner = -6.702324136098221e-001;
    param01[26].ycorner = -4.580734767913810e-001;
    param01[26].length = 1.907348632812500e-007;
    param01[26].depth   =  1000;

    param01[27].xcorner = -6.702323307991023e-001;
    param01[27].ycorner = -4.580733914375297e-001;
    param01[27].length = 2.384185791015625e-008;
    param01[27].depth   =  1000;
}

void mandelbrot(int width, int height) {

    double    xcorner, ycorner, length;
    int       xdot, ydot, depth;
    int       i, ii;

    xdot = width;
    ydot = height;

    for (ii = 3; ii < 4; ii++) {
        double start_time, end_time;

        i = ii % 28;
        start_time = second();
        mandel(param01[i].xcorner, param01[i].ycorner, param01[i].length, xdot, ydot, param01[i].depth);
        end_time = second();
        printf("time = %.4f\n", end_time - start_time);
    }

}


void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth) {
    double xgap, ygap;
    double xx, yy;
    int    i, j;
    int    col01;

/*
    xgap = length / xdot;
    ygap = length / xdot;
*/
    xgap = length / ydot;
    ygap = length / ydot;

    xcorner = xcorner - (xdot - ydot) / 2 * xgap;

    printf("xdot = %d    ydot = %d\r\n", xdot, ydot);
    printf("xcorner = %.15e\r\n", xcorner);
    printf("ycorner = %.15e\r\n", ycorner);
    printf("length  = %.15e\r\n", length);

 
    xx = xcorner;
    yy = ycorner;
    for(j = 0; j < ydot; j++) {
        for(i = 0; i < xdot; i++) {
            col01 = (short) mand( xx, yy, depth);
            if (col01 == 0) pset(i, j, 0);
            else            pset(i, j, col01 % 63 + 1);

            xx = xx + xgap;
        }

        yy = yy + ygap;
        xx = xcorner;
    }
}

int mand(double a1, double b1, int depth1) {
    int     i1;
    double  x1, y1, tmp1, x2, y2;

    i1 = 0;
    x1 = 0; y1 = 0;
    x2 = x1 * x1;   y2 = y1 * y1;
    while ((i1 < depth1) && (x2+y2 <=4)) {
        tmp1 = x2 - y2 + a1;
        y1 = 2 * x1 * y1 + b1;
        x1 = tmp1;
        x2 = x1 * x1;  y2 = y1 * y1;
        i1++;
    }

    if (i1 == depth1) i1 = 0;
    else            i1++;
    return i1;
}

double second() {
    static time_t t01 = 0;
    double t;

    if (t01 == 0) {
        t01 = time(NULL);
    }

    t = time(NULL) - t01;

    return t;
}



void gstart() {
/*
    union REGS regs;
    regs.h.ah = 0x40;
#ifdef __386__
    int386(0x18, &regs, &regs);
#else
    int86(0x18, &regs, &regs);
#endif
*/
    outp(0xa2, 0x0d);
}

void screen256_pack() {
    short *sp;
    outp(0x6a, 0x07);
    outp(0x6a, 0x21);
    outp(0xa4, 0x00);
    outp(0xa6, 0x00);
    outp(0x09a8, 0x00);
    sp = (short *)0xe0100;
    *sp = 0;
    sp = (short *)0xe0102;
    *sp = 0;
}

void fill0(char a) {
    char *p01;
    short *sp;
    int  i, j;
    for (j = 0; j < 16; j++) {
        sp = (short *)0xe0004;
        *sp = (short)j;
        p01 = (char *)VRAM0;
        for (i = 0; i < 0x8000; i++) {
            *p01++ = a;
        }
    }
}

void fill1(char a) {
    char *p01;
    short *sp;
    int  i, j;
    for (j = 0; j < 16; j++) {
        sp = (short *)0xe0006;
        *sp = (short)j;
        p01 = (char *)VRAM1;
        for (i = 0; i < 0x8000; i++) {
            *p01++ = a;
        }
    }
}

void pset(int x, int y, int color) {
    int      n, offset0;
    short    bank0;
    short    *sp;
    char     *cp;

    n = y * 640 + x;
    bank0 = (short)(n >> 15);
    sp = (short *)0xe0004;
    *sp = bank0;
    offset0 = n & 0x7fff;
    cp = (char *)(VRAM0 + offset0);
    *cp = color;
}

void set_palette(int num, int colr, int colg, int colb) {
    outp(0xa8, num);
    outp(0xaa, colg);
    outp(0xac, colr);
    outp(0xae, colb);
}
