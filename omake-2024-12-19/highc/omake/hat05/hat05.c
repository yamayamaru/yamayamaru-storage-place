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

    EGB_init(egb_work, EgbWorkSize);              // EGB������
    EGB_resolution(egb_work, 0, 12);	          // 640�~480��� 256�F �y�[�W0
    EGB_resolution(egb_work, 1, 12);	          // 640�~480��� 256�F �y�[�W1
    EGB_writePage(egb_work, 0x0);                 // �������݃y�[�W�w��

    EGB_paintMode(egb_work, 0x22);                //  �ʓh���x�^�h��Ɏw��, ���E���x�^�h��Ɏw��(EGB_pset�͋��E���x�^�h��Ɏw�肷��)
    EGB_color(egb_work, 0, 255);                  //  �F���ʔԍ��őO�i�F�ݒ�
    EGB_color(egb_work, 1, 0);                    //  �F���ʔԍ��Ŕw�i�F�ݒ�

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
        DWORD(para + 0)  = 1;                         //  �p���b�g�ݒ��
        DWORD(para + 4)  = i;                         //  �F���ʔԍ�
        BYTE( para + 8)  = col2b[i] * 8;              //  �̊K��
        BYTE( para + 9)  = col2r[i] * 8;              //  �Ԃ̊K��
        BYTE( para + 10) = col2g[i] * 8;              //  �΂̊K��
        BYTE( para + 11) = 0;
        EGB_palette( egb_work, 0, para);              //  �p���b�g�ݒ�
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

            EGB_color(egb_work, 0, zz);       //  �F���ʔԍ��őO�i�F�ݒ�
            EGB_color(egb_work, 2, zz);       //  �F���ʔԍ��Ŗʓh�F�ݒ�

//            WORD(para + 0) = px;
//            WORD(para + 2) = py;
//            WORD(para + 4) = px;
//            WORD(para + 6) = py;
//            EGB_rectangle(egb_work, para)

            WORD(para + 0) = 1;               //  ���W�_��
            WORD(para + 2) = px;              //  �w���W
            WORD(para + 4) = py;              //  �x���W
            EGB_pset(egb_work, para);         //  �|�C���g
            d[sxi] = sy;
        }
    }

    while (!kbhit());

    EGB_color(egb_work, 1, 0);                     //  �F���ʔԍ��Ŕw�i�F�ݒ�
    EGB_clearScreen(egb_work);

    return 0;
}
