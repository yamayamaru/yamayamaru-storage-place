#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <i86.h>
#include <dos.h>
#include <conio.h>

double d[160];
void gstart(void);
void color16(void);
void pset(int x, int y, int color);
void fill_blue(char a);
void fill_red(char a);
void fill_green(char a);
void fill_int(char a);
void clear_text(void);
void put_vram_char(int x, int y, int ch, int attri);

int main(int argc, char *argv[]) {
    int       i, j, x, y, zz, px, py, f, col;
    double    dr, r, z, sx, sy;
    int       xmax, ymax;

    gstart();
    color16();

    clear_text();

    fill_blue(0x00);
    fill_red(0x00);
    fill_green(0x00);
    fill_int(0x00);

    xmax = 640;
    ymax = 400;

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
                col = col | 4;
                f = 1;
            }
            if ((zz >= 4)) {
                col = col | 2;
                f = 1;
            }

            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);
            if (f == 1) {
		for (j = 0; j < 3; j++) {
                    for (i = 0; i < 3; i++) {
                        pset(px + i, py + j, col | 0x08);
                    }
                }
            }
            d[(int)sx] = sy;
        }
    }

    return 0;
}


#define PC98_XLINE_WIDTH    640
#define VRAM_BLUE  0xa8000000UL
#define VRAM_RED   0xb0000000UL
#define VRAM_GREEN 0xb8000000UL
#define VRAM_INT   0xe0000000UL
#define TEXT_VRAM  0xa0000000UL
#define TEXT_VRAM_ATTRI  0xa2000000UL

void gstart(void) {     /* グラフィック表示ON */
    union REGS regs;
    regs.h.ah = 0x40;
#ifdef __386__
    int386(0x18, &regs, &regs);
#else
    int86(0x18, &regs, &regs);
#endif
}

void color16(void) {    /* 16色表示にする */
    outp(0x6a, 1);
    outp(0xa2, 0x4b);
    outp(0xa0, 0);
    outp(0x68, 8);
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

void fill_blue(char a) {
    char far *p01;
    unsigned int  i;

    p01 = (char far *)VRAM_BLUE;
    for (i = 0; i < 0x8000; i++) {
        *p01++ = a;
    }
}

void fill_red(char a) {
    char far *p01;
    unsigned int  i;

    p01 = (char far *)VRAM_RED;
    for (i = 0; i < 0x8000; i++) {
        *p01++ = a;
    }
}

void fill_green(char a) {
    char far *p01;
    unsigned int  i;

    p01 = (char far *)VRAM_GREEN;
    for (i = 0; i < 0x8000; i++) {
        *p01++ = a;
    }
}

void fill_int(char a) {
    char far *p01;
    unsigned int  i;

    p01 = (char far *)VRAM_INT;
    for (i = 0; i < 0x8000; i++) {
        *p01++ = a;
    }
}

void pset(int x, int y, int color) {
    unsigned int addr01;
    unsigned int bit01;
    char far *p;
    unsigned char c;

    addr01 = (unsigned int)(((unsigned long)PC98_XLINE_WIDTH * y + x) >> 3);
    bit01 = x & 0x07;
    c = 0x80 >> bit01;

    p = (char far *)VRAM_BLUE;
    p += addr01;
    if (color & 0x01) {
        *p = *p | c;
    } else {
        *p = *p & ~c;
    }
    p = (char far *)VRAM_GREEN;
    p += addr01;
    if (color & 0x02) {
        *p = *p | c;
    } else {
        *p = *p & ~c;
    }
    p = (char far *)VRAM_RED;
    p += addr01;
    if (color & 0x04) {
        *p = *p | c;
    } else {
        *p = *p & ~c;
    }
    p = (char far *)VRAM_INT;
    p += addr01;
    if (color & 0x08) {
        *p = *p | c;
    } else {
        *p = *p & ~c;
    }
}
