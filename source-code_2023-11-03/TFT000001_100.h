// TFT000001.h
// Version : 0.0.2
//
//    ILI9486, ILI9341 LCD Graphics Library for Raspberry Pi Pico
//                                         https://twitter.com/yama23238
//
//    今まで作ったものを利用しやすい形にまとめてライブラリにしてみました。
//    まだ試作段階ですのでご了承ください。
//    一応動いたので公開しました。
//    試行錯誤で制作しているので、今後、作り直しや互換性に問題が出ることがあり得ます。
//    
//    yama_2_GFX_H_beta and yama_2_GFX_H_beta_Canvas is based on Adafruit GFX.
//    Copyright (c) 2012 Adafruit Industries.
//    Released under the BSD License
//    https://github.com/adafruit/Adafruit-GFX-Library/blob/master/license.txt
//
//    init_ILI9341,pixel_write16,pixel_write,fill_color16,fillRect16,
//    drawPixel16,drawRGBBitmap16,drawFastVLine16,drawFastVLine24,drawFastHLine16,
//    drawFastHLine24,drawBitmap16,drawBitmap24 in the TFT000001 class is:
//    Copyright (c) 2016 Hiroshi Narimatsu
//    Released under the MIT License
//    https://github.com/h-nari/Humblesoft_ILI9341/blob/master/LICENSE
//
//    set_rotation_ILI9486,set_rotation_ILI9341,init_ILI9486,setAddrWindow,
//    setWindow, pioinit in the TFT000001 class 
//    and pio_8bit_parallel.pio is :
//    Copyright (c) 2020 Bodmer (https://github.com/Bodmer)
//    Software License Agreement (FreeBSD License)
//    https://github.com/Bodmer/TFT_eSPI/blob/master/license.txt
//
//      これらのプログラムの使用に当たってはご自分の責任において使用してください
//      これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/omapfb.h>

#ifndef _YAMA_2_GFX_H_BETA_H
#define _YAMA_2_GFX_H_BETA_H
 
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_BLACK      0xd7     ///< 0x0000    0,   0,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_NAVY       0xda     ///< 0x0010    0,   0, 128
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_DARKGREEN  0xdc     ///< 0x0400    0, 128,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_DARKCYAN   0xdb     ///< 0x0410    0, 125, 123
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_MAROON     0xdf     ///< 0x8000  128,   0,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_PURPLE     0xde     ///< 0x8010  128,   0, 128
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_OLIVE      0xdd     ///< 0x8400  128, 128,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_LIGHTGREY  0xd9     ///< 0xc618  198, 195, 198
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_DARKGREY   0xd8     ///< 0x8410  128, 128, 128
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_BLUE       0xd2     ///< 0x001f    0,   0, 255
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_GREEN      0xd2     ///< 0x07e0    0, 255,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_CYAN       0xb4     ///< 0x07ff    0, 255, 255
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_RED        0x23     ///< 0xf800  255,   0,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_MAGENTA    0x1e     ///< 0xf81f  255,   0, 255
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_YELLOW     0x05     ///< 0xffe0  255, 255,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_WHITE      0x00     ///< 0xffff  255, 255, 255
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_ORANGE     0x11     ///< 0xfcc0  255, 152,   0
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_GREENYELLOW  0x4c   ///< 0x9fe6  152, 255,  48
#define YAMA_2_GFX_H_BETA_WEB216_PALETTE_PINK       0x13     ///< 0xfb59  255, 104, 200


#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_BLACK      0x00     ///< 0x0000    0,   0,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_NAVY       0x04     ///< 0x0010    0,   0, 128
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_DARKGREEN  0x02     ///< 0x0400    0, 128,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_DARKCYAN   0x06     ///< 0x0410    0, 125, 123
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_MAROON     0x01     ///< 0x8000  128,   0,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_PURPLE     0x05     ///< 0x8010  128,   0, 128
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_OLIVE      0x03     ///< 0x8400  128, 128,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_LIGHTGREY  0x07     ///< 0xc618  198, 195, 198
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_DARKGREY   0xf8     ///< 0x8410  128, 128, 128
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_BLUE       0xfc     ///< 0x001f    0,   0, 255
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_GREEN      0xfa     ///< 0x07e0    0, 255,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_CYAN       0xfe     ///< 0x07ff    0, 255, 255
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_RED        0xf9     ///< 0xf800  255,   0,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_MAGENTA    0xfd     ///< 0xf81f  255,   0, 255
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_YELLOW     0xfb     ///< 0xffe0  255, 255,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_WHITE      0xff     ///< 0xffff  255, 255, 255
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_ORANGE     0xc6     ///< 0xfce0  255, 156,   0
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_GREENYELLOW  0x8d   ///< 0xafea  168, 255,  80
#define YAMA_2_GFX_H_BETA_DEFAULT_PALETTE_PINK       0xc0     ///< 0xfaf5  255,  92, 168

#define YAMA_2_GFX_H_BETA_16BIT_COLOR_BLACK 0x0000       ///<   0,   0,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_NAVY 0x000F        ///<   0,   0, 123
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_DARKGREEN 0x03E0   ///<   0, 125,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_DARKCYAN 0x03EF    ///<   0, 125, 123
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_MAROON 0x7800      ///< 123,   0,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_PURPLE 0x780F      ///< 123,   0, 123
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_OLIVE 0x7BE0       ///< 123, 125,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_LIGHTGREY 0xC618   ///< 198, 195, 198
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_DARKGREY 0x7BEF    ///< 123, 125, 123
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_BLUE 0x001F        ///<   0,   0, 255
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_GREEN 0x07E0       ///<   0, 255,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_CYAN 0x07FF        ///<   0, 255, 255
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_RED 0xF800         ///< 255,   0,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_MAGENTA 0xF81F     ///< 255,   0, 255
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_YELLOW 0xFFE0      ///< 255, 255,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_WHITE 0xFFFF       ///< 255, 255, 255
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_ORANGE 0xFD20      ///< 255, 165,   0
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define YAMA_2_GFX_H_BETA_16BIT_COLOR_PINK 0xFC18        ///< 255, 130, 198

#define YAMA_2_GFX_H_BETA_24BIT_COLOR_BLACK      0x000000 ///<   0,   0,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_NAVY       0x000080 ///<   0,   0, 128
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_DARKGREEN  0x008000 ///<   0, 128,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_DARKCYAN   0x008080 ///<   0, 128, 128
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_MAROON     0x800000 ///< 128,   0,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_PURPLE     0x800080 ///< 128,   0, 128
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_OLIVE      0x808000 ///< 128, 128,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_LIGHTGREY  0xC6C3C6 ///< 198, 195, 198
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_DARKGREY   0x808080 ///< 128, 128, 128
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_BLUE       0x0000FF ///<   0,   0, 255
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_GREEN      0x00FF00 ///<   0, 255,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_CYAN       0x00FFFF ///<   0, 255, 255
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_RED        0xFF0000 ///< 255,   0,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_MAGENTA    0xFF00FF ///< 255,   0, 255
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_YELLOW     0xFFFF00 ///< 255, 255,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_WHITE      0xFFFFFF ///< 255, 255, 255
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_ORANGE     0xFFA500 ///< 255, 165,   0
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_GREENYELLO 0xADFF29 ///< 173, 255,  41
#define YAMA_2_GFX_H_BETA_24BIT_COLOR_PINK       0xFF82C6 ///< 255, 130, 198

#define yama_2_GFX_H_beta_min(a, b) (((a) < (b)) ? (a) : (b))
#define yama_2_GFX_H_beta_swap_int32_t(a, b)                                                    \
{                                                                              \
    int32_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
}
#define yama_2_GFX_H_beta_abs(a)    ((a >= 0) ? a : (-(a)))


struct yama_2_GFX_H_beta_RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

class yama_2_GFX_H_beta {
protected:
    int32_t  WIDTH;
    int32_t  HEIGHT;
    int32_t  _width;
    int32_t  _height;
    uint8_t  rotation = 0;


    inline int bit_test(const uint8_t *data, int n) {
        return (data[n >> 3] >> (7 - (n & 7))) & 1;
    }


public:
    yama_2_GFX_H_beta(int32_t w, int32_t h);
    void init_yama_2_GFX_beta(int32_t w, int32_t h, int32_t colorMode);
    virtual void drawPixel(int32_t x, int32_t y, uint32_t color) = 0;
    virtual void startWrite(void);
    virtual void writePixel(int32_t x, int32_t y, uint32_t color);
    virtual void writeFillRect(int32_t x, int32_t y, int32_t w, int32_t h,
                             uint32_t color);
    virtual void writeFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
    virtual void writeFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
    virtual void writeLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                         uint32_t color);
    virtual void endWrite(void);
    virtual void setRotation(uint8_t r);

    virtual void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
    virtual void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
    virtual void fillRect(int32_t x, int32_t y, int32_t w, int32_t h,
                        uint32_t color);
    virtual void fillScreen(uint32_t color);
    virtual void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                        uint32_t color);
    virtual void drawRect(int32_t x, int32_t y, int32_t w, int32_t h,
                        uint32_t color);
    virtual void drawCircle(int32_t x0, int32_t y0, int32_t r,
                                  uint32_t color);
    virtual void drawCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                        uint8_t cornername, uint32_t color);
    virtual void fillCircle(int32_t x0, int32_t y0, int32_t r,
                                  uint32_t color);
    virtual void fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                        uint8_t corners, int32_t delta,
                                        uint32_t color);
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h,
                                     int32_t r, uint32_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h,
                                     int32_t r, uint32_t color);
    void drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                                    int32_t x2, int32_t y2, uint32_t color);
    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                                    int32_t x2, int32_t y2, uint32_t color);
    void drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                  int32_t w, int32_t h, uint32_t color);
    void drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                  int32_t w, int32_t h, uint32_t color,
                                  uint32_t bg);
    void drawBitmap(int32_t x, int32_t y, uint8_t *bitmap, int32_t w,
                                  int32_t h, uint32_t color);
    void drawBitmap(int32_t x, int32_t y, uint8_t *bitmap, int32_t w,
                                  int32_t h, uint32_t color, uint32_t bg);


    void drawBitmap(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg);
    void drawBitmap(int32_t x, int32_t y, uint8_t *bitmap01, uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg);


    void drawXBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                   int32_t w, int32_t h, uint32_t color);
    void drawGrayscaleBitmap8(int32_t x, int32_t y,
                                           const uint8_t bitmap[], int32_t w,
                                           int32_t h);
    void drawGrayscaleBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                           int32_t w, int32_t h);
    void drawGrayscaleBitmap8(int32_t x, int32_t y,
                                           const uint8_t bitmap[],
                                           const uint8_t mask[], int32_t w,
                                           int32_t h);
    void drawGrayscaleBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                           uint8_t *mask, int32_t w, int32_t h);


    void drawRGBBitmap8(int32_t x, int32_t y,
                                           const uint8_t bitmap[], int32_t w,
                                           int32_t h);
    void drawRGBBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                           int32_t w, int32_t h);
    void drawRGBBitmap8(int32_t x, int32_t y,
                                           const uint8_t bitmap[],
                                           const uint8_t mask[], int32_t w,
                                           int32_t h);
    void drawRGBBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                           uint8_t *mask, int32_t w, int32_t h);


    void setColorMode8(void);
    void setColorMode16(void);
    void setColorMode24(void);
    int32_t getColorMode(void);

    void drawRGBBitmap16(int32_t x, int32_t y, const uint8_t _bitmap[],
                                     int32_t w, int32_t h);
    void drawRGBBitmap16(int32_t x, int32_t y, uint8_t *_bitmap,
                                     int32_t w, int32_t h);
    void drawRGBBitmap16(int32_t x, int32_t y, const uint8_t _bitmap[],
                                     const uint8_t mask[], int32_t w, int32_t h);
    void drawRGBBitmap16(int32_t x, int32_t y, uint8_t *_bitmap,
                                     uint8_t *mask, int32_t w, int32_t h);

    void drawRGBBitmap24(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t w, int32_t h);
    void drawRGBBitmap24(int32_t x, int32_t y, const uint8_t bitmap[],
                                     const uint8_t mask[], int32_t w, int32_t h);

    void drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height);
    void drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height);
    void drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height, void *palette_address);
    void drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height, void *palette_address);

    void drawPalette256Bitmap16(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height, uint16_t *palette_address = NULL);
    void drawPalette256Bitmap16(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height, uint16_t *palette_address = NULL);
    void drawPalette256Bitmap24(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height, uint32_t *palette_address = NULL);
    void drawPalette256Bitmap24(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height, uint32_t *palette_address = NULL);

    void drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
    void drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
    void drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
    void drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);

    void setDefaultPalette256();
    void setWeb216Palette256();
    void setPalette256(uint8_t palette_num, uint32_t color);
    uint32_t getPalette256(uint8_t palette_num);
    void setPalette256(uint8_t palette_num, uint32_t color, int32_t colorMode);
    uint32_t getPalette256(uint8_t palette_num, int32_t colorMode);

    void setDefaultPalette256_16();
    void setWeb216Palette256_16();
    void setPalette256_16(uint8_t palette_num, uint16_t color);
    uint32_t getPalette256_16(uint8_t palette_num);

    void setDefaultPalette256_24();
    void setWeb216Palette256_24();
    void setPalette256_24(uint8_t palette_num, uint32_t color);
    uint32_t getPalette256_24(uint8_t palette_num);

    void *getPaletteTable256();
    void *getPaletteTable256(int32_t colorMode);
    uint16_t *getPaletteTable256_16();
    uint32_t *getPaletteTable256_24();

    uint32_t width(void);
    uint32_t height(void);
 
    uint16_t color565(uint32_t colr, uint32_t colg, uint32_t colb);
    uint16_t color565(uint32_t color);
    uint32_t colorRGB24(uint32_t colr, uint32_t colg, uint32_t colb);
    uint32_t color(uint32_t colr, uint32_t colg, uint32_t colb);
    uint32_t color(uint32_t color);
    uint32_t color16(uint16_t color);
    uint32_t color16to24(uint16_t color);
    uint16_t color24to16(uint32_t color);

protected:
    int32_t colorModeflag;
    static const int palette_number = 256;
    uint16_t __attribute__ ((aligned(4))) palette256_table[palette_number];
    uint32_t __attribute__ ((aligned(4))) palette256_24_table[palette_number];
    static const uint16_t default_palette256_data[];
    static const uint16_t web216_palette256_data[];
    static const uint8_t default_palette256_24_data[];
    static const uint8_t web216_palette256_24_data[];
};
 
#endif // _YAMA_2_GFX_H_BETA_H




#ifndef _TFT000001_H
#define _TFT000001_H

#define TFT000001_WEB216_PALETTE_BLACK      0xd7     ///< 0x0000    0,   0,   0
#define TFT000001_WEB216_PALETTE_NAVY       0xda     ///< 0x0010    0,   0, 128
#define TFT000001_WEB216_PALETTE_DARKGREEN  0xdc     ///< 0x0400    0, 128,   0
#define TFT000001_WEB216_PALETTE_DARKCYAN   0xdb     ///< 0x0410    0, 125, 123
#define TFT000001_WEB216_PALETTE_MAROON     0xdf     ///< 0x8000  128,   0,   0
#define TFT000001_WEB216_PALETTE_PURPLE     0xde     ///< 0x8010  128,   0, 128
#define TFT000001_WEB216_PALETTE_OLIVE      0xdd     ///< 0x8400  128, 128,   0
#define TFT000001_WEB216_PALETTE_LIGHTGREY  0xd9     ///< 0xc618  198, 195, 198
#define TFT000001_WEB216_PALETTE_DARKGREY   0xd8     ///< 0x8410  128, 128, 128
#define TFT000001_WEB216_PALETTE_BLUE       0xd2     ///< 0x001f    0,   0, 255
#define TFT000001_WEB216_PALETTE_GREEN      0xd2     ///< 0x07e0    0, 255,   0
#define TFT000001_WEB216_PALETTE_CYAN       0xb4     ///< 0x07ff    0, 255, 255
#define TFT000001_WEB216_PALETTE_RED        0x23     ///< 0xf800  255,   0,   0
#define TFT000001_WEB216_PALETTE_MAGENTA    0x1e     ///< 0xf81f  255,   0, 255
#define TFT000001_WEB216_PALETTE_YELLOW     0x05     ///< 0xffe0  255, 255,   0
#define TFT000001_WEB216_PALETTE_WHITE      0x00     ///< 0xffff  255, 255, 255
#define TFT000001_WEB216_PALETTE_ORANGE     0x11     ///< 0xfcc0  255, 152,   0
#define TFT000001_WEB216_PALETTE_GREENYELLOW  0x4c   ///< 0x9fe6  152, 255,  48
#define TFT000001_WEB216_PALETTE_PINK       0x13     ///< 0xfb59  255, 104, 200


#define TFT000001_DEFAULT_PALETTE_BLACK      0x00     ///< 0x0000    0,   0,   0
#define TFT000001_DEFAULT_PALETTE_NAVY       0x04     ///< 0x0010    0,   0, 128
#define TFT000001_DEFAULT_PALETTE_DARKGREEN  0x02     ///< 0x0400    0, 128,   0
#define TFT000001_DEFAULT_PALETTE_DARKCYAN   0x06     ///< 0x0410    0, 125, 123
#define TFT000001_DEFAULT_PALETTE_MAROON     0x01     ///< 0x8000  128,   0,   0
#define TFT000001_DEFAULT_PALETTE_PURPLE     0x05     ///< 0x8010  128,   0, 128
#define TFT000001_DEFAULT_PALETTE_OLIVE      0x03     ///< 0x8400  128, 128,   0
#define TFT000001_DEFAULT_PALETTE_LIGHTGREY  0x07     ///< 0xc618  198, 195, 198
#define TFT000001_DEFAULT_PALETTE_DARKGREY   0xf8     ///< 0x8410  128, 128, 128
#define TFT000001_DEFAULT_PALETTE_BLUE       0xfc     ///< 0x001f    0,   0, 255
#define TFT000001_DEFAULT_PALETTE_GREEN      0xfa     ///< 0x07e0    0, 255,   0
#define TFT000001_DEFAULT_PALETTE_CYAN       0xfe     ///< 0x07ff    0, 255, 255
#define TFT000001_DEFAULT_PALETTE_RED        0xf9     ///< 0xf800  255,   0,   0
#define TFT000001_DEFAULT_PALETTE_MAGENTA    0xfd     ///< 0xf81f  255,   0, 255
#define TFT000001_DEFAULT_PALETTE_YELLOW     0xfb     ///< 0xffe0  255, 255,   0
#define TFT000001_DEFAULT_PALETTE_WHITE      0xff     ///< 0xffff  255, 255, 255
#define TFT000001_DEFAULT_PALETTE_ORANGE     0xc6     ///< 0xfce0  255, 156,   0
#define TFT000001_DEFAULT_PALETTE_GREENYELLOW  0x8d   ///< 0xafea  168, 255,  80
#define TFT000001_DEFAULT_PALETTE_PINK       0xc0     ///< 0xfaf5  255,  92, 168

#define TFT000001_16BIT_COLOR_BLACK 0x0000       ///<   0,   0,   0
#define TFT000001_16BIT_COLOR_NAVY 0x000F        ///<   0,   0, 123
#define TFT000001_16BIT_COLOR_DARKGREEN 0x03E0   ///<   0, 125,   0
#define TFT000001_16BIT_COLOR_DARKCYAN 0x03EF    ///<   0, 125, 123
#define TFT000001_16BIT_COLOR_MAROON 0x7800      ///< 123,   0,   0
#define TFT000001_16BIT_COLOR_PURPLE 0x780F      ///< 123,   0, 123
#define TFT000001_16BIT_COLOR_OLIVE 0x7BE0       ///< 123, 125,   0
#define TFT000001_16BIT_COLOR_LIGHTGREY 0xC618   ///< 198, 195, 198
#define TFT000001_16BIT_COLOR_DARKGREY 0x7BEF    ///< 123, 125, 123
#define TFT000001_16BIT_COLOR_BLUE 0x001F        ///<   0,   0, 255
#define TFT000001_16BIT_COLOR_GREEN 0x07E0       ///<   0, 255,   0
#define TFT000001_16BIT_COLOR_CYAN 0x07FF        ///<   0, 255, 255
#define TFT000001_16BIT_COLOR_RED 0xF800         ///< 255,   0,   0
#define TFT000001_16BIT_COLOR_MAGENTA 0xF81F     ///< 255,   0, 255
#define TFT000001_16BIT_COLOR_YELLOW 0xFFE0      ///< 255, 255,   0
#define TFT000001_16BIT_COLOR_WHITE 0xFFFF       ///< 255, 255, 255
#define TFT000001_16BIT_COLOR_ORANGE 0xFD20      ///< 255, 165,   0
#define TFT000001_16BIT_COLOR_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define TFT000001_16BIT_COLOR_PINK 0xFC18        ///< 255, 130, 198

#define TFT000001_24BIT_COLOR_BLACK      0x000000 ///<   0,   0,   0
#define TFT000001_24BIT_COLOR_NAVY       0x000080 ///<   0,   0, 128
#define TFT000001_24BIT_COLOR_DARKGREEN  0x008000 ///<   0, 128,   0
#define TFT000001_24BIT_COLOR_DARKCYAN   0x008080 ///<   0, 128, 128
#define TFT000001_24BIT_COLOR_MAROON     0x800000 ///< 128,   0,   0
#define TFT000001_24BIT_COLOR_PURPLE     0x800080 ///< 128,   0, 128
#define TFT000001_24BIT_COLOR_OLIVE      0x808000 ///< 128, 128,   0
#define TFT000001_24BIT_COLOR_LIGHTGREY  0xC6C3C6 ///< 198, 195, 198
#define TFT000001_24BIT_COLOR_DARKGREY   0x808080 ///< 128, 128, 128
#define TFT000001_24BIT_COLOR_BLUE       0x0000FF ///<   0,   0, 255
#define TFT000001_24BIT_COLOR_GREEN      0x00FF00 ///<   0, 255,   0
#define TFT000001_24BIT_COLOR_CYAN       0x00FFFF ///<   0, 255, 255
#define TFT000001_24BIT_COLOR_RED        0xFF0000 ///< 255,   0,   0
#define TFT000001_24BIT_COLOR_MAGENTA    0xFF00FF ///< 255,   0, 255
#define TFT000001_24BIT_COLOR_YELLOW     0xFFFF00 ///< 255, 255,   0
#define TFT000001_24BIT_COLOR_WHITE      0xFFFFFF ///< 255, 255, 255
#define TFT000001_24BIT_COLOR_ORANGE     0xFFA500 ///< 255, 165,   0
#define TFT000001_24BIT_COLOR_GREENYELLO 0xADFF29 ///< 173, 255,  41
#define TFT000001_24BIT_COLOR_PINK       0xFF82C6 ///< 255, 130, 198



#define TFT000001_TFT_WIDTH   1920
#define TFT000001_TFT_HEIGHT  1080



class TFT000001 : public yama_2_GFX_H_beta {
public:

    TFT000001(int32_t width, int32_t height);


public:

void getFrameBufferSize(int* width, int* height, int* colorWidth);
void getFrameBufferSize2(int* width, int* height, int* colorWidth);

void init();
void deinit();

inline void pixel_write16(uint16_t color);
void writeLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                             uint32_t color);
void pixel_write(int32_t x, int32_t y, uint32_t color);
void fill_color16(uint16_t color, uint32_t len);
void fill_color24(uint32_t color, uint32_t len);
void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void fillRect16(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void fillRect24(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void drawPixel(int32_t x, int32_t y, uint32_t color);
void drawPixel_framebuff(uint32_t *framebuffer, int32_t x, int32_t y, uint32_t color);
void drawPixel16(int32_t x, int32_t y, uint32_t color);
void drawPixel16a(int32_t x, int32_t y, uint32_t color);
void drawRGBBitmap16to16(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void drawPixel24(int32_t x, int32_t y, uint32_t color);
void drawPixel24a(int32_t x, int32_t y, uint32_t color);
void pixel_write24(int32_t x, int32_t y, uint32_t color);
void drawRGBBitmap24to24(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void drawCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint32_t color);
void writeFastVLine_framebuff(uint32_t *frameBuffer, int32_t x, int32_t y, int32_t h, uint32_t color);
void writeFastHLine_framebuff(uint32_t *frameBuffer, int32_t x, int32_t y, int32_t w, uint32_t color);
void fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color);
void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                                    int32_t x2, int32_t y2, uint32_t color);



void drawPalette256Bitmap16(int32_t x, int32_t y, const uint8_t bitmap[], uint32_t w, uint32_t h, uint16_t *palette_address = NULL);
void drawPalette256Bitmap24(int32_t x, int32_t y, const uint8_t bitmap[], uint32_t w, uint32_t h, uint32_t *palette_address = NULL);

void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
void drawFastVLine16(int32_t x, int32_t y, int32_t h, uint32_t color);
void drawFastVLine24(int32_t x, int32_t y, int32_t h, uint32_t color);
void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
void drawFastHLine16(int32_t x, int32_t y, int32_t w, uint32_t color);
void drawFastHLine24(int32_t x, int32_t y, int32_t w, uint32_t color);
void drawBitmap16(int32_t x, int32_t y,const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color, uint32_t bg);
void drawBitmap24(int32_t x, int32_t y,const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color, uint32_t bg);
void drawGrayscaleBitmap(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
void drawGrayscaleBitmap(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
void drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
void drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
void drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
void drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert);
void drawRGBBitmap16to16(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[],
                       int32_t w, int32_t h);
void drawRGBBitmap24to24(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[],
                       int32_t w, int32_t h);

void drawBitmap(int32_t x, int32_t y,const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color, uint32_t bg);
void drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                       int32_t w, int32_t h, uint32_t color);
void drawBitmap(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg);

void drawBitmap16(int32_t x, int32_t y, const uint8_t bitmap[],
                       int32_t w, int32_t h, uint32_t color);
void drawBitmap24(int32_t x, int32_t y, const uint8_t bitmap[],
                       int32_t w, int32_t h, uint32_t color);
void drawBitmap16(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg);
void drawBitmap24(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg);

void drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height);
void drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height);
void drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t w, int32_t h, void *palette_address);
void drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t w, int32_t h, void *palette_address);

void drawPalette256Bitmap16(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t w, int32_t h, uint16_t *palette_address = NULL);
void drawPalette256Bitmap24(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t w, int32_t h, uint32_t *palette_address = NULL);

void drawRGBBitmap16(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void drawRGBBitmap24(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void drawRGBBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[],
                       int32_t w, int32_t h);
void drawRGBBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[],
                       int32_t w, int32_t h);

void drawRGBBitmap24to16(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void drawRGBBitmap16to24(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void drawRGBBitmap24to16(int32_t x, int32_t y, const uint8_t bitmap[],
                        const uint8_t bitmap_mask[], int32_t w, int32_t h);
void drawRGBBitmap16to24(int32_t x, int32_t y, const uint8_t bitmap[],
                        const uint8_t bitmap_mask[], int32_t w, int32_t h);

inline uint32_t color16to24inline(uint16_t color) {
    uint32_t colr, colg, colb, col;
    colr = ((((uint32_t)color >> 11) & 0x1f) << 3);
    colg = ((((uint32_t)color >> 5) & 0x3f) << 2);
    colb = (((uint32_t)color & 0x1f) << 3);
    colr = (colr != 0) ? (colr | 0x07) : colr;
    colg = (colg != 0) ? (colg | 0x03) : colg;
    colb = (colb != 0) ? (colb | 0x07) : colb;
    col = (colr << 16) | (colg << 8) | colb;
    return col;
}

uint32_t get_bit18_flag();

int32_t _init_width, _init_height, _init_color_width;
uint32_t _bit18_flag = 0;
char framebuf01[256] = "/dev/fb0";

uint32_t *frameBuffer = NULL;
int fd01;




protected:
int pushColor_x1;
int pushColor_y1;
int pushColor_x2;
int pushColor_y2;
int pushColor_left;
public:
void resetPushColor24(int x1, int y1, int x2, int y2);
void pushColors24(uint8_t *data, int32_t size);




};

#endif  // _TFT000001_H





#ifndef _YAMA_2_GFX_H_BETA_GFXCANVAS_H
#define _YAMA_2_GFX_H_BETA_GFXCANVAS_H
 
/// A GFX 1-bit canvas context for graphics
class yama_2_GFX_H_beta_Canvas1 : public yama_2_GFX_H_beta {
public:
  yama_2_GFX_H_beta_Canvas1(uint32_t w, uint32_t h);
  ~yama_2_GFX_H_beta_Canvas1(void);
  void drawPixel(int32_t x, int32_t y, uint32_t color);
  void fillScreen(uint32_t color);
  bool getPixel(int32_t x, int32_t y) const;
  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer(void) const { return buffer; }
 
protected:
  bool getRawPixel(int32_t x, int32_t y) const;
 
private:
  uint8_t *buffer;
 
};
 
/// A GFX 8-bit canvas context for graphics
class yama_2_GFX_H_beta_Canvas8 : public yama_2_GFX_H_beta {
public:
  yama_2_GFX_H_beta_Canvas8(uint32_t w, uint32_t h);
  ~yama_2_GFX_H_beta_Canvas8(void);
  void drawPixel(int32_t x, int32_t y, uint32_t color);
  void fillScreen(uint32_t color);
  void writeFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
  uint8_t getPixel(int32_t x, int32_t y) const;
  /**********************************************************************/
  /*!
   @brief    Get a pointer to the internal buffer memory
   @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer(void) const { return buffer; }
 
protected:
  uint8_t getRawPixel(int32_t x, int32_t y) const;
 
private:
  uint8_t *buffer;
};
 
///  A GFX 16-bit canvas context for graphics
class yama_2_GFX_H_beta_Canvas16 : public yama_2_GFX_H_beta {
public:
  yama_2_GFX_H_beta_Canvas16(uint32_t w, uint32_t h);
  ~yama_2_GFX_H_beta_Canvas16(void);
  void drawPixel(int32_t x, int32_t y, uint32_t color);
  void fillScreen(uint32_t color);
  void byteSwap(void);
  uint16_t getPixel(int32_t x, int32_t y) const;

  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer(void) const { return (uint8_t *)buffer; }
 
protected:
  uint16_t getRawPixel(int32_t x, int32_t y) const;
 
private:
  uint16_t *buffer;
};

///  A GFX 24-bit canvas context for graphics
class yama_2_GFX_H_beta_Canvas24 : public yama_2_GFX_H_beta {
public:
  yama_2_GFX_H_beta_Canvas24(uint32_t w, uint32_t h);
  ~yama_2_GFX_H_beta_Canvas24(void);
  void drawPixel(int32_t x, int32_t y, uint32_t color);
  void fillScreen(uint32_t color);
  void byteSwap(void);
  uint32_t getPixel(int32_t x, int32_t y) const;

  /**********************************************************************/
  /*!
    @brief    Get a pointer to the internal buffer memory
    @returns  A pointer to the allocated buffer
  */
  /**********************************************************************/
  uint8_t *getBuffer(void) const { return (uint8_t *)buffer; }
 
protected:
  uint32_t getRawPixel(int32_t x, int32_t y) const;
 
private:
  struct yama_2_GFX_H_beta_RGB *buffer;
};

#endif // _YAMA_2_GFX_H_BETA_GFXCANVAS_H
