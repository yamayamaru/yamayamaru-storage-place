#include <stdio.h>
#include <stdlib.h>
#include <egb.h>
#include <dos.h>
#include <conio.h>


#define VsyncNotOccured        0
#define VsyncOccured          -1
#define Ylines               480
#define MaxBar          (640 / 8)
#define HByte          (1024 / 2)
#define PaintColor    0xffffffff

char egb_work[EgbWorkSize];
char para[64];

int vsync_init(_Handler handler);
int vsync_end(void);

volatile int vsyncflag = 0;

#pragma Calling_convention(_INTERRUPT|_CALLING_CONVENTION);
_Far void vsync_handler(void);
#pragma Calling_convention();

_Far unsigned char *vram0;

int main(int argc, char *argv[]) {
    int i, j, d;
    int save_intmask;

    EGB_init(egb_work, EgbWorkSize);
    EGB_resolution(egb_work, 0, 3) ;	         // 640×480画面 16色
    EGB_resolution(egb_work, 1, 3) ;
    EGB_displayPage(egb_work, 0, 1);             // page 0を前,   page 0を表示
    EGB_writePage(egb_work, 0) ;

    EGB_displayStart(egb_work, 2, 1, 1);         // 画面の拡大率 横1倍、縦1倍
    EGB_displayStart(egb_work, 3, 640, 480);     // 表示画面の大きさ 640x480
    EGB_displayStart(egb_work, 0, 0, 0);         // 表示開始位置   x = 0, y = 0
    EGB_displayStart(egb_work, 1, 0, 0);         // 仮想画面の移動 x = 0, y = 0

    EGB_paintMode(egb_work, 0x22);               //  面塗をベタ塗りに指定, 境界をベタ塗りに指定(EGB_psetは境界をベタ塗りに指定する)
    EGB_color(egb_work, 0, 15);                  //  色識別番号で前景色設定
    EGB_color(egb_work, 1, 0);                   //  色識別番号で背景色設定
    EGB_color(egb_work, 2, 15);                  //  色識別番号で面塗色設定

    EGB_clearScreen(egb_work);


    _FP_SEG(vram0) = 0x0104;                     // 640x480 16色 : 0x0104  640x480 256色 : 0x0104  320x240 32768色 : 0x010c  512x480 32768色 : 0x0104
    _FP_OFF(vram0) = 0;

    save_intmask = vsync_init(vsync_handler);


    for (j = 0; j < MaxBar; j++) {
        while (vsyncflag != VsyncOccured);
        d = j * 4;
        for (i = 0; i < Ylines; i++) {
            *((_Far unsigned int *)(vram0 + d)) = PaintColor;
            d += HByte;
        }
        vsyncflag = VsyncNotOccured;
    }

    vsync_end();

    EGB_color(egb_work, 1, 0);                     //  色識別番号で背景色設定
    EGB_clearScreen(egb_work);

    return 0;
}


#define VsyncNumber  0x4b
#define VSYNCclear 0x05ca
#define IMR_S      0x0012
#define IMR_M      0x0002
#define OCW1_S     0x0012
#define OCW2_M     0x0000
#define OCW2_S     0x0010
#define EOIcommand   0x20          // 0x2 = 001：非指定EOI
#define EOIcommand_S 0x63          // 0x6 = 011：指定EOI（優先度回転しない）    0x3 = 0x0b - 0x08  (vsyncは0x0b slaveの0x03)
#define EOIcommand_M 0x67          // 0x6 = 011：指定EOI（優先度回転しない）    0x3 = 0x0b - 0x08  (vsyncは0x0b slaveの0x03)
#define CLI        _inline(0xfa)
#define STI        _inline(0xfb)
#define PicWait    _inline(0xe6, 0x6c, 0xf5, 0xf5)


void vsync_run(void) {
    // VSYNC割り込み原因をクリアする(VSYNC割り込み原因クリアレジスタに値は何でもいいので書き込めばクリアされる)
    _outp(VSYNCclear, 0);


    // ハンドラのメイン処理
    vsyncflag = VsyncOccured;


    // End Of Interruptコマンドの書き込み (非指定EOI)
    _outp(OCW2_S, EOIcommand);
   PicWait;
    _outp(OCW2_M, EOIcommand);
}

#pragma Calling_convention(_INTERRUPT|_CALLING_CONVENTION);
_Far void vsync_handler(void) {

    CLI;

    _inline(0x6a, 0x14);      // push   14h
    _inline(0x1f);            // pop    ds

    _inline(0x0f, 0xa8);      // push gs
    _inline(0x0f, 0xa0);      // push fs

    vsync_run();

    _inline(0x0f, 0xa1);      // pop fs
    _inline(0x0f, 0xa9);      // pop gs
}
#pragma Calling_convention();


_real_int_handler_t saver;
_Handler savep = 0;
unsigned char saveIMR_M;
unsigned char saveIMR_S;

int vsync_init(_Handler handler) {

   int intno = VsyncNumber;

   saver = _getrvect(intno);  // 元のベクタを得る。
   savep = _getpvect(intno);


    // 両ベクタを386ネイティブモードハンドラに設定する。

   if (_setrpvectp(intno, handler) == -1) {
      return -1;
   }

   CLI;

   saveIMR_M = _inp(IMR_M);
   PicWait;
   saveIMR_S = _inp(IMR_S);
   PicWait;

   _outp(OCW1_S, saveIMR_S & 0xF7);                // 0xF7 = 0b11110111 : slave VSYNC 割り込み有効にする(0:割り込み有効 1:割り込み無効)

   STI;

   return ~((saveIMR_S << 8) | saveIMR_M);
}

int vsync_end(void) {
   unsigned char byte01;
   int intno = VsyncNumber;
   int ret01;

   CLI;

   byte01 = _inp(IMR_S) | (saveIMR_S & 0x08);     // 0x08 = 0b00001000 : slave VSYNC 割り込み無効にする(0:割り込み有効 1:割り込み無効)
   PicWait;

   _outp(OCW1_S, byte01);

   STI;

   ret01 = 0;
   // リアルモードハンドラを復元する。
   if (_setrvect(intno, saver) == -1) {
       ret01 = -1;
   }

   // 386ネイティブモードハンドラを復元する。
   if (_setpvect(intno, savep) == -1) {
       ret01 = -2;
   }

   return ret01;
}
