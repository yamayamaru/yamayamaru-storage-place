//
//    CTC0は時間計測カウンタ用、CTC3はRS232C用、SIOAはRS232C用に使ってます
//
//    fillRect,FillScreen,drawRGBBitmap,drawPixel,drawLine,
//    drawFastVLine,drawFastHLine,drawRect,drawCircle,fillCircle,
//    fillCircleHelper,drawRoundRect,fillRoundRect,drawTriangle,
//    fillTriangle is based on Adafruit GFX.
//    Copyright (c) 2012 Adafruit Industries.
//    Released under the BSD License
//    https://github.com/adafruit/Adafruit-GFX-Library/blob/master/license.txt
//
//    set_rotation,setAddrWindow,tft_setup, is :
//    Copyright (c) 2020 Bodmer (https://github.com/Bodmer)
//    Software License Agreement (FreeBSD License)
//    https://github.com/Bodmer/TFT_eSPI/blob/master/license.txt
//
//      これらのプログラムの使用に当たってはご自分の責任において使用してください
//      これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define CTC0PORT        0x10
#define CTC1PORT        0x11
#define CTC2PORT        0x12
#define CTC3PORT        0x13
#define PIOAPORT_DATA   0x1c
#define PIOAPORT_CMD    0x1d
#define PIOBPORT_DATA   0x1e
#define PIOBPORT_CMD    0x1f
#define SIOAPORT_DATA   0x18
#define SIOAPORT_CMD    0x19
#define SIOBPORT_DATA   0x1a
#define SIOBPORT_CMD    0x1b
#define SIOVECT         0xf800
#define PIO1VECT        0xf810
#define CTC0VECT        0xf818
#define SIOVECT_PTR  ((volatile uint16_t *) SIOVECT)
#define PIO1VECT_PTR ((volatile uint16_t *) PIO1VECT)
#define CTC0VECT_PTR ((volatile uint16_t *) CTC0VECT)


#define ILI9486_TFTWIDTH  320
#define ILI9486_TFTHEIGHT 480

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

#define ILI9486_NOP     0x00
#define ILI9486_SWRST   0x01

#define ILI9486_SLPIN   0x10
#define ILI9486_SLPOUT  0x11

#define ILI9486_INVOFF  0x20
#define ILI9486_INVON   0x21

#define ILI9486_DISPOFF 0x28
#define ILI9486_DISPON  0x29

#define ILI9486_CASET   0x2A
#define ILI9486_PASET   0x2B
#define ILI9486_RAMWR   0x2C
#define ILI9486_RAMRD   0x2E

#define ILI9486_MADCTL  0x36

#define ILI9486_GMCTRP1 0xE0
#define ILI9486_GMCTRN1 0xE1

#define ILI9486_MADCTL_MY  0x80
#define ILI9486_MADCTL_MX  0x40
#define ILI9486_MADCTL_MV  0x20
#define ILI9486_MADCTL_ML  0x10
#define ILI9486_MADCTL_RGB 0x00
#define ILI9486_MADCTL_BGR 0x08
#define ILI9486_MADCTL_MH  0x04
#define ILI9486_MAD_SS     0x02
#define ILI9486_MAD_GS     0x01



#define min01(a, b) (((a) < (b)) ? (a) : (b))
#define swap01(a, b)                                                       \
{                                                                          \
    int t = a;                                                             \
    a = b;                                                                 \
    b = t;                                                                 \
}
#define swap_int32_01(a, b)                                                \
{                                                                          \
    int32_t t = a;                                                         \
    a = b;                                                                 \
    b = t;                                                                 \
}
#define abs01(a)    ((a >= 0) ? a : (-(a)))


void tft_setup(void);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void setRotation(int mode);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
void fillScreen(uint32_t color);
void drawRGBBitmap(int x, int y, uint8_t bitmap[], int w, int h);
void drawPixel(int x, int y, uint32_t color);
void drawLine(int x0, int y0, int x1, int y1, uint32_t color);
void drawFastVLine(int x, int y, int h, uint32_t color);
void drawFastHLine(int x, int y, int w, uint32_t color);
void drawRect(int x, int y, int w, int h, uint32_t color);
void drawCircle(int x0, int y0, int32_t r, uint32_t color);
void fillCircle(int x0, int y0, int32_t r, uint32_t color);
void fillCircleHelper(int x0, int y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color);
void drawRoundRect(int x, int y, int w, int h, int32_t r, uint32_t color);
void fillRoundRect(int x, int y, int w, int h, int32_t r, uint32_t color);
void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
uint32_t color16(uint16_t color);
uint16_t color565(uint16_t colr, uint16_t colg, uint16_t colb);

void init_pio();
void gpio_rst_off();
void gpio_rst_on();
void gpio_dc_off();
void gpio_dc_on();
void writedata(uint8_t c);
void writecommand(uint8_t byte);


void outp(uint8_t port, uint8_t data);
uint8_t inp(uint8_t port);
void z80_ei();
void z80_di();
void super_aki80_init(void);
void init_sio(void);
int getch(void);
int kbhit(void);
int sioaout_ok(void);
void init_ctc0(void);
void stop_ctc0(void);
unsigned long millis(void);
void delay(unsigned long millisec);
extern void ctc0_intcall();
extern void sioa_intcall();
extern void iretmon();
int sioa_read_available();
int sioa_read();




uint8_t  rotation = 0;
int _init_width  = ILI9486_TFTWIDTH;
int _init_height = ILI9486_TFTHEIGHT;
int _width  = ILI9486_TFTWIDTH;
int _height = ILI9486_TFTHEIGHT;



void loop();
void main(void) {

    z80_di();

    super_aki80_init();
    init_sio();
    init_ctc0();

    z80_ei();

    init_pio();

    tft_setup();


    setRotation(3);

    for (;;) {
        loop();
    }
}


void hat02();

void loop() {
    hat02();

    delay(60000);
}


float d[160];
int xmax, ymax;
void hat02() {
    int       i, x, y, zz, px, py, f, colr, colg, colb, sxi;
    int32_t   col;
    float    dr, r, z, sx, sy, a;
    unsigned long start_time, end_time;

    fillRect(0, 0, _width, _height, ILI9486_BLACK);
    xmax = _width;
    ymax = _height;

    printf("start\r\n");
    start_time = millis();

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }
    dr = 3.141592653589793 / 180.0;
    for (y = -180; y <= 180; y += 6) {
        for (x = -180; x < 180; x += 4) {
                        a = (float)x * (float)x + (float)y * (float)y;
            r = dr * sqrtf(a);
            z = 100 * cosf(r) - 30 * cosf(3*r);
            sx = 80 + x / 3 - y / 6;
            sy = 40 - y / 6 - z / 4;
                        sxi = sx;
            if ((sxi < 0) || (sxi >= 160)) continue;
            if (d[sxi] <= sy) continue;
            zz = (int)((z + 100) * 0.035) + 1;
            colr = 0;
                        colb = 0;
                        colg = 0;
            f = 0;
            if ((zz == 1) || (zz == 3) || (zz == 5) || (zz == 7)) {
                colb = 255;
                f = 1;
            }
            if ((zz == 2) || (zz == 3) || (zz >= 6)) {
                colr = 255;
                f = 1;
            }
            if ((zz >= 4)) {
                colg = 255;
                f = 1;
            }
            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);
            if (f == 1) {
                col = ((int32_t)colr << 16) | ((int32_t)colg << 8) | (int32_t)colb;
                fillRect(px, py, 3, 3, col);
            }
            d[sxi] = sy;
        }
    }

    end_time = millis();
    printf("time = %ld sec\r\n", (end_time - start_time) / 1000UL);
}




uint8_t pio_write_porta_data;
void init_pio() {
    outp(PIOAPORT_CMD, 0x0f);
    outp(PIOBPORT_CMD, 0x0f);
    outp(PIOAPORT_DATA, 0xff);
    outp(PIOBPORT_DATA, 0xff);

    pio_write_porta_data = 0xff;
}

void gpio_rst_off() {
    __asm
        ld      a, (_pio_write_porta_data)
        set     5, a
        out     (PIOAPORT_DATA), a
        ld      (_pio_write_porta_data), a
    __endasm;
}

void gpio_rst_on() {
    __asm
        ld      a, (_pio_write_porta_data)
        res     5, a
        out     (PIOAPORT_DATA), a
        ld      (_pio_write_porta_data), a
    __endasm;
}

void gpio_dc_off() {
    __asm
        ld      a, (_pio_write_porta_data)
        set     6, a
        out     (PIOAPORT_DATA), a
        ld      (_pio_write_porta_data), a
    __endasm;
}

void gpio_dc_on() {
    __asm
        ld      a, (_pio_write_porta_data)
        res     6, a
        out     (PIOAPORT_DATA), a
        ld      (_pio_write_porta_data), a
    __endasm;
}

void writedata(uint8_t c) {
    __asm
        out         (PIOBPORT_DATA), a
        ld      a, (_pio_write_porta_data)
        res     7, a
        out     (PIOAPORT_DATA), a
        set     7, a
        out     (PIOAPORT_DATA), a
        ld      (_pio_write_porta_data), a
    __endasm;
}

#define  spi_write(x)   writedata(x)

void writecommand(uint8_t byte) {
    gpio_dc_on();
    writedata(byte);
    gpio_dc_off();
}

void tft_setup(){
    gpio_rst_off();
    delay(200);
    gpio_rst_on();
    delay(200);
    gpio_rst_off();
    delay(200);


    // From https://github.com/notro/fbtft/blob/master/fb_ili9486.c

    //writecommand(0x01); // SW reset
    //delay(120);

    writecommand(0x11); // Sleep out, also SW reset
    delay(120);

    writecommand(0x3A);
    writedata(0x66);           // 18 bit colour interface

    writecommand(0xC2);
    writedata(0x44);

    writecommand(0xC5);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0xE0);
    writedata(0x0F);
    writedata(0x1F);
    writedata(0x1C);
    writedata(0x0C);
    writedata(0x0F);
    writedata(0x08);
    writedata(0x48);
    writedata(0x98);
    writedata(0x37);
    writedata(0x0A);
    writedata(0x13);
    writedata(0x04);
    writedata(0x11);
    writedata(0x0D);
    writedata(0x00);

    writecommand(0xE1);
    writedata(0x0F);
    writedata(0x32);
    writedata(0x2E);
    writedata(0x0B);
    writedata(0x0D);
    writedata(0x05);
    writedata(0x47);
    writedata(0x75);
    writedata(0x37);
    writedata(0x06);
    writedata(0x10);
    writedata(0x03);
    writedata(0x24);
    writedata(0x20);
    writedata(0x00);

    writecommand(ILI9486_INVOFF);
//    writecommand(ILI9486_INVON);

    writecommand(0x36);
    writedata(0x48);

    writecommand(0x29);                     // display on
    delay(150);

    setRotation(0);
}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    gpio_dc_on();
    spi_write(ILI9486_CASET); // Column addr set
    gpio_dc_off();
    spi_write(x0 >> 8);
    spi_write(x0);
    spi_write(x1 >> 8);
    spi_write(x1);

    gpio_dc_on();
    spi_write(ILI9486_PASET); // Row addr set
    gpio_dc_off();
    spi_write(y0>>8);
    spi_write(y0);     // YSTART
    spi_write(y1>>8);
    spi_write(y1);     // YEND

    gpio_dc_on();
    spi_write(ILI9486_RAMWR); // write to RAM
    gpio_dc_off();
}

void setRotation(int mode) {
    int rotation;

    writecommand(ILI9486_MADCTL);
    rotation = mode % 8;
    switch (rotation) {
     case 0: // Portrait
       writedata(ILI9486_MADCTL_BGR | ILI9486_MADCTL_MX);
       _width  = _init_width;
       _height = _init_height;
       break;
     case 1: // Landscape (Portrait + 90)
       writedata(ILI9486_MADCTL_BGR | ILI9486_MADCTL_MV);
       _width  = _init_height;
       _height = _init_width;
       break;
     case 2: // Inverter portrait
       writedata(ILI9486_MADCTL_BGR | ILI9486_MADCTL_MY);
       _width  = _init_width;
       _height = _init_height;
      break;
     case 3: // Inverted landscape
       writedata(ILI9486_MADCTL_BGR | ILI9486_MADCTL_MV | ILI9486_MADCTL_MX | ILI9486_MADCTL_MY);
       _width  = _init_height;
       _height = _init_width;
       break;
    }
    delay(1);
}

void fill_color(uint32_t color, uint32_t len)
{
    uint32_t i;
    uint8_t r, g, b;
    r = (color >> 16) & 0xff;
    g = (color >>  8) & 0xff;
    b = (color) & 0xff;
    for(i = 0; i < len; i++) {
        writedata(r);    // red
        writedata(g);    // green
        writedata(b);    // blue
    }
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) {
    uint32_t count01;
    int32_t w01, h01;
    if(w > 0 && h > 0){
        if((x >= _width) || (y >= _height)) return;
        if((x + w - 1) >= _width)  w = _width  - x;
        if((y + h - 1) >= _height) h = _height - y;

        setAddrWindow(x, y, x+w-1, y+h-1);
        w01 = (int32_t)w;
        h01 = (int32_t)h;
        count01 = w01 * h01;
        fill_color(color, count01);
    }
}

void fillScreen(uint32_t color) {
    fillRect(0, 0, _width, _height, color);
}

// 24bit RGB (r, g, b)
void drawRGBBitmap(int x, int y, uint8_t bitmap[], int w, int h) {
    int32_t i, j;
    const uint8_t *p = (uint8_t *)bitmap;

    setAddrWindow(x, y, x + w - 1, y + h - 1);
    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
            writedata(*p++ & 0xff);    // red
            writedata(*p++ & 0xff);    // green
            writedata(*p++ & 0xff);    // blue
        }
    }
}

void drawPixel(int x, int y, uint32_t color)
{
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

    setAddrWindow(x, y, x + 1, y + 1);

    writedata((color >> 16) & 0xff);    // red
    writedata((color >>  8) & 0xff);    // green
    writedata((color) & 0xff);          // blue
}

void drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
  int steep = abs01(y1 - y0) > abs01(x1 - x0);
  if (steep) {
    swap01(x0, y0);
    swap01(x1, y1);
  }

  if (x0 > x1) {
    swap01(x0, x1);
    swap01(y0, y1);
  }

  int dx, dy;
  dx = x1 - x0;
  dy = abs01(y1 - y0);

  int err = dx / 2;
  int ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void drawFastVLine(int x, int y, int h, uint32_t color) {
  drawLine(x, y, x, y + h - 1, color);
}

void drawFastHLine(int x, int y, int w, uint32_t color) {
  drawLine(x, y, x + w - 1, y, color);
}

void drawRect(int x, int y, int w, int h, uint32_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

void drawCircle(int x0, int y0, int32_t r, uint32_t color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drawPixel(x0, y0 + r, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void drawCircleHelper(int x0, int y0, int32_t r,
                                    uint8_t cornername, uint32_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}


void fillCircle(int x0, int y0, int32_t r, uint32_t color) {
  drawFastVLine(x0, y0 - r, 2 * r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

void fillCircleHelper(int x0, int y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color) {

  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;
  int px = x;
  int py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void drawRoundRect(int x, int y, int w, int h, int32_t r, uint32_t color) {
  int32_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  drawFastHLine(x + r, y, w - 2 * r, color);         // Top
  drawFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
  drawFastVLine(x, y + r, h - 2 * r, color);         // Left
  drawFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
  // draw four corners
  drawCircleHelper(x + r, y + r, r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

void fillRoundRect(int x, int y, int w, int h, int32_t r, uint32_t color) {
  int32_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  fillRect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {

  int32_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap01(y0, y1);
    swap01(x0, x1);
  }
  if (y1 > y2) {
    swap01(y2, y1);
    swap01(x2, x1);
  }
  if (y0 > y1) {
    swap01(y0, y1);
    swap01(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    drawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  int32_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swap_int32_01(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      swap_int32_01(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }
}

uint32_t color16(uint16_t color) {
    uint16_t colr, colg, colb;
        uint32_t col;
    colr = (((color >> 11) & 0x1f) << 3);
    colg = (((color >> 5) & 0x3f) << 2);
    colb = ((color & 0x1f) << 3);
    colr = (colr != 0) ? (colr | 0x07) : colr;
    colg = (colg != 0) ? (colg | 0x03) : colg;
    colb = (colb != 0) ? (colb | 0x07) : colb;
    col = ((uint32_t)colr << 16) | ((uint32_t)colg << 8) | (uint32_t)colb;
    return col;
}

uint16_t color565(uint16_t colr, uint16_t colg, uint16_t colb) {
    uint16_t color;
    color = (uint16_t)(((colr & 0xf8) >> 3) << 11) | (((colg & 0xfc) >> 2) << 5) | ((colb & 0xf8) >> 3);

    return color;
}





// SDCCでは1文字出力関数のputcharを作っておくとprintfで文字出力ができるようにります
int putchar(int ch) {
    __asm
        push    ix
        push    hl

        ld      e, l
        ld      d, #0

putchar_loop0001:
        in      a, (SIOAPORT_CMD)
        and     #0x04
        jp      z, putchar_loop0001
        ld      a, e
        out     (SIOAPORT_DATA), a

        pop     hl
        pop     ix
    __endasm;

    return 0;
}

void outp(uint8_t port, uint8_t data) {
   __asm
       ld      c, a
       out     (c), l
   __endasm;;
}

uint8_t inp(uint8_t port) {
   __asm
       ld      c, a
       in      a, (c)
   __endasm;;
}

void z80_ei() {
    __asm
        ei
    __endasm;;
}

void z80_di() {
    __asm
        di
    __endasm;;
}

void super_aki80_init(void) {
    __asm
        ld                bc, #SIOVECT
        ld                a, b
        ld                i, a
        im                2
    __endasm;;
}

void intcall01() {
    __asm

_ctc0_intcall::
        push            ix
        push            iy
        push            af
        push            hl
        push            bc
        push            de

        call            _ctc0_intcall_func

        pop             de
        pop             bc
        pop             hl
        pop             af
        pop             iy
        pop             ix
        ei
        reti

_sioa_intcall::
        push            ix
        push            iy
        push            af
        push            hl
        push            bc
        push            de

        call            _sioa_intcall_func

        pop             de
        pop             bc
        pop             hl
        pop             af
        pop             iy
        pop             ix
        ei
        reti

_iretmon::
        ei
        reti

    __endasm;;
}

void init_ctc0(void) {
    volatile unsigned int *ctcvect01;
    ctcvect01 = CTC0VECT_PTR;
    *ctcvect01 = (volatile unsigned int)ctc0_intcall;
    __asm
        di
        ld              bc, #CTC0VECT       ;set interrupt vect
        ld              a, c
        out             (CTC0PORT), a
        ld              a, #0x87     ;割り込みあり、タイマーモード、1/16、
        out             (CTC0PORT), a
        ld              a, #154      ; 1/4000秒, 9830400/4000/16 = 153.6
        out             (CTC0PORT), a
        ei
    __endasm;;
}

void stop_ctc0(void) {
    __asm
        di
        ld              a, #0x03
        out             (CTC0PORT), a
        ei
    __endasm;;
}

volatile unsigned long counter01 = 0;
void ctc0_intcall_func(void) {
    counter01++;
}

unsigned long millis(void) {
    return (unsigned long)counter01 / 4;
}

void delay(unsigned long millisec) {
    unsigned long t1;
    t1 = millis();
    while ((millis() - t1) < millisec);
}

extern volatile int sioa_intcall_buf_rd_point = 0;
extern volatile int sioa_intcall_buf_wr_point = 0;
extern volatile int sioa_intcall_ch_count = 0;
void init_sio(void) {

    *(SIOVECT_PTR + 0) = (uint16_t)sioa_intcall;
    *(SIOVECT_PTR + 1) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 2) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 3) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 4) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 5) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 6) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 7) = (uint16_t)iretmon;

    outp(SIOAPORT_CMD, 0x00);
    outp(SIOAPORT_CMD, 0x18);    // reset
    outp(SIOAPORT_CMD, 0x14);
    //outp(SIOAPORT_CMD, 0x04);  // x1,  parity non
    outp(SIOAPORT_CMD, 0x44);    // x16, parity non
    outp(SIOAPORT_CMD, 0x03);
    outp(SIOAPORT_CMD, 0xc1);    // recive character length = 8bit, receiver enable
    outp(SIOAPORT_CMD, 0x05);
    outp(SIOAPORT_CMD, 0x68);    // send character length = 8bit, send enable
    outp(SIOAPORT_CMD, 0x01);
    outp(SIOAPORT_CMD, 0x10);    // rx interrupt enable

    outp(SIOBPORT_CMD, 0x00);
    outp(SIOBPORT_CMD, 0x18);    // reset
    outp(SIOBPORT_CMD, 0x02);
    outp(SIOBPORT_CMD, (SIOVECT & 0xff));    // set sio interrupt vector


    outp(CTC3PORT, 0x07);        // CTC3 x16
    outp(CTC3PORT, 0x04);        // CTC3 count = 4

    sioa_intcall_buf_rd_point = 0;
    sioa_intcall_buf_wr_point = 0;
    sioa_intcall_ch_count = 0;

}

#define CH_BUFFER_SIZE 1024
volatile int sioa_intcall_buf_rd_point;
volatile int sioa_intcall_buf_wr_point;
volatile int sioa_intcall_ch_count;
unsigned char sioa_intcall_buf[CH_BUFFER_SIZE];
void sioa_intcall_func(void) {
    uint8_t a, ch;
    a = inp(SIOAPORT_CMD);
    if (((int)a & 0x01) == 1) {
        ch = inp(SIOAPORT_DATA);
        if (sioa_intcall_ch_count < CH_BUFFER_SIZE) {
            sioa_intcall_ch_count++;
            sioa_intcall_buf[sioa_intcall_buf_wr_point] = ch;
            sioa_intcall_buf_wr_point++;
            if (sioa_intcall_buf_wr_point >= CH_BUFFER_SIZE) {
                 sioa_intcall_buf_wr_point -= CH_BUFFER_SIZE;
            }
        }
    }
}

int sioa_read() {
    int ch;
    if (sioa_intcall_ch_count) {
        z80_di();
        sioa_intcall_ch_count--;
        ch = sioa_intcall_buf[sioa_intcall_buf_rd_point];
        sioa_intcall_buf_rd_point++;
        if (sioa_intcall_buf_rd_point >= CH_BUFFER_SIZE) {
            sioa_intcall_buf_rd_point -= CH_BUFFER_SIZE;
        }
        z80_ei();
                return ch;
    } else {
        return -1;
    }
}

int sioa_read_available() {
    return (sioa_intcall_ch_count > 0);
}


int kbhit(void) {
    uint8_t stat01 = inp(SIOAPORT_CMD);
    int ret = -1;
    if ((int)stat01 & 0x1) {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}

int sioaout_ok(void) {
    uint8_t stat01 = inp(SIOAPORT_CMD);
    int ret = 0;
    if((int)stat01 & 0x4) {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}
