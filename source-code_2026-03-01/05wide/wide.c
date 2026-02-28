/*
 *    横長画面モードサンプル
 *
 *    注) setscrn.obj, gra.obj io.objをリンクしてください
 */

#include <stdio.h>
#include <stdlib.h>
#include <fmcfrb.h>     /* KYBO */
#include <egb.h>
#include <spr.h>

extern NormalHigh(), NormalLow(), NormalVideo();
extern WideHigh(), WideLow(), WideVideo(), screenterm(), EGB_singleLine();
extern wait();

char *setPat();
void spDef();

void main(int argc, char *argv[])
{
    unsigned int ky, ene;
    int   sx, sy, dx, dy;
    char  *title;
    char  egbwork[1538];

    struct ttl { short int x, y, num;  char moji[23]; };

    struct ttl title1 = {1,17,23,"横長画面モードサンプル"};
    struct ttl title2 = {1,17,23,"標準画面モードサンプル"};

    dx = 1;    dy = 1;
    sx = 80;
    sy = 120;

    if (argc == 1) {
        printf("パラメータがありません。\n");
        exit(1);
    }

    KYB_init();;
    KYB_clic(1);
    argv++;


    switch (atoi(*argv)) {
        case 0:
            NormalHigh(egbwork);
            title = (char *)&title2;
            break;
        case 1:
            WideHigh(egbwork);
            title = (char *)&title1;
            break;
        case 2:
            NormalLow(egbwork);
            title = (char *)&title2;
            break;
        case 3:
            WideLow(egbwork);
            title = (char *)&title1;
            break;
        case 4:
            NormalVideo(egbwork);
            title = (char *)&title2;
            break;
        case 5:
            WideVideo(egbwork);
            title = (char *)&title1;
            break;
        default:
            NormalHigh(egbwork);
            title = (char *)&title2;
            break;
    }

    EGB_writePage(egbwork, 0);     /* ＣＧ画面 */
    EGB_color(egbwork, 1, 4);      /* 背景は暗い青 */
    EGB_clearScreen(egbwork);
    EGB_color(egbwork, 0, 0x7fff);
    EGB_sjisString(egbwork, title);
    EGB_singleLine(egbwork, 0, 120, 256, 120);
    EGB_singleLine(egbwork, 128, 18, 128, 240);
    EGB_singleLine(egbwork, 0, 18, 0, 239);
    EGB_singleLine(egbwork, 0, 239, 255, 239);
    EGB_singleLine(egbwork, 255, 239, 255, 18);
    EGB_singleLine(egbwork, 255, 18, 0, 18);


    /* スプライト定義 */
    spDef();

    /* 表示開始  表示戸数は2個 */
    SPR_display(1, 2);

    ky = 0xffff;
    while (ky == 0xffff) {         /* キー入力で終了 */

        SPR_setPosition(0, 1023, 1, 1, sx, sy+18);
        SPR_setPosition(0, 1022, 1, 1, 240-sx, 206-sy+18);

        sx = sx + dx;
        sy = sy + dy;
        if (sx == 0 || sx == 240) dx = (dx > 0) ? -1:1;
        if (sy == 0 || sy == 206) dy = (dy > 0) ? -1:1;

        wait(0x2000);
        ky = KYB_read(1, &ene);
    }

    /* 表示終了 */
    SPR_display(0, 2);

    screenterm(egbwork);
}

void spDef()
{
    char    *Pat;

    SPR_init();    /* 初期化 */

    /* pattern dataを定義 */
    Pat = setPat();

    /* pattern dataをパターン番号128に設定 */
    SPR_define(1, 128, 1, 1, Pat);

    /* スプライト番号1023, 1022にパターン番号128を設定 */
    SPR_setAttribute(1023, 1, 1, 128, 0);
    SPR_setAttribute(1022, 1, 1, 128, 0);

}


char *setPat()
{
    static short pat[] = {
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x5fff, 0x5fff,
        0x5fff, 0x4fff, 0x4fff, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x5fff, 0x5fff, 0x6bff, 0x6bff,
        0x5fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x5fff, 0x6bff, 0x7fff, 0x73ff, 0x6bff,
        0x5fff, 0x57ff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x8000, 0x8000,
        0x8000, 0x8000, 0x5fff, 0x4fff, 0x73ff, 0x73ff, 0x6bff, 0x5fff,
        0x5fff, 0x57ff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x8000,
        0x8000, 0x8000, 0x5fff, 0x6bff, 0x6bff, 0x6bff, 0x5fff, 0x5fff,
        0x57ff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x8000,
        0x8000, 0x4fff, 0x5fff, 0x5fff, 0x5fff, 0x5fff, 0x5fff, 0x5fff,
        0x57ff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x3fff,
        0x8000, 0x4fff, 0x4fff, 0x57ff, 0x57ff, 0x57ff, 0x57ff, 0x4fff,
        0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x3fff,
        0x8000, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x3fff,
        0x8000, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x3fff, 0x3fff,
        0x8000, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x8000, 0x8000, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x2fff, 0x8009,
        0x8000, 0x8000, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x3fff, 0x2fff, 0x8000,
        0x8000, 0x8000, 0x8000, 0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x4fff, 0x4fff, 0x4fff, 0x4fff, 0x3fff, 0x2fff, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x4fff, 0x4fff, 0x4fff, 0x4fff,
        0x3fff, 0x3fff, 0x3fff, 0x3fff, 0x2fff, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x3fff, 0x3fff,
        0x3fff, 0x3fff, 0x3fff, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
        0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000
    };

    return (char *)pat;
}
