#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "..\..\include\egb.h"


double d[160];
char col2r[161];
char col2g[161];
char col2b[161];

char egb_work[EgbWorkSize];
char para[128];
int main(int argc, char *argv[]) {
    int       i, x1, y1, zz, px, py, f, sxi;
    double    dr, r, z, sx, sy, x, y;
    int       xmax, ymax;

    xmax = 640;
    ymax = 480;

    EGB_init(egb_work, EgbWorkSize);              // EGB初期化
    EGB_resolution(egb_work, 0, 12);	          // 640×480画面 256色 ページ0
    EGB_resolution(egb_work, 1, 12);	          // 640×480画面 256色 ページ1
    EGB_writePage(egb_work, 0x0);                 // 書き込みページ指定

    EGB_paintMode(egb_work, 0x22);                //  面塗をベタ塗りに指定, 境界をベタ塗りに指定(EGB_psetは境界をベタ塗りに指定する)
    EGB_color(egb_work, 0, 255);                  //  色識別番号で前景色設定
    EGB_color(egb_work, 1, 0);                    //  色識別番号で背景色設定

    EGB_clearScreen(egb_work);

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

    for (i = 1; i <=160; i++) {
        DWORD(para + 0)  = 1;                         //  パレット設定個数
        DWORD(para + 4)  = i;                         //  色識別番号
        BYTE( para + 8)  = col2b[i] * 8;              //  青の階調
        BYTE( para + 9)  = col2r[i] * 8;              //  赤の階調
        BYTE( para + 10) = col2g[i] * 8;              //  緑の階調
        BYTE( para + 11) = 0;
        EGB_palette( egb_work, 0, para);              //  パレット設定
    }

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }

    dr = 3.141592653589793 / 180.0;
    for (y1 = -1800; y1 <= 1800; y1 += 10) {
        y = y1 / 10.0;
        for (x1 = -1800; x1 < 1800; x1 += 2) {
            x = x1 / 10.0;
            r = dr * sqrt((double)x * (double)x + (double)y * (double)y);
            z = 100 * cos(r) - 30 * cos(3*r);
            sx = 80 + x / 3 - y / 6;
            sy = 40 - y / 6 - z / 4;
            sxi = (int)sx;
            if ((sxi < 0) || (sxi >= 160)) continue;
            if (d[sxi] <= sy) continue;

            zz = (int)((z + 100) * 0.035 / 6 * 159) + 1;

            if (zz > 160) zz = 160;
            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);

            EGB_color(egb_work, 0, zz);       //  色識別番号で前景色設定
            EGB_color(egb_work, 2, zz);       //  色識別番号で面塗色設定

//            WORD(para + 0) = px;
//            WORD(para + 2) = py;
//            WORD(para + 4) = px;
//            WORD(para + 6) = py;
//            EGB_rectangle(egb_work, para)

            WORD(para + 0) = 1;               //  座標点数
            WORD(para + 2) = px;              //  Ｘ座標
            WORD(para + 4) = py;              //  Ｙ座標
            EGB_pset(egb_work, para);         //  ポイント
            d[sxi] = sy;
        }
    }

    while (!kbhit());

    EGB_color(egb_work, 1, 0);                     //  色識別番号で背景色設定
    EGB_clearScreen(egb_work);

    return 0;
}
