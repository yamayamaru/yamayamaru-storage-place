/*
 *    スプライト画面のグラフィック化サンプル
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fmcfrb.h>
#include <egb.h>
#include <spr.h>

extern NormalHigh(), screenterm(), wait(), EGB_singleLine(), point(), pict(), geta();


char *setPat();
char *setHead();
void spDef(), trans(), save();


void main(int argc, char *argv[])
{
    unsigned int  ky, ene;
    int    sx, sy, dx, dy;

    char *fname;
    char *egbwork[1536];

    struct ttl { short int x, y, num; char moji[32]; };
    struct ttl title = {1, 17, 32, "スプライト画面のグラフィック化  "};

    dx = 1;    dy = 1;
    sx = 60;
    sy = 120;

    if (argc == 1) {
        printf("ファイル名がありません\n");
        exit(1);
    }

    KYB_init();
    KYB_clic(1);
    *argv++;    fname = *argv;

    NormalHigh(egbwork);

    EGB_writePage(egbwork, 0);          /* ＣＧ画面 */
    EGB_color(egbwork, 1, 4);           /* 背景は暗い青 */
    EGB_clearScreen(egbwork);
    EGB_color(egbwork, 0,0x7fff);
    EGB_sjisString(egbwork, (char *)&title);
    EGB_singleLine(egbwork, 0, 120, 256, 120);
    EGB_singleLine(egbwork, 128, 18, 128, 240);
    EGB_singleLine(egbwork, 0, 18, 0, 239);
    EGB_singleLine(egbwork, 0, 239, 255, 239);
    EGB_singleLine(egbwork, 255, 239, 255, 18);
    EGB_singleLine(egbwork, 255, 18, 0, 18);


    /* スプライト定義 */
    spDef();

    /* 表示開始  表示個数は２個 */
    SPR_display(1, 2);

    ky = 0xffff;
    while (ky == 0xffff) {

        SPR_setPosition(0, 1023, 1, 1, sx, sy+18);
        SPR_setPosition(0, 1022, 1, 1, 240-sx, 206-sy+18);

        sx = sx + dx;
        sy = sy + dy;
        if (sx == 0 || sx == 240) dx = (dx > 0) ? -1: 1;
        if (sy == 0 || sy == 206) dy = (dy > 0) ? -1: 1;

        wait(0x2000);
        ky = KYB_read(1, &ene);
    }

    /* 表示終了 */
    SPR_display(0, 2);

    /* 画面の転送とセーブ */
    trans(egbwork);
    save(egbwork, fname);

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

    /* スプライト番号 1023, 1022にパターン番号128を設定 */
    SPR_setAttribute(1023, 1, 1, 128, 0);
    SPR_setAttribute(1022, 1, 1, 128, 0);


}

void trans(char *egbwork)
{
    int    c, x, y;
    for (y = 0; y < 240; y++) {
        for (x = 0; x < 256; x++) {
            EGB_writePage(egbwork, 1);
            c = point(egbwork, x, y);
            if ((c & 0x8000) == 0) {
                EGB_writePage(egbwork, 0);   /* ＣＧ画面 */
                pict(egbwork, x, y, c);
            }
        }
    }
    return;
}

void save(char *egbwork, char *fname)
{
    int    i;
    static short saveArea[256*256-1];

    char   c;
    char   *Header;
    char   *Data;
    FILE   *fp;

    EGB_writePage(egbwork, 0);     /* ＣＧ画面 */
    geta(egbwork, saveArea, 0, 0, 255, 239);

    fp = fopen(fname, "wb");
    if (fp == 0) {
        screenterm(egbwork);
        printf("%s をオープンすることができません\n", fname);
        exit(1);
    }

    Header = setHead();
    Data   = (char *)saveArea;

    for (i = 0; i < 0x100; i++) {
        c = *Header++;
        putc(c, fp);
    }

    for (i = 0; i < 256*240*2; i++) {
        c = *Data++;
        putc(c, fp);
    }

    return;

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


char *setHead()
{
    static char TiffHeader1[] = {
        0x49, 0x49, 0x2a, 0x00, 0x08, 0x00, 0x00, 0x00,
        0x0f, 0x00, 0xff, 0x00, 0x03, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x01, 0x01, 0x03, 0x00, 0x01, 0x00,
        0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x02, 0x01,
        0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00,
        0x00, 0x00, 0x03, 0x01, 0x03, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05, 0x01,
        0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x0a, 0x01, 0x03, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x11, 0x01,
        0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x12, 0x01, 0x03, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x15, 0x01,
        0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x18, 0x01, 0x03, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x01,
        0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff, 0x7f,
        0x00, 0x00, 0x1a, 0x01, 0x05, 0x00, 0x01, 0x00,
        0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x1b, 0x01,
        0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0xf8, 0x00,
        0x00, 0x00, 0x1c, 0x01, 0x03, 0x00, 0x01, 0x00,
        0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x4b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
        0x4b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
    };

    return (char *)TiffHeader1;
}
