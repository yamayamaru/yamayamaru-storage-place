#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <graphics.h>
#include <conio.h>

void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth);
int mand(double a1, double b1, int depth1);

struct m_param {
    double    xcorner;
    double    ycorner;
    double    length;
    int       depth;
} m_param01[5];

void    set_m_param01(void);
double  second(void);
void    ginit(void);
int main(int argc, char **argv) {

    double    xcorner, ycorner, length;
    int       xdot, ydot, depth;
    double    start_time, end_time;
    int       i;
    int       number01;

    ginit();

    set_m_param01();

    xdot = 400;
    ydot = 400;

    number01 = 0;
    xcorner = m_param01[number01].xcorner;
    ycorner = m_param01[number01].ycorner;
    length  = m_param01[number01].length;
    depth   = m_param01[number01].depth;

    start_time = second();

    for (number01 = 0; number01 < 5; number01++){
        xcorner = m_param01[number01].xcorner;
        ycorner = m_param01[number01].ycorner;
        length  = m_param01[number01].length;
        depth   = m_param01[number01].depth;

        mandel(xcorner, ycorner, length, xdot, ydot, depth);
    }
    end_time = second();

    printf("execute time %.15fsec\n", end_time - start_time);

    getch();
    closegraph();

    return 0;
}

int far gdriver = DETECT;
int far gmode;
int errorcode;
int xmax, ymax;
void ginit(void) {

    initgraph(&gdriver, &gmode, "");

    errorcode = graphresult();
    if (errorcode != grOk) {
        printf("Graphics error: %s\n", grapherrormsg(errorcode));
        printf("Press any key to halt:");
        getch();
        exit(1);
    }

    setcolor(getmaxcolor());
    xmax = getmaxx();
    ymax = getmaxy();
}

void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth) {
    double xgap, ygap;
    double xx, yy;
    int    i, j;
    int    col01, colr, colg, colb;

    xgap = length / xdot;
    ygap = length / xdot;
    xx = xcorner;
    yy = ycorner;
    for(j = 0; j < ydot; j++) {
        for(i = 0; i < xdot; i++) {
            col01 = (short) mand( xx, yy, depth);
            if (col01 == 0) putpixel(i, j, 0);
            else            putpixel(i, j, col01 % (getmaxcolor() - 1) + 1);

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

void    set_m_param01(void) {

        m_param01[0].xcorner = -2.4;                        m_param01[00].ycorner = -1.7;
        m_param01[0].length  =  3.2;                        m_param01[00].depth   = 1000;
        m_param01[1].xcorner = -6.1099999999999965e-001;    m_param01[01].ycorner = 6e-001;
        m_param01[1].length  =  1.0000000000000001e-001;    m_param01[01].depth   = 1000;
        m_param01[2].xcorner = -1.7878664118448890e+000;    m_param01[02].ycorner = -4.6994043985999932e-003;
        m_param01[2].length  =  1.5258789062500001e-006;    m_param01[02].depth   = 1000;
        m_param01[3].xcorner =  3.6540625000000060e-001;    m_param01[03].ycorner =  1.1625000000000031e-001;
        m_param01[3].length  =  6.2500000000000003e-003;    m_param01[03].depth   = 1000;
        m_param01[4].xcorner = -1.6247249348958287e-001;    m_param01[04].ycorner = -1.0341339518229167e+000;
        m_param01[4].length  =  2.4414062500000001e-005;    m_param01[04].depth   = 1000;
}

double second(void) {
    return time(0) * 1.0;
}
