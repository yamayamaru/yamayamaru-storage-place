/*
 *    ０．５ドットスクロールサンプル
 *
 *    注）setscrn.obj, gra.obj, io.objをリンクしてください
 */

#include <stdio.h>
#include <string.h>
#include "..\include\fmcfrb.h"              /* KYBD */
#include "..\include\egb.h"

#define SCREEN_WIDTH 320

extern void CRTC(), screenterm(), EGB_singleLine();
void waitVsync();
void leftHalfScroll();
void rightHalfScroll(char *egbwork);
void leftScroll(char *egbwork);
void rightScroll(char *egbwork);
void upScroll(char *egbwork);
void downScroll(char *egbwork);
unsigned int get_fa0Data();
unsigned int get_xScrollCnt();
void EGB_fill_rect01(char *egbwork, int x1, int y1, int x2, int y2, int col);

void main()
{
    unsigned int    ky, ene;
    int             i, c, endflag = 0;
    char            egbwork[1536];

    struct {
        short int    x, y, num;
        char         moji[64];
    } title = { 0, 16, 30, "０．５ドットスクロールサンプル" }, buf01;

    KYB_init();
    KYB_clic(1);

    EGB_init(egbwork, 1536);
    EGB_resolution(egbwork, 0, 10);
    EGB_resolution(egbwork, 1, 10);
    EGB_displayPage(egbwork, 1, 3);

    EGB_writePage(egbwork, 1);
    EGB_color(egbwork, 1, 0x8000);
    EGB_color(egbwork, 0, 0x7fff);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 1, 0, 0);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, SCREEN_WIDTH, 240);
    EGB_sjisString(egbwork, (char *)&title);

    for (i = 0; i < 320; i=i + 4) {
        EGB_singleLine(egbwork, i, 120, i, 239);
    }

    EGB_writePage(egbwork, 0);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 1, 0, 0);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, SCREEN_WIDTH, 240);

    c = 0;
    for (i = 0; i < SCREEN_WIDTH; i++) {
        EGB_color(egbwork, 0, c);
        EGB_singleLine(egbwork, i, 0, i, 239);
        c = c + 25;
    }

    ky = 0xffff;
    while (1) {        /* retで終了 */
        ky = 0xffff;
        while (ky == 0xffff) ky = KYB_read(1, &ene);
        switch (ky) {
            case 13:
                endflag = 1;
                break;
            case 28:
                waitVsync(egbwork);
                rightHalfScroll(egbwork);
                EGB_writePage(egbwork, 1);
                EGB_fill_rect01(egbwork, 0, 16, 8 * 15, 64, 0x8000);
                EGB_color(egbwork, 0, 0x7fff);
                sprintf(buf01.moji, "keycode=%d", ky);
                buf01.x = 0;
                buf01.y = 32;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "fa0Data=%d", get_fa0Data());
                buf01.x = 0;
                buf01.y = 48;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "xScrollCnt=%d", get_xScrollCnt());
                buf01.x = 0;
                buf01.y = 64;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                EGB_writePage(egbwork, 0);
                break;

            case 30:
                waitVsync(egbwork);
                upScroll(egbwork);
                EGB_writePage(egbwork, 1);
                EGB_fill_rect01(egbwork, 0, 16, 8 * 15, 64, 0x8000);
                EGB_color(egbwork, 0, 0x7fff);
                sprintf(buf01.moji, "keycode=%d", ky);
                buf01.x = 0;
                buf01.y = 32;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "fa0Data=%d", get_fa0Data());
                buf01.x = 0;
                buf01.y = 48;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "xScrollCnt=%d", get_xScrollCnt());
                buf01.x = 0;
                buf01.y = 64;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                EGB_writePage(egbwork, 0);
                break;

            case 31:
                waitVsync(egbwork);
                downScroll(egbwork);
                EGB_writePage(egbwork, 1);
                EGB_fill_rect01(egbwork, 0, 16, 8 * 15, 64, 0x8000);
                EGB_color(egbwork, 0, 0x7fff);
                sprintf(buf01.moji, "keycode=%d", ky);
                buf01.x = 0;
                buf01.y = 32;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "fa0Data=%d", get_fa0Data());
                buf01.x = 0;
                buf01.y = 48;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "xScrollCnt=%d", get_xScrollCnt());
                buf01.x = 0;
                buf01.y = 64;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                EGB_writePage(egbwork, 0);
                break;

            default:
                waitVsync(egbwork);
                leftHalfScroll(egbwork);
                EGB_writePage(egbwork, 1);
                EGB_fill_rect01(egbwork, 0, 16, 8 * 15, 64, 0x8000);
                EGB_color(egbwork, 0, 0x7fff);
                sprintf(buf01.moji, "keycode=%d", ky);
                buf01.x = 0;
                buf01.y = 32;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "fa0Data=%d", get_fa0Data());
                buf01.x = 0;
                buf01.y = 48;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                sprintf(buf01.moji, "xScrollCnt=%d", get_xScrollCnt());
                buf01.x = 0;
                buf01.y = 64;
                buf01.num = strlen(buf01.moji);
                EGB_sjisString(egbwork, (char *)&buf01);
                EGB_writePage(egbwork, 0);
                break;
        }
        if (endflag == 1) break;
    }

    screenterm(egbwork);
}


/*
 *    ０．５スクロールルーチン
 *
 */

#define fa0   17
#define haj0  18

int scroll_x_cnt = 0;
int scroll_y_cnt = 0;
/* haj0の初期値（この値は画面モードによって異なる。赤本参照） */
unsigned int fa0Data = 0;
unsigned int haj0Data = 0x8a;

void view01(char *egbwork)
{
    if (scroll_x_cnt == 1024) scroll_x_cnt = 0;
    scroll_x_cnt &= 0x3ff;
    scroll_y_cnt &= 0xff;
    fa0Data = (scroll_x_cnt / 4 + scroll_y_cnt * 256) & 0xffff;
    CRTC(fa0, fa0Data);
    CRTC(haj0, haj0Data - scroll_x_cnt % 4);
}

void leftHalfScroll(char *egbwork)
{
    scroll_x_cnt++;
    view01(egbwork);
}

void rightHalfScroll(char *egbwork)
{
    scroll_x_cnt--;
    view01(egbwork);
}

void leftScroll(char *egbwork)
{
    scroll_x_cnt += 2;
    view01(egbwork);
}

void rightScroll(char *egbwork)
{
    scroll_x_cnt -= 2;
    view01(egbwork);
}

void upScroll(char *egbwork)
{
    scroll_y_cnt += 1;
    view01(egbwork);
}

void downScroll(char *egbwork)
{
    scroll_y_cnt -= 1;
    view01(egbwork);
}

unsigned int get_fa0Data(void)
{
    return fa0Data;
}

unsigned int get_xScrollCnt(void)
{
    return (scroll_x_cnt % 4);
}
void waitVsync(char *egbwork)
{
    int    par = 0;
    EGB_palette(egbwork, 1, (char *)&par);
}

void EGB_fill_rect01(char *egbwork, int x1, int y1, int x2, int y2, int col)
{
    char para01[64];

    EGB_paintMode(egbwork, 0x20);  // 面塗モード設定
    EGB_color(egbwork, 0, col);
    EGB_color(egbwork, 2, col);    // 面塗色設定

    WORD(para01 + 0) = x1;
    WORD(para01 + 2) = y1;
    WORD(para01 + 4) = x2;
    WORD(para01 + 6) = y2;
    EGB_rectangle(egbwork, para01);

    EGB_color(egbwork, 0, 0x7fff);
    EGB_color(egbwork, 2, 0x7fff);
    EGB_paintMode(egbwork, 0x22);
}
