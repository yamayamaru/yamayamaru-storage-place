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

    EGB_init(egb_work, EgbWorkSize);              // EGB������
    EGB_resolution(egb_work, 0, 12);	          // 640�~480��� 256�F �y�[�W0
    EGB_resolution(egb_work, 1, 12);	          // 640�~480��� 256�F �y�[�W1
    EGB_writePage(egb_work, 0x0);                 // �������݃y�[�W�w��

    EGB_paintMode(egb_work, 0x22);                //  �ʓh���x�^�h��Ɏw��, ���E���x�^�h��Ɏw��(EGB_pset�͋��E���x�^�h��Ɏw�肷��)
    EGB_color(egb_work, 0, 255);                  //  �F���ʔԍ��őO�i�F�ݒ�
    EGB_color(egb_work, 1, 0);                    //  �F���ʔԍ��Ŕw�i�F�ݒ�

    EGB_clearScreen(egb_work);

    for (i = 1; i <= 8; i++) {
        DWORD(para + 0)  = 1;                         //  �p���b�g�ݒ��
        DWORD(para + 4)  = i;                         //  �F���ʔԍ�
        BYTE( para + 8)  = ((i & 1)) * 255;           //  �̊K��
        BYTE( para + 9)  = ((i & 2) >> 1) * 255;      //  �Ԃ̊K��
        BYTE( para + 10) = ((i & 4) >> 2) * 255;      //  �΂̊K��
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
                EGB_color(egb_work, 0, zz);       //  �F���ʔԍ��őO�i�F�ݒ�
                EGB_color(egb_work, 2, zz);       //  �F���ʔԍ��Ŗʓh�F�ݒ�

                WORD(para + 0) = px;
                WORD(para + 2) = py;
                WORD(para + 4) = px + 3;
                WORD(para + 6) = py + 3;
                EGB_rectangle(egb_work, para);

//                WORD(para + 0) = 1;               //  ���W�_��
//                WORD(para + 2) = px;              //  �w���W
//                WORD(para + 4) = py;              //  �x���W
//                EGB_pset(egb_work, para);         //  �|�C���g
            }

            d[sxi] = sy;
        }
    }

    while (!kbhit());

    EGB_color(egb_work, 1, 0);                     //  �F���ʔԍ��Ŕw�i�F�ݒ�
    EGB_clearScreen(egb_work);

    return 0;
}
