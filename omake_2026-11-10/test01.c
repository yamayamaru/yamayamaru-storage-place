#include <stdio.h>
#include "newlib_stub.h"

void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth);
int mand(double a1, double b1, int depth1);

int main(int argc, char *argv[])
{
    double    xcorner, ycorner, length;
    int       xdot, ydot, depth;

    xdot = 48;
    ydot = 24;


    xcorner =  -2.4;
    ycorner =  -1.7;
    length  =   3.2;
    depth   =  1000;
    mandel(xcorner, ycorner, length, xdot, ydot, depth);

/*
    xcorner = -6.1099999999999965e-001;
    ycorner =  6e-001;
    length  =  1.0000000000000001e-001;
    depth   =  1000;
    mandel(xcorner, ycorner, length, xdot, ydot, depth);

    xcorner = -1.7878664118448890e+000;
    ycorner = -4.6994043985999932e-003;
    length  =  1.5258789062500001e-006;
    depth   =  1000;
    mandel(xcorner, ycorner, length, xdot, ydot, depth);

    xcorner = -1.255058807618605e+000;
    ycorner =  3.834129844820908e-001;
    length  =  5.960464477539063e-009;
    depth   =  1000;
    mandel(xcorner, ycorner, length, xdot, ydot, depth);
*/

    return 0;
}

void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth)
{
    double xgap, ygap;
    double xx, yy;
    int    i, j, a;

    xgap = length / xdot;
    ygap = length / ydot;
    xx = xcorner;
    yy = ycorner;
    for(j = 0; j < ydot; j++) {
        for(i = 0; i < xdot; i++) {
          a = mand( xx, yy, depth);
          if (a == 0) {
                putchar(' ');
          } else {
              switch (a % 15 + 1) {
                  case 1:
                      printf("\033[0;31m");
                      break;
                  case 2:
                      printf("\033[0;32m");
                      break;
                  case 3:
                      printf("\033[0;33m");
                      break;
                  case 4:
                      printf("\033[0;34m");
                      break;
                  case 5:
                      printf("\033[0;35m");
                      break;
                  case 6:
                      printf("\033[0;36m");
                      break;
                  case 7:
                      printf("\033[0;37m");
                      break;
                  case 8:
                      printf("\033[1;30m");
                      break;
                  case 9:
                      printf("\033[1;31m");
                      break;
                  case 10:
                      printf("\033[1;32m");
                      break;
                  case 11:
                      printf("\033[1;33m");
                      break;
                  case 12:
                      printf("\033[1;34m");
                      break;
                  case 13:
                      printf("\033[1;35m");
                      break;
                  case 14:
                      printf("\033[1;36m");
                      break;
                  case 15:
                      printf("\033[1;37m");
                      break;
              }
              putchar('*');
          }
          xx = xx + xgap;
        }
        printf("\r\n");
        yy = yy + ygap;
        xx = xcorner;
    }
    printf("\033[0m");
    printf("\r\n");
    printf("\r\n");

}

int mand(double a1, double b1, int depth1)
{
    int     i;
    double  x1, y1, tmp1, x2, y2;

    i = 0;
    x1 = 0; y1 = 0;
    x2 = x1 * x1;   y2 = y1 * y1;
    while ((i < depth1) && (x2+y2 <=4)) {
        tmp1 = x2 - y2 + a1;
        y1 = 2 * x1 * y1 + b1;
        x1 = tmp1;
        x2 = x1 * x1;  y2 = y1 * y1;
        i++;
    }

    if (i == depth1) i = 0;
    else             i++;
    return i;
}
