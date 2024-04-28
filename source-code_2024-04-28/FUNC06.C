#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>

// F-BASIC386Ç©ÇÁåƒÇ—èoÇ∑ä÷êîÇÕ__cdeclÇ…Ç∑ÇÈÇ±Ç∆

#define VRAM_WIDTH 1024
#define M_PI (3.141592653589793)

char far *vram;

void pset01(int x, int y, int color);
void line01(int x1, int y1, int x2, int y2, int color);

void __cdecl func03(int *result, int pa, int pb, int pca, int pcb, int pstep01) {
    int i, j, k;
    int xc, yc;
    double rad, x, y, x1, y1, a, b, ca, cb;
    double ag, an, al, a1, a2, po, c1, c2, c3, s1, s2, s3, xx, yy, xi, yi;
    int step01;

    vram = MK_FP(0x010c, 0);  // 640x480 256 color

    for (j = 0; j < 480; j++) {
        for (i = 0; i < 640; i++) {
            vram[j * VRAM_WIDTH + i] = 0;
        }
    }

    a = pa;   b = pb;
    ca = pca; cb = pcb;
    step01 = pstep01;
    xc = 640 / 2;  yc = 480 / 2;
    rad = M_PI / 180.0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j <= 360; j += 2) {
            ag = j * rad;
            al = ag * 6 + M_PI / 2.0;
            a1 = ag + M_PI * i;
            a2 = ag + M_PI / 2.0;
            x = a * cos(al - M_PI / 2.0);
            y = b * sin(al - M_PI / 2.0);
            po = sqrt(x * x + y * y);
            c1 = cos(al);
            s1 = sin(al);
            c2 = cos(a1);
            s2 = sin(a1);
            c3 = cos(a2);
            s3 = sin(a2);
            for (k = 0; k <= 360; k += step01) {
                an = k * rad;
                x = a * cos(an);
                y = b * sin(an);
                xx = x * c1 - y * s1;
                y = x * s1 + y * c1;
                y = y + po;
                x = xx;
                xx = x * c2 - y * s2;
                y = x * s2 + y * c2;
                x = xx;
                xi = ca * c3;
                yi = cb * s3;
                x = x + xi;
                y = y + yi;
                if (k > 0) {
                    line01(x1 + xc, -y1 + yc, x + xc, -y + yc, j % 255 + 1);
//                    pset01(x + xc, -y + yc, j % 255 + 1);
                } else {
                    pset01(x + xc, -y + yc, j % 255 + 1);
                }
                x1 = x;
                y1 = y;
            }
        }
    }

    *result = 0;
}

void line01(int x0, int y0, int x1, int y1, int color) {
    int steep = abs(y1 - y0) > abs(x1 - x0);
    int dx, dy;
    int err;
    int ystep;
    int tmp;

    if (steep) {
        tmp = x0;
        x0 = y0;
        y0 = tmp;

        tmp = x1;
        x1 = y1;
        y1 = tmp;
    }
    if (x0 > x1) {
        tmp = x0;
        x0 = x1;
        x1 = tmp;

        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    dx = x1 - x0;
    dy = abs(y1 - y0);

    err = dx / 2;
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }
    for (; x0 <= x1; x0++) {
        if (steep) {
            pset01(y0, x0, color);
        } else {
            pset01(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void pset01(int x, int y, int color) {
    vram[y * VRAM_WIDTH + x] = (unsigned char)color;
}

int main() {
    return 0;
}
