// https://i-satoh.hatenablog.com/entry/65438175
// ここのサイトのMotionJPEGの再生プログラムを参考にして
// FM TOWNSで動画再生のテストをしてみました。
// 動画のみで音声は再生されません
//
// 今回のはHigh Cのライブラリを使ったOpenWatcom C Ver2.0用のFM TOWNSのTOWNS OS用32768色版です
// High Cのライブラリを使ったOpenWatcom C Ver2.0で正常に動作するように修正しました
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
// 231行目あたりのloop関数の中身を編集してご自分の動画ファイルを再生するように変更してください
// (拡張子は大文字の.MJPにしてください)
//
// 対応している動画形式はMotionJPEGです。
// 拡張子は大文字半角の.MJPで保存してください。
// MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
// ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
// ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
// ffmpeg -i test_in3.mp4 -b:v 750k  -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
// ffmpeg -i test_in4.mp4 -b:v 200k  -r 12.0 -s 120x90 -vcodec mjpeg -an test_o4.mov
// ffmpeg -i test_in5.mp6 -b:v 70k    -r 4.0 -s 120x90 -vcodec mjpeg -an test_o5.mov
// 拡張子は大文字の.MJPで保存してください。
//
// これらのプログラムの使用に当たってはご自分の責任において使用してください
// これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dos.h>
#include "..\..\include\fmcfrb.h"
#include "..\..\include\egb.h"
#include <conio.h>


// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoderのJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cに手を加えたものを使用しています
// https://i-satoh.hatenablog.com/entry/65438175
#include "JPEGDec/JPEGDeco.h"



int work01[256*1024/4];

#define VRAM_SCREEN_WIDTH   (512)
#define VRAM_SCREEN_HIGHT   (256)
#define SCREEN_WIDTH        (320)
#define SCREEN_HEIGHT       (240)


#define ILI9486_BLACK      ((uint32_t)0x000000) ///<   0,   0,   0
#define ILI9486_NAVY       ((uint32_t)0x000080) ///<   0,   0, 128
#define ILI9486_DARKGREEN  ((uint32_t)0x008000) ///<   0, 128,   0
#define ILI9486_DARKCYAN   ((uint32_t)0x008080) ///<   0, 128, 128
#define ILI9486_MAROON     ((uint32_t)0x800000) ///< 128,   0,   0
#define ILI9486_PURPLE     ((uint32_t)0x800080) ///< 128,   0, 128
#define ILI9486_OLIVE      ((uint32_t)0x808000) ///< 128, 128,   0
#define ILI9486_LIGHTGREY  ((uint32_t)0xC6C3C6) ///< 198, 195, 198
#define ILI9486_DARKGREY   ((uint32_t)0x808080) ///< 128, 128, 128
#define ILI9486_BLUE       ((uint32_t)0x0000FF) ///<   0,   0, 255
#define ILI9486_GREEN      ((uint32_t)0x00FF00) ///<   0, 255,   0
#define ILI9486_CYAN       ((uint32_t)0x00FFFF) ///<   0, 255, 255
#define ILI9486_RED        ((uint32_t)0xFF0000) ///< 255,   0,   0
#define ILI9486_MAGENTA    ((uint32_t)0xFF00FF) ///< 255,   0, 255
#define ILI9486_YELLOW     ((uint32_t)0xFFFF00) ///< 255, 255,   0
#define ILI9486_WHITE      ((uint32_t)0xFFFFFF) ///< 255, 255, 255
#define ILI9486_ORANGE     ((uint32_t)0xFFA500) ///< 255, 165,   0
#define ILI9486_GREENYELLO ((uint32_t)0xADFF29) ///< 173, 255,  41
#define ILI9486_PINK       ((uint32_t)0xFF82C6) ///< 255, 130, 198


#define _width   SCREEN_WIDTH
#define _height  SCREEN_HEIGHT

#define DEFFRATE     10  // default frame rate

struct rgb01 {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct rgb01 color01(unsigned int color) {
    struct rgb01 col;
    col.r = (color >> 16) & 0xff;
    col.g = (color >> 8) & 0xff;
    col.b = (color) & 0xff;
    return col;
}

void dmaWait(void);

#define true  1
#define false 0

typedef int   bool;


bool drawJpgBmppicture(char *fname);
bool drawJpgBmppicture_mjpeg(char *fname, unsigned char frate);
long millis01(); 
long time_us_64();
void delay01(int a);

int ginit(int mode1, int page1, char *workaddr01, int worksize01);
void set_color(int color);
void set_bgcolor(int color);
void set_color_GRB(int color);
void set_bgcolor_GRB(int color);
void clear_screen(void);
void draw_pixel(int x, int y, int color);
void set_palette(int pal_num01, int r01, int g01, int b01);
void draw_string(int x, int y, char *str1);
int set_screen_ratio(int x_ratio, int y_ratio);
int set_display_start(int mode, int x_start, int y_start);

#define drawPixel  draw_pixel_vram_16bit_02
void draw_pixel_vram_16bit_02(int x, int y, int color);

void fillScreen(int color);
void fillRect(int x1, int y1, int x2, int y2, int color);
void resetPushColor(int x1, int y1, int x2, int y2);
void pushColor(unsigned int color);
void pushColors(unsigned char *data, int size);

void int_to_str(char *str1, int a);
void uint_to_str(char *str1, unsigned int a);

int init_timer01(TIM_TIME *timer01, int *timer_num01);
unsigned int get_count_timer01(int timer_num01);
int close_timer01(int timer_num01);

int fread01(unsigned char *buf, int blocksize, int n, FILE *fp);

void setup();
void loop();

double fps0001 = 0.0;

char buf01[1024];
char buf02[256];

int timer_num01;
TIM_TIME timer01;
int main(int argc, char *argv[]) {
    int ret01;
    union REGS regs;

    setup();

    set_bgcolor_GRB(color15(ILI9486_BLACK));
    clear_screen();

    if (init_timer01(&timer01, &timer_num01)) {
        set_bgcolor_GRB(color15(ILI9486_RED));
        clear_screen();

        strcpy(buf01, "タイマの初期化に失敗しました");
        draw_string(0, 20, buf01);

        // キーバッファのクリア
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);

        while (!kbhit());

        exit(1);
    }


    for(;;) {
        loop();
        if (kbhit()) break;
    }


    ret01 = close_timer01(timer_num01);

    clear_screen();
    if (ret01 == (char)0) {
        strcpy(buf01, "タイマ解放OK");
        draw_string(0, 20, buf01);
    } else {
        strcpy(buf01, "タイマ解放NG");
        draw_string(0, 20, buf01);
    }

    set_bgcolor_GRB(color15(ILI9486_BLACK));
    clear_screen();


    ginit(3, 0, (char *)work01, 256*1024);

    return 0;
}


void setup() {
  int ret01, i;

  if ((ret01 = ginit(10, 0, (char *)work01, 256*1024)) != 0) {
       puts("ginit failed!");
       while(!kbhit());
       exit(1);
  }

// SCREEN10 320x240全画面
// 仮想画面中の表示開始位置を設定します。                                       
// モード　　0:画面の表示開始位置の設定                                         
// 　　　　　1:仮想画面中の移動                                                 
// 　　　　　2:画面の拡大                                                       
// 　　　　　3:表示画面の大きさ    
  set_display_start(3, 0, 0);
  set_display_start(2, 2, 2);
  set_display_start(0, 0, 0);
  set_display_start(1, 0, 0);
  set_display_start(3, 320, 240);

  set_bgcolor_GRB(0);
  clear_screen();
}

void loop() {
    // drawJpgBmppicture_mjpeg(動画ファイル名, FPS)で指定します。
    // MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
    // ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
    // ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
    // ffmpeg -i test_in3.mp4 -b:v 750k  -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
    // ffmpeg -i test_in4.mp4 -b:v 200k  -r 12.0 -s 120x90 -vcodec mjpeg -an test_o4.mov
    // ffmpeg -i test_in5.mp6 -b:v 70k    -r 4.0 -s 120x90 -vcodec mjpeg -an test_o5.mov
    // 拡張子は大文字の.MJPで保存してください。
    // JPEG画像やBMP画像の表示もできますが、表示されない場合は一度Windows付属のペイントで保存してから試してみてください。

    fillScreen(color15(ILI9486_GREEN));
    drawJpgBmppicture_mjpeg("SNAIL01.JPG", 0);
    delay01(1000);
    drawJpgBmppicture_mjpeg("SCENIC01.BMP", 0);
    delay01(1000);

/*
    fillScreen(color15(ILI9486_BLUE));
    drawJpgBmppicture_mjpeg("test_o1.MJP", 13);
    delay01(5000);

    fillScreen(color15(ILI9486_BLUE));
    drawJpgBmppicture_mjpeg("test_o2.MJP", 22);
    delay01(5000);

    fillScreen(color15(ILI9486_BLUE));
    drawJpgBmppicture_mjpeg("test_o3.MJP", 38);
    delay01(5000);
*/

    fillScreen(color15(ILI9486_BLACK));
    drawJpgBmppicture_mjpeg("V1005FPS.MJP", 5);
    delay01(1000);

    fillScreen(color15(ILI9486_BLACK));
    drawJpgBmppicture_mjpeg("V1104FPS.MJP", 4);
    delay01(1000);
}




// Jpeg/Motion-Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture_mjpeg(char *fname, unsigned char frate) {
  // variables
  bool rlt = false;
  FILE *fp;
  unsigned long nxtdt;
  char *ext;
  long picdly;
  long picdly01 = 0;

  JPEGDecoder_init();
  set_color_GRB(color15(ILI9486_WHITE));
  set_bgcolor_GRB(color15(ILI9486_BLACK));
  clear_screen();

  // check BMP/JPEG file
  ext = strrchr(fname, '.');
  // if(!ext || (memcmp(ext + 1, "BMP", 3) && memcmp(ext + 1, "JPG", 3) && memcmp(ext + 1, "MJP", 3))) return false;
  if (!ext) {
    fillScreen(color15(ILI9486_PURPLE));
    return false;
  }
  if (!(!memcmp(ext + 1, "BMP", 3) || !memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3))) {
    fillScreen(color15(ILI9486_YELLOW));
    return false;
  }
  // check jpeg file
  if(!memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3)){

    // variables
    unsigned char  *pImg;
    int x, y, bx, by;
    struct rgb01 col = color01(ILI9486_BLACK);
    bool inifrmset = true;

    // draw picture
    
    long time01 = 0L;
    long time02 = 0L;
    long time00 = millis01();
    double fps_aver = 0.0;
    int count_fps = 0;


    // check & set mottion jpeg
    if(!memcmp(ext + 1, "MJP", 3)) {
      JPEGDecoder_mjpegflag = 1;
    } else {
      JPEGDecoder_mjpegflag = 0;
    }

    do{
      unsigned int clbufpt;
      struct rgb01 *clbuf;
      long xofset;
      long yofset;

      if (kbhit()) break;

      time02 = time01;
      time01 = millis01();
      if (time02 > 0L) {
        double time1;
        double fps01;

        time1 = (time01 - time02) / 1000.0;
        fps01 = 1.0 / time1;
        fps_aver += fps01;
        count_fps++;
        if (((millis01() - time00) / 1000.0) > 5.0) {
          if(count_fps > 0) {
            fps0001 = fps_aver / count_fps;
            picdly01 = picdly;
//            printf("time = %.4f\r\n", time1);
//            printf("fps = %.4f\r\n", fps_aver / count_fps);
            fps_aver = 0.0;
            count_fps = 0;
          }
          time00 = millis01();
        }
      }
      // set next picture draw timing
      if(JPEGDecoder_mjpegflag) nxtdt = millis01() + 1000 / ((frate > 0) ? frate : DEFFRATE);
      // check decording parameter
      if((JPEGDecoder_decode(fname, (unsigned char)0) < 0))  return false;
      if(!JPEGDecoder_height || !JPEGDecoder_width) return false;
      // set draw offset parameters
      xofset = (_width - JPEGDecoder_width) / 2;
      yofset = (_height - JPEGDecoder_height) / 2;
      if(inifrmset){
        // set top black belt
        dmaWait();

//        映像の上側を消去
//        if(yofset > 0) fillRect(0, 0, _width, yofset, color01(ILI9486_BLACK));
        // set left & right black belt
        if(xofset > 0){
//          映像の左右を消去
//          fillRect(0, (yofset > 0) ? yofset : 0, xofset, (yofset > 0) ? JPEGDecoder_height : _height, color01(BLACK));
//          fillRect(xofset + JPEGDecoder_width, (yofset > 0) ? yofset : 0, _width - JPEGDecoder_width - xofset, (yofset > 0) ? JPEGDecoder_height : _height, color01(ILI9486_BLACK));
        }
        inifrmset = false;
      }
      // start jpeg decode
//      struct rgb01 clbuf[JPEGDecoder_MCUWidth * JPEGDecoder_MCUHeight];
      if ((clbuf = (struct rgb01 *)malloc(JPEGDecoder_MCUWidth * JPEGDecoder_MCUHeight * sizeof(struct rgb01))) == NULL) {
          draw_string(0, 40, "malloc failed!");
          while(!kbhit());
          exit(-1);
      }
      memset(clbuf, 0, JPEGDecoder_MCUWidth * JPEGDecoder_MCUHeight * sizeof(struct rgb01));
      clbufpt = 0;
      while(JPEGDecoder_read()){
        long dxstrt;
        long dystrt;
        long dxend;
        long dyend;
        bool drawstart;
        // check draw range and get draw parameters
        pImg = JPEGDecoder_pImage ;
        dxstrt = JPEGDecoder_MCUx * JPEGDecoder_MCUWidth + xofset; if(dxstrt >= _width) continue;
        if(dxstrt < 0) dxstrt = 0;
        dystrt = JPEGDecoder_MCUy * JPEGDecoder_MCUHeight + yofset; if(dystrt >= _height) continue;
        if(dystrt < 0) dystrt = 0;
        dxend = (JPEGDecoder_MCUx + 1) * JPEGDecoder_MCUWidth - 1 + xofset; if(dxend < 0) continue;
        if(dxend >= _width) dxend = _width - 1;
        dyend = (JPEGDecoder_MCUy + 1) * JPEGDecoder_MCUHeight - 1 + yofset; if(dyend < 0) continue;
        if(dyend >= _height) dyend = _height - 1;
        // draw princture
        drawstart = true;
        
        clbufpt = 0;
        for(by = 0 ; by < JPEGDecoder_MCUHeight ; by++){
          // draw decoded image
          y = JPEGDecoder_MCUy * JPEGDecoder_MCUHeight + by;
          // check image end
          if(y >= JPEGDecoder_height) break;
          if(((y + yofset) < 0) || ((y + yofset) >= _height)) continue;
          // draw image
          for(bx = 0 ; bx < JPEGDecoder_MCUWidth ; bx++){
            x = JPEGDecoder_MCUx * JPEGDecoder_MCUWidth + bx;
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
            if(JPEGDecoder_comps == 1) { // Grayscale
              col.r = pImg[0]; col.g = pImg[0]; col.b = pImg[0];
            } else {// RGB
              col.r = pImg[0]; col.g = pImg[1]; col.b = pImg[2];
            }
            // check picture draw range & set color
            if(x >= JPEGDecoder_width) col = color01(ILI9486_BLACK);
            if(((xofset + x) >= 0) && ((yofset + y) >= 0) && ((xofset + x) < _width) && ((yofset + y) < _height))   clbuf[clbufpt++] = col;
            pImg += JPEGDecoder_comps;
          }
        }
        // draw picture
        if(clbufpt) pushColors((unsigned char *)clbuf, clbufpt);
      }

      free(clbuf);

       // 映像の下側を描画
//      // set bottom black belt
//      if(yofset > 0) fillRect(0, yofset + JPEGDecoder_height, _width, _height - (yofset + JPEGDecoder_height), color01(ILI9486_BLACK));

      // check next picture wait timing
      if(JPEGDecoder_mjpegflag){
        picdly = nxtdt - millis01();


        fillRect(0, 0, 40, 25, ILI9486_BLACK);
        int_to_str(buf01, (int)(fps0001 * 1000.0));
//        sprintf(buf01, "%.4f", fps0001);
        draw_string(0, 20, buf01);
//        int_to_str(buf01, (int)(picdly01));
////        sprintf(buf01, "%ld", picdly01);
//        draw_string(0, 40, buf01);



        if(picdly > 0) delay01(picdly);
      }
      //
    } while(JPEGDecoder_mjpegflag && JPEGDecoder_nextjpgpos);
    // end of drawing
    rlt = true;

  // check bitmap file
  } else {
    // check file header
    #define RDPLEN   256   // read packets length
    unsigned char rdbuf[RDPLEN * 3];
    unsigned long lwidth;
    unsigned long lheight;
    unsigned long offset;
    int drwidth;
    int drhight;
    int drxofset;
    int dryofset;
    unsigned long lpwidth;
    struct rgb01 blkblt[_width];
    struct rgb01 blkblt2[_width];
    size_t ret01;
    int y;

    fp = fopen(fname, "rb");
    ret01 = fread01(rdbuf, 1, 34, fp);
    if((rdbuf[0] != 'B') || (rdbuf[1] != 'M')) goto rtnend;  // check ID
    if(rdbuf[30] || rdbuf[31] || rdbuf[32] || rdbuf[33]) goto rtnend;  // check no compressin
    if((rdbuf[28] | ((unsigned short)rdbuf[29] << 8)) != 24) goto rtnend;  // check 24bits mode
    // get file parameters
    lwidth = rdbuf[18] | ((unsigned long)rdbuf[19] << 8) |
                           ((unsigned long)rdbuf[20] << 16) | ((unsigned long)rdbuf[21] << 24);
    lheight = rdbuf[22] | ((unsigned long)rdbuf[23] << 8) |
                           ((unsigned long)rdbuf[24] << 16) | ((unsigned long)rdbuf[25] << 24);
    offset = rdbuf[10] | ((unsigned long)rdbuf[11] << 8) |
                           ((unsigned long)rdbuf[12] << 16) | ((unsigned long)rdbuf[13] << 24);
    drwidth = (lwidth <= _width) ? lwidth : _width;
    drhight = (lheight <= _height) ? lheight : _height;
    drxofset = (_width - lwidth) / 2;
    dryofset = (_height - lheight) / 2;
    lpwidth = (lwidth * 3) + ((((lwidth * 3) % 4) > 0) ? 4 - ((lwidth * 3) % 4) : 0);

    // draw screen
    memset(blkblt, 0, sizeof(blkblt));
    for(y = 0 ; y < _height ; y++){
      int ly;
      // draw blank line
      if((_height > lheight) && ((y < dryofset) || (y >= (dryofset + drhight)))){

        resetPushColor(0, y, _width-1, y);
        pushColors((unsigned char *)blkblt, _width);
        continue;
      }
      // draw y line picture
      for(ly = 0 ; ly < drhight ; ly++){
        int x;

        // draw x line
        for(x = 0 ; x < _width ; x++){
          int lx;
          unsigned long rowidx;

          // draw left & right blank
          if((_width > lwidth) && ((x < drxofset) || (x >= (drxofset + drwidth)))){
            // pushColor(ILI9486_BLACK);
            blkblt2[x] = color01(ILI9486_BLACK);
            continue;
          }
          // draw x line picture
          rowidx = (lheight - ly - 1 + ((dryofset < 0) ? dryofset : 0)) * lpwidth +
                                                 ((drxofset < 0) ? -drxofset * 3 : 0);
          fseek(fp, rowidx + offset, SEEK_SET);
          for(lx = 0 ; lx < drwidth ; lx += RDPLEN){
            int ln;
            int rdpklen = ((lx + RDPLEN) < drwidth) ? RDPLEN : drwidth - lx;
            if(fread01(rdbuf, (size_t)1, rdpklen * 3, fp) == 0) goto rtnend;
//            struct rgb01 clbuf[rdpklen];
            for(ln = 0 ; ln < rdpklen ; ln++) {
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
        pushColors((unsigned char *)blkblt2, _width);

      }
      y += (drhight - 1);
    }
    rlt = true;
  }

  // process end
 rtnend:
  if(!memcmp(ext + 1, "BMP", 3)) fclose(fp);
  JPEGDecoder_close();
  return rlt;
}

// Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture(char *fname) {
  return drawJpgBmppicture_mjpeg(fname, 10);
}

int fread01(unsigned char *buf, int blocksize, int n, FILE *fp) {
    int   a = 0;
    int   ret01;
    while (1) {
        ret01 = fread(buf + a, blocksize, n - a, fp);
        if (ret01 == 0) break;
        a += ret01;
        if ((n - a) <= 0) break;
    }

    return a;
}

int pushColor_x1, pushColor_y1, pushColor_x2, pushColor_y2, pushColor_left;
void resetPushColor(int x1, int y1, int x2, int y2) {
    pushColor_x1 = x1;
    pushColor_left = x1;
    pushColor_y1 = y1;
    pushColor_x2 = x2;
    pushColor_y2 = y2;
}

void pushColor(unsigned int color) {
    int color01;

    color01 = ((((int)color >> 16) >> 3) << 5) | (((((int)color >> 8) & 0xff) >> 3) << 10) | (((int)color & 0xff) >> 3);
    drawPixel(pushColor_x1, pushColor_y1, color01);
    pushColor_x1++;
    if (pushColor_x1 == pushColor_x1 + 1) {
        pushColor_y1++;
        pushColor_x1 = 0;
    }
}

void pushColors(unsigned char *data, int size) {
    unsigned char  *p;
    int      color01;
    int      i;
    p = data;
    for (i = 0; i < size; i++) {
//        color01 = ((int)*p++) << 16 | ((int)*p++) << 8 | ((int)*p++);
        color01 = ((((int)*p++) >> 3) << 5) | ((((int)*p++) >> 3) << 10) | (((int)*p++) >> 3);
        drawPixel(pushColor_x1, pushColor_y1, color01);
        pushColor_x1++;
        if (pushColor_x1 == pushColor_x2 + 1) {
            pushColor_y1++;
            pushColor_x1 = pushColor_left;
    }
    }
}

int color15(int color) {
    return ((((int)color >> 16) >> 3) << 5) | (((((int)color >> 8) & 0xff) >> 3) << 10) | (((int)color & 0xff) >> 3);
}

void dmaWait(void) {
}

int second() {
    struct dostime_t time01;
    _dos_gettime(&time01);

    return (double)(((time01.hour * 3600L + time01.minute * 60L + time01.second) * 1000 + time01.hsecond * 10) / 1000.0);
}

long time_us_64() {
    return (long)(get_count_timer01(timer_num01) * 10 * 1000);
}

long millis01() {
    return (long)get_count_timer01(timer_num01) * 10;
}

void delay01(int a) {
    double time0001, time0002;
    if (a <= 0) return;
    time0001 = millis01();
    time0002 = (double)a;
    while ((millis01() - time0001) < time0002);
}

extern  short far *vram01;
void draw_pixel_vram_16bit_02(int x, int y, int color) {
//    if (x < 0 || x >= SCREEN_WIDTH) return;
//    if (y < 0 || y >= SCREEN_HEIGHT) return ;

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
            draw_pixel_vram_16bit_02(i, j, color);
        }
    }
}

void fillScreen(int color) {
    fillRect(0, 0, _width-1, _height-1, color);
}

short far *vram01;
char *workaddr;
int  worksize;
int ginit(int mode, int page, char *workaddr01, int worksize01) {
    char ret01;

    vram01 = MK_FP(0x0104, 0x00000000);    // 16,32768color = 0x0104    256color = 0x010c

    workaddr = workaddr01;
    worksize = worksize01;
    
    ret01 = EGB_init(workaddr, worksize);                 // EGB初期化
    if (ret01 != 0) return ret01;
    ret01 = EGB_resolution(workaddr, 0, mode);            // 画面モード ページ0
    if (ret01 != 0) return ret01;
//    ret01 = EGB_resolution(workaddr, 1, mode);            // 画面モード ページ1
//    if (ret01 != 0) return ret01;
    ret01 = EGB_writePage(workaddr, page);                // 書き込みページ指定
    if (ret01 != 0) return ret01;

    return 0;
}

void set_color(int color) {
    EGB_colorIGRB(workaddr, 0, color);
}

void set_bgcolor(int color) {
    EGB_color(workaddr, 1, color);
}

void set_color_GRB(int color) {
    int color01;
    color01 = (((color >> 10) & 0x1f) << (16 + 3)) | (((color >> 5) & 0x1f) << (8 + 3)) | ((color & 0x1f) << 3);
    EGB_colorIGRB(workaddr, 0, color01);
}

void set_bgcolor_GRB(int color) {
    int color01;
    color01 = (((color >> 10) & 0x1f) << (16 + 3)) | (((color >> 5) & 0x1f) << (8 + 3)) | ((color & 0x1f) << 3);
    EGB_colorIGRB(workaddr, 1, color01);
}

void clear_screen(void) {
    EGB_clearScreen(workaddr);
}

int set_screen_ratio(int x_ratio, int y_ratio) {
    int ret01;
    ret01 = EGB_displayStart(workaddr, 2, x_ratio, y_ratio);
    return ret01;
}

int set_display_start(int mode, int x_start, int y_start) {
    int ret01;
    ret01 = EGB_displayStart(workaddr, mode, x_start, y_start);
    return ret01;
}


void draw_pixel(int x, int y, int color) {

    struct draw_pixel_data {
        short   num;
        short   x;
        short   y;
    };
    struct draw_pixel_data draw_pixel_data01;
    struct draw_pixel_data *p;

    set_color(color);

    draw_pixel_data01.num = 1;
    draw_pixel_data01.x = (short)x;
    draw_pixel_data01.y = (short)y;
    p = &draw_pixel_data01;

    EGB_pset(workaddr, (char *)p);
}

void set_palette(int pal_num01, int r01, int g01, int b01) {
    struct set_palette_data {
        int     num;
        int     pal_num;
        char    b;
        char    r;
        char    g;
        char    zero;
    };
    struct set_palette_data set_palette_data01;
    struct set_palette_data *p;

    set_palette_data01.num = 1;
    set_palette_data01.pal_num = pal_num01;
    set_palette_data01.b = b01;
    set_palette_data01.r = r01;
    set_palette_data01.g = g01;
    set_palette_data01.zero = 0;

    p = &set_palette_data01;
    EGB_palette(workaddr, 0, (char *)p);
}

void draw_string(int x, int y, char *str1) {
    struct draw_string_data {
        short     x01;
        short     y01;
        short     strlen;
        char      str[];
    };
    int strlen01;
    struct draw_string_data *draw_string_data01;

    strlen01 = strlen(str1);

    draw_string_data01 = (struct draw_string_data *)malloc(6 + strlen01 + 1);

    draw_string_data01->x01 = x;
    draw_string_data01->y01 = y;
    draw_string_data01->strlen = (short)strlen01;
    memcpy(draw_string_data01->str, str1, strlen01 + 1);
    EGB_sjisString(workaddr, (char *)draw_string_data01);
}

void int_to_str(char *str1, int a) {
    char *p = str1;
    int  b = a;
    if (a < 0) {
        b = -a;
        p[0] = '-';
        p++;
    }
    uint_to_str(p, b);
}

void uint_to_str(char *str1, unsigned int a) {
    int i;
    char str[10];
    char *p, *p2;
    for (i = 0; i < 10; i++) {
        str[i] = (char)0;
    }
    for (i = 0; i < 10; i++) {
        str[i] = (char)(a % 10 + 0x30);
        a = a / 10;
    }
    p = str+10;
    for (i = 0; i < 10; i++) {
        p--;
        if (*p != (char)0x30) break;
    }
    if (i == 10) {
        str1[0] = (char)(0x30 + 0);
        str1[1] = (char)0;
    } else {
        p2 = str1;
        for (; i < 10; i++) {
            *p2++ = (char)*p--;
        }
        *p2 = (char)0;
    }
}

int func01() {
    return 0;
}

int init_timer01(TIM_TIME *timer01, int *timer_num01) {
    int ret01;
    timer01->mode = 1;
    timer01->inf  = 0;
    BSETCODEADR((char *)&timer01->adr, func01);
    timer01->hcycle = 0xffff;
    timer01->lcycle = 0xffff;

    ret01 = TIM_settime(timer01, timer_num01);

    return ret01;
}

unsigned int get_count_timer01(int timer_num01) {
    TIM_TIME timer01;
    TIM_rdtime(&timer01, timer_num01);
    return (((unsigned int)timer01.hcycle << 16) | (unsigned int)timer01.lcycle);
}

int close_timer01(int timer_num01) {
    int ret01;

    ret01 = TIM_clrtime(timer_num01);
    return ret01;
}
