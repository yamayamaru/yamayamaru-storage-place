/*
 *  画面設定
 *
 */

#include "..\include\egb.h"
extern CRTC(), palette();

/*  ＭＳ－ＤＯＳ画面設定 */

void screenterm(char *egbwork)
{
    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 1);
    EGB_resolution(egbwork, 1, 1);
    EGB_displayPage(egbwork, 1, 3);
    CRTC(0x18, 0x80);
    palette(egbwork, 7, 0xbf, 0xbf, 0xbf);
    return;
}

/* ハイスキャン画面設定 */

void NormalHigh(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 5);
    EGB_resolution(egbwork, 1, 5);

    EGB_writePage(egbwork, 1);     /* スプライト画面 */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* ＣＧ画面 */
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);
    EGB_displayPage(egbwork, 1, 3);

    return;

}

/* ロースキャン画面設定 */

void NormalLow(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 8);
    EGB_resolution(egbwork, 1, 8);

    EGB_writePage(egbwork, 1);    /* スプライト画面 */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);    /* ＣＧ画面 */
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;;

}

/* インターレース画面設定 */

void NormalVideo(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 7);
    EGB_resolution(egbwork, 1, 7);

    EGB_writePage(egbwork, 1);    /* スプライト画面 */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);    /* ＣＧ画面 */
    EGB_displayStart(egbwork, 2, 4, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;

}

/* ハイスキャン横長画面設定 */

void WideHigh(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 5);
    EGB_resolution(egbwork, 1, 5);

    EGB_writePage(egbwork, 1);     /* スプライト画面 */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* ＣＧ画面 */
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 256, 240);
    EGB_displayPage(egbwork, 1, 3);

    CRTC(0, 80);;                       /* hsw1 */
    CRTC(1, 590);                       /* hsw2 */
    CRTC(4, 669);                       /* hst */
    CRTC(29, 3);                        /* er1 */
    CRTC(9, 130);                       /* hds0 */ /* ＣＧ描画を横長にする */
    CRTC(18, 130);                      /* haj0 */
    CRTC(10, 642);                      /* hde0 */

    CRTC(11, 130);                      /* hds1 */ /* スプライトを横長にする */
    CRTC(22, 130);                      /* haj1 */
    CRTC(12, 642);                      /* hde1 */

    return;

}

/* ロースキャン横長画面設定 */

void WideLow(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 8);
    EGB_resolution(egbwork, 1, 8);

    EGB_writePage(egbwork, 1);    /* スプライト画面 */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* ＣＧ画面 */
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;

}

/* インターレース横長画面設定 */

void WideVideo(char *egbwork)
{

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 7);
    EGB_resolution(egbwork, 1, 7);

    EGB_writePage(egbwork, 1);    /* スプライト画面 */
    EGB_color(egbwork, 1, 0x8000);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayStart(egbwork, 3, 256, 240);

    EGB_writePage(egbwork, 0);     /* ＣＧ画面 */
    EGB_displayStart(egbwork, 2, 5, 1);
    EGB_displayPage(egbwork, 1, 3);
    EGB_displayStart(egbwork, 3, 256, 240);

    return;
}
