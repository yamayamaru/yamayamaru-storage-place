#include <stdio.h>
#include <stdlib.h>
#include <egb.h>
#include <conio.h>

char egb_work[EgbWorkSize];
char para[128];

void mandel(int xcorner, int ycorner, int length, int xdot, int ydot, int depth);
int mand(int a1, int b1, int depth1);

int imul01(int aa, int bb);
//void imul01_asm(int a, int b, unsigned int *hi, unsigned int *lo);

int main(int argc, char *argv[]) {
    int       i;
    int       xmax, ymax;
    int       xcorner, ycorner, length, length1;
    int       depth;

    xmax = 640;
    ymax = 480;

    EGB_init(egb_work, EgbWorkSize);
    EGB_resolution(egb_work, 0, 12) ;	          // 640×480画面 256色
    EGB_resolution(egb_work, 1, 12) ;
    EGB_writePage(egb_work, 0x0) ;

    EGB_paintMode(egb_work, 0x22);                //  面塗をベタ塗りに指定
    EGB_color(egb_work, 0, 255);                  //  色識別番号で前景色設定
    EGB_color(egb_work, 1, 0);                    //  色識別番号で背景色設定

    for (i = 1; i < 128; i++) {
        DWORD(para + 0)  = 1;                         //  パレット設定個数
        DWORD(para + 4)  = i;                         //  色識別番号
        BYTE( para + 8)  = i % 128 * 2 + 1;           //  青の階調
        BYTE( para + 9)  = i % 128 + 128;             //  赤の階調
        BYTE( para + 10) = i % 128 * 2 + 1;           //  緑の階調
        BYTE( para + 11) = 0;
        EGB_palette( egb_work, 0, para);
    }

    EGB_clearScreen(egb_work);

    xcorner = -2.4 * 16777216;
    ycorner = -1.7 * 16777216;
    length  = 3.2 * 16777216;
    depth   = 1000;

    length1 = imul01(length, ((xmax * 65536 / ymax) * 256));
    xcorner = xcorner - (length1 - length) / 2;

    mandel(xcorner, ycorner, length1, xmax, ymax, depth);

    while (!kbhit());

    EGB_color(egb_work, 1, 0);                     //  色識別番号で背景色設定
    EGB_clearScreen(egb_work);

    return 0;
}

void mandel(int xcorner, int ycorner, int length, int xdot, int ydot, int depth) {
    int    xgap, ygap;
    int    xx, yy;
    int    i, j;
    int    col01;

    xgap = length / xdot;
    ygap = length / xdot; /*xと同じにする*/
    xx = xcorner;
    yy = ycorner;
    for(j = 0; j < ydot; j++) {
        for(i = 0; i < xdot; i++) {
          col01 = (short) mand(xx, yy, depth);

          EGB_color(egb_work, 0, col01 % 128);       //  色識別番号で前景色設定
          EGB_color(egb_work, 2, col01 % 128);       //  色識別番号で面塗色設定

//            WORD(para + 0) = i;
//            WORD(para + 2) = j;
//            WORD(para + 4) = i;
//            WORD(para + 6) = j;
//            EGB_rectangle(egb_work, para)

          WORD(para + 0) = 1;               //  座標点数
          WORD(para + 2) = i;              //  Ｘ座標
          WORD(para + 4) = j;              //  Ｙ座標
          EGB_pset(egb_work, para);         //  ポイント

          xx = xx + xgap;
        }
        yy = yy + ygap;
        xx = xcorner;
    }
}

/*
int mand(int a1, int b1, int depth1) {
    int     i1;
    int  x1, y1, tmp1, x2, y2;

    i1 = 0;
    x1 = 0; y1 = 0;
    x2 = imul01(x1, x1);   y2 = imul01(y1, y1);
    while ((i1 < depth1) && (x2+y2 <= (4 << 24))) {
        tmp1 = x2 - y2 + a1;
        y1 = imul01(2 * x1, y1) + b1;
        x1 = tmp1;
        x2 = imul01(x1, x1);  y2 = imul01(y1, y1);
        i1++;
    }

    if (i1 == depth1) i1 = 0;
    else            i1++;
    return i1;
}

int imul01(int a, int b) {
    unsigned int hi, lo;
    int          ret01;

    imul01_asm(a, b, &hi, &lo);

    ret01 = (hi << 8) | (lo >> 24);

    return ret01;
}
*/