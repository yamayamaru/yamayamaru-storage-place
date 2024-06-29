#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__WATCOMC__)
    #include <i86.h>
#endif
#include <dos.h>
#include <math.h>
#include <conio.h>


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef char int8_t;
typedef short int16_t;
typedef long int32_t;

#define PC98_GLIB_WIDTH  640
#define PC98_GLIB_HEIGHT 400

#define PC98_XLINE_WIDTH 640

#define VRAM0            0xa8000000UL
#define VRAM1            0xb0000000UL
#define TEXT_VRAM        0xa0000000UL
#define TEXT_VRAM_ATTRI  0xa2000000UL

double second();

void gstart();
void screen256_pack();
void pc9821_off();
void fill0(char a);
void fill1(char a);
void pset(int x, int y, int color);
void fillscreen(uint8_t color);
void set_palette(int num, int colr, int colg, int colb);
void clear_text(void);
void put_vram_char(int x, int y, int ch, int attri);

int col2r[161], col2g[161], col2b[161];
double d[160];

int main(int argc, char *argv[]) {
    int       c, i, j, x1, y1, zz, px, py, f, col, sxi;
    double    dr, r, z, x, y, sx, sy;
    int       xmax, ymax;

    gstart();
    screen256_pack();

    fillscreen(0);

    clear_text();

    xmax = 640;
    ymax = 400;

    for (i = 0; i <= 160; i++) {
        col2r[i] = 0;
        col2g[i] = 0;
        col2b[i] = 0;
        if (i <= 31) {
            col2b[i] = i;
        }
        if ((i > 32) && (i <= 63)) {
            col2b[i] = (63 - i);
            col2r[i] = (i -33);
        }
        if (i == 64) {
            col2r[i] = (i - 33);
        }
        if ((i > 64) && (i <= 95)) {
            col2r[i] = (95 - i);
            col2g[i] = (i - 65);
        }
        if (i == 96) {
            col2g[i] = (i - 65);
        }
        if ((i > 96) && (i <= 127)) {
            col2g[i] = ((127 - i) + (i - 97));
            col2b[i] = (i - 97);
        }
        if (i == 128) {
            col2g[i] = (i - 97);
            col2b[i] = (i - 97);
        }
        if ((i > 128) && (i <= 159)) {
            col2g[i] = ((159 - i) + (i - 129));
            col2b[i] = ((159 - i) + (i - 129));
            col2r[i] = (i - 129);
        }
        if (i >= 160) {
            col2g[i] = 31;
            col2b[i] = 31;
            col2r[i] = 31;
        }
    }

    for (i = 0; i <= 160; i++) {
        set_palette(i, col2r[i] * 8, col2g[i] * 8, col2b[i] * 8);
    }

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }
    dr = 3.141592653589793 / 180.0;
    for (y1 = -1800; y1 <= 1800; y1 += 10) {
        y = y1 / 10.0;
        for (x1 = -1800; x1 < 1800; x1 += 2) {
            x = x1 / 10.0;
            r = dr * sqrt(x * x + y * y);
            z = 100 * cos(r) - 30 * cos(3*r);
            sx = 80 + x / 3 - y / 6;
            sxi = (int)sx;
            sy = 40 - y / 6 - z / 4;
            if ((sxi < 0) || (sxi >= 160)) continue;
            if (d[sxi] <= sy) continue;
            zz = (int)((z + 100) * 0.035 / 7.0 * 160) + 1;

            if (zz > 160) zz = 160;
            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);
            for (j = 0; j < 1; j++) {
                for (i = 0; i < 1; i++) {
                    pset(px + i, py + j, zz);
                }
            }
            d[sxi] = sy;
        }
    }

    c = getch();

    pc9821_off();

    return 0;
}

void clear_text(void) {
    int i, j;
    for (j = 0; j < 40; j++) {
        for (i = 0; i < 80; i++) {
            put_vram_char(i, j, 0x20, 0xe1);
        }
    }
}

/*
  アトリビュート
    bit0: 表示  1:表示 0:非表示
    bit1: 点滅  1:点滅 0:通常
    bit2: 反転  1:反転 0:通常
    bit3: アンダーライン     1:アンダーライン 0:通常
    bit4: バーティカルライン 1:縦線           0:通常
    bit5: 青
    bit6: 赤
    bit7; 緑
*/
void put_vram_char(int x, int y, int ch, int attri) {
    char far *p01;
    unsigned int d;
    d = 2 * (x + y * 80);
    p01 = (char far *)TEXT_VRAM;
    *(p01 + d) = (char)ch;            // 半角キャラクタのみ;
    *(p01 + d + 1) = 0;
    p01 = (char far *)TEXT_VRAM_ATTRI;
    *(p01 + d) = (char)attri;
    *(p01 + d + 1) = (char)0;
}

void gstart() {
    outp(0xa2, 0x0d);
}

void screen256_pack() {
    short far *sp;
    outp(0x6a, 0x07);
    outp(0x6a, 0x21);
    outp(0xa4, 0x00);
    outp(0xa6, 0x00);
    outp(0x09a8, 0x00);
    sp = (short far *)0xe0000100UL;
    *sp = 0;
    sp = (short far *)0xe0000102UL;
    *sp = 0;
}

void pc9821_off() {
    char far *p;
    // 16[MB] System Area -> VRAM OFF
    p = (char far *)0xe0000102UL;
    *p = 0x00;

    // 16 Color (standard) mode
    outp(0x6a,0x07);
    outp(0x6a,0x20);
    outp(0x6a,0x06);

    // TEXT Start
    outp(0x62,0x0d);
}

void fill0(char a) {
    char far *p01;
    short far *sp;
    unsigned int  i, j;
    for (j = 0; j < 16; j++) {
        sp = (short far *)0xe0000004UL;
        *sp = (short)j;
        p01 = (char far *)VRAM0;
        for (i = 0; i < 0x8000U; i++) {
            *p01++ = a;
        }
    }
}

void fill1(char a) {
    char far *p01;
    short far *sp;
    unsigned int  i, j;
    for (j = 0; j < 16; j++) {
        sp = (short far *)0xe0000006UL;
        *sp = (short)j;
        p01 = (char far *)VRAM1;
        for (i = 0; i < 0x8000U; i++) {
            *p01++ = a;
        }
    }
}

void pset(int x, int y, int color) {
    long     n;
    int      offset0;
    short    bank0;
    short far *sp;
    char  far *cp;

    n = y * 640L + x;
    bank0 = (short)(n >> 15);
    sp = (short far *)0xe0000004UL;
    *sp = bank0;
    offset0 = (int)(n & 0x7fffUL);
    cp = (char far *)(VRAM0 + offset0);
    *cp = color;
}

void fillscreen(uint8_t color) {
    char  far *p01;
    short far *sp;
    unsigned int  i, j;
    char colr, colg, colb;

    for (j = 0; j < 16; j++) {
        sp = (short far *)0xe0000004UL;
        *sp = (short)j;
        p01 = (char far *)VRAM0;
        for (i = 0; i < 0x8000U; i++) {
	    *p01++ = color;
        }
    }
}

void set_palette(int num, int colr, int colg, int colb) {
    outp(0xa8, num);
    outp(0xaa, colg);
    outp(0xac, colr);
    outp(0xae, colb);
}
