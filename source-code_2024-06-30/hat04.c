#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <msxalib.h>
#include <msxclib.h>
#include <math.h>

void   time01_start();
unsigned int  time01(time_t *timeptr);

void chgcpu_subfunc(char mode);
void chgcpu_r800_dram();
void chgcpu_r800_rom();
void chgcpu_z80();

double d[160];

int main(int argc, char *argv[]) {
    int       i, x, y, zz, px, py, f, col;
    double    dr, r, z, sx, sy;
    int       xmax, ymax;
    unsigned int      time_start, time_end;
    long      time02;


    chgcpu_r800_dram();


    ginit();
    screen(5);
    color(15, 1, 1);



    for (i = 0; i < 8; i++) {
        palett(i+1, ((i >> 1) & 0x1) * 7, ((i >> 2) & 0x1) * 7, (i & 0x1) * 7);
    }


    time01_start();
    time_start = time01(NULL);


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

    time_end = time01(NULL);

    time02 = (long)time_end;
    time02 -= time_start;
    if (time02 < 0) time02 += 0x10000L;


	chgcpu_z80();


    getch();
    screen(0);

    printf("time = %ld sec\n", time02 / 60);

    return 0;
}

void   time01_start() {
    unsigned int *p;
    p = (unsigned int *)0xfc9eU;
    *p = 0;
}

unsigned int time01(time_t *timeptr) {
  unsigned int    t=-1;
  unsigned int    *p;

  p = (unsigned int *)0xfc9eU;
  t = *p;
  if (timeptr) {
    *timeptr=t;
  }
  return t;
}


void chgcpu_subfunc(char mode) {
#asm

    ld      hl, 2
    add     hl, sp
    ld      a, (hl)

    push    af
    ld      a,(0fcc1h)      // a:slot EXPTBL:0xfcc1
    ld      hl,0180h      // hl:RDSLT read address
    call    000ch          // RDSLT
    ei
    pop     hl

    cp      0c3h
    jr      nz,chgcpu_subfunc_jmp01

    ld      iy,(0fcc1h-1)   // EXPTBL:0xfcc1
    ld      a,h         // A register:0x00:z80 mode 0x81:R800 ROM mode   0x82:R800 DRAM mode
    ld      ix,0180h      // ix:CALSLT call address
    call    001ch          // CALSLT

chgcpu_subfunc_jmp01:

#endasm
}

void chgcpu_r800_dram() {
    chgcpu_subfunc(0x82);
}

void chgcpu_r800_rom() {
    chgcpu_subfunc(0x81);
}

void chgcpu_z80() {
    chgcpu_subfunc(0x00);
}

