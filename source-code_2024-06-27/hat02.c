#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <msxalib.h>
#include <msxclib.h>
#include <math.h>


double d[160];

int main(int argc, char *argv[]) {
    int       i, x, y, zz, px, py, f, col;
    double    dr, r, z, sx, sy;
    int       xmax, ymax;

    ginit();
    screen(5);
    color(15, 1, 1);

    for (i = 0; i < 8; i++) {
        palett(i+1, ((i >> 1) & 0x1) * 7, ((i >> 2) & 0x1) * 7, (i & 0x1) * 7);
    }

    boxfil(0, 0, 255, 211, 1, 0);

    xmax = 256;
    ymax = 212;

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }
    dr = 3.141592653589793 / 180.0;
    for (y = -180; y <= 180; y += 6) {
        for (x = -180; x < 180; x += 4) {
            r = dr * sqrt((double)x * (double)x + (double)y * (double)y);
            z = 100 * cos(r) - 30 * cos(3*r);
            sx = 80 + x / 3 - y / 6;
            sy = 40 - y / 6 - z / 4;
            if ((sx < 0) || (sx >= 160)) continue;
            if (d[(int)sx] <= sy) continue;
            zz = (int)((z + 100) * 0.035) + 1;
            col = 0;
            f = 0;
            if ((zz == 1) || (zz == 3) || (zz == 5) || (zz == 7)) {
                col = col | 1;
                f = 1;
            }
            if ((zz == 2) || (zz == 3) || (zz >= 6)) {
                col = col | 2;
                f = 1;
            }
            if ((zz >= 4)) {
                col = col | 4;
                f = 1;
            }

            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);
            if ((f == 1) & (px < xmax) & (px >= 0) & (py < ymax) & (py >= 0)) {
                /*line_m(px, py, px, py, col + 1, 0);*/
                boxfil(px, py, px, py, col + 1, 0);
            }
            d[(int)sx] = sy;
        }
    }

    getch();
    screen(0);

    return 0;
}
