// https://i-satoh.hatenablog.com/entry/65438175
// ここのサイトのMotionJPEGの再生プログラムを参考にして
// Linuxのフレームバッファで動画再生のテストをしてみました。
// 動画のみで音声は再生されません
//
// 今回のはLinuxのフレームバッファ(32bit ARGB専用)です
//
// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder
// のJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cを参考にして手を加えました
// botmerさんがメンテナンスされているものがあるようですがかなり手を加えられているのでそれでは動作しません
// また、botmerさんがメンテナンスされているものはRGB565専用になってるようなので使いません
//
// SDカードに保存されたMotionJPEGの動画を表示するものです。
// 動画は映像のみの再生です、音声は再生されません。
// 長時間動画の再生はできてます。
//
// このプログラムには動画ファイルは付属しません
// 120行目あたりのloop関数の中身を編集してご自分の動画ファイルを再生するように変更してください
// (拡張子は大文字の.MJPにしてください)
//
// 対応している動画形式はMotionJPEGです。
// 拡張子は大文字半角の.MJPで保存してください。
// MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
// ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
// ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
// ffmpeg -i test_in3.mp4 -b:v 750k -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
// 拡張子は大文字の.MJPで保存してください。
//
// これらのプログラムの使用に当たってはご自分の責任において使用してください
// これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません

#include "TFT000001_100.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <signal.h>

// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoderのJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cに手を加えたものを使用しています
// https://i-satoh.hatenablog.com/entry/65438175
#include "JPEGDecoder_for_linux/JPEGDecoder.h"



#define ILI9486_TFTWIDTH  800
#define ILI9486_TFTHEIGHT 600



TFT000001 tft = TFT000001(ILI9486_TFTWIDTH, ILI9486_TFTHEIGHT);


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

#define _width   ILI9486_TFTWIDTH
#define _height  ILI9486_TFTHEIGHT

#define DEFFRATE     5  // default frame rate

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

/*
void resetPushColor(int x1, int y1, int x2, int y2); 
void pushColor(uint32_t color);
void pushColors(uint8_t *data, int32_t size);
*/
inline static void dmaWait(void);


bool drawJpgBmppicture(char *fname);
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate);
void delay(long milli_sec);
long millis(); 
long time_us_64();

void sig_handler(int signo);
void setup();
void loop();
int main(int argc, char *argv[]) {

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        return -1;
    }



    setup();

	for(;;) {
	    loop();
	}
}

void sig_handler(int signo) {
    if (signo == SIGINT) {
        tft.deinit();
        exit(0);
    }
}

void setup() {
  tft.init();
  //setRotation(3);
  tft.fillScreen(ILI9486_BLUE);
  delay(1000);
}

void loop() {
    // drawJpgBmppicture_mjpeg(動画ファイル名, FPS)で指定します。
    // MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
    // ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
    // ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
    // ffmpeg -i test_in3.mp4 -b:v 750k -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
    // このプログラムはMotionJPEGのファイルの拡張子は.MJPとして認識するので拡張子を.MJPに変更してください。
    // 426x320、2700kの動画だと13FPSくらいです。
    // 320x240、1500kの動画だと22FPSくらいです。
    // 240x160、750kで38FPSくらいでした。
    // JPEG画像やBMP画像の表示もできますが、表示されない場合は一度Windows付属のペイントで保存してから試してみてください。

    tft.fillScreen(ILI9486_GREEN);
    drawJpgBmppicture_mjpeg("snail01.JPG", 0);
    delay(1000);
    drawJpgBmppicture_mjpeg("scenic01.BMP", 0);
    delay(1000);

/*
    tft.fillScreen(ILI9486_BLUE);
    drawJpgBmppicture_mjpeg("test_o1.MJP", 13);
    delay(5000);

    tft.fillScreen(ILI9486_BLUE );
    drawJpgBmppicture_mjpeg("test_o2.MJP", 22);
    delay(5000);

    tft.fillScreen(ILI9486_BLUE);
    drawJpgBmppicture_mjpeg("test_o3.MJP", 38);
    delay(5000);
*/

    tft.fillScreen(ILI9486_RED);
//    drawJpgBmppicture_mjpeg("A4_12FPS.MJP", 12);
//    drawJpgBmppicture_mjpeg("bad2-15.MJP", 15);


    drawJpgBmppicture_mjpeg("macr1-13.MJP", 13);


//    drawJpgBmppicture_mjpeg("maca1-22.MJP", 22);
//    drawJpgBmppicture_mjpeg("macb1-38.MJP", 38);
    delay(5000);

    drawJpgBmppicture_mjpeg("A5_9FPS.MJP", 9);
    delay(5000);

    drawJpgBmppicture_mjpeg("A6_9FPS.MJP", 9);
    delay(5000);

    drawJpgBmppicture_mjpeg("A4_9FPS.MJP", 9);
    delay(5000);
}




// Jpeg/Motion-Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate) {
  // variables
  bool rlt = false;
  FILE *fp;
  unsigned long nxtdt;

  // check BMP/JPEG file
  char *ext = strrchr(fname, '.');
  // if(!ext || (memcmp(ext + 1, "BMP", 3) && memcmp(ext + 1, "JPG", 3) && memcmp(ext + 1, "MJP", 3))) return false;
  if (!ext) {
    tft.fillScreen(ILI9486_PURPLE);
    return false;
  }
  if (!(!memcmp(ext + 1, "BMP", 3) || !memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3))) {
    tft.fillScreen(ILI9486_YELLOW);
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
    struct rgb01 col = color01(ILI9486_BLACK);
    bool inifrmset = true;
    // draw picture
    
    long time01 = 0L;
    long time02 = 0L;
    long time00 = millis();
    double fps_aver = 0.0;
    int count_fps = 0;
    
    do{
      time02 = time01;
      time01 = millis();
      if (time02 > 0L) {
        double time1 = (time01 - time02) / 1000.0;
        double fps01 = 1.0 / time1;
        fps_aver += fps01;
        count_fps++;
        if (((millis() - time00) / 1000.0) > 5.0) {
          if(count_fps > 0) {
            printf("time = %.4f\r\n", time1);
            printf("fps = %.4f\r\n", fps_aver / count_fps);
            fps_aver = 0.0;
            count_fps = 0;
          }
          time00 = millis();
        }
      }
      // set next picture draw timing
      if(JpegDec.mjpegflag) nxtdt = millis() + 1000 / ((frate > 0) ? frate : DEFFRATE);
      // check decording parameter
      if((JpegDec.decode(fname, 0) < 0) || !JpegDec.height || !JpegDec.width) return false;
      // set draw offset parameters
      long xofset = (_width - JpegDec.width) / 2;
      long yofset = (_height - JpegDec.height) / 2;
      if(inifrmset){
        // set top black belt
        dmaWait();
        if(yofset > 0) tft.fillRect(0, 0, _width, yofset, ILI9486_BLACK);
        // set left & right black belt
        if(xofset > 0){
          tft.fillRect(0, (yofset > 0) ? yofset : 0, xofset, (yofset > 0) ? JpegDec.height : _height, ILI9486_BLACK);
          tft.fillRect(xofset + JpegDec.width, (yofset > 0) ? yofset : 0, _width - JpegDec.width - xofset, (yofset > 0) ? JpegDec.height : _height, ILI9486_BLACK);
        }
        inifrmset = false;
      }
      // start jpeg decode
      struct rgb01 clbuf[JpegDec.MCUWidth * JpegDec.MCUHeight];
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
 /*
              dmaWait();

              spi_begin();
              setAddrWindow(dxstrt, dystrt, dxend, dyend);
*/
//              resetPushColor(dxstrt, dystrt, dxend, dyend);
              tft.resetPushColor24(dxstrt, dystrt, dxend, dyend);
              
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
            if(x >= JpegDec.width) col = color01(ILI9486_BLACK);
            if(((xofset + x) >= 0) && ((yofset + y) >= 0) && ((xofset + x) < _width) && ((yofset + y) < _height))   clbuf[clbufpt++] = col;
            pImg += JpegDec.comps;
          }
        }
        // draw picture
//        if(clbufpt) pushColors((uint8_t *)clbuf, clbufpt);
        if(clbufpt) tft.pushColors24((uint8_t *)clbuf, clbufpt);
      }

      // set bottom black belt
      if(yofset > 0) tft.fillRect(0, yofset + JpegDec.height, _width, _height - (yofset + JpegDec.height), ILI9486_BLACK);
      // check next picture wait timing
      if(JpegDec.mjpegflag){
        long picdly = nxtdt - millis();
        if(picdly > 0) delay(picdly);
      }
      //
    } while(JpegDec.mjpegflag && JpegDec.nextjpgpos);
    // end of drawing
    rlt = true;

  // check bitmap file
  } else {
    // check file header
    #define RDPLEN   256   // read packets length
    unsigned char rdbuf[RDPLEN * 3];
	size_t ret01;
    fp = fopen(fname, "r");
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
/*
        dmaWait();
        spi_begin();
        setAddrWindow(0, y, _width-1, y);
        pushColors((uint8_t *)blkblt, _width);
*/
//        resetPushColor(0, y, _width-1, y);
//        pushColors((uint8_t *)blkblt, _width);
        tft.resetPushColor24(0, y, _width-1, y);
		tft.pushColors24((uint8_t *)blkblt, _width);
        continue;
      }
      // draw y line picture
      for(int ly = 0 ; ly < drhight ; ly++){
        // draw x line
        for(int x = 0 ; x < _width ; x++){
          // draw left & right blank
          if((_width > lwidth) && ((x < drxofset) || (x >= (drxofset + drwidth)))){
            // pushColor(ILI9486_BLACK);
            blkblt2[x] = color01(ILI9486_BLACK);
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
/*
        spi_begin();
        setAddrWindow(0, y + ly, _width-1, y + ly);
        pushColors((uint8_t *)blkblt2, _width);
*/
//        resetPushColor(0, y + ly, _width-1, y + ly);
//        pushColors((uint8_t *)blkblt2, _width);
        tft.resetPushColor24(0, y + ly, _width-1, y + ly);
		tft.pushColors24((uint8_t *)blkblt2, _width);

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

/*
int pushColor_x1, pushColor_y1, pushColor_x2, pushColor_y2, pushColor_left;
void resetPushColor(int x1, int y1, int x2, int y2) {
    pushColor_x1 = x1;
	pushColor_left = x1;
    pushColor_y1 = y1;
	pushColor_x2 = x2;
	pushColor_y2 = y2;
}
void pushColor(uint32_t color) {

    tft.drawPixel(pushColor_x1, pushColor_y1, color);
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
	    color01 = ((int)*p++) << 16 | ((int)*p++) << 8 | ((int)*p++);
	    tft.drawPixel(pushColor_x1, pushColor_y1, color01);
	    pushColor_x1++;
	    if (pushColor_x1 == pushColor_x2 + 1) {
            pushColor_y1++;
		    pushColor_x1 = pushColor_left;
	    }
	}
}
*/

inline static void dmaWait(void) {
}

struct timespec delay_nano_sleep_param01_req, delay_nano_sleep_param01_rem;

void delay(long milli_sec) {
    int nanosleep_ret;
    delay_nano_sleep_param01_req.tv_sec = milli_sec / 1000;
    delay_nano_sleep_param01_req.tv_nsec = (milli_sec % 1000) * 1000 * 1000;
    nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    while (nanosleep_ret == -1) {
        delay_nano_sleep_param01_req.tv_sec = delay_nano_sleep_param01_rem.tv_sec;
        delay_nano_sleep_param01_req.tv_nsec = delay_nano_sleep_param01_rem.tv_nsec;
        nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    }
}

#include <sys/time.h>
double second()
{


  struct timeval tm;
  double t ;

  static int base_sec = 0,base_usec = 0;

  gettimeofday(&tm, NULL);

  if(base_sec == 0 && base_usec == 0)
    {
      base_sec = tm.tv_sec;
      base_usec = tm.tv_usec;
      t = 0.0;
  } else {
    t = (double) (tm.tv_sec-base_sec) +
      ((double) (tm.tv_usec-base_usec))/1.0e6 ;
  }

  return t ;
}

long time_us_64() {
    return (long)(second() * 1000.0 * 1000.0);
}

long millis() {
    return (long)(second() * 1000.0);
}
