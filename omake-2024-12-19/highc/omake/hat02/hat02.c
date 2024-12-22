#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "..\..\include\egb.h"

double d[160];

char egb_work[EgbWorkSize];
char para[128];
int main(int argc, char *argv[]) {
    int       i, x1, y1, zz, px, py, f, sxi, col;
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

    for (i = 1; i <= 8; i++) {
        DWORD(para + 0)  = 1;                         //  パレット設定個数
        DWORD(para + 4)  = i;                         //  色識別番号
        BYTE( para + 8)  = ((i & 1)) * 255;           //  青の階調
        BYTE( para + 9)  = ((i & 2) >> 1) * 255;      //  赤の階調
        BYTE( para + 10) = ((i & 4) >> 2) * 255;      //  緑の階調
        BYTE( para + 11) = 0;
        EGB_palette( egb_work, 0, para);
    }

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }

    dr = 3.141592653589793 / 180.0;
    for (y1 = -1800; y1 <= 1800; y1 += 60) {
        y = y1 / 10.0;
        for (x1 = -1800; x1 < 1800; x1 += 20) {
            x = x1 / 10.0;
            r = dr * sqrt((double)x * (double)x + (double)y * (double)y);
            z = 100 * cos(r) - 30 * cos(3*r);
            sx = 80 + x / 3 - y / 6;
            sy = 40 - y / 6 - z / 4;
            sxi = (int)sx;
            if ((sxi < 0) || (sxi >= 160)) continue;
            if (d[sxi] <= sy) continue;

            zz = (int)((z + 100) * 0.035) + 1;
            f = 0;
            col = 0;

            if ((zz == 1) || (zz == 3) || (zz == 5) || (zz == 7)) {
                f = 1;
                col = col + 1;
            };
            if ((zz == 2) || (zz == 3) || (zz >= 6)) {
                f = 1;
                col = col + 2;
            };
            if (zz >= 4) {
                f = 1;
                col = col + 4;
            };
            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);

            if (f == 1) {
                EGB_color(egb_work, 0, zz);       //  色識別番号で前景色設定
                EGB_color(egb_work, 2, zz);       //  色識別番号で面塗色設定

                WORD(para + 0) = px;
                WORD(para + 2) = py;
                WORD(para + 4) = px + 3;
                WORD(para + 6) = py + 3;
                EGB_rectangle(egb_work, para);

//                WORD(para + 0) = 1;               //  座標点数
//                WORD(para + 2) = px;              //  Ｘ座標
//                WORD(para + 4) = py;              //  Ｙ座標
//                EGB_pset(egb_work, para);         //  ポイント
            }

            d[sxi] = sy;
        }
    }

    while (!kbhit());

    EGB_color(egb_work, 1, 0);                     //  色識別番号で背景色設定
    EGB_clearScreen(egb_work);

    return 0;
}
