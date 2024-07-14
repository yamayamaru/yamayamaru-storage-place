// https://i-satoh.hatenablog.com/entry/65438175
// ここのサイトのMotionJPEGの再生プログラムを参考にして
// Linuxのフレームバッファで動画再生のテストをしてみました。
// 動画のみで音声は再生されません
//
// 今回のはPC9821の256色パックドモード用です
// 実行にはOpenWatcom C Ver2.0のDOSエクステンダーDOS4GWを使います
// DOSエクステンダーを使うことで1MB以上のメモリへのアクセスが可能になり、
// PC9821の256色パックドモードの場合、VRAMを0x00F00000に出現させることができるので、
// 512KBのVRAMへのリニアアクセスが可能になります
//
// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder
// のJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cを参考にして手を加えました
// botmerさんがメンテナンスされているものがあるようですがかなり手を加えられているのでそれでは動作しません
// また、botmerさんがメンテナンスされているものはRGB565専用になってるようなので使いません
//
// HDDに保存されたMotionJPEGの動画を表示するものです。
// 動画は映像のみの再生です、音声は再生されません。
// 長時間動画の再生はできてます。
//
// このプログラムには動画ファイルは付属しません
// 255行目あたりのloop関数の中身を編集してご自分の動画ファイルを再生するように変更してください
// (拡張子は大文字の.MJPにしてください)
//
// 対応している動画形式はMotionJPEGです。
// 拡張子は大文字半角の.MJPで保存してください。
// MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
// ffmpeg -i 'test01.mp4' -b:v 750k -r 10.0 -s 320x240 -vcodec mjpeg -an V4_10FPS.mov
// ffmpeg -i 'test02.mp4' -b:v 125k -r 5.0   -s 120x90 -vcodec mjpeg -an V5_05FPS.mov
// ffmpeg -i 'test03.mp4' -b:v 200k -r 15.0  -s 120x90 -vcodec mjpeg -an V6_15FPS.mov
// ffmpeg -i 'test04.mp4' -b:v 140k -r 8.0   -s 120x90 -vcodec mjpeg -an V7_08FPS.mov
// ffmpeg -i 'test05.mp4' -b:v 200k -r 12.0  -s 120x90 -vcodec mjpeg -an V8_08FPS.mov
// 拡張子は大文字の.MJPで保存してください。
//
// これらのプログラムの使用に当たってはご自分の責任において使用してください
// これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません






#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <i86.h>
#include <dos.h>
#include <malloc.h>
#include <conio.h>


#define _16MB_CHECK_ 1
#define _HSYNC_ 0

int data_address01;

// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoderのJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cに手を加えたものを使用しています
// https://i-satoh.hatenablog.com/entry/65438175
#include "JPEGDecoder/JPEGDecoder.h"



int work01[256*1024/4];

#define VRAM_SCREEN_WIDTH   (640)
#define VRAM_SCREEN_HIGHT   (400)
#define SCREEN_WIDTH        (640)
#define SCREEN_HEIGHT       (400)
#define VRAM_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)

typedef union X_{
    struct{
        unsigned short l;
        unsigned short h;
    }w;
    unsigned long x;
}X;

typedef struct DPMI_MEM_{
    unsigned int selecter;
    unsigned int p_address;        // Physical Address
    unsigned int p_size;        // Physical Address Size
    unsigned int l_address;        // Linear Address
}DPMI_MEM;

#if _HSYNC_
static int Hmode;
#endif


#define BLACK  0

#define WEB216_PALETTE_BLACK      0xd7     ///< 0x0000    0,   0,   0
#define WEB216_PALETTE_NAVY       0xda     ///< 0x0010    0,   0, 128
#define WEB216_PALETTE_DARKGREEN  0xdc     ///< 0x0400    0, 128,   0
#define WEB216_PALETTE_DARKCYAN   0xdb     ///< 0x0410    0, 125, 123
#define WEB216_PALETTE_MAROON     0xdf     ///< 0x8000  128,   0,   0
#define WEB216_PALETTE_PURPLE     0xde     ///< 0x8010  128,   0, 128
#define WEB216_PALETTE_OLIVE      0xdd     ///< 0x8400  128, 128,   0
#define WEB216_PALETTE_LIGHTGREY  0xd9     ///< 0xc618  198, 195, 198
#define WEB216_PALETTE_DARKGREY   0xd8     ///< 0x8410  128, 128, 128
#define WEB216_PALETTE_BLUE       0xd2     ///< 0x001f    0,   0, 255
#define WEB216_PALETTE_GREEN      0xd2     ///< 0x07e0    0, 255,   0
#define WEB216_PALETTE_CYAN       0xb4     ///< 0x07ff    0, 255, 255
#define WEB216_PALETTE_RED        0x23     ///< 0xf800  255,   0,   0
#define WEB216_PALETTE_MAGENTA    0x1e     ///< 0xf81f  255,   0, 255
#define WEB216_PALETTE_YELLOW     0x05     ///< 0xffe0  255, 255,   0
#define WEB216_PALETTE_WHITE      0x00     ///< 0xffff  255, 255, 255
#define WEB216_PALETTE_ORANGE     0x11     ///< 0xfcc0  255, 152,   0
#define WEB216_PALETTE_GREENYELLOW  0x4c   ///< 0x9fe6  152, 255,  48
#define WEB216_PALETTE_PINK       0x13     ///< 0xfb59  255, 104, 200

#define _width   SCREEN_WIDTH
#define _height  SCREEN_HEIGHT

#define DEFFRATE     10  // default frame rate

struct rgb01 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct rgb01 color01(uint32_t color) {
    struct rgb01 col;
    col.r = (color >> 16) & 0xff;
    col.g = (color >> 8) & 0xff;
    col.b = (color) & 0xff;
    return col;
}

int dpmi_simulate_real_mode_interrupt(int int_no, int copy_stack_data_size, struct simulate_real_mode_interrupt_regs *regs, unsigned int regs_seg);
int dpmi_allocate_dos_memory_block(int alloc_size, unsigned int *real_seg, unsigned int *prot_selector);
int dpmi_free_dos_memory_block(int prot_selector);
int dpmi_resize_dos_memory_block(int new_alloc_size, int prot_selector);
int init_timer(int interval, unsigned int funcseg, unsigned int funcaddr, void far *prot_func_addr, unsigned int prot_selector01);
void close_timer();
long long millis01(); 
long long time_us_64();
void delay01(unsigned int millis1);
extern long long far *timer_count_data01;

char *VRAM;


inline static void dmaWait(void);

bool drawJpgBmppicture(char *fname);
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate);
int kbhit01(void);

void set_palette(int no,int red,int green,int blue);
void pc9821_on(void);
void pc9821_off();
void dpmi_vram_off(DPMI_MEM *dm);
int dpmi_vram_on(DPMI_MEM *dm);
extern int get_ds(void);

#define drawPixel draw_pixel
void draw_pixel_fast(int x, int y, int color);
void draw_pixel(int x, int y, int color);

void clear_text(void);
void put_vram_char(int x, int y, int ch, int attri);

void fillScreen(int color);
void fillRect(int x1, int y1, int x2, int y2, int color);
void resetPushColor(int x1, int y1, int x2, int y2);
void pushColor(uint32_t color);
void pushColors(uint8_t *data, int32_t size);
int color_web216(int color);

extern const uint8_t palette256_web216[];

void loop();
int main(int argc, char *argv[]) {
    unsigned char *VVRAM;
    int i,j,x,y,my_ds;
    float palk;
    int ret;
    DPMI_MEM dm;
    int ret01;
    unsigned int seg01, prot_selector01;
    long long count0001 = 0;

#if _16MB_CHECK_
    if( (inp(0x043b)&0x04) ){
        puts("16MBシステム空間の設定を変更して下さい。");
        return 0;
    }
#endif

    memset(&dm,0,sizeof(DPMI_MEM));
    dm.selecter  = 0xFFFFFFFF;
    dm.p_address = 0x00F00000;    // 16[MB] System Area
    dm.p_size    = 0x00080000;    // VRAM 512[KB]
    dm.l_address = 0xFFFFFFFF;

    pc9821_on();
    ret = dpmi_vram_on(&dm);
    my_ds = get_ds();


    if(dpmi_allocate_dos_memory_block(65520 / 16, &seg01, &prot_selector01) != 0) {
        printf( "dpmi_allocate_dos_memory_block failed\n" );
        return 0;
    }


    init_timer(1, seg01, 0x0000, (void far *)MK_FP(prot_selector01, 0x0000), prot_selector01);


    VRAM = (char *)dm.l_address;
    printf("リニアVRAMアドレス=0x%08x\n", dm.l_address);

    for (i = 0; i < 256; i++) {
      set_palette(i, palette256_web216[i * 4 + 2], palette256_web216[i * 4 + 1], palette256_web216[i * 4 + 0]);
    }

    clear_text();
    fillScreen(WEB216_PALETTE_BLACK);
    delay(100);

    for(;;) {
        loop();
        if (kbhit01()) break;
        delay(1000);
    }

    close_timer();

    if (dpmi_free_dos_memory_block(prot_selector01) != 0) {
        printf( "dpmi_free_dos_memory_block failed\n" );
    } else {
//        printf( "dpmi_free_dos_memory_block success\n" );
    }

    dpmi_vram_off(&dm);
    pc9821_off();

    return 0;
}

void loop() {
    // drawJpgBmppicture_mjpeg(動画ファイル名, FPS)で指定します。
    // MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
    // ffmpeg -i 'test01.mp4' -b:v 750k -r 10.0 -s 320x240 -vcodec mjpeg -an V4_10FPS.mov
    // ffmpeg -i 'test02.mp4' -b:v 125k -r 5.0   -s 120x90 -vcodec mjpeg -an V5_05FPS.mov
    // ffmpeg -i 'test03.mp4' -b:v 200k -r 15.0  -s 120x90 -vcodec mjpeg -an V6_15FPS.mov
    // ffmpeg -i 'test04.mp4' -b:v 140k -r 8.0   -s 120x90 -vcodec mjpeg -an V7_08FPS.mov
    // ffmpeg -i 'test05.mp4' -b:v 200k -r 12.0  -s 120x90 -vcodec mjpeg -an V8_08FPS.mov
    // このプログラムはMotionJPEGのファイルの拡張子は.MJPとして認識するので拡張子を.MJPに変更してください。
    // JPEG画像やBMP画像の表示もできますが、表示されない場合は一度Windows付属のペイントで保存してから試してみてください。
    // 下記のように動画ファイル名とFPSを指定してください

    fillScreen(WEB216_PALETTE_BLACK);
    drawJpgBmppicture_mjpeg("SNAIL01.JPG", 0);
    delay(1000);
    drawJpgBmppicture_mjpeg("SCENIC01.BMP", 0);
    delay(1000);

/*
    fillScreen(WEB216_PALETTE_BLUE);
    drawJpgBmppicture_mjpeg("test_o1.MJP", 10);
    delay(5000);

    fillScreen(WEB216_PALETTE_BLUE );
    drawJpgBmppicture_mjpeg("test_o2.MJP", 5);
    delay(5000);

    fillScreen(WEB216_PALETTE_BLUE);
    drawJpgBmppicture_mjpeg("test_o3.MJP", 15);
    delay(5000);
*/

    fillScreen(WEB216_PALETTE_BLACK);

    drawJpgBmppicture_mjpeg("V8_08FPS.MJP", 12);
    delay(5000);
}




// Jpeg/Motion-Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate) {
  // variables
  bool rlt = false;
  FILE *fp;
  unsigned long nxtdt;
  char *ext;

  // check BMP/JPEG file
  ext = strrchr(fname, '.');
  // if(!ext || (memcmp(ext + 1, "BMP", 3) && memcmp(ext + 1, "JPG", 3) && memcmp(ext + 1, "MJP", 3))) return false;
  if (!ext) {
    fillScreen(WEB216_PALETTE_PURPLE);
    return false;
  }
  if (!(!memcmp(ext + 1, "BMP", 3) || !memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3))) {
    fillScreen(WEB216_PALETTE_YELLOW);
    return false;
  }
  // check jpeg file
  if(!memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3)){
    // check & set mottion jpeg
    if(!memcmp(ext + 1, "MJP", 3)) {
      JpegDec.mjpegflag = 1;
    } else {
      JpegDec.mjpegflag = 0;
    }

    // variables
    uint8 *pImg;
    int x, y, bx, by;
    struct rgb01 col = color01(BLACK);
    bool inifrmset = true;

    // draw picture
    
    long long time01 = 0LL;
    long long time02 = 0LL;
    long long time00 = millis01();
    double fps_aver = 0.0;
    int count_fps = 0;
    
    do{
      if (kbhit01()) break;

      time02 = time01;
      time01 = millis01();
      if (time02 > 0LL) {
        double time1 = (time01 - time02) / 1000.0;
        double fps01 = 1.0 / time1;
        fps_aver += fps01;
        count_fps++;
        if (((millis01() - time00) / 1000.0) > 5.0) {
          if(count_fps > 0) {
            printf("time = %.4f\r\n", time1);
            printf("fps = %.4f\r\n", fps_aver / count_fps);
            fps_aver = 0.0;
            count_fps = 0;
          }
          time00 = millis01();
        }
      }

      // set next picture draw timing
      if(JpegDec.mjpegflag) nxtdt = millis01() + 1000 / ((frate > 0) ? frate : DEFFRATE);
      // check decording parameter
      if((JpegDec.decode(fname, 0) < 0) || !JpegDec.height || !JpegDec.width) return false;
      // set draw offset parameters
      long xofset = (_width - JpegDec.width) / 2;
      long yofset = (_height - JpegDec.height) / 2;
      if(inifrmset){
        // set top black belt
        dmaWait();
        if(yofset > 0) fillRect(0, 0, _width, yofset, color_web216(BLACK));
        // set left & right black belt
        if(xofset > 0){
          fillRect(0, (yofset > 0) ? yofset : 0, xofset, (yofset > 0) ? JpegDec.height : _height, color_web216(BLACK));
          fillRect(xofset + JpegDec.width, (yofset > 0) ? yofset : 0, _width - JpegDec.width - xofset, (yofset > 0) ? JpegDec.height : _height, color_web216(BLACK));
        }
        inifrmset = false;
      }
      // start jpeg decode
//      struct rgb01 clbuf[JpegDec.MCUWidth * JpegDec.MCUHeight];
      struct rgb01 *clbuf;
      clbuf = (struct rgb01 *)malloc(JpegDec.MCUWidth * JpegDec.MCUHeight * sizeof(struct rgb01));
      uint16_t clbufpt = 0;
      while(JpegDec.read()){
        // check draw range and get draw parameters
        pImg = JpegDec.pImage ;
        long dxstrt = JpegDec.MCUx * JpegDec.MCUWidth + xofset; if(dxstrt >= _width) continue;
        if(dxstrt < 0) dxstrt = 0;
        long dystrt = JpegDec.MCUy * JpegDec.MCUHeight + yofset; if(dystrt >= _height) continue;
        if(dystrt < 0) dystrt = 0;
        long dxend = (JpegDec.MCUx + 1) * JpegDec.MCUWidth - 1 + xofset; if(dxend < 0) continue;
        if(dxend >= _width) dxend = _width - 1;
        long dyend = (JpegDec.MCUy + 1) * JpegDec.MCUHeight - 1 + yofset; if(dyend < 0) continue;
        if(dyend >= _height) dyend = _height - 1;
        // draw princture
        bool drawstart = true;
        
        clbufpt = 0;
        for(by = 0 ; by < JpegDec.MCUHeight ; by++){
          // draw decoded image
          y = JpegDec.MCUy * JpegDec.MCUHeight + by;
          // check image end
          if(y >= JpegDec.height) break;
          if(((y + yofset) < 0) || ((y + yofset) >= _height)) continue;
          // draw image
          for(bx = 0 ; bx < JpegDec.MCUWidth ; bx++){
            x = JpegDec.MCUx * JpegDec.MCUWidth + bx;
            // check image draw range
            if(((x + xofset) < 0) || ((x + xofset) >= _width)) continue;
            // start drawing
            if(drawstart){
              // set draw window

              resetPushColor(dxstrt, dystrt, dxend, dyend);
              
              // clear draw start flag
              drawstart = false;
            }
            // get & set pixel data
            if(JpegDec.comps == 1) { // Grayscale
              col.r = pImg[0]; col.g = pImg[0]; col.b = pImg[0];
            } else {// RGB
              col.r = pImg[0]; col.g = pImg[1]; col.b = pImg[2];
            }
            // check picture draw range & set color
            if(x >= JpegDec.width) col = color01(BLACK);
            if(((xofset + x) >= 0) && ((yofset + y) >= 0) && ((xofset + x) < _width) && ((yofset + y) < _height))   clbuf[clbufpt++] = col;
            pImg += JpegDec.comps;
          }
        }
        // draw picture
        if(clbufpt) pushColors((uint8_t *)clbuf, clbufpt);
      }

      free(clbuf);

      // set bottom black belt
      if(yofset > 0) fillRect(0, yofset + JpegDec.height, _width, _height - (yofset + JpegDec.height), color_web216(BLACK));
      // check next picture wait timing
      if(JpegDec.mjpegflag){
        long picdly = nxtdt - millis01();
        if(picdly > 0) {
//            printf("picdly = %ld\n", picdly);
            delay01(picdly);
        }
      }
      //
    } while(JpegDec.mjpegflag && JpegDec.nextjpgpos);
    // end of drawing
    rlt = true;

  // check bitmap file
  } else {
    // draw BMP file
    // check file header
    #define RDPLEN   256   // read packets length
    unsigned char rdbuf[RDPLEN * 3];
	size_t ret01;
    fp = fopen(fname, "rb");
    ret01 = fread(rdbuf, 1, 34, fp);
    if((rdbuf[0] != 'B') || (rdbuf[1] != 'M')) goto rtnend;  // check ID
    if(rdbuf[30] || rdbuf[31] || rdbuf[32] || rdbuf[33]) goto rtnend;  // check no compressin
    if((rdbuf[28] | ((unsigned short)rdbuf[29] << 8)) != 24) goto rtnend;  // check 24bits mode
    // get file parameters
    unsigned long lwidth = rdbuf[18] | ((unsigned long)rdbuf[19] << 8) |
                           ((unsigned long)rdbuf[20] << 16) | ((unsigned long)rdbuf[21] << 24);
    unsigned long lheight = rdbuf[22] | ((unsigned long)rdbuf[23] << 8) |
                           ((unsigned long)rdbuf[24] << 16) | ((unsigned long)rdbuf[25] << 24);
    unsigned long offset = rdbuf[10] | ((unsigned long)rdbuf[11] << 8) |
                           ((unsigned long)rdbuf[12] << 16) | ((unsigned long)rdbuf[13] << 24);
    int drwidth = (lwidth <= _width) ? lwidth : _width;
    int drhight = (lheight <= _height) ? lheight : _height;
    int drxofset = (_width - lwidth) / 2;
    int dryofset = (_height - lheight) / 2;
    unsigned long lpwidth = (lwidth * 3) + ((((lwidth * 3) % 4) > 0) ? 4 - ((lwidth * 3) % 4) : 0);

    // draw screen
    struct rgb01 blkblt[_width];
	memset(blkblt, 0, sizeof(blkblt));
    struct rgb01 blkblt2[_width];
    for(int y = 0 ; y < _height ; y++){
      // draw blank line
      if((_height > lheight) && ((y < dryofset) || (y >= (dryofset + drhight)))){

        resetPushColor(0, y, _width-1, y);
        pushColors((uint8_t *)blkblt, _width);
        continue;
      }
      // draw y line picture
      for(int ly = 0 ; ly < drhight ; ly++){
        // draw x line
        for(int x = 0 ; x < _width ; x++){
          // draw left & right blank
          if((_width > lwidth) && ((x < drxofset) || (x >= (drxofset + drwidth)))){
            // pushColor(BLACK);
            blkblt2[x] = color01(BLACK);
            continue;
          }
          // draw x line picture
          unsigned long rowidx = (lheight - ly - 1 + ((dryofset < 0) ? dryofset : 0)) * lpwidth +
                                                 ((drxofset < 0) ? -drxofset * 3 : 0);
          fseek(fp, rowidx + offset, SEEK_SET);
          for(int lx = 0 ; lx < drwidth ; lx += RDPLEN){
            int rdpklen = ((lx + RDPLEN) < drwidth) ? RDPLEN : drwidth - lx;
            if(fread(rdbuf, (size_t)1, rdpklen * 3, fp) <= 0) goto rtnend;
//            struct rgb01 clbuf[rdpklen];
            for(int ln = 0 ; ln < rdpklen ; ln++) {
//              clbuf[ln].r = rdbuf[ln * 3 + 2];
//              clbuf[ln].g = rdbuf[ln * 3 + 1];
//              clbuf[ln].b = rdbuf[ln * 3];
              blkblt2[x + lx + ln].r = rdbuf[ln * 3 + 2];
              blkblt2[x + lx + ln].g = rdbuf[ln * 3 + 1];
              blkblt2[x + lx + ln].b = rdbuf[ln * 3 + 0];
            }
          }
          x += (drwidth - 1);
        }
        dmaWait();

        resetPushColor(0, y + ly, _width-1, y + ly);
        pushColors((uint8_t *)blkblt2, _width);

      }
      y += (drhight - 1);
    }
    rlt = true;
  }

  // process end
 rtnend:
  if(!memcmp(ext + 1, "BMP", 3)) fclose(fp);
  return rlt;
}

// Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture(char *fname) {
  return drawJpgBmppicture_mjpeg(fname, 10);
}

int pushColor_x1, pushColor_y1, pushColor_x2, pushColor_y2, pushColor_left;
void resetPushColor(int x1, int y1, int x2, int y2) {
    pushColor_x1 = x1;
    pushColor_left = x1;
    pushColor_y1 = y1;
    pushColor_x2 = x2;
    pushColor_y2 = y2;
}

void pushColor(uint32_t color) {

    drawPixel(pushColor_x1, pushColor_y1, color_web216(color));
	pushColor_x1++;
	if (pushColor_x1 == pushColor_x1 + 1) {
        pushColor_y1++;
		pushColor_x1 = 0;
	}
}

void pushColors(uint8_t *data, int32_t size) {
    uint8_t  *p;
    int      color01;

    p = data;
	for (int i = 0; i < size; i++) {
            // color01 = ((int)*p++) << 16 | ((int)*p++) << 8 | ((int)*p++);
            // translate web216 color
            color01 = 215 - ((((int)*p++) / 51) * 36 + (((int)*p++) / 51) * 6 + (((int)*p++) / 51));
	    drawPixel(pushColor_x1, pushColor_y1, color01);
	    pushColor_x1++;
	    if (pushColor_x1 == pushColor_x2 + 1) {
                pushColor_y1++;
                pushColor_x1 = pushColor_left;
	    }
	}
}

inline static void dmaWait(void) {
}

int kbhit01(void) {
    union REGS regs;
    regs.h.ah = 1;
#ifdef __386__
    int386(0x18, &regs, &regs);
#else
    int86(0x18, &regs, &regs);
#endif
    return regs.h.bh;
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

    for (j = y1; j <= y2; j++) {
        for (i = x1; i <= x2; i++) {
            drawPixel(i, j, color);
        }
    }
}

void fillScreen(int color) {
    fillRect(0, 0, _width-1, _height-1, color);
}

int color_web216(int color) {
    int r, g, b, color01;
    r = (color >> 16) / 51;            // 255 / 5 = 51
    g = (color >> 8) / 51;
    b = (color) / 51;
    color01 = 215 - (r * 36 + g * 6 + b);
    return color01;
}

#define TEXT_VRAM  0xa0000UL
#define TEXT_VRAM_ATTRI  0xa2000UL
/*
  アトリビュート
    bit0: 表示  1:表示 0:非表示
    bit1: 点滅  1:点滅 0:通常
    bit2: 反転  1:反転 0:通常
    bit3: アンダーライン     1:アンダーライン 0:通常
    bit4: バーティカルライン 1:縦線           0:通常
    bit5: 青
    bit6: 赤
    bit7; 緑
*/
void put_vram_char(int x, int y, int ch, int attri) {
    char *p01;
    unsigned int d;
    d = 2 * (x + y * 80);
    p01 = (char *)TEXT_VRAM;
    *(p01 + d) = (char)ch;            // 半角キャラクタのみ;
    *(p01 + d + 1) = 0;
    p01 = (char *)TEXT_VRAM_ATTRI;
    *(p01 + d) = (char)attri;
    *(p01 + d + 1) = (char)0;
}

void clear_text(void) {
    int i, j;
    for (j = 0; j < 40; j++) {
        for (i = 0; i < 80; i++) {
            put_vram_char(i, j, 0x20, 0xe1);
        }
    }
}

void draw_pixel_fast(int x, int y, int color)
{
    VRAM[y * _width + x]=(char)color;
}


void draw_pixel(int x, int y, int color) {
    if (x >= _width || x < 0) return ;
    if (y >=_height || y < 0) return ;
    VRAM[y * _width + x]=(char)color;
}


void set_palette(int no,int red,int green,int blue) {
    outp(0xa8,no);
    outp(0xaa,green);
    outp(0xac,red);
    outp(0xae,blue);
}

void pc9821_on(void)
{
/*
#if _HSYNC_
    // HSYNC -> 31.47 KHz 
    if( (inp(0x09a8) & 0x01) == 0){
        outp(0x09a8,0x01);
        Hmode = 1;
    }
#endif
*/

    // 256 color mode
    outp(0x6a,0x07);
    outp(0x6a,0x21);
    outp(0x6a,0x06);

    // Packed Pixel
    *((char *)0xe0100) = 0x00;

    // 16[MB] System Area -> VRAM ON
    *((char *)0xe0102) = 0x01;

    // Graphics Start
    outp(0xa2,0x0d);
}

void pc9821_off()
{
    // 16[MB] System Area -> VRAM OFF
    *((char *)0xe0102) = 0x00;

    // 16 Color (standard) mode
    outp(0x6a,0x07);
    outp(0x6a,0x20);
    outp(0x6a,0x06);

/*
#if _HSYNC_
    // HSYNC -> 24.8? KHz
    if(Hmode==1)
        outp(0x09a8,0x00);
#endif
*/

    // TEXT Start
    outp(0x62,0x0d);

}

void dpmi_vram_off(DPMI_MEM *dm)
{
    union REGS r;
    X tmp;

    if(dm->selecter != 0xFFFFFFFF){
        r.x.eax = 0x0001;        // DPMI Function
        r.x.ebx = dm->selecter;
        int386(0x31, &r, &r);
        if( r.x.cflag ){
            // Failed
            puts("ディスクリプタを開放できませんでした。");
        } else {
            dm->selecter = 0xFFFFFFFF;
        }
    }

    if(dm->l_address != 0xFFFFFFFF){
        r.x.eax = 0x0801;        // DPMI Function
        tmp.x = dm->l_address;    // BX:CX = メモリの物理アドレス
        r.x.ebx = tmp.w.h;
        r.x.ecx = tmp.w.l;

        int386(0x31, &r, &r);
        if( r.x.cflag ){
            // Failed
            puts("物理アドレスのマッピングの解除ができませんでした。");
        } else {
            dm->l_address = 0xFFFFFFFF;
        }
    }
}

int dpmi_vram_on(DPMI_MEM *dm)
{
    union REGS r;
    X tmp;

    // ・ディスクリプタの割り当て
    r.x.eax = 0x0000;        // DPMI Function
    r.x.ecx = 1;            // アロケートするディスクリプタの数
    int386(0x31, &r, &r);
    if( r.x.cflag ){
        // Failed
        puts("ディスクリプタをアロケートできませんでした。");
        return(0);
    }
    // アロケートされたセレクタ
    dm->selecter = r.x.eax;


    // ・物理アドレスのマッピング
    r.x.eax = 0x0800;        // DPMI Function
    tmp.x = dm->p_address;    // BX:CX = メモリの物理アドレス
    r.x.ebx = tmp.w.h;
    r.x.ecx = tmp.w.l;
    tmp.x = dm->p_size;        // SI:DI = マップする領域のバイト数
    r.x.esi = tmp.w.h;
    r.x.edi = tmp.w.l;
    int386(0x31, &r, &r);
    if( r.x.cflag ){
        // Failed
        puts("物理アドレスをマッピングできませんでした。");
        return(0);
    }
    // physical address mapping -> Linear Address
    tmp.w.h = r.x.ebx;
    tmp.w.l = r.x.ecx;
    dm->l_address = tmp.x;


    // ・セグメントのベースアドレスを設定
    r.x.eax = 0x0007;        // DPMI Function
    r.x.ebx = dm->selecter;        // セレクタ
    // CX:DX = セグメントに対する新しい32ビットリニアベースアドレス
    r.x.ecx = tmp.w.h;
    r.x.edx = tmp.w.l;
    int386(0x31, &r, &r);
    if( r.x.cflag ){
        // Failed
        puts("ベースアドレスを設定できませんでした。");
        return(0);
    }

    // ・セグメントのリミットを設定
    r.x.eax = 0x0008;        // DPMI Function
    r.x.ebx = dm->selecter;        // セレクタ
    tmp.x = dm->p_size;        // リミットサイズ
    r.x.ecx = tmp.w.h;
    r.x.edx = tmp.w.l;
    int386(0x31, &r, &r);
    if( r.x.cflag ){
        // Failed
        puts("リミットを設定できませんでした。");
        return(0);
    }

    return(1);
}

extern int get_ds(void);
#pragma aux get_ds = \
           "mov       ax, ds" \
           "mov       eax, 0ffffh" \
           value      [eax] \
           modify     [eax];



struct simulate_real_mode_interrupt_regs {
    unsigned int   edi;
    unsigned int   esi;
    unsigned int   ebp;
    unsigned int   zero;
    unsigned int   ebx;
    unsigned int   edx;
    unsigned int   ecx;
    unsigned int   eax;
    unsigned short flags;
    unsigned short es;
    unsigned short ds;
    unsigned short fs;
    unsigned short gs;
    unsigned short ip;
    unsigned short cs;
    unsigned short sp;
    unsigned short ss;
};

// timer bios handler(16bit real mode code)
char timer_func_code01[256] = {
          0x06, 0x1e, 0x66, 0x50, 0x66, 0x53, 0x66, 0x51,
          0x66, 0x52, 0x66, 0x57, 0x66, 0x56, 0x66, 0x55,

          0x0e, 0x07, 0x0e, 0x1f,
          0x66, 0xba, 0x00, 0x00, 0x00, 0x00,
          0x66, 0xb8, 0x01, 0x00, 0x00, 0x00,
          0x66, 0x03, 0x06, 0x00, 0x10,
          0x66, 0x13, 0x16, 0x04, 0x10, 0x66, 0xa3, 0x00, 0x10,
          0x66, 0x89, 0x16, 0x04, 0x10, 0x66, 0xa1, 0x08, 0x10,
          0x66, 0x83, 0xf8, 0x00, 0x74, 0x0e,
          0xb4, 0x02, 0x8b, 0x0e, 0x0c, 0x10,
          0x0e, 0x07, 0x8d, 0x1e, 0x00, 0x00, 0xcd, 0x1c,

          0x66, 0x5d, 0x66, 0x5e, 0x66, 0x5f, 0x66, 0x5a,
          0x66, 0x59, 0x66, 0x5b, 0x66, 0x58, 0x1f, 0x07, 0xcf
};

int second(void) {
    return ((double)(*timer_count_data01) / 100.0);
}

long long time_us_64() {
    return ((*timer_count_data01) * 10 * 1000);
}

long long millis01() {
    return ((*timer_count_data01) * 10);
}

void delay01(unsigned int millis1) {
    long long t1;
    t1 = millis01();
    while ((millis01() - t1) <= (long long)millis1);
}

long long far *timer_count_data01;
unsigned int timer_func_seg01;
unsigned int timer_func_addr01;
unsigned char far *timer_prot_func_addr01;
unsigned int far *timer_func_flag01;
unsigned int far *timer_func_seg02;
unsigned int far *timer_func_seg03;
unsigned int far *timer_interval01;
// interval : 10msec * interval
int init_timer(int interval, unsigned int funcseg, unsigned int funcaddr, void far *prot_func_addr, unsigned int prot_selector01) {
    struct simulate_real_mode_interrupt_regs regs01;
    unsigned char far *p;
    int       i;
    FILE      *fp01;

    timer_func_seg01 = funcseg;
    timer_func_addr01 = funcaddr;
    timer_prot_func_addr01 = (unsigned char far *)prot_func_addr;
    p = timer_prot_func_addr01;

    timer_count_data01 = (long long far *)((void far *)MK_FP(prot_selector01, 0x1000));
    *timer_count_data01 = 0;
    timer_func_flag01 = (unsigned int far *)((void far *)MK_FP(prot_selector01, 0x1008));
    *timer_func_flag01 = 5;

    timer_interval01 = (unsigned int far *)((void far *)MK_FP(prot_selector01, 0x100c));
    *timer_interval01 = interval;

    timer_func_seg02  = (unsigned int far *)((void far *)MK_FP(prot_selector01, 0x1010));
    *timer_func_seg02 = funcseg;
    timer_func_seg03   = (unsigned int far *)((void far *)MK_FP(prot_selector01, 0x1014));

/*
    if ((fp01 = fopen("count01.com", "rb")) == NULL) {
        printf("cannot open count_timer_prog01.com\n");
    }
*/
    for (i = 0; i < 92; i++) {
        *p++ = timer_func_code01[i];
//        *p++ = fgetc(fp01);
    }
/*
    fclose(fp01);
*/
    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x02 << 8;
    regs01.ecx = 10;
    regs01.ebx = timer_func_addr01;
    regs01.es  = timer_func_seg01;
    regs01.ds  = timer_func_seg01;
    dpmi_simulate_real_mode_interrupt(0x1c, 16, &regs01, 0);

    return 0;
}

void close_timer() {
    *timer_func_flag01 = 0;
    delay(100);
}


/* alloc_size : block size in (16-byte) paragraphs */
/*
error code:
0007H	memory control blocks damaged (also returned by DPMI 0.9 hosts)
0008H	insufficient memory (also returned by DPMI 0.9 hosts).
8011H	descriptor unavailable
*/
int dpmi_allocate_dos_memory_block(int alloc_size, unsigned int *real_seg, unsigned int *prot_selector) {
    unsigned short real_seg01, prot_selector01;
    int ret01;
    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp

        mov     ebx, alloc_size
        mov     eax, 0100h
        int     31h
        jnc      dpmi_allocate_dos_memory_block_jmp01
        mov     ret01, eax                    ; failed
        jmp     dpmi_allocate_dos_memory_block_jmp02

dpmi_allocate_dos_memory_block_jmp01:         ; success
        mov     real_seg01, ax
        mov     prot_selector01, dx
        xor     eax, eax
        mov     ret01, eax

dpmi_allocate_dos_memory_block_jmp02:
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    *real_seg = (int)real_seg01;
    *prot_selector = (int)prot_selector01;
    return ret01;
}

/*
error code:
0007H	memory control blocks damaged (also returned by DPMI 0.9 hosts).
0009H	incorrect memory segment specified (also returned by DPMI 0.9 hosts).
8022H	invalid selector
*/
int dpmi_free_dos_memory_block(int prot_selector) {
    unsigned short prot_selector01;
    int ret01;
    prot_selector01 = (unsigned short)prot_selector;
    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp

        mov     dx, prot_selector01;
        mov     eax, 0101h
        int     31h
        jnc     dpmi_free_dos_memory_block_jmp01
        mov     ret01, eax                    ; failed
        jmp     dpmi_free_dos_memory_block_jmp02

dpmi_free_dos_memory_block_jmp01:             ; success
        xor     eax, eax
        mov     ret01, eax

dpmi_free_dos_memory_block_jmp02:
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    return ret01;
}

/*
error code :
0007H	memory control blocks damaged (also returned by DPMI 0.9 hosts).
0008H	insufficient memory (also returned by DPMI 0.9 hosts).
0009H	incorrect memory segment specified (also returned by DPMI 0.9 hosts).
8011H	descriptor unavailable
8022H	invalid selector

BX = maximum possible block size (paragraphs)
*/
int dpmi_resize_dos_memory_block(int new_alloc_size, int prot_selector) {
    unsigned short prot_selector01;
    int ret01;
    prot_selector01 = (unsigned short)prot_selector;

    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp

        mov     ebx, new_alloc_size
        mov     dx, prot_selector01;
        mov     eax, 0102h
        int     31h
        jnc     dpmi_resize_dos_memory_block_jmp01
        mov     ret01, eax                      ; failed
        jmp     dpmi_resize_dos_memory_block_jmp02

dpmi_resize_dos_memory_block_jmp01:             ; success
        xor     eax, eax
        mov     ret01, eax

dpmi_resize_dos_memory_block_jmp02:
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    return ret01;
}

/*
if regs_seg == 0 then es = ds
else                  es = regs_seg

error code :
8012H	linear memory unavailable (stack)
8013H	physical memory unavailable (stack)
8014H	backing store unavailable (stack)
8021H	invalid value (CX too large)
*/
int dpmi_simulate_real_mode_interrupt(int int_no, int copy_stack_data_size, struct simulate_real_mode_interrupt_regs *regs, unsigned int regs_seg) {
    int ret01;
    char int_no01;
    unsigned short es01;
    unsigned int addr01;

    es01 = (unsigned short)regs_seg;
    int_no01 = (char)int_no;
    regs->zero = 0;

    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs

        mov	ax, es01             ;if (es01 == 0) es = ds else es = es01;
        cmp     ax, 0
        jz      simulate_real_mode_interrupt_jmp03
        mov	ax, es01
        mov     es, ax
        jmp     simulate_real_mode_interrupt_jmp04
simulate_real_mode_interrupt_jmp03:
	push	ds
	pop	es
simulate_real_mode_interrupt_jmp04:

        mov     eax, 0300h
        mov     bl, int_no01
        mov     bh, 0
        mov     ecx, copy_stack_data_size
        mov     edi, regs
        int     31h
        jnc     simulate_real_mode_interrupt_jmp01
        mov     ret01, eax                ; failed

        jmp     simulate_real_mode_interrupt_jmp02
simulate_real_mode_interrupt_jmp01:       ; success
        mov     ax, es
        mov     es01, ax
        mov     addr01, edi
        xor     eax, eax
        mov     ret01, eax

simulate_real_mode_interrupt_jmp02:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

    }

    return ret01;
}



const uint8_t palette256_web216[] = {
    0xff,0xff,0xff,0x00,0xcc,0xff,0xff,0x00,0x99,0xff,0xff,0x00,0x66,0xff,0xff,0x00,0x33,0xff,0xff,0x00,
    0x00,0xff,0xff,0x00,0xff,0xcc,0xff,0x00,0xcc,0xcc,0xff,0x00,0x99,0xcc,0xff,0x00,0x66,0xcc,0xff,0x00,
    0x33,0xcc,0xff,0x00,0x00,0xcc,0xff,0x00,0xff,0x99,0xff,0x00,0xcc,0x99,0xff,0x00,0x99,0x99,0xff,0x00,
    0x66,0x99,0xff,0x00,0x33,0x99,0xff,0x00,0x00,0x99,0xff,0x00,0xff,0x66,0xff,0x00,0xcc,0x66,0xff,0x00,
    0x99,0x66,0xff,0x00,0x66,0x66,0xff,0x00,0x33,0x66,0xff,0x00,0x00,0x66,0xff,0x00,0xff,0x33,0xff,0x00,
    0xcc,0x33,0xff,0x00,0x99,0x33,0xff,0x00,0x66,0x33,0xff,0x00,0x33,0x33,0xff,0x00,0x00,0x33,0xff,0x00,
    0xff,0x00,0xff,0x00,0xcc,0x00,0xff,0x00,0x99,0x00,0xff,0x00,0x66,0x00,0xff,0x00,0x33,0x00,0xff,0x00,
    0x00,0x00,0xff,0x00,0xff,0xff,0xcc,0x00,0xcc,0xff,0xcc,0x00,0x99,0xff,0xcc,0x00,0x66,0xff,0xcc,0x00,
    0x33,0xff,0xcc,0x00,0x00,0xff,0xcc,0x00,0xff,0xcc,0xcc,0x00,0xcc,0xcc,0xcc,0x00,0x99,0xcc,0xcc,0x00,
    0x66,0xcc,0xcc,0x00,0x33,0xcc,0xcc,0x00,0x00,0xcc,0xcc,0x00,0xff,0x99,0xcc,0x00,0xcc,0x99,0xcc,0x00,
    0x99,0x99,0xcc,0x00,0x66,0x99,0xcc,0x00,0x33,0x99,0xcc,0x00,0x00,0x99,0xcc,0x00,0xff,0x66,0xcc,0x00,
    0xcc,0x66,0xcc,0x00,0x99,0x66,0xcc,0x00,0x66,0x66,0xcc,0x00,0x33,0x66,0xcc,0x00,0x00,0x66,0xcc,0x00,
    0xff,0x33,0xcc,0x00,0xcc,0x33,0xcc,0x00,0x99,0x33,0xcc,0x00,0x66,0x33,0xcc,0x00,0x33,0x33,0xcc,0x00,
    0x00,0x33,0xcc,0x00,0xff,0x00,0xcc,0x00,0xcc,0x00,0xcc,0x00,0x99,0x00,0xcc,0x00,0x66,0x00,0xcc,0x00,
    0x33,0x00,0xcc,0x00,0x00,0x00,0xcc,0x00,0xff,0xff,0x99,0x00,0xcc,0xff,0x99,0x00,0x99,0xff,0x99,0x00,
    0x66,0xff,0x99,0x00,0x33,0xff,0x99,0x00,0x00,0xff,0x99,0x00,0xff,0xcc,0x99,0x00,0xcc,0xcc,0x99,0x00,
    0x99,0xcc,0x99,0x00,0x66,0xcc,0x99,0x00,0x33,0xcc,0x99,0x00,0x00,0xcc,0x99,0x00,0xff,0x99,0x99,0x00,
    0xcc,0x99,0x99,0x00,0x99,0x99,0x99,0x00,0x66,0x99,0x99,0x00,0x33,0x99,0x99,0x00,0x00,0x99,0x99,0x00,
    0xff,0x66,0x99,0x00,0xcc,0x66,0x99,0x00,0x99,0x66,0x99,0x00,0x66,0x66,0x99,0x00,0x33,0x66,0x99,0x00,
    0x00,0x66,0x99,0x00,0xff,0x33,0x99,0x00,0xcc,0x33,0x99,0x00,0x99,0x33,0x99,0x00,0x66,0x33,0x99,0x00,
    0x33,0x33,0x99,0x00,0x00,0x33,0x99,0x00,0xff,0x00,0x99,0x00,0xcc,0x00,0x99,0x00,0x99,0x00,0x99,0x00,
    0x66,0x00,0x99,0x00,0x33,0x00,0x99,0x00,0x00,0x00,0x99,0x00,0xff,0xff,0x66,0x00,0xcc,0xff,0x66,0x00,
    0x99,0xff,0x66,0x00,0x66,0xff,0x66,0x00,0x33,0xff,0x66,0x00,0x00,0xff,0x66,0x00,0xff,0xcc,0x66,0x00,
    0xcc,0xcc,0x66,0x00,0x99,0xcc,0x66,0x00,0x66,0xcc,0x66,0x00,0x33,0xcc,0x66,0x00,0x00,0xcc,0x66,0x00,
    0xff,0x99,0x66,0x00,0xcc,0x99,0x66,0x00,0x99,0x99,0x66,0x00,0x66,0x99,0x66,0x00,0x33,0x99,0x66,0x00,
    0x00,0x99,0x66,0x00,0xff,0x66,0x66,0x00,0xcc,0x66,0x66,0x00,0x99,0x66,0x66,0x00,0x66,0x66,0x66,0x00,
    0x33,0x66,0x66,0x00,0x00,0x66,0x66,0x00,0xff,0x33,0x66,0x00,0xcc,0x33,0x66,0x00,0x99,0x33,0x66,0x00,
    0x66,0x33,0x66,0x00,0x33,0x33,0x66,0x00,0x00,0x33,0x66,0x00,0xff,0x00,0x66,0x00,0xcc,0x00,0x66,0x00,
    0x99,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x33,0x00,0x66,0x00,0x00,0x00,0x66,0x00,0xff,0xff,0x33,0x00,
    0xcc,0xff,0x33,0x00,0x99,0xff,0x33,0x00,0x66,0xff,0x33,0x00,0x33,0xff,0x33,0x00,0x00,0xff,0x33,0x00,
    0xff,0xcc,0x33,0x00,0xcc,0xcc,0x33,0x00,0x99,0xcc,0x33,0x00,0x66,0xcc,0x33,0x00,0x33,0xcc,0x33,0x00,
    0x00,0xcc,0x33,0x00,0xff,0x99,0x33,0x00,0xcc,0x99,0x33,0x00,0x99,0x99,0x33,0x00,0x66,0x99,0x33,0x00,
    0x33,0x99,0x33,0x00,0x00,0x99,0x33,0x00,0xff,0x66,0x33,0x00,0xcc,0x66,0x33,0x00,0x99,0x66,0x33,0x00,
    0x66,0x66,0x33,0x00,0x33,0x66,0x33,0x00,0x00,0x66,0x33,0x00,0xff,0x33,0x33,0x00,0xcc,0x33,0x33,0x00,
    0x99,0x33,0x33,0x00,0x66,0x33,0x33,0x00,0x33,0x33,0x33,0x00,0x00,0x33,0x33,0x00,0xff,0x00,0x33,0x00,
    0xcc,0x00,0x33,0x00,0x99,0x00,0x33,0x00,0x66,0x00,0x33,0x00,0x33,0x00,0x33,0x00,0x00,0x00,0x33,0x00,
    0xff,0xff,0x00,0x00,0xcc,0xff,0x00,0x00,0x99,0xff,0x00,0x00,0x66,0xff,0x00,0x00,0x33,0xff,0x00,0x00,
    0x00,0xff,0x00,0x00,0xff,0xcc,0x00,0x00,0xcc,0xcc,0x00,0x00,0x99,0xcc,0x00,0x00,0x66,0xcc,0x00,0x00,
    0x33,0xcc,0x00,0x00,0x00,0xcc,0x00,0x00,0xff,0x99,0x00,0x00,0xcc,0x99,0x00,0x00,0x99,0x99,0x00,0x00,
    0x66,0x99,0x00,0x00,0x33,0x99,0x00,0x00,0x00,0x99,0x00,0x00,0xff,0x66,0x00,0x00,0xcc,0x66,0x00,0x00,
    0x99,0x66,0x00,0x00,0x66,0x66,0x00,0x00,0x33,0x66,0x00,0x00,0x00,0x66,0x00,0x00,0xff,0x33,0x00,0x00,
    0xcc,0x33,0x00,0x00,0x99,0x33,0x00,0x00,0x66,0x33,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x33,0x00,0x00,
    0xff,0x00,0x00,0x00,0xcc,0x00,0x00,0x00,0x99,0x00,0x00,0x00,0x66,0x00,0x00,0x00,0x33,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0xc0,0xc0,0xc0,0x00,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,
    0x00,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0xff,0xff,0xff,0x00,
    0xf6,0xf6,0xf6,0x00,0xee,0xee,0xee,0x00,0xe6,0xe6,0xe6,0x00,0xde,0xde,0xde,0x00,0xd5,0xd5,0xd5,0x00,
    0xcd,0xcd,0xcd,0x00,0xc5,0xc5,0xc5,0x00,0xbd,0xbd,0xbd,0x00,0xb4,0xb4,0xb4,0x00,0xac,0xac,0xac,0x00,
    0xa4,0xa4,0xa4,0x00,0x9c,0x9c,0x9c,0x00,0x94,0x94,0x94,0x00,0x8b,0x8b,0x8b,0x00,0x83,0x83,0x83,0x00,
    0x7b,0x7b,0x7b,0x00,0x73,0x73,0x73,0x00,0x6a,0x6a,0x6a,0x00,0x62,0x62,0x62,0x00,0x5a,0x5a,0x5a,0x00,
    0x52,0x52,0x52,0x00,0x4a,0x4a,0x4a,0x00,0x41,0x41,0x41,0x00,0x39,0x39,0x39,0x00,0x31,0x31,0x31,0x00,
    0x29,0x29,0x29,0x00,0x20,0x20,0x20,0x00,0x18,0x18,0x18,0x00,0x10,0x10,0x10,0x00,0x08,0x08,0x08,0x00,
    0x00,0x00,0x00,0x00
};
