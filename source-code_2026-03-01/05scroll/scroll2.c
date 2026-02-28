/*
 *    ０．５ドットスクロールサンプル
 *
 *    注）setscrn.obj, gra.obj, io.objをリンクしてください
 */

#include <stdio.h>
#include <string.h>
#include <fmcfrb.h>              /* KYBD */
#include <egb.h>

extern void CRTC(), screenterm(), EGB_singleLine();
void waitVsync();
void leftHalfScroll();
void rightHalfScroll(char *egbwork);
void leftScroll(char *egbwork);
void rightScroll(char *egbwork);

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
    EGB_displayStart(egbwork, 3, 320, 240);
    EGB_sjisString(egbwork, (char *)&title);

    for (i = 0; i < 320; i=i + 4) {
        EGB_singleLine(egbwork, i, 120, i, 239);
    }

    EGB_writePage(egbwork, 0);
    EGB_clearScreen(egbwork);
    EGB_displayStart(egbwork, 1, 0, 0);
    EGB_displayStart(egbwork, 2, 2, 2);
    EGB_displayStart(egbwork, 3, 320, 240);

    c = 0;
    for (i = 0; i < 320; i++) {
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
                sprintf(buf01.moji, "keycode=%d", ky);
                buf01.x = 0;
                buf01.y = 32;
                buf01.num = strlen(buf01.moji);
                waitVsync(egbwork);
                rightHalfScroll(egbwork);
                EGB_writePage(egbwork, 1);
                {
                    char para01[64];
                    EGB_paintMode(egbwork, 0x20);// 面塗モード設定
                    EGB_color(egbwork, 0, 0x8000);
                    EGB_color(egbwork, 2, 0x8000);    // 面塗色設定
                    WORD(para01 + 0) = 0;
                    WORD(para01 + 2) = 16;
                    WORD(para01 + 4) = 8 * 32;
                    WORD(para01 + 6) = 32;
                    EGB_rectangle(egbwork, para01);
                    EGB_color(egbwork, 2, 0x7fff);
                    EGB_paintMode(egbwork, 0x22 );
                }
                EGB_color(egbwork, 0, 0x7fff);
                EGB_sjisString(egbwork, (char *)&buf01);
                EGB_writePage(egbwork, 0);
                break;

            default:
                sprintf(buf01.moji, "keycode=%d", ky);
                buf01.x = 0;
                buf01.y = 32;
                buf01.num = strlen(buf01.moji);
                waitVsync(egbwork);
                leftHalfScroll(egbwork);
                EGB_writePage(egbwork, 1);
                {
                    char para01[64];
                    EGB_paintMode(egbwork, 0x20);// 面塗モード設定
                    EGB_color(egbwork, 0, 0x8000);
                    EGB_color(egbwork, 2, 0x8000);    // 面塗色設定
                    WORD(para01 + 0) = 0;
                    WORD(para01 + 2) = 16;
                    WORD(para01 + 4) = 8 * 32;
                    WORD(para01 + 6) = 32;
                    EGB_rectangle(egbwork, para01);
                    EGB_color(egbwork, 2, 0x7fff);
                    EGB_paintMode(egbwork, 0x22 );
                }
                EGB_color(egbwork, 0, 0x7fff);
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

int cnt = 0;
/* haj0の初期値（この値は画面モードによって異なる。赤本参照） */
int haj0Data = 0x8a;

void view01(char *egbwork)
{
    if (cnt == 1024) cnt = 0;
    cnt &= 0x03ff;
    CRTC(fa0, cnt / 4);
    CRTC(haj0, haj0Data - cnt % 4);
}

void leftHalfScroll(char *egbwork)
{
    cnt++;
    view01(egbwork);
}

void rightHalfScroll(char *egbwork)
{
    cnt--;
    view01(egbwork);
}

void leftScroll(char *egbwork)
{
    cnt += 2;
    view01(egbwork);
}

void rightScroll(char *egbwork)
{
    cnt -= 2;
    view01(egbwork);
}

void waitVsync(char *egbwork)
{
    int    par = 0;
    EGB_palette(egbwork, 1, (char *)&par);
}
