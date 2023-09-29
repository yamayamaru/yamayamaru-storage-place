
#include <stdio.h>
#include <stdlib.h>
#include "msxalib.h"
#include "msxclib.h"


#include <time.h>

void chgcpu_subfunc(char mode);
void chgcpu_r800_dram();
void chgcpu_r800_rom();
void chgcpu_z80();
int putchar(int ch);
void time_subfunc01(unsigned char *sec, unsigned char *min, unsigned char *hour) __sdcccall(0);
void date_subfunc01(unsigned char *day, unsigned char *mon, unsigned int *year, unsigned char *wday);
unsigned char RtcRead(struct tm *t);
unsigned char RtcRead01(struct tm *t);
time_t time01(time_t *timeptr);
long second();



void mandel(float xcorner, float ycorner, float length, float xdot, float ydot, float depth);
int mand(float a1, float b1, int depth1);
void set_m_param01();

struct m_param {
    float    xcorner;
    float    ycorner;
    float    length;
    int       depth;
} m_param01[15];


long second();

int main(int argc, char *argv[])
{

    float    xcorner, ycorner, length;
    int       xdot, ydot, depth;
    long     start_time, end_time;
    float    ratio01;
    int       i;
    int      number01;


    chgcpu_r800_dram();


    ginit();
    color(0, 0, 0);
    screen(8);
    boxfil(0, 0, 255, 211, 0, 0);

    start_time = second();

    i = 0;
    set_m_param01();

        xdot = 100;
        ydot = 100;

/*
    xcorner = -2.4;
    ycorner = -1.7;
    length = 3.2;
    depth = 1000;
*/

//    for (number01 = 0; number01 < 15; number01++){
    for (number01 = 0; number01 < 2; number01++){
        xcorner = m_param01[number01].xcorner;
        ycorner = m_param01[number01].ycorner;
        length  = m_param01[number01].length;
        depth   = m_param01[number01].depth;
//        depth   = 1000;
		depth   = 100;

        mandel(xcorner, ycorner, length, xdot, ydot, depth);
    }

    end_time = second();


    chgcpu_z80();


    getch();
    color(15, 1, 1);
    screen(0);

    printf("time = %ld sec\n", (end_time - start_time));

    return 0;
}

int col(int color) {
        int  c;

    if (color == 0) return 0;
        c = color % 255 + 1;

    if (c == 0) return 0;
    if (c >=128) return (c - 128 + 1);
        else         return (c + 128);
}

void mandel(float xcorner, float ycorner, float length, float xdot, float ydot, float depth)
{
    float xgap, ygap;
    float xx, yy;
    int    i, j;
        int    c, color;

    xgap = length / xdot;
    ygap = length / ydot;
    xx = xcorner;
    yy = ycorner;
    for(j = 0; j < ydot; j++) {
        for(i = 0; i < xdot; i++) {
          c = mand( xx, yy, depth);

          line(i, j, i, j, col(c), 0);

          xx = xx + xgap;
        }
        yy = yy + ygap;
        xx = xcorner;
    }
}




int mand(float a1, float b1, int depth1)
{
    int     i1;
    float  x1, y1, tmp1, x2, y2;

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

void set_m_param01(){

        m_param01[0].xcorner =  -2.4;
        m_param01[0].ycorner =  -1.7;
        m_param01[0].length  =   3.2;
        m_param01[0].depth   =  1000;

        m_param01[1].xcorner = -6.1099999999999965e-001;
        m_param01[1].ycorner =  6e-001;
        m_param01[1].length  =  1.0000000000000001e-001;
        m_param01[1].depth   =  1000;

        m_param01[2].xcorner = -7.512120844523107e-001;
        m_param01[2].ycorner = 2.939359283447132e-002;
        m_param01[2].length = 2.441406250000000e-005;
        m_param01[2].depth = 1000;

        m_param01[3].xcorner = -7.424999999999979e-002;
        m_param01[3].ycorner = -6.523749999999998e-001;
        m_param01[3].length = 3.125000000000000e-003;
        m_param01[3].depth   =  1000;

        m_param01[4].xcorner = -7.366145833333310e-002;
        m_param01[4].ycorner = -6.500052083333332e-001;
        m_param01[4].length = 3.125000000000000e-003;
        m_param01[4].depth   =  1000;

        m_param01[5].xcorner = -7.451562499999977e-002;
        m_param01[5].ycorner = -6.500117187500000e-001;
        m_param01[5].length = 7.812500000000000e-004;
        m_param01[5].depth   =  1000;

        m_param01[6].xcorner = -7.409765624999977e-002;
        m_param01[6].ycorner = -6.494752604166667e-001;
        m_param01[6].length = 1.953125000000000e-004;
        m_param01[6].depth   =  1000;

        m_param01[7].xcorner = -1.023473307291662e-001;
        m_param01[7].ycorner = 9.571370442708340e-001;
        m_param01[7].length = 4.882812500000000e-005;
        m_param01[7].depth   =  1000;

        m_param01[8].xcorner = -7.424999999999979e-002;
        m_param01[8].ycorner = -6.523749999999998e-001;
        m_param01[8].length = 3.125000000000000e-003;
        m_param01[8].depth   =  1000;

        m_param01[9].xcorner = -7.366145833333310e-002;
        m_param01[9].ycorner = -6.500052083333332e-001;
        m_param01[9].length = 3.125000000000000e-003;
        m_param01[9].depth   =  1000;

        m_param01[10].xcorner = -7.451562499999977e-002;
        m_param01[10].ycorner = -6.500117187500000e-001;
        m_param01[10].length = 7.812500000000000e-004;
        m_param01[10].depth   =  1000;

        m_param01[11].xcorner = -7.409765624999977e-002;
        m_param01[11].ycorner = -6.494752604166667e-001;
        m_param01[11].length = 1.953125000000000e-004;
        m_param01[11].depth   =  1000;

        m_param01[12].xcorner = -1.023473307291662e-001;
        m_param01[12].ycorner = 9.571370442708340e-001;
        m_param01[12].length = 4.882812500000000e-005;
        m_param01[12].depth   =  1000;

        m_param01[13].xcorner = -1.165292968750000e+000;
        m_param01[13].ycorner = 2.393867187500003e-001;
        m_param01[13].length = 3.906250000000000e-004;
        m_param01[13].depth   =  1000;

        m_param01[14].xcorner = -6.703997395833329e-001;
        m_param01[14].ycorner = -4.582591145833326e-001;
        m_param01[14].length = 3.906250000000000e-004;
        m_param01[14].depth   =  1000;


}









void chgcpu_subfunc(char mode) {
    __asm
    push    ix

    push    af
    ld      a,(0xfcc1)      // a:slot EXPTBL:0xfcc1
    ld      hl,#0x0180      // hl:RDSLT read address
    call    0x000c          // RDSLT
    ei
    pop     hl

    cp      #0xc3
    jr      nz,chgcpu_subfunc_jmp01

    ld      iy,(0xfcc1-1)   // EXPTBL:0xfcc1
    ld      a,h         // A register:0x00:z80 mode 0x81:R800 ROM mode   0x82:R800 DRAM mode
    ld      ix,#0x0180      // ix:CALSLT call address
    call    0x001c          // CALSLT

chgcpu_subfunc_jmp01:
    pop     ix
    __endasm;
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

int putchar(int ch) {
    __asm
    push    ix

    push    hl
    ld      e, l
    ld      c, #2
    call    0x0005
    pop     hl

    pop     ix
    __endasm;
}

void time_subfunc01(unsigned char *sec, unsigned char *min, unsigned char *hour) __sdcccall(0) {
    __asm
    push    ix
    ld      ix, #0
    add     ix, sp

    push    ix
    ld      c, #0x2c
    call    0x0005
    ld      c, l
    ld      b, h
    pop     ix
    ld      l, 4(ix)
    ld      h, 5(ix)
    ld      (hl), d     ;sec
    ld      l, 6(ix)
    ld      h, 7(ix)
    ld      (hl), c     ;min
    ld      l, 8(ix)
    ld      h, 9(ix)
    ld      (hl), b     ;hour

    pop     ix
    __endasm;
}

void date_subfunc01(unsigned char *day, unsigned char *mon, unsigned int *year, unsigned char *wday) {
    __asm
    push    ix
    ld      ix, #0
    add     ix, sp

    push    de
    push    hl
    ld      c, #0x2a
    call    0x0005
    ld      c, l
    ld      b, h
    pop     hl
    ld      (hl), e         ;day
    pop     hl
    ld      (hl), d         ;mon
    ld      l, 4(ix)
    ld      h, 5(ix)
    ld      (hl), c         ;year  bc
    inc     hl
    ld      (hl), b
    ld      l, 6(ix)
    ld      h, 7(ix)
    ld      (hl), a         ;wday
    pop     ix
    __endasm;
}

unsigned char RtcRead(struct tm *t) {
    unsigned char sec, min, hour, day, mon, wday;
    unsigned int  year;

    time_subfunc01(&sec, &min, &hour);
    date_subfunc01(&day, &mon, &year, &wday);

    t->tm_sec = sec;
    t->tm_min = min;
    t->tm_hour = hour;
    t->tm_mday = day;
    t->tm_mon = mon;
    t->tm_year = year;
    t->tm_wday = wday;

    return 1;
}

unsigned char RtcRead01(struct tm *t) {
    unsigned char sec, min, hour, day, mon, wday;
    unsigned int  year;

    time_subfunc01(&sec, &min, &hour);
    date_subfunc01(&day, &mon, &year, &wday);

    t->tm_sec = sec;
    t->tm_min = min;
    t->tm_hour = hour;
    t->tm_mday = day;
    t->tm_mon = mon;
    t->tm_year = year;
    t->tm_wday = wday;

    return 1;
}

time_t time01(time_t *timeptr) {
  struct tm now;
  time_t t01=-1L;

  if (RtcRead01(&now)) {
    t01=mktime(&now);
  }
  if (timeptr) {
    *timeptr=t01;
  }
  return t01;
}

long second() {
    static long t = 0;
    if (t == 0) {
        t = time01(NULL);
    }
    return (time01(NULL) - t);
}
