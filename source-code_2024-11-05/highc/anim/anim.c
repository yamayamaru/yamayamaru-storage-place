/*
 *    ３２０ｘ２４０ドット２５６色モードサンプル
 *
 *    注）setscrn.obj, gra.obj, io.objをリンクしてください
 */

#include <stdio.h>
#include <fmcfrb.h>              /* KYBD */
#include <egb.h>

extern screenterm(), wait(), EGB_singleLine(), palette();
void screenMake(), back(), tama(), kage(), tane(), disp();


void main()
{
    unsigned int    ky, ene;
    int    page, dr;

    char   egbwork[1536];

    screenMake(egbwork);
    KYB_init();
    /* パタパタアニメ */

    ky = 0xffff;  page = 0;  dr = 1;
    while (ky == 0xffff) {    /* キー入力で終了 */

        disp(egbwork, page);
        page = page + dr;
        if (page ==  4) { page = 2;  dr = -1; }
        if (page == -1) { page = 1;  dr =  1; }

        wait(0x120000);
        ky = KYB_read(1, &ene);
    }

    /* たねあかし */

    tane(egbwork);

    ky = KYB_read(0, &ene);

    screenterm(egbwork);

}

void screenMake(char *egbwork)
{
    int i;
    struct { short int x, y, num;  char moji[26]; }
        title = {0, 16, 26, "320ｘ240dot 256Color 画面" };

    EGB_init(egbwork, 1536);
    EGB_displayPage(egbwork, 0, 0);     /* 画面非表示 */
    EGB_resolution(egbwork, 0, 12);
    EGB_writePage(egbwork, 0);
    EGB_writeMode(egbwork, 0);
    EGB_color(egbwork, 1, 0);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 0, 0, 0);
    EGB_displayStart(egbwork, 1, 0, 0);
    EGB_displayStart(egbwork, 2, 1, 1);
    EGB_displayStart(egbwork, 3, 639, 479);

    for (i = 0; i < 256; i++) palette(egbwork, i, i, i, i);

    title.x = 0; title.y = 16;
    EGB_color(egbwork, 0, 255);  EGB_sjisString(egbwork, (char *)&title);
    back(egbwork, 0, 0);
    kage(egbwork, 0, 0, 10, 6);  tama(egbwork, 0, 0, 160, 120);

    title.x = 0; title.y = 256;
    EGB_color(egbwork, 0, 255);  EGB_sjisString(egbwork, (char *)&title);
    back(egbwork, 0, 240);
    kage(egbwork, 0, 240, 12, 7);  tama(egbwork, 0, 240, 160, 130);

    title.x = 320; title.y = 16;
    EGB_color(egbwork, 0, 255);  EGB_sjisString(egbwork, (char *)&title);
    back(egbwork, 320, 0);
    kage(egbwork, 320, 0, 13, 7);  tama(egbwork, 320, 0, 160, 160);

    title.x = 320; title.y = 256;
    EGB_color(egbwork, 0, 255);  EGB_sjisString(egbwork, (char *)&title);
    back(egbwork, 320, 240);
    kage(egbwork, 320, 240, 13, 8);  tama(egbwork, 320, 240, 160, 210);

    EGB_displayStart(egbwork, 2, 2, 2);      /* 画面の拡大 */
    EGB_displayPage(egbwork, 0, 1);          /* 画面の表示 */

}

void back(char *egbwork, int gx, int gy)
{
    int x, y;
    for (x = 0; x < 256; x++) {
        EGB_color(egbwork, 0, 255 - x);
        y = (int)((double)(x * x) * 0.003);
        EGB_singleLine(egbwork, x + gx + 64, gy + 200 - y, x + gx + 64, gy + 200);
    }
    for (y = 0; y < 38; y++) {
        EGB_color(egbwork, 0, 255 - (int)((double)(y * y) * 0.1));
        EGB_singleLine(egbwork, gx, gy + 201 + y, gx + 319, gy + 201 + y);
    }
}

void tama(char *egbwork, int gx, int gy, int x, int y)
{
    int i;
    struct { short x, y, r; } par;

    EGB_paintMode(egbwork, 0x22);
    for (i = 0; i < 15; i++) {
        EGB_color(egbwork, 0, 112 + i * 8);
        EGB_color(egbwork, 2, 112 + i * 8);
        par.x = gx + x - (i >> 2);
        par.y = gy + y - (i >> 2);
        par.r = 16 - i;
        EGB_circle(egbwork, (char *)&par);
    }
}

void kage(char *egbwork, int gx, int gy, int rx, int ry)
{
    struct { short x, y, rx, ry; } par;

    EGB_paintMode(egbwork, 0x22);
    EGB_color(egbwork, 0, 10);
    EGB_color(egbwork, 2, 10);
    par.x = 160 + gx;
    par.y = 228 + gy;
    par.rx = rx;
    par.ry = ry;

    EGB_ellipse(egbwork, (char *)&par);
}

void disp(char *egbwork, int sw)
{
    switch (sw) {
    case 0:
        EGB_displayStart(egbwork, 1, 0, 0);
        break;
    case 1:
        EGB_displayStart(egbwork, 1, 0, 240);
        break;
    case 2:
        EGB_displayStart(egbwork, 1, 320, 0);
        break;
    case 3:
        EGB_displayStart(egbwork, 1, 320, 240);
        break;
    }
}

void tane(char *egbwork)
{
    struct scn { short int x, y, num;  char moji[3]; };
    struct scn scn1 = {  25 * 8       ,   16,  3, "1  "};
    struct scn scn2 = {  25 * 8       ,  256,  3, "2  "};
    struct scn scn3 = {  320 + 25 * 8 ,   16,  3, "3  "};
    struct scn scn4 = {  320 + 25 * 8 ,  256,  3, "4  "};

    EGB_displayStart(egbwork, 1, 0, 0);
    EGB_displayStart(egbwork, 2, 1, 1);

    EGB_color(egbwork, 0, 255);
    EGB_sjisString(egbwork, (char *)&scn1);
    EGB_sjisString(egbwork, (char *)&scn2);
    EGB_sjisString(egbwork, (char *)&scn3);
    EGB_sjisString(egbwork, (char *)&scn4);
}
