/************************************************************************/
/*       JPEGファイルを読み込みます                                     */
/*                                                                      */
/*      使用法  RUN386 TJPGREAD sample.jpg                              */
/*      コンパイルするにはJPEGサポートライブラリが必要です             */
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xld.h>
#include "jpeg.h"

#include <egb.h>
#include <conio.h>
#include <dos.h>

static int put_data();
static int get_data();

FILE   *ifp     ;                 /* jpegfileファイルポインタ           */

int work01[EgbWorkSize];
#define VRAM_SCREEN_WIDTH   (512)
#define VRAM_SCREEN_HIGHT   (256)
#define SCREEN_WIDTH        (320)
#define SCREEN_HEIGHT       (240)
void draw_pixel_vram_16bit(int x, int y, int color);
void fillRect(int x1, int y1, int x2, int y2, int color);
void fillScreen(int color);
int ginit(int mode, int page, char *workaddr01, int worksize01);

int pic_width01;
int pic_height01;
double ratio01x = 1.0;
double ratio01y = 1.0;
double ratio01d = 1.0;
int    ratio01 = 65536;

void main(int argc, char **argv){
        JPEGCard   JPCard   ;     /*  JPEGカード情報格納ポインタ        */
        int        Huffman  ;     /*  ハフマンテーブル設定指示          */
        int        dsize    ;     /*  ﾛｰﾄﾞﾊﾞﾌｧｻｲｽﾞ                      */
        char       *dbuf    ;     /*  ﾛｰﾄﾞﾊﾞｯﾌｧ へのﾎﾟｲﾝﾀ               */
        int        width    ;     /*  画像サイズ (横)                   */
        int        height   ;     /*  画像サイズ (縦)                   */
        long       subs     ;     /*  ｻﾌﾞｻﾝﾌﾟﾘﾝｸﾞ                       */
        int        color    ;     /*  色情報                            */
        long       SOIofs   ;     /*  SOI ｵﾌｾｯﾄ                         */
        int        len      ;     /*  作業領域サイズ                    */
        int        swidth   ;     /*  展開バッファ横ドット数            */
        int        sline    ;     /*  展開ライン数                      */
        char       *cbuf    ;     /*  作業バッファポインタ              */
        int        bpp      ;     /*  bit/pixel                         */
        char       *sbuf    ;     /*  展開バッファへのポインタ          */

        int        ret01;

        if ((ret01 = ginit(10, 0, (char *)work01, EgbWorkSize)) != 0) {
             puts("ginit failed!");
             while(!kbhit());
             exit(1);
        }

        fillScreen(0);

        /*==============================================================*/
        /*  ダイナミックリンク対応                                      */
        /*==============================================================*/
        _XLD_init(0);                  /*  DLL初期設定                  */
        _XLD_setMemFunc(malloc, free); /*  DLLメモリ関数の登録          */
        _XLD_setLinkPath(".\\");       /*  DLL検索パスの設定            */
        /*==============================================================*/
        /*  処理ファイルのオープン(JFIF)                                */
        /*==============================================================*/
        if ((ifp = fopen(argv[1], "rb")) == NULL)
                exit(-1);
        /*==============================================================*/
        /*  JPEG圧縮初期化                                              */
        /*==============================================================*/
        Huffman = 0 ;
        JPEG_loadInit(&JPCard, Huffman);
        /*==============================================================*/
        /*  データバッファ操作関数の登録                                */
        /*==============================================================*/
        JPEG_setLoadFunc(put_data, get_data);
        /*==============================================================*/
        /*  ロードバッファの設定                                        */
        /*==============================================================*/
        dsize = 512 ;
        dbuf  = malloc(dsize);

        JPEG_setLoadBuffer(dbuf, dsize);
        /*==============================================================*/
        /*  JFIF情報の獲得                                              */
        /*==============================================================*/
        SOIofs = 0;
        JPEG_getSOF(&width, &height, &subs, &color, SOIofs);
        /*==============================================================*/
        /*  作業領域の獲得                                              */
        /*==============================================================*/
        bpp = 16 ;
        len = JPEG_loadBufSize(bpp, width, height, subs, color);

        sline = 16;
        cbuf = malloc(len);
        sbuf = malloc(2*width*sline);
        swidth = width ;
        /*==============================================================*/
        /*  伸長処理                                                    */
        /*==============================================================*/
        pic_width01 = width;
        pic_height01 = height;
        ratio01x = (double)SCREEN_WIDTH  / (double)pic_width01;
        ratio01y = (double)SCREEN_HEIGHT / (double)pic_height01;
        ratio01d = (ratio01x < ratio01y) ? ratio01x : ratio01y;
        if (ratio01d > 1.0) ratio01d = 1.0;
        ratio01 = ratio01d * 65536;

        JPEG_loadImageJFIF(bpp, width, height, 1, subs, sbuf, swidth,
                            sline, cbuf, color, SOIofs, &JPCard, NULL);
        /*==============================================================*/
        /*                                                              */
        /*==============================================================*/
        fclose(ifp);
        free(cbuf);
        free(sbuf);
        free(dbuf);
        /*==============================================================*/
        /*  JPEG圧縮終了                                                */
        /*==============================================================*/
        JPEG_loadEnd(&JPCard);

        getch();
        fillScreen(0);
}

/************************************************************************/
/*                                                                      */
/*  get_data                                                            */
/*                                                                      */
/************************************************************************/
static int get_data(char *buf, long bsize, long ofs){
        fseek(ifp, ofs, SEEK_SET);
        fread(buf, 1, bsize, ifp);
        return(0 );
}

/************************************************************************/
/*                                                                      */
/*  put_data                                                            */
/*                                                                      */
/************************************************************************/
static int put_data(char *buf, int ofs, int line){
        int i, j, count01, old_line01, line01;
        short *wbuf01;
        wbuf01 = (short *)buf;
        count01 = 0;
        old_line01 = 0;

        for (j = 0; j < line; j++) {
            line01 = ((ofs + j) * ratio01) >> 16;
            if (old_line01 != line01) {
                for (i = 0; i < pic_width01; i++) {
                    old_line01 = line01;
                    draw_pixel_vram_16bit((i * ratio01) >> 16, line01, wbuf01[count01++]);
                }
            } else {
                count01 += pic_width01;
            }
        }

        return(0);
}


_Far short *vram01;
char *workaddr;
int  worksize;
int ginit(int mode, int page, char *workaddr01, int worksize01) {
    char ret01;

    _FP_SEG(vram01) = 0x0104;               // 16,32768color = 0x0104    256color = 0x010c
    _FP_OFF(vram01) = 0x00000000;

    workaddr = workaddr01;
    worksize = worksize01;
    
    ret01 = EGB_init(workaddr, worksize);                 // EGB初期化
    EGB_resolution(workaddr, 0, mode);            // 画面モード ページ0
    EGB_resolution(workaddr, 1, mode);            // 画面モード ページ1
    EGB_writePage(workaddr, page);                // 書き込みページ指定

    EGB_displayStart(workaddr, 2, 2, 2);  /* 画面の拡大率 横2倍、縦2倍 */
    EGB_displayStart(workaddr, 3, 320, 240);    /* 表示画面の大きさ 320x240 */
    EGB_displayStart(workaddr, 0, 0, 0);  /* 表示開始位置 x = 0, y = 0 */
    EGB_displayStart(workaddr, 1, 0, 0);  /* 仮想画面の移動 x = 0, y = 0 */

    return ret01;
}

extern _Far short *vram01;
void draw_pixel_vram_16bit(int x, int y, int color) {
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    *(vram01 + (y * VRAM_SCREEN_WIDTH + x)) = (short)color;
}

void fillRect(int x1, int y1, int x2, int y2, int color) {
    int xx1, yy1, xx2, yy2, i, j;

    if (x1 <= x2) {
        xx1 = x1;
        xx2 = x2;
    } else {
        xx1 = x2;
        xx2 = x1;
    }
    if (y1 <= y2) {
        yy1 = y1;
        yy2 = y2;
    } else {
        yy1 = y2;
        yy2 = y1;
    }

    for (j = yy1; j <= yy2; j++) {
        for (i = xx1; i <= xx2; i++) {
            draw_pixel_vram_16bit(i, j, color);
        }
    }
}

void fillScreen(int color) {
    fillRect(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, color);
}
