// https://i-satoh.hatenablog.com/entry/65438175
// ここのサイトのMotionJPEGの再生プログラムを参考にして
// Teensy 4.1で動画再生のテストをしてみました。
// ILI9341のLCD専用になってます。
// SDカードに保存されたMotionJPEGの動画を表示するものです。
// 動画は映像のみの再生です、音声は再生されません。
// 320x240の動画だと15FPSくらい、
// 240x160で24FPSくらいですが、長時間動画の再生はできてます。
// 対応している動画形式はMotionJPEGです。
// 拡張子は.MJPで保存してください。
// MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
// ffmpeg -i test_in1.mp4 -b:v 1500k -r 15.0 -s 320x240 -vcodec mjpeg -an test_o1.mov
// ffmpeg -i test_in2.mp4 -b:v 750k -r 24.0 -s 240x160 -vcodec mjpeg -an test_o2.mov
// 拡張子は.MJPで保存してください。
// 動画ファイルは付属しません。
// 各自用意した動画ファイルを再生するようにloop()を編集してください。
// 画面表示がされない場合、73行目あたりのLCDのクロック設定を10000000にしてみてください
// teensyのボードマネージャのバージョンが1.58.0及び1.58.1では正常に動作しないようです
// 1.57.2と1.56.1では正常に動作しました
 
#include <SPI.h>
#include <stdio.h>
#include <string.h>
 
// Teensy用のSPIDMAライブラリのTsyDMASPIを使用してます。TsyDMASPIは付属のものを使ってください
// https://github.com/hideakitai/TsyDMASPI
#include "TsyDMASPI.h"
 
// Teensy付属のSDライブラリを読み込むようにしてください
// 場所はC:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\SD\src
#include "SD.h"
 
// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoderのJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cを使ってます。
// https://i-satoh.hatenablog.com/entry/65438175
#include "JPEGDecoder.h"
 
#define DEFFRATE     5  // default frame rate
 
#define MOSI 11
#define SCK  13
#define CS   10
#define MISO 12
 
#define RST  19
#define DC   18
 
 
#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320
 
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198
 
#define spi SPI
 
const int tft_frequency = 30000000;
SPISettings _spi_settings = SPISettings(tft_frequency, MSBFIRST, SPI_MODE0);
 
uint8_t _tft_rst = RST;
uint8_t _tft_dc  = DC;
uint8_t _tft_cs  = CS;
uint8_t  rotation = 0;
int32_t _init_width = ILI9341_TFTWIDTH;
int32_t _init_height = ILI9341_TFTHEIGHT;
int32_t _width = _init_width;
int32_t _height = _init_height;
 
volatile bool   DMA_Enabled = false;
#define DMA_BUFF_SIZE 32766
volatile uint8_t __attribute__ ((aligned(4))) buffer00[DMA_BUFF_SIZE];
File root;
 
void tft_setup(void);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void setRotation(int mode);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillScreen(uint16_t color);
void drawRGBBitmap(int32_t x, int32_t y, uint8_t bitmap[], int32_t w, int32_t h);
void pushColor(uint16_t color);
void pushColors(uint8_t *data, int32_t size);
inline static void dmaWait(void);
inline static bool dmaBusy(void);
bool initDMA(void);
void deInitDMA(void);
void drawRGBBitmap_dma(int32_t x, int32_t y, uint8_t bitmap[], int32_t w, int32_t h);
void pushColors_dma(uint8_t *data, int32_t size);
void pushColors_dma_buffer00(int32_t size);
bool drawJpgBmppicture(char *fname);
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate);
void printDirectory(File dir, int numTabs);
void printf1(const char* format, ...);
 
void setup() {
  Serial.begin(115200);
 
  spi.begin();
  initDMA();
 
  tft_setup();
  setRotation(3);
  fillScreen(ILI9341_BLACK);
  if (SD.begin(BUILTIN_SDCARD)) {
    Serial.println("initialization done.");
    root = SD.open("/");
    printDirectory(root, 0);
    Serial.println("done!");
  } else {
    Serial.println("initialization failed!");
  }
}
 
void loop() {
    // drawJpgBmppicture_mjpeg(動画ファイル名, FPS)で指定します。
    // MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
    // ffmpeg -i test_in1.mp4 -b:v 1500k -r 15.0 -s 320x240 -vcodec mjpeg -an test_o1.mov
    // ffmpeg -i test_in2.mp4 -b:v 750k -r 24.0 -s 240x160 -vcodec mjpeg -an test_o2.mov
    // このプログラムはMotionJPEGのファイルの拡張子は.MJPとして認識するので拡張子を変更してください。
    // 320x240の動画だと15FPSくらいです。
    // 240x160で24FPSくらいでした。
    // JPEG画像やBMP画像の表示もできますが、表示されない場合は一度Windows付属のペイントで保存してから試してみてください。
 
    fillScreen(ILI9341_BLACK);
    drawJpgBmppicture_mjpeg("SNAIL01.JPG", 0);
    delay(5000);
    drawJpgBmppicture_mjpeg("SCENIC01.BMP", 0);
    delay(5000);
 
    fillScreen(ILI9341_BLACK);
    
    drawJpgBmppicture_mjpeg("W1_15FPS.MJP", 15);
    delay(5000);
 
    drawJpgBmppicture_mjpeg("W2_15FPS.MJP", 15);
    delay(5000);
 
    drawJpgBmppicture_mjpeg("W1_15FPS.MJP", 15);
    delay(5000);
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
      // files have sizes, directories do not
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
 
//    vsprintf(str_buff01, format, ap);
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
  if(!ext || (memcmp(ext + 1, "BMP", 3) && memcmp(ext + 1, "JPG", 3) && memcmp(ext + 1, "MJP", 3))) return false;
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
    uint16_t col = ILI9341_BLACK;
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
        if(yofset > 0) fillRect(0, 0, _width, yofset, ILI9341_BLACK);
        // set left & right black belt
        if(xofset > 0){
          fillRect(0, (yofset > 0) ? yofset : 0, xofset, (yofset > 0) ? JpegDec.height : _height, ILI9341_BLACK);
          fillRect(xofset + JpegDec.width, (yofset > 0) ? yofset : 0, _width - JpegDec.width - xofset, (yofset > 0) ? JpegDec.height : _height, ILI9341_BLACK);
        }
        inifrmset = false;
      }
      // start jpeg decode
      uint16_t clbuf[JpegDec.MCUWidth * JpegDec.MCUHeight];
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
 
              spi_begin();
              setAddrWindow(dxstrt, dystrt, dxend, dyend);
              
              // clear draw start flag
              drawstart = false;
            }
            // get & set pixel data
            if(JpegDec.comps == 1) // Grayscale
              col = (((unsigned short)pImg[0] >> 3) << 11) | (((unsigned short)pImg[0] >> 2) << 5) | (pImg[0] >> 3);
            else // RGB
              col = (((unsigned short)pImg[0] >> 3) << 11) | (((unsigned short)pImg[1] >> 2) << 5) | (pImg[2] >> 3);
            // check picture draw range & set color
            if(x >= JpegDec.width) col = ILI9341_BLACK;
            if(((xofset + x) >= 0) && ((yofset + y) >= 0) && ((xofset + x) < _width) && ((yofset + y) < _height))
               clbuf[clbufpt++] = (uint16_t)((col >> 8) & 0xff) | ((col << 8) & 0xff00);  // エンディアン変換
            pImg += JpegDec.comps;
          }
        }
        // draw picture
        if(clbufpt) pushColors_dma((uint8_t *)clbuf, clbufpt);
      }
 
      // set bottom black belt
      if(yofset > 0) fillRect(0, yofset + JpegDec.height, _width, _height, ILI9341_BLACK);
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
    uint16_t blkblt[_width]; memset(blkblt, 0, sizeof(blkblt));
    uint16_t blkblt2[_width];
    for(int y = 0 ; y < _height ; y++){
      // draw blank line
      if((_height > lheight) && ((y < dryofset) || (y >= (dryofset + drhight)))){
        dmaWait();
        spi_begin();
        setAddrWindow(0, y, _width-1, y);
        pushColors_dma((uint8_t *)blkblt, _width);
        continue;
      }
      // draw y line picture
      for(int ly = 0 ; ly < drhight ; ly++){
        // draw x line
        for(int x = 0 ; x < _width ; x++){
          // draw left & right blank
          if((_width > lwidth) && ((x < drxofset) || (x >= (drxofset + drwidth)))){
//            pushColor(ILI9341_BLACK);
            blkblt2[x] = ILI9341_BLACK;
            continue;
          }
          // draw x line picture
          unsigned long rowidx = (lheight - ly - 1 + ((dryofset < 0) ? dryofset : 0)) * lpwidth +
                                                 ((drxofset < 0) ? -drxofset * 3 : 0);
          fp.seek(rowidx + offset);
          uint16_t col01;
          for(int lx = 0 ; lx < drwidth ; lx += RDPLEN){
            int rdpklen = ((lx + RDPLEN) < drwidth) ? RDPLEN : drwidth - lx;
            if(fp.read(rdbuf, rdpklen * 3) <= 0) goto rtnend;
//            uint16_t clbuf[rdpklen];
            for(int ln = 0 ; ln < rdpklen ; ln++) {
              col01 = (((unsigned short)rdbuf[ln * 3 + 2] >> 3) << 11) |
                           (((unsigned short)rdbuf[ln * 3 + 1] >> 2) << 5) | (rdbuf[ln * 3] >> 3);
//              clbuf[ln] = (uint16_t)((col01 >> 8) & 0xff) | ((col01 << 8) & 0xff00);  // エンディアン変換
              blkblt2[x + lx + ln] = (uint16_t)((col01 >> 8) & 0xff) | ((col01 << 8) & 0xff00);  // エンディアン変換
            }
          }
          x += (drwidth - 1);
        }
        dmaWait();
        spi_begin();
        setAddrWindow(0, y + ly, _width-1, y + ly);
//        pushColors_dma((uint8_t *)clbuf, rdpklen);
        pushColors_dma((uint8_t *)blkblt2, _width);
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
 
#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09
 
#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13
 
#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0D
#define ILI9341_RDSELFDIAG  0x0F
 
#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29
 
#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E
 
#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_PIXFMT  0x3A
 
#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6
 
#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7
 
#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD
 
#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
 
 
#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04
  
inline void gpio_rst_on(void){
    digitalWriteFast(_tft_rst, LOW);
}
inline void gpio_rst_off(void){
    digitalWriteFast(_tft_rst, HIGH);
}
inline void gpio_dc_on(void){
    digitalWriteFast(_tft_dc, LOW);
}
inline void gpio_dc_off(void){
    digitalWriteFast(_tft_dc, HIGH);
}
inline void gpio_cs_on(void){
    digitalWriteFast(_tft_cs, LOW);
}
inline void gpio_cs_off(void){
    digitalWriteFast(_tft_cs, HIGH);
}
inline void writedata(uint8_t byte){
    spi.transfer(byte);
}
void writecommand(uint8_t byte) {
    gpio_dc_on();
    writedata(byte);
    gpio_dc_off();
}
inline void spi_write(uint8_t byte){
    writedata(byte);
}
 
inline void spi_write16(uint16_t word){
    spi.transfer((uint8_t)word);
    spi.transfer((uint8_t)(word >> 8));
}
 
void spi_begin() {
  gpio_cs_on();
  spi.beginTransaction(_spi_settings);
}
 
void spi_end() {
  spi.endTransaction();
  gpio_cs_off();
}
 
void tft_setup(){
  pinMode(_tft_dc, OUTPUT);
  pinMode(_tft_cs, OUTPUT);
  pinMode(_tft_rst, OUTPUT);
 
  gpio_rst_off();
  delay(200);
  gpio_rst_on();
  delay(200);
  gpio_rst_off();
  delay(200);
 
  spi_begin();
 
  writecommand(0xEF);
  writedata(0x03);
  writedata(0x80);
  writedata(0x02);
 
  writecommand(0xCF);  
  writedata(0x00); 
  writedata(0XC1); 
  writedata(0X30); 
 
  writecommand(0xED);  
  writedata(0x64); 
  writedata(0x03); 
  writedata(0X12); 
  writedata(0X81); 
 
  writecommand(0xE8);  
  writedata(0x85); 
  writedata(0x00); 
  writedata(0x78); 
 
  writecommand(0xCB);  
  writedata(0x39); 
  writedata(0x2C); 
  writedata(0x00); 
  writedata(0x34); 
  writedata(0x02); 
 
  writecommand(0xF7);  
  writedata(0x20); 
 
  writecommand(0xEA);  
  writedata(0x00); 
  writedata(0x00); 
 
  writecommand(ILI9341_PWCTR1);    //Power control 
  writedata(0x23);   //VRH[5:0] 
 
  writecommand(ILI9341_PWCTR2);    //Power control 
  writedata(0x10);   //SAP[2:0];BT[3:0] 
 
  writecommand(ILI9341_VMCTR1);    //VCM control 
  writedata(0x3e); //¶O±E¶Eμ÷?U
  writedata(0x28); 
  
  writecommand(ILI9341_VMCTR2);    //VCM control2 
  writedata(0x86);  //--
 
  writecommand(ILI9341_MADCTL);    // Memory Access Control 
  writedata(0x48);
 
  writecommand(ILI9341_PIXFMT);    
  writedata(0x55); 
  
  writecommand(ILI9341_FRMCTR1);    
  writedata(0x00);  
  writedata(0x18); 
 
  writecommand(ILI9341_DFUNCTR);    // Display Function Control 
  writedata(0x08); 
  writedata(0x82);
  writedata(0x27);  
 
  writecommand(0xF2);    // 3Gamma Function Disable 
  writedata(0x00); 
 
  writecommand(ILI9341_GAMMASET);    //Gamma curve selected 
  writedata(0x01); 
 
  writecommand(ILI9341_GMCTRP1);    //Set Gamma 
  writedata(0x0F); 
  writedata(0x31); 
  writedata(0x2B); 
  writedata(0x0C); 
  writedata(0x0E); 
  writedata(0x08); 
  writedata(0x4E); 
  writedata(0xF1); 
  writedata(0x37); 
  writedata(0x07); 
  writedata(0x10); 
  writedata(0x03); 
  writedata(0x0E); 
  writedata(0x09); 
  writedata(0x00); 
  
  writecommand(ILI9341_GMCTRN1);    //Set Gamma 
  writedata(0x00); 
  writedata(0x0E); 
  writedata(0x14); 
  writedata(0x03); 
  writedata(0x11); 
  writedata(0x07); 
  writedata(0x31); 
  writedata(0xC1); 
  writedata(0x48); 
  writedata(0x08); 
  writedata(0x0F); 
  writedata(0x0C); 
  writedata(0x31); 
  writedata(0x36); 
  writedata(0x0F); 
 
  writecommand(ILI9341_SLPOUT);    //Exit Sleep 
  spi_end();
  delay(120);        
  spi_begin();
  writecommand(ILI9341_DISPON);    //Display on 
  spi_end();
}
 
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  gpio_dc_on();
  spi_write(ILI9341_CASET); // Column addr set
  gpio_dc_off();
  spi_write(x0 >> 8);
  spi_write(x0);
  spi_write(x1 >> 8);
  spi_write(x1);
 
  gpio_dc_on();
  spi_write(ILI9341_PASET); // Row addr set
  gpio_dc_off();
  spi_write(y0>>8);
  spi_write(y0);     // YSTART
  spi_write(y1>>8);
  spi_write(y1);     // YEND
 
  gpio_dc_on();
  spi_write(ILI9341_RAMWR); // write to RAM
  gpio_dc_off();
}
 
void setRotation(int mode) {
    int rotation;
 
    dmaWait();
    
    spi_begin();
 
    writecommand(ILI9341_MADCTL);
    rotation = mode % 8;
    switch (rotation) {
     case 0: // Portrait
       writedata(ILI9341_MADCTL_BGR | ILI9341_MADCTL_MX);
       _width  = _init_width;
       _height = _init_height;
       break;
     case 1: // Landscape (Portrait + 90)
       writedata(ILI9341_MADCTL_BGR | ILI9341_MADCTL_MV);
       _width  = _init_height;
       _height = _init_width;
       break;
     case 2: // Inverter portrait
       writedata(ILI9341_MADCTL_BGR | ILI9341_MADCTL_MY);
       _width  = _init_width;
       _height = _init_height;
      break;
     case 3: // Inverted landscape
       writedata(ILI9341_MADCTL_BGR | ILI9341_MADCTL_MV | ILI9341_MADCTL_MX | ILI9341_MADCTL_MY);
       _width  = _init_height;
       _height = _init_width;
       break;
    }
    delayMicroseconds(10);
    spi_end();
}
 
void pushColor(uint16_t color) {
  dmaWait();
        
  spi.transfer((uint8_t)color);
  spi.transfer((uint8_t)(color >> 8));
}
 
void fill_color(uint16_t color, uint32_t len) {
  uint8_t hi = color >> 8;
  uint8_t lo = color;
  for(uint32_t i=0; i<len;i++) {
    writedata(hi);
    writedata(lo);
  }
}
 
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if(w > 0 && h > 0){
    if((x >= _width) || (y >= _height)) return;
    if((x + w - 1) >= _width)  w = _width  - x;
    if((y + h - 1) >= _height) h = _height - y;
    
    dmaWait();
 
    spi_begin();
    setAddrWindow(x, y, x+w-1, y+h-1);
    fill_color(color, w * h);
    spi_end();
  }
}
 
void fillScreen(uint16_t color) {
  fillRect(0, 0, _width, _height, color);
}
 
void drawRGBBitmap(int32_t x, int32_t y, uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint16_t *p = (uint16_t *)bitmap;
 
    dmaWait();
 
    spi_begin();
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
          spi_write16(*p++);
        }
    }
    spi_end();
}
 
inline static void dmaWait(void) {
    if (!DMA_Enabled) return;
    TsyDMASPI0.yield();
}
 
inline static bool dmaBusy(void) {
    if (!DMA_Enabled) return false;
    size_t size;
    size =  TsyDMASPI0.remained();
    if (size == 0) {
      return false;
    }
    return true;
}
 
void deInitDMA(void) {
    if (!DMA_Enabled) return;
    TsyDMASPI0.end();
    DMA_Enabled = false;
}
 
bool initDMA(void) {
    if (DMA_Enabled == true) return false;
 
    TsyDMASPI0.begin(CS, _spi_settings);
    DMA_Enabled = true;
    return true;
}
 
void drawRGBBitmap_dma(int32_t x, int32_t y, uint8_t bitmap[], int32_t w, int32_t h) {
    uint8_t  *p;
 
    dmaWait();
 
    spi_end();
    spi_begin();
    setAddrWindow(x, y, x + w - 1, y + h - 1);
 
    int len = w * h * 2;
    
    p = bitmap;
 
    if (len > DMA_BUFF_SIZE) {
        for (int i = 0; i < len / DMA_BUFF_SIZE; i++) { 
            dmaWait();
            memcpy(buffer00, p, DMA_BUFF_SIZE);
            TsyDMASPI0.transfer((volatile uint8_t *)buffer00, DMA_BUFF_SIZE);
            p += DMA_BUFF_SIZE;
        }
        if (len % DMA_BUFF_SIZE > 0) {
            dmaWait();
            memcpy(buffer00, p, len % DMA_BUFF_SIZE);
            TsyDMASPI0.transfer((volatile uint8_t *)buffer00, len % DMA_BUFF_SIZE);
        }
    } else {
        memcpy(buffer00, p, len);
        TsyDMASPI0.transfer((volatile uint8_t *)buffer00, len);
    }
}
 
void pushColors(uint8_t *data, int32_t size) {
    uint8_t  *p;
    uint8_t  lo, hi;
 
    dmaWait();
 
    p = data;
    for (int32_t i = 0; i < size; i++) {
        hi = *p++;
        lo = *p++;
        spi.transfer(hi);
        spi.transfer(lo);
    }
}
 
void pushColors_dma(uint8_t *data, int32_t size) {
    uint8_t  *p;
 
 
    dmaWait();
 
    p = data;
    int len = size * 2;
    if (len > DMA_BUFF_SIZE) {
        for (int i = 0; i < len / DMA_BUFF_SIZE; i++) { 
            dmaWait();
            memcpy(buffer00, p, DMA_BUFF_SIZE);
            TsyDMASPI0.transfer((volatile uint8_t *)buffer00, DMA_BUFF_SIZE);
            p += DMA_BUFF_SIZE;
        }
        if (len % DMA_BUFF_SIZE > 0) {
            dmaWait();
            memcpy(buffer00, p, len % DMA_BUFF_SIZE);
            TsyDMASPI0.transfer((volatile uint8_t *)buffer00, len % DMA_BUFF_SIZE);
        }
    } else {
        memcpy(buffer00, p, len);
        TsyDMASPI0.transfer((volatile uint8_t *)buffer00, len);
    }
}