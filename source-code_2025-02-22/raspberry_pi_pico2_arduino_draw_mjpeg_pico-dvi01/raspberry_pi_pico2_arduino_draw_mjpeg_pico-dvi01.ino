// https://i-satoh.hatenablog.com/entry/65438175
// ここのサイトのMotionJPEGの再生プログラムを参考にして
// Raspberry Pi Pico2でPico-DVIを使って動画再生のテストをしてみました。
// Pimoroni Pico Plus2でも動作しました
// SDカードに保存されたMotionJPEGの動画を表示するものです。
// Raspberry Pi Pico用のArduinoのearlephilhowerのArduino用です
// 動画は映像のみの再生です、音声は再生されません。
// FPSは遅いですが、長時間動画の再生はできてます。
// SDカードのアダプタが必要です
#pragma GCC optimize("O2")

#include <stdio.h>
#include <string.h>

#include <PicoDVI.h>

#include <SPI.h>

// SDカードのライブラリはPico用Arduinoに付属しているものを使ってください
#include "SD.h"

#include "JPEGDecoder.h"

#define DEFFRATE     5  // default frame rate


// SDカードではspi1を使います。CSはGP9、SCKはGP10、MOSIはGP11、MISOはGP8を使います
#define SDCARD_SPI_CS   9
#define SDCARD_SPI_SCK  10
#define SDCARD_SPI_MOSI 11
#define SDCARD_SPI_MISO 8
#define SDCARD_SPI_PORT spi1


#define PICO_DVI_WIDTH  320
#define PICO_DVI_HEIGHT 240
 
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


SPIClassRP2040 spi00 = SPIClassRP2040(SDCARD_SPI_PORT, SDCARD_SPI_MISO, -1, SDCARD_SPI_SCK, SDCARD_SPI_MOSI);
DVIGFX16 display(DVI_RES_320x240p60, pico_sock_cfg);

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

int32_t _width = PICO_DVI_WIDTH;
int32_t _height = PICO_DVI_HEIGHT;

uint32_t color16(int color);
void drawPixel(int x, int y, int color);
void resetPushColor(int x1, int y1, int x2, int y2);
void pushColor(uint32_t color);
void pushColors(uint8_t *data, int32_t size);
void setRotation(int mode);
void fillRect(int x1, int y1, int x2, int y2, int color);
void fillScreen(int color);

inline static bool dmaBusy(void);
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate);
void printDirectory(File dir, int numTabs);
void printf1(const char* format, ...);

File root;

void setup() {
  Serial.begin(115200);
  if (!display.begin()) { // Blink LED if insufficient RAM
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  setRotation(0);
  fillScreen(ILI9486_BLACK);
  if (SD.begin(SDCARD_SPI_CS, spi00)) {
    Serial.println("initialization done.");
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("done!");
  } else {
    Serial.println("initialization failed!");
  }
}

int led_flag01 = 1;
void loop() {
    led_flag01 = led_flag01 ^ 1;
    digitalWrite(LED_BUILTIN, led_flag01);
    // drawJpgBmppicture_mjpeg(動画ファイル名, FPS)で指定します。
    // MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
    // ffmpeg -i test_in4.mp4 -q 5 -r 13 -s 240x180 -vcodec mjpeg -an BA413FPS.mov
    // ffmpeg -i test_in5.mp4 -q 5 -r  9 -s 240x180 -vcodec mjpeg -an R2_09FPS.mov
    // ffmpeg -i test_in3.mp4 -q 5 -r  8 -s 320x240 -vcodec mjpeg -an BA4_8FPS.mov
    // このプログラムはMotionJPEGのファイルの拡張子は.MJPとして認識するので拡張子を変更してください。
    // JPEG画像やBMP画像の表示もできますが、表示されない場合は一度Windows付属のペイントで保存してから試してみてください。

    fillScreen(ILI9486_BLACK);
    drawJpgBmppicture_mjpeg("SNAIL01.JPG", 0);
    delay(5000);
    drawJpgBmppicture_mjpeg("SCENIC01.BMP", 0);
    delay(5000);

    fillScreen(ILI9486_BLUE);
    drawJpgBmppicture_mjpeg("BA413FPS.MJP", 13);
    delay(1000);

    fillScreen(ILI9486_BLUE);
    drawJpgBmppicture_mjpeg("R2_09FPS.MJP", 9);
    delay(1000);

    fillScreen(ILI9486_BLUE);
    drawJpgBmppicture_mjpeg("BA4_8FPS.MJP", 8);
    delay(1000);
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

#include <stdarg.h>
char str_buff01[1024];
void printf1(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsnprintf(str_buff01, 1024, format, ap);
    Serial.print(str_buff01);
    va_end(ap);
}

// Jpeg/Motion-Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate) {
  // variables
  bool rlt = false;
  File fp;
  unsigned long nxtdt;

  // check BMP/JPEG file
  char *ext = strrchr(fname, '.');
  // if(!ext || (memcmp(ext + 1, "BMP", 3) && memcmp(ext + 1, "JPG", 3) && memcmp(ext + 1, "MJP", 3))) return false;
  if (!ext) {
    fillScreen(ILI9486_PURPLE);
    return false;
  }
  if (!(!memcmp(ext + 1, "BMP", 3) || !memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3))) {
    fillScreen(ILI9486_YELLOW);
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
            printf1("time = %.4f\r\n", time1);
            printf1("fps = %.4f\r\n", fps_aver / count_fps);
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
        if(yofset > 0) fillRect(0, 0, _width, yofset, ILI9486_BLACK);
        // set left & right black belt
        if(xofset > 0){
          fillRect(0, (yofset > 0) ? yofset : 0, xofset, (yofset > 0) ? JpegDec.height : _height, ILI9486_BLACK);
          fillRect(xofset + JpegDec.width, (yofset > 0) ? yofset : 0, _width - JpegDec.width - xofset, (yofset > 0) ? JpegDec.height : _height, ILI9486_BLACK);
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
              dmaWait();

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
            if(x >= JpegDec.width) col = color01(ILI9486_BLACK);
            if(((xofset + x) >= 0) && ((yofset + y) >= 0) && ((xofset + x) < _width) && ((yofset + y) < _height))
              clbuf[clbufpt++] = col;
            pImg += JpegDec.comps;
          }
        }
        // draw picture
        if(clbufpt) pushColors((uint8_t *)clbuf, clbufpt);
      }

      // set bottom black belt
      if(yofset > 0) fillRect(0, yofset + JpegDec.height, _width, _height, ILI9486_BLACK);
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
    fp = SD.open(fname);
    fp.read(rdbuf, 34);
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
    struct rgb01 blkblt[_width]; memset(blkblt, 0, sizeof(blkblt));
    struct rgb01 blkblt2[_width];
    for(int y = 0 ; y < _height ; y++){
      // draw blank line
      if((_height > lheight) && ((y < dryofset) || (y >= (dryofset + drhight)))){
        dmaWait();
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
            // pushColor(ILI9486_BLACK);
            blkblt2[x] = color01(ILI9486_BLACK);
            continue;
          }
          // draw x line picture
          unsigned long rowidx = (lheight - ly - 1 + ((dryofset < 0) ? dryofset : 0)) * lpwidth +
                                                 ((drxofset < 0) ? -drxofset * 3 : 0);
          fp.seek(rowidx + offset);
          for(int lx = 0 ; lx < drwidth ; lx += RDPLEN){
            int rdpklen = ((lx + RDPLEN) < drwidth) ? RDPLEN : drwidth - lx;
            if(fp.read(rdbuf, rdpklen * 3) <= 0) goto rtnend;
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
  if(!memcmp(ext + 1, "BMP", 3)) fp.close();
  return rlt;
}

// Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture(char *fname) {
  return drawJpgBmppicture_mjpeg(fname, 10);
}

uint32_t color16(int color) {
    int r, g, b, color01;

    r = (color >> 16) & 0xf8;
    g = (color >> 8)  & 0xfc;
    b = (color)       & 0xf8;
    color01 = (r << 8) | (g << 3) | (b >> 3);

    return color01;
}

void drawPixel(int x, int y, int color) {
    display.drawPixel(x, y, color16(color));
}

int pushColor_x1, pushColor_y1, pushColor_x2, pushColor_y2, pushColor_left;
void resetPushColor(int x1, int y1, int x2, int y2) {
    pushColor_x1 = x1;
    pushColor_left = x1;
    pushColor_y1 = y1;
    pushColor_x2 = x2;
    pushColor_y2 = y2;
}

void setRotation(int mode) {
    display.setRotation(mode);
}

void pushColor(uint32_t color) {
  drawPixel(pushColor_x1, pushColor_y1, color);
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
      //color01 = ((((int)*p++) & 0xf8) << 8) | ((((int)*p++) & 0xfc) << 3) | ((((int)*p++) & 0xf8) >> 3);
	    drawPixel(pushColor_x1, pushColor_y1, color01);
	    pushColor_x1++;
	    if (pushColor_x1 == pushColor_x2 + 1) {
                pushColor_y1++;
                pushColor_x1 = pushColor_left;
	    }
	}
}

void fillRect(int x1, int y1, int x2, int y2, int color) {
  display.fillRect(x1, y1, x2, y2, color16(color));
}

void fillScreen(int color) {
  fillRect(0, 0, _width, _height, color);
}


inline static void dmaWait(void) {
}
