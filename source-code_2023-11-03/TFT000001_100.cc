// TFT000001.cpp
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
//    pioinit in the TFT000001 class 
//    and pio_8bit_parallel.pio is :
//    Copyright (c) 2020 Bodmer (https://github.com/Bodmer)
//    Software License Agreement (FreeBSD License)
//    https://github.com/Bodmer/TFT_eSPI/blob/master/license.txt
//
//      これらのプログラムの使用に当たってはご自分の責任において使用してください
//      これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
//

#include "TFT000001_100.h"

yama_2_GFX_H_beta::yama_2_GFX_H_beta(int32_t w, int32_t h) {
    int32_t width, height, colorWidth;

    init_yama_2_GFX_beta(width, height, 24);
}

void yama_2_GFX_H_beta::init_yama_2_GFX_beta(int32_t w, int32_t h, int32_t colorMode) {
    WIDTH = w;
    HEIGHT = h;
    _width = WIDTH;
    _height = HEIGHT;
    rotation = 0;
    colorModeflag = colorMode;
    setDefaultPalette256();
}

void yama_2_GFX_H_beta::writeLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                             uint32_t color) {
  int32_t steep = yama_2_GFX_H_beta_abs(y1 - y0) > yama_2_GFX_H_beta_abs(x1 - x0);
  if (steep) {
    yama_2_GFX_H_beta_swap_int32_t(x0, y0);
    yama_2_GFX_H_beta_swap_int32_t(x1, y1);
  }
 
  if (x0 > x1) {
    yama_2_GFX_H_beta_swap_int32_t(x0, x1);
    yama_2_GFX_H_beta_swap_int32_t(y0, y1);
  }
 
  int32_t dx, dy;
  dx = x1 - x0;
  dy = yama_2_GFX_H_beta_abs(y1 - y0);
 
  int32_t err = dx / 2;
  int32_t ystep;
 
  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }
 
  for (; x0 <= x1; x0++) {
    if (steep) {
      writePixel(y0, x0, color);
    } else {
      writePixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}
 
void yama_2_GFX_H_beta::startWrite() {}
 
void yama_2_GFX_H_beta::writePixel(int32_t x, int32_t y, uint32_t color) {
  drawPixel(x, y, color);
}
 
void yama_2_GFX_H_beta::writeFastVLine(int32_t x, int32_t y, int32_t h,
                                  uint32_t color) {
  // Overwrite in subclasses if startWrite is defined!
  // Can be just writeLine(x, y, x, y+h-1, color);
  // or writeFillRect(x, y, 1, h, color);
  drawFastVLine(x, y, h, color);
}
 
void yama_2_GFX_H_beta::writeFastHLine(int32_t x, int32_t y, int32_t w,
                                  uint32_t color) {
  // Overwrite in subclasses if startWrite is defined!
  // Example: writeLine(x, y, x+w-1, y, color);
  // or writeFillRect(x, y, w, 1, color);
  drawFastHLine(x, y, w, color);
}
 
void yama_2_GFX_H_beta::writeFillRect(int32_t x, int32_t y, int32_t w, int32_t h,
                                 uint32_t color) {
  // Overwrite in subclasses if desired!
  fillRect(x, y, w, h, color);
}
 
void yama_2_GFX_H_beta::endWrite() {}
 
void yama_2_GFX_H_beta::drawFastVLine(int32_t x, int32_t y, int32_t h,
                                 uint32_t color) {
  startWrite();
  writeLine(x, y, x, y + h - 1, color);
  endWrite();
}
 
void yama_2_GFX_H_beta::drawFastHLine(int32_t x, int32_t y, int32_t w,
                                 uint32_t color) {
  startWrite();
  writeLine(x, y, x + w - 1, y, color);
  endWrite();
}
 
void yama_2_GFX_H_beta::fillRect(int32_t x, int32_t y, int32_t w, int32_t h,
                            uint32_t color) {
  startWrite();
  for (int32_t i = x; i < x + w; i++) {
    writeFastVLine(i, y, h, color);
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::fillScreen(uint32_t color) {
  fillRect(0, 0, _width, _height, color);
}
 
void yama_2_GFX_H_beta::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                            uint32_t color) {
  // Update in subclasses if desired!
  if (x0 == x1) {
    if (y0 > y1)
      yama_2_GFX_H_beta_swap_int32_t(y0, y1);
    drawFastVLine(x0, y0, y1 - y0 + 1, color);
  } else if (y0 == y1) {
    if (x0 > x1)
      yama_2_GFX_H_beta_swap_int32_t(x0, x1);
    drawFastHLine(x0, y0, x1 - x0 + 1, color);
  } else {
    startWrite();
    writeLine(x0, y0, x1, y1, color);
    endWrite();
  }
}
 
void yama_2_GFX_H_beta::drawCircle(int32_t x0, int32_t y0, int32_t r,
                              uint32_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;
 
  startWrite();
  writePixel(x0, y0 + r, color);
  writePixel(x0, y0 - r, color);
  writePixel(x0 + r, y0, color);
  writePixel(x0 - r, y0, color);
 
  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
 
    writePixel(x0 + x, y0 + y, color);
    writePixel(x0 - x, y0 + y, color);
    writePixel(x0 + x, y0 - y, color);
    writePixel(x0 - x, y0 - y, color);
    writePixel(x0 + y, y0 + x, color);
    writePixel(x0 - y, y0 + x, color);
    writePixel(x0 + y, y0 - x, color);
    writePixel(x0 - y, y0 - x, color);
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawCircleHelper(int32_t x0, int32_t y0, int32_t r,
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
      writePixel(x0 + x, y0 + y, color);
      writePixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      writePixel(x0 + x, y0 - y, color);
      writePixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      writePixel(x0 - y, y0 + x, color);
      writePixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      writePixel(x0 - y, y0 - x, color);
      writePixel(x0 - x, y0 - y, color);
    }
  }
}
 
void yama_2_GFX_H_beta::fillCircle(int32_t x0, int32_t y0, int32_t r,
                              uint32_t color) {
  startWrite();
  writeFastVLine(x0, y0 - r, 2 * r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
  endWrite();
}
 
void yama_2_GFX_H_beta::fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color) {
 
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;
  int32_t px = x;
  int32_t py = y;
 
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
        writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}
 
void yama_2_GFX_H_beta::drawRect(int32_t x, int32_t y, int32_t w, int32_t h,
                            uint32_t color) {
  startWrite();
  writeFastHLine(x, y, w, color);
  writeFastHLine(x, y + h - 1, w, color);
  writeFastVLine(x, y, h, color);
  writeFastVLine(x + w - 1, y, h, color);
  endWrite();
}
 
void yama_2_GFX_H_beta::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h,
                                 int32_t r, uint32_t color) {
  int32_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  startWrite();
  writeFastHLine(x + r, y, w - 2 * r, color);         // Top
  writeFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
  writeFastVLine(x, y + r, h - 2 * r, color);         // Left
  writeFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
  // draw four corners
  drawCircleHelper(x + r, y + r, r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
  endWrite();
}
 
void yama_2_GFX_H_beta::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h,
                                 int32_t r, uint32_t color) {
  int32_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  startWrite();
  writeFillRect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
  endWrite();
}
 
void yama_2_GFX_H_beta::drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                                int32_t x2, int32_t y2, uint32_t color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}
 
void yama_2_GFX_H_beta::fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                                int32_t x2, int32_t y2, uint32_t color) {
 
  int32_t a, b, y, last;
 
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    yama_2_GFX_H_beta_swap_int32_t(y0, y1);
    yama_2_GFX_H_beta_swap_int32_t(x0, x1);
  }
  if (y1 > y2) {
    yama_2_GFX_H_beta_swap_int32_t(y2, y1);
    yama_2_GFX_H_beta_swap_int32_t(x2, x1);
  }
  if (y0 > y1) {
    yama_2_GFX_H_beta_swap_int32_t(y0, y1);
    yama_2_GFX_H_beta_swap_int32_t(x0, x1);
  }
 
  startWrite();
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
    writeFastHLine(a, y0, b - a + 1, color);
    endWrite();
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
      yama_2_GFX_H_beta_swap_int32_t(a, b);
    writeFastHLine(a, y, b - a + 1, color);
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
      yama_2_GFX_H_beta_swap_int32_t(a, b);
    writeFastHLine(a, y, b - a + 1, color);
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                              int32_t w, int32_t h, uint32_t color) {
 
  int32_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;
 
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = *((const uint8_t *)(&bitmap[j * byteWidth + i / 8]));
      if (byte & 0x80)
        writePixel(x + i, y, color);
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                              int32_t w, int32_t h, uint32_t color,
                              uint32_t bg) {
 
  int32_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;
 
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = *((const uint8_t *)(&bitmap[j * byteWidth + i / 8]));
      writePixel(x + i, y, (byte & 0x80) ? color : bg);
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawBitmap(int32_t x, int32_t y, uint8_t *bitmap, int32_t w,
                              int32_t h, uint32_t color) {
 
  int32_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;
 
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = bitmap[j * byteWidth + i / 8];
      if (byte & 0x80)
        writePixel(x + i, y, color);
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawBitmap(int32_t x, int32_t y, uint8_t *bitmap, int32_t w,
                              int32_t h, uint32_t color, uint32_t bg) {
 
  int32_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;
 
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = bitmap[j * byteWidth + i / 8];
      writePixel(x + i, y, (byte & 0x80) ? color : bg);
    }
  }
  endWrite();
}



void yama_2_GFX_H_beta::drawBitmap(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg) {
    int i, j, n;
    startWrite();
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            n = j * width + i;
            if (bit_test(bitmap_mask01, n)) {
                writePixel(x + i, y + j, (bit_test(bitmap01, n)) ? fg : bg);
            }
        }
    }
    endWrite();
}

void yama_2_GFX_H_beta::drawBitmap(int32_t x, int32_t y, uint8_t *bitmap01, uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg) {
    int i, j, n;
    startWrite();
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            n = j * width + i;
            if (bit_test(bitmap_mask01, n)) {
                writePixel(x + i, y + j, (bit_test(bitmap01, n)) ? fg : bg);
            }
        }
    }
    endWrite();
}




void yama_2_GFX_H_beta::drawXBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                               int32_t w, int32_t h, uint32_t color) {
 
  int32_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;
 
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte >>= 1;
      else
        byte = *((const uint8_t *)(&bitmap[j * byteWidth + i / 8]));
      // Nearly identical to drawBitmap(), only the bit order
      // is reversed here (left-to-right = LSB to MSB):
      if (byte & 0x01)
        writePixel(x + i, y, color);
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawGrayscaleBitmap8(int32_t x, int32_t y,
                                       const uint8_t bitmap[], int32_t w,
                                       int32_t h) {
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      writePixel(x + i, y, (uint8_t)*((const uint8_t *)(&bitmap[j * w + i])));
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawGrayscaleBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                       int32_t w, int32_t h) {
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      writePixel(x + i, y, bitmap[j * w + i]);
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawGrayscaleBitmap8(int32_t x, int32_t y,
                                       const uint8_t bitmap[],
                                       const uint8_t mask[], int32_t w,
                                       int32_t h) {
  int32_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = *((const uint8_t *)(&mask[j * bw + i / 8]));
      if (byte & 0x80) {
        writePixel(x + i, y, (uint8_t)*((const uint8_t *)(&bitmap[j * w + i])));
      }
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawGrayscaleBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                       uint8_t *mask, int32_t w, int32_t h) {
  int32_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        writePixel(x + i, y, bitmap[j * w + i]);
      }
    }
  }
  endWrite();
}


void yama_2_GFX_H_beta::drawRGBBitmap8(int32_t x, int32_t y,
                                       const uint8_t bitmap[], int32_t w,
                                       int32_t h) {
    drawGrayscaleBitmap8(x, y, bitmap, w, h);
}

void yama_2_GFX_H_beta::drawRGBBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                       int32_t w, int32_t h) {
    drawGrayscaleBitmap8(x, y, bitmap, w, h);
}

void yama_2_GFX_H_beta::drawRGBBitmap8(int32_t x, int32_t y,
                                       const uint8_t bitmap[],
                                       const uint8_t mask[], int32_t w,
                                       int32_t h) {
    drawGrayscaleBitmap8(x, y, bitmap, mask, w, h);
}

void yama_2_GFX_H_beta::drawRGBBitmap8(int32_t x, int32_t y, uint8_t *bitmap,
                                       uint8_t *mask, int32_t w, int32_t h) {
    drawGrayscaleBitmap8(x, y, bitmap, mask, w, h);
}

void yama_2_GFX_H_beta::setColorMode8(void) {
    colorModeflag = 8;
}
void yama_2_GFX_H_beta::setColorMode16(void) {
    colorModeflag = 16;
}
void yama_2_GFX_H_beta::setColorMode24(void) {
    colorModeflag = 24;
}

int32_t yama_2_GFX_H_beta::getColorMode(void) {
    return colorModeflag;
}

void yama_2_GFX_H_beta::drawRGBBitmap16(int32_t x, int32_t y, const uint8_t _bitmap[],
                                 int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      writePixel(x + i, y, *((const uint16_t *)(&bitmap[j * w + i])));
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawRGBBitmap16(int32_t x, int32_t y, uint8_t *_bitmap,
                                 int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      writePixel(x + i, y, bitmap[j * w + i]);
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawRGBBitmap16(int32_t x, int32_t y, const uint8_t _bitmap[],
                                 const uint8_t mask[], int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = *((const uint8_t *)(&mask[j * bw + i / 8]));
      if (byte & 0x80) {
        writePixel(x + i, y, *((const uint16_t *)(&bitmap[j * w + i])));
      }
    }
  }
  endWrite();
}
 
void yama_2_GFX_H_beta::drawRGBBitmap16(int32_t x, int32_t y, uint8_t *_bitmap,
                                 uint8_t *mask, int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        writePixel(x + i, y, bitmap[j * w + i]);
      }
    }
  }
  endWrite();
}

void yama_2_GFX_H_beta::drawRGBBitmap24(int32_t x, int32_t y, const uint8_t _bitmap[],
                                 int32_t w, int32_t h) {
  struct yama_2_GFX_H_beta_RGB *bitmap = (struct yama_2_GFX_H_beta_RGB *)_bitmap;
  int32_t n;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      n = j * w + i;
      writePixel(x + i, y, ((bitmap[n].r << 16) | (bitmap[n].g << 8) | bitmap[n].b));
    }
  }
  endWrite();
}

void yama_2_GFX_H_beta::drawRGBBitmap24(int32_t x, int32_t y, const uint8_t _bitmap[],
                                 const uint8_t mask[], int32_t w, int32_t h) {
  struct yama_2_GFX_H_beta_RGB *bitmap = (struct yama_2_GFX_H_beta_RGB *)_bitmap;
  int32_t n;
  int32_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  startWrite();
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = *((const uint8_t *)(&mask[j * bw + i / 8]));
      if (byte & 0x80) {
        n = j * w + i;
        writePixel(x + i, y, ((bitmap[n].r << 16) | (bitmap[n].g << 8) | bitmap[n].b));
      }
    }
  }
  endWrite();
}

void yama_2_GFX_H_beta::setRotation(uint8_t x) {
  rotation = (x & 3);
  switch (rotation) {
  case 0:
  case 2:
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 1:
  case 3:
    _width = HEIGHT;
    _height = WIDTH;
    break;
  }
}
 
 
uint32_t yama_2_GFX_H_beta::width(void){
    return _width;
}
 
uint32_t yama_2_GFX_H_beta::height(void){
    return _height;
}

void yama_2_GFX_H_beta::drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, width, height);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, width, height);
    }
}

void yama_2_GFX_H_beta::drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, bitmap_mask, width, height);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, bitmap_mask, width, height);
    }
}

void yama_2_GFX_H_beta::drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height, void *palette_address) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, width, height, (uint16_t *)palette_address);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, width, height, (uint32_t *)palette_address);
    }
}

void yama_2_GFX_H_beta::drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height, void *palette_address) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, bitmap_mask, width, height, (uint16_t *)palette_address);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, bitmap_mask, width, height, (uint32_t *)palette_address);
    }
}

void yama_2_GFX_H_beta::drawPalette256Bitmap16(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height, uint16_t *palette_address) {
    int i, j, n;
    uint16_t *palette;
    if (palette_address = NULL) palette = palette256_table;
    else                        palette = palette_address;
    startWrite();
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            n = j * width + i;
            writePixel(x + i, y + j, palette[bitmap[n]]);
        }
    }
    endWrite();
}

void yama_2_GFX_H_beta::drawPalette256Bitmap16(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height, uint16_t *palette_address) {
    int i, j, n;
    uint16_t *palette;
    if (palette_address = NULL) palette = palette256_table;
    else                        palette = palette_address;
    startWrite();
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            n = j * width + i;
            if (bit_test(bitmap_mask, n)) {
                writePixel(x + i, y + j, palette[bitmap[n]]);
            }
        }
    }
    endWrite();
}

void yama_2_GFX_H_beta::drawPalette256Bitmap24(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height, uint32_t *palette_address) {
    int i, j, n;
    uint32_t *palette;
    if (palette_address = NULL) palette = palette256_24_table;
    else                        palette = palette_address;
    startWrite();
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            n = j * width + i;
            writePixel(x + i, y + j, palette[bitmap[n]]);
        }
    }
    endWrite();
}

void yama_2_GFX_H_beta::drawPalette256Bitmap24(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height, uint32_t *palette_address) {
    int i, j, n;
    uint32_t *palette;
    if (palette_address = NULL) palette = palette256_24_table;
    else                        palette = palette_address;
    startWrite();
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            n = j * width + i;
            if (bit_test(bitmap_mask, n)) {
                writePixel(x + i, y + j, palette[bitmap[n]]);
            }
        }
    }
    endWrite();
}

void yama_2_GFX_H_beta::drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
  int32_t xi,yi;
  const uint8_t *p = bitmap;
  uint32_t red, green, blue;
  uint16_t c, col;

  if (red_flag)   red = 0x1f;
  else            red = 0;
  if (green_flag) green = 0x1f;
  else            green = 0;
  if (blue_flag)  blue = 0x1f;
  else            blue = 0;

  for(yi=0; yi<h; yi++){
    for(xi=0; xi<w; xi++) {
      c = (uint16_t)*p++;
      if (invert)
          col = (uint16_t)(((((255 - c) >> 3) & red) << 11) | (((((255 - c) >> 3) & green) << 1) << 5) | (((255 - c) >> 3) & blue));
      else
          col = (uint16_t)((((c >> 3) & red) << 11) | ((((c >> 3) & green) << 1) << 5) | ((c >> 3) & blue));

      drawPixel(x + xi, y + yi, col);
    }
  }
}

void yama_2_GFX_H_beta::drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
  int32_t xi,yi;
  const uint8_t *p = bitmap;
  uint32_t red, green, blue;
  uint16_t c, col;
  int32_t  n;

  if (red_flag)   red = 0x1f;
  else            red = 0;
  if (green_flag) green = 0x1f;
  else            green = 0;
  if (blue_flag)  blue = 0x1f;
  else            blue = 0;

  n = 0;
  for(yi=0; yi<h; yi++){
    for(xi=0; xi<w; xi++) {
      c = (uint16_t)*p++;
      if (((bitmap_mask[n >> 3]) >> (7 - (n & 7)) & 1) == 1) {
          if (invert)
              col = (uint16_t)(((((255 - c) >> 3) & red) << 11) | (((((255 - c) >> 3) & green) << 1) << 5) | (((255 - c) >> 3) & blue));
          else
              col = (uint16_t)((((c >> 3) & red) << 11) | ((((c >> 3) & green) << 1) << 5) | ((c >> 3) & blue));
          drawPixel(x + xi, y + yi, col);
      }
      n++;
    }
  }
}

void yama_2_GFX_H_beta::drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
  int32_t xi,yi;
  const uint8_t *p = bitmap;
  uint32_t red, green, blue;
  uint32_t c, col;

  if (red_flag)   red = 0x1f;
  else            red = 0;
  if (green_flag) green = 0x1f;
  else            green = 0;
  if (blue_flag)  blue = 0x1f;
  else            blue = 0;

  for(yi=0; yi<h; yi++){
    for(xi=0; xi<w; xi++) {
      c = (uint32_t)*p++;
      if (invert)
          col = (uint32_t)((((255 - c) & red) << 16) | (((255 - c) & green) << 8) | ((255 - c) & blue));
      else
          col = (uint32_t)(((c & red) << 16) | ((c & green) << 8) | (c & blue));
      drawPixel(x + xi, y + yi, col);
    }
  }
}

void yama_2_GFX_H_beta::drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
  int32_t xi,yi;
  const uint8_t *p = bitmap;
  uint32_t red, green, blue;
  uint32_t c, col;
  int32_t  n;

  if (red_flag)   red = 0x1f;
  else            red = 0;
  if (green_flag) green = 0x1f;
  else            green = 0;
  if (blue_flag)  blue = 0x1f;
  else            blue = 0;

  n = 0;
  for(yi=0; yi<h; yi++){
    for(xi=0; xi<w; xi++) {
      c = (uint32_t)*p++;
      if (((bitmap_mask[n >> 3]) >> (7 - (n & 7)) & 1) == 1) {
          if (invert)
              col = (uint32_t)((((255 - c) & red) << 16) | (((255 - c) & green) << 8) | ((255 - c) & blue));
          else
              col = (uint32_t)(((c & red) << 16) | ((c & green) << 8) | (c & blue));
          drawPixel(x + xi, y + yi, col);
      }
      n++;
    }
  }
}

uint16_t yama_2_GFX_H_beta::color565(uint32_t colr, uint32_t colg, uint32_t colb) {
    uint16_t color;
    color = (uint16_t)(((colr & 0xff) >> 3) << 11) | (((colg & 0xff) >> 2) << 5) | ((colb & 0xff) >> 3);
 
    return color;
}

uint16_t yama_2_GFX_H_beta::color565(uint32_t color) {
    uint16_t col;
    uint32_t colr, colg, colb;
    colr = color >> 16;
    colg = color >> 8;
    colb = color;
    col = (uint16_t)(((colr & 0xff) >> 3) << 11) | (((colg & 0xff) >> 2) << 5) | ((colb & 0xff) >> 3);
 
    return col;
}

uint32_t yama_2_GFX_H_beta::colorRGB24(uint32_t colr, uint32_t colg, uint32_t colb) {
    uint32_t col;
    col = ((colr & 0xff) << 16) | ((colg & 0xff) << 8) | (colb & 0xff);
    return col;  
}

uint32_t yama_2_GFX_H_beta::color(uint32_t color) {
    if (getColorMode() == 16) {
        return (uint32_t)color565(color);
    }
    return color;
}

uint32_t yama_2_GFX_H_beta::color(uint32_t colr, uint32_t colg, uint32_t colb) {
    if (getColorMode() == 16) {
        return (uint32_t)color565(colr, colg, colb);
    }
    uint32_t col;
    col = ((colr & 0xff) << 16) | ((colg & 0xff) << 8) | (colb & 0xff);
    return col;
}

uint32_t yama_2_GFX_H_beta::color16(uint16_t color) {
    if (getColorMode() == 24) {
        return color16to24(color);
    }
    return (uint32_t)color;
}

uint32_t yama_2_GFX_H_beta::color16to24(uint16_t color) {
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

uint16_t color24to16(uint32_t color) {
    uint32_t colr, colg, colb;
    uint16_t col;
    colr = ((color >> 16) >> 3) & 0x1f;
    colg = ((color >> 8) >> 2) & 0x3f;
    colb = (color >> 3) & 0x1f;
    col = (uint16_t)((colr << 11) | (colg << 5) | colb);
    return col;
}

void yama_2_GFX_H_beta::setDefaultPalette256() {
    setDefaultPalette256_16();
    setDefaultPalette256_24();
}

void yama_2_GFX_H_beta::setWeb216Palette256() {
    setWeb216Palette256_16();
    setWeb216Palette256_24();
}

void yama_2_GFX_H_beta::setPalette256(uint8_t palette_num, uint32_t color) {
    if (getColorMode() == 16) {
        setPalette256_16(palette_num, (uint16_t)color);
    } else if (getColorMode() == 24) {
        setPalette256_24(palette_num, (uint32_t)color);
    }
}

uint32_t yama_2_GFX_H_beta::getPalette256(uint8_t palette_num) {
    uint32_t ret;
    ret = 0;
    if (getColorMode() == 16) {
        ret = getPalette256_16(palette_num);
    } else if (getColorMode() == 24) {
        ret = getPalette256_24(palette_num);
    }

    return ret;
}

void yama_2_GFX_H_beta::setPalette256(uint8_t palette_num, uint32_t color, int32_t colorMode) {
    if (colorMode == 16) {
        setPalette256_16(palette_num, (uint16_t)color);
    } else if (colorMode == 24) {
        setPalette256_24(palette_num, (uint32_t)color);
    }
}

uint32_t yama_2_GFX_H_beta::getPalette256(uint8_t palette_num, int32_t colorMode) {
    uint32_t ret;
    ret = 0;
    if (colorMode == 16) {
        ret = getPalette256_16(palette_num);
    } else if (colorMode == 24) {
        ret = getPalette256_24(palette_num);
    }

    return ret;
    }

void yama_2_GFX_H_beta::setDefaultPalette256_16() {
    memcpy(palette256_table, default_palette256_data, palette_number * 2);
}

void yama_2_GFX_H_beta::setWeb216Palette256_16() {
    memcpy(palette256_table, web216_palette256_data, palette_number * 2);
    
}

void yama_2_GFX_H_beta::setPalette256_16(uint8_t palette_num, uint16_t color) {
    palette256_table[palette_num] = color;
}

uint32_t yama_2_GFX_H_beta::getPalette256_16(uint8_t palette_num) {
    return (uint32_t)palette256_table[palette_num];
}


void yama_2_GFX_H_beta::setDefaultPalette256_24() {
    memcpy(palette256_24_table, default_palette256_24_data, palette_number * 4);
}

void yama_2_GFX_H_beta::setWeb216Palette256_24() {
    memcpy(palette256_24_table, web216_palette256_24_data, palette_number * 4);
    
}

void yama_2_GFX_H_beta::setPalette256_24(uint8_t palette_num, uint32_t color) {
    palette256_24_table[palette_num] = color;
}

uint32_t yama_2_GFX_H_beta::getPalette256_24(uint8_t palette_num) {
    return palette256_24_table[palette_num];
}

void *yama_2_GFX_H_beta::getPaletteTable256() {
    void *palette;
    if (getColorMode() == 16) {
      palette = (void *)getPaletteTable256_16();
    } else if (getColorMode() == 24) {
      palette = (void *)getPaletteTable256_24();
    }
    return palette;
}

void *yama_2_GFX_H_beta::getPaletteTable256(int32_t colorMode) {
    void *palette;
    if (colorMode == 16) {
      palette = (void *)getPaletteTable256_16();
    } else if (colorMode == 24) {
      palette = (void *)getPaletteTable256_24();
    }
    return palette;
}

uint16_t *yama_2_GFX_H_beta::getPaletteTable256_16() {
    return palette256_table;
}

uint32_t *yama_2_GFX_H_beta::getPaletteTable256_24() {
    return palette256_24_table;
}

const uint16_t yama_2_GFX_H_beta::default_palette256_data[] = {
    0x0000, 0x8000, 0x0400, 0x8400, 0x0010, 0x8010, 0x0410, 0xc618, 
    0xc6f8, 0xa65e, 0x29f5, 0x29ff, 0x2ae0, 0x2aea, 0x2af5, 0x2aff, 
    0x2be0, 0x2bea, 0x2bf5, 0x2bff, 0x2ce0, 0x2cea, 0x2cf5, 0x2cff, 
    0x2de0, 0x2dea, 0x2df5, 0x2dff, 0x2ee0, 0x2eea, 0x2ef5, 0x2eff, 
    0x2fe0, 0x2fea, 0x2ff5, 0x2fff, 0x5000, 0x500a, 0x5015, 0x501f, 
    0x50e0, 0x50ea, 0x50f5, 0x50ff, 0x51e0, 0x51ea, 0x51f5, 0x51ff, 
    0x52e0, 0x52ea, 0x52f5, 0x52ff, 0x53e0, 0x53ea, 0x53f5, 0x53ff, 
    0x54e0, 0x54ea, 0x54f5, 0x54ff, 0x55e0, 0x55ea, 0x55f5, 0x55ff, 
    0x56e0, 0x56ea, 0x56f5, 0x56ff, 0x57e0, 0x57ea, 0x57f5, 0x57ff, 
    0x7800, 0x780a, 0x7815, 0x781f, 0x78e0, 0x78ea, 0x78f5, 0x78ff, 
    0x79e0, 0x79ea, 0x79f5, 0x79ff, 0x7ae0, 0x7aea, 0x7af5, 0x7aff, 
    0x7be0, 0x7bea, 0x7bf5, 0x7bff, 0x7ce0, 0x7cea, 0x7cf5, 0x7cff, 
    0x7de0, 0x7dea, 0x7df5, 0x7dff, 0x7ee0, 0x7eea, 0x7ef5, 0x7eff, 
    0x7fe0, 0x7fea, 0x7ff5, 0x7fff, 0xa800, 0xa80a, 0xa815, 0xa81f, 
    0xa8e0, 0xa8ea, 0xa8f5, 0xa8ff, 0xa9e0, 0xa9ea, 0xa9f5, 0xa9ff, 
    0xaae0, 0xaaea, 0xaaf5, 0xaaff, 0xabe0, 0xabea, 0xabf5, 0xabff, 
    0xace0, 0xacea, 0xacf5, 0xacff, 0xade0, 0xadea, 0xadf5, 0xadff, 
    0xaee0, 0xaeea, 0xaef5, 0xaeff, 0xafe0, 0xafea, 0xaff5, 0xafff, 
    0xd000, 0xd00a, 0xd015, 0xd01f, 0xd0e0, 0xd0ea, 0xd0f5, 0xd0ff, 
    0xd1e0, 0xd1ea, 0xd1f5, 0xd1ff, 0xd2e0, 0xd2ea, 0xd2f5, 0xd2ff, 
    0xd3e0, 0xd3ea, 0xd3f5, 0xd3ff, 0xd4e0, 0xd4ea, 0xd4f5, 0xd4ff, 
    0xd5e0, 0xd5ea, 0xd5f5, 0xd5ff, 0xd6e0, 0xd6ea, 0xd6f5, 0xd6ff, 
    0xd7e0, 0xd7ea, 0xd7f5, 0xd7ff, 0xf80a, 0xf815, 0xf8e0, 0xf8ea, 
    0xf8f5, 0xf8ff, 0xf9e0, 0xf9ea, 0xf9f5, 0xf9ff, 0xfae0, 0xfaea, 
    0xfaf5, 0xfaff, 0xfbe0, 0xfbea, 0xfbf5, 0xfbff, 0xfce0, 0xfcea, 
    0xfcf5, 0xfcff, 0xfde0, 0xfdea, 0xfdf5, 0xfdff, 0xfee0, 0xfeea, 
    0xfef5, 0xfeff, 0xffea, 0xfff5, 0xce7f, 0xfe7f, 0x37ff, 0x67ff, 
    0x9fff, 0xcfff, 0x03e0, 0x03ea, 0x03f5, 0x03ff, 0x04e0, 0x04ea, 
    0x04f5, 0x04ff, 0x05e0, 0x05ea, 0x05f5, 0x05ff, 0x06e0, 0x06ea, 
    0x06f5, 0x06ff, 0x07ea, 0x07f5, 0x2800, 0x280a, 0x2815, 0x281f, 
    0x28e0, 0x28ea, 0x28f5, 0x28ff, 0x29e0, 0x29ea, 0xffde, 0xa514, 
    0x8410, 0xf800, 0x07e0, 0xffe0, 0x001f, 0xf81f, 0x07ff, 0xffff
};

const uint16_t yama_2_GFX_H_beta::web216_palette256_data[] = {
    0xffff, 0xfff9, 0xfff3, 0xffed, 0xffe6, 0xffe0, 0xfe5f, 0xfe59, 
    0xfe53, 0xfe4d, 0xfe46, 0xfe40, 0xfcdf, 0xfcd9, 0xfcd3, 0xfccd, 
    0xfcc6, 0xfcc0, 0xfb5f, 0xfb59, 0xfb53, 0xfb4d, 0xfb46, 0xfb40, 
    0xf99f, 0xf999, 0xf993, 0xf98d, 0xf986, 0xf980, 0xf81f, 0xf819, 
    0xf813, 0xf80d, 0xf806, 0xf800, 0xcfff, 0xcff9, 0xcff3, 0xcfed, 
    0xcfe6, 0xcfe0, 0xce5f, 0xce59, 0xce53, 0xce4d, 0xce46, 0xce40, 
    0xccdf, 0xccd9, 0xccd3, 0xcccd, 0xccc6, 0xccc0, 0xcb5f, 0xcb59, 
    0xcb53, 0xcb4d, 0xcb46, 0xcb40, 0xc99f, 0xc999, 0xc993, 0xc98d, 
    0xc986, 0xc980, 0xc81f, 0xc819, 0xc813, 0xc80d, 0xc806, 0xc800, 
    0x9fff, 0x9ff9, 0x9ff3, 0x9fed, 0x9fe6, 0x9fe0, 0x9e5f, 0x9e59, 
    0x9e53, 0x9e4d, 0x9e46, 0x9e40, 0x9cdf, 0x9cd9, 0x9cd3, 0x9ccd, 
    0x9cc6, 0x9cc0, 0x9b5f, 0x9b59, 0x9b53, 0x9b4d, 0x9b46, 0x9b40, 
    0x999f, 0x9999, 0x9993, 0x998d, 0x9986, 0x9980, 0x981f, 0x9819, 
    0x9813, 0x980d, 0x9806, 0x9800, 0x6fff, 0x6ff9, 0x6ff3, 0x6fed, 
    0x6fe6, 0x6fe0, 0x6e5f, 0x6e59, 0x6e53, 0x6e4d, 0x6e46, 0x6e40, 
    0x6cdf, 0x6cd9, 0x6cd3, 0x6ccd, 0x6cc6, 0x6cc0, 0x6b5f, 0x6b59, 
    0x6b53, 0x6b4d, 0x6b46, 0x6b40, 0x699f, 0x6999, 0x6993, 0x698d, 
    0x6986, 0x6980, 0x681f, 0x6819, 0x6813, 0x680d, 0x6806, 0x6800, 
    0x37ff, 0x37f9, 0x37f3, 0x37ed, 0x37e6, 0x37e0, 0x365f, 0x3659, 
    0x3653, 0x364d, 0x3646, 0x3640, 0x34df, 0x34d9, 0x34d3, 0x34cd, 
    0x34c6, 0x34c0, 0x335f, 0x3359, 0x3353, 0x334d, 0x3346, 0x3340, 
    0x319f, 0x3199, 0x3193, 0x318d, 0x3186, 0x3180, 0x301f, 0x3019, 
    0x3013, 0x300d, 0x3006, 0x3000, 0x07ff, 0x07f9, 0x07f3, 0x07ed, 
    0x07e6, 0x07e0, 0x065f, 0x0659, 0x0653, 0x064d, 0x0646, 0x0640, 
    0x04df, 0x04d9, 0x04d3, 0x04cd, 0x04c6, 0x04c0, 0x035f, 0x0359, 
    0x0353, 0x034d, 0x0346, 0x0340, 0x019f, 0x0199, 0x0193, 0x018d, 
    0x0186, 0x0180, 0x001f, 0x0019, 0x0013, 0x000d, 0x0006, 0x0000, 
    0x8410, 0xc618, 0x0010, 0x0410, 0x0400, 0x8400, 0x8010, 0x8000, 
    0xffdf, 0xf79e, 0xef5d, 0xe71c, 0xdedb, 0xd69a, 0xce59, 0xc618, 
    0xbdd7, 0xb596, 0xad55, 0xa514, 0x9cd3, 0x9492, 0x8c51, 0x8410, 
    0x7bcf, 0x738e, 0x6b4d, 0x630c, 0x5acb, 0x528a, 0x4a49, 0x4208, 
    0x39c7, 0x3186, 0x2945, 0x2104, 0x18c3, 0x1082, 0x0841, 0x0000
};

const uint8_t yama_2_GFX_H_beta::default_palette256_24_data[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x80,0x00,0x00,0x00,
    0x80,0x00,0x80,0x00,0x80,0x80,0x00,0x00,0xc0,0xc0,0xc0,0x00,0xc0,0xdc,0xc0,0x00,0xf0,0xca,0xa6,0x00,
    0xaa,0x3f,0x2a,0x00,0xff,0x3f,0x2a,0x00,0x00,0x5f,0x2a,0x00,0x55,0x5f,0x2a,0x00,0xaa,0x5f,0x2a,0x00,
    0xff,0x5f,0x2a,0x00,0x00,0x7f,0x2a,0x00,0x55,0x7f,0x2a,0x00,0xaa,0x7f,0x2a,0x00,0xff,0x7f,0x2a,0x00,
    0x00,0x9f,0x2a,0x00,0x55,0x9f,0x2a,0x00,0xaa,0x9f,0x2a,0x00,0xff,0x9f,0x2a,0x00,0x00,0xbf,0x2a,0x00,
    0x55,0xbf,0x2a,0x00,0xaa,0xbf,0x2a,0x00,0xff,0xbf,0x2a,0x00,0x00,0xdf,0x2a,0x00,0x55,0xdf,0x2a,0x00,
    0xaa,0xdf,0x2a,0x00,0xff,0xdf,0x2a,0x00,0x00,0xff,0x2a,0x00,0x55,0xff,0x2a,0x00,0xaa,0xff,0x2a,0x00,
    0xff,0xff,0x2a,0x00,0x00,0x00,0x55,0x00,0x55,0x00,0x55,0x00,0xaa,0x00,0x55,0x00,0xff,0x00,0x55,0x00,
    0x00,0x1f,0x55,0x00,0x55,0x1f,0x55,0x00,0xaa,0x1f,0x55,0x00,0xff,0x1f,0x55,0x00,0x00,0x3f,0x55,0x00,
    0x55,0x3f,0x55,0x00,0xaa,0x3f,0x55,0x00,0xff,0x3f,0x55,0x00,0x00,0x5f,0x55,0x00,0x55,0x5f,0x55,0x00,
    0xaa,0x5f,0x55,0x00,0xff,0x5f,0x55,0x00,0x00,0x7f,0x55,0x00,0x55,0x7f,0x55,0x00,0xaa,0x7f,0x55,0x00,
    0xff,0x7f,0x55,0x00,0x00,0x9f,0x55,0x00,0x55,0x9f,0x55,0x00,0xaa,0x9f,0x55,0x00,0xff,0x9f,0x55,0x00,
    0x00,0xbf,0x55,0x00,0x55,0xbf,0x55,0x00,0xaa,0xbf,0x55,0x00,0xff,0xbf,0x55,0x00,0x00,0xdf,0x55,0x00,
    0x55,0xdf,0x55,0x00,0xaa,0xdf,0x55,0x00,0xff,0xdf,0x55,0x00,0x00,0xff,0x55,0x00,0x55,0xff,0x55,0x00,
    0xaa,0xff,0x55,0x00,0xff,0xff,0x55,0x00,0x00,0x00,0x7f,0x00,0x55,0x00,0x7f,0x00,0xaa,0x00,0x7f,0x00,
    0xff,0x00,0x7f,0x00,0x00,0x1f,0x7f,0x00,0x55,0x1f,0x7f,0x00,0xaa,0x1f,0x7f,0x00,0xff,0x1f,0x7f,0x00,
    0x00,0x3f,0x7f,0x00,0x55,0x3f,0x7f,0x00,0xaa,0x3f,0x7f,0x00,0xff,0x3f,0x7f,0x00,0x00,0x5f,0x7f,0x00,
    0x55,0x5f,0x7f,0x00,0xaa,0x5f,0x7f,0x00,0xff,0x5f,0x7f,0x00,0x00,0x7f,0x7f,0x00,0x55,0x7f,0x7f,0x00,
    0xaa,0x7f,0x7f,0x00,0xff,0x7f,0x7f,0x00,0x00,0x9f,0x7f,0x00,0x55,0x9f,0x7f,0x00,0xaa,0x9f,0x7f,0x00,
    0xff,0x9f,0x7f,0x00,0x00,0xbf,0x7f,0x00,0x55,0xbf,0x7f,0x00,0xaa,0xbf,0x7f,0x00,0xff,0xbf,0x7f,0x00,
    0x00,0xdf,0x7f,0x00,0x55,0xdf,0x7f,0x00,0xaa,0xdf,0x7f,0x00,0xff,0xdf,0x7f,0x00,0x00,0xff,0x7f,0x00,
    0x55,0xff,0x7f,0x00,0xaa,0xff,0x7f,0x00,0xff,0xff,0x7f,0x00,0x00,0x00,0xaa,0x00,0x55,0x00,0xaa,0x00,
    0xaa,0x00,0xaa,0x00,0xff,0x00,0xaa,0x00,0x00,0x1f,0xaa,0x00,0x55,0x1f,0xaa,0x00,0xaa,0x1f,0xaa,0x00,
    0xff,0x1f,0xaa,0x00,0x00,0x3f,0xaa,0x00,0x55,0x3f,0xaa,0x00,0xaa,0x3f,0xaa,0x00,0xff,0x3f,0xaa,0x00,
    0x00,0x5f,0xaa,0x00,0x55,0x5f,0xaa,0x00,0xaa,0x5f,0xaa,0x00,0xff,0x5f,0xaa,0x00,0x00,0x7f,0xaa,0x00,
    0x55,0x7f,0xaa,0x00,0xaa,0x7f,0xaa,0x00,0xff,0x7f,0xaa,0x00,0x00,0x9f,0xaa,0x00,0x55,0x9f,0xaa,0x00,
    0xaa,0x9f,0xaa,0x00,0xff,0x9f,0xaa,0x00,0x00,0xbf,0xaa,0x00,0x55,0xbf,0xaa,0x00,0xaa,0xbf,0xaa,0x00,
    0xff,0xbf,0xaa,0x00,0x00,0xdf,0xaa,0x00,0x55,0xdf,0xaa,0x00,0xaa,0xdf,0xaa,0x00,0xff,0xdf,0xaa,0x00,
    0x00,0xff,0xaa,0x00,0x55,0xff,0xaa,0x00,0xaa,0xff,0xaa,0x00,0xff,0xff,0xaa,0x00,0x00,0x00,0xd4,0x00,
    0x55,0x00,0xd4,0x00,0xaa,0x00,0xd4,0x00,0xff,0x00,0xd4,0x00,0x00,0x1f,0xd4,0x00,0x55,0x1f,0xd4,0x00,
    0xaa,0x1f,0xd4,0x00,0xff,0x1f,0xd4,0x00,0x00,0x3f,0xd4,0x00,0x55,0x3f,0xd4,0x00,0xaa,0x3f,0xd4,0x00,
    0xff,0x3f,0xd4,0x00,0x00,0x5f,0xd4,0x00,0x55,0x5f,0xd4,0x00,0xaa,0x5f,0xd4,0x00,0xff,0x5f,0xd4,0x00,
    0x00,0x7f,0xd4,0x00,0x55,0x7f,0xd4,0x00,0xaa,0x7f,0xd4,0x00,0xff,0x7f,0xd4,0x00,0x00,0x9f,0xd4,0x00,
    0x55,0x9f,0xd4,0x00,0xaa,0x9f,0xd4,0x00,0xff,0x9f,0xd4,0x00,0x00,0xbf,0xd4,0x00,0x55,0xbf,0xd4,0x00,
    0xaa,0xbf,0xd4,0x00,0xff,0xbf,0xd4,0x00,0x00,0xdf,0xd4,0x00,0x55,0xdf,0xd4,0x00,0xaa,0xdf,0xd4,0x00,
    0xff,0xdf,0xd4,0x00,0x00,0xff,0xd4,0x00,0x55,0xff,0xd4,0x00,0xaa,0xff,0xd4,0x00,0xff,0xff,0xd4,0x00,
    0x55,0x00,0xff,0x00,0xaa,0x00,0xff,0x00,0x00,0x1f,0xff,0x00,0x55,0x1f,0xff,0x00,0xaa,0x1f,0xff,0x00,
    0xff,0x1f,0xff,0x00,0x00,0x3f,0xff,0x00,0x55,0x3f,0xff,0x00,0xaa,0x3f,0xff,0x00,0xff,0x3f,0xff,0x00,
    0x00,0x5f,0xff,0x00,0x55,0x5f,0xff,0x00,0xaa,0x5f,0xff,0x00,0xff,0x5f,0xff,0x00,0x00,0x7f,0xff,0x00,
    0x55,0x7f,0xff,0x00,0xaa,0x7f,0xff,0x00,0xff,0x7f,0xff,0x00,0x00,0x9f,0xff,0x00,0x55,0x9f,0xff,0x00,
    0xaa,0x9f,0xff,0x00,0xff,0x9f,0xff,0x00,0x00,0xbf,0xff,0x00,0x55,0xbf,0xff,0x00,0xaa,0xbf,0xff,0x00,
    0xff,0xbf,0xff,0x00,0x00,0xdf,0xff,0x00,0x55,0xdf,0xff,0x00,0xaa,0xdf,0xff,0x00,0xff,0xdf,0xff,0x00,
    0x55,0xff,0xff,0x00,0xaa,0xff,0xff,0x00,0xff,0xcc,0xcc,0x00,0xff,0xcc,0xff,0x00,0xff,0xff,0x33,0x00,
    0xff,0xff,0x66,0x00,0xff,0xff,0x99,0x00,0xff,0xff,0xcc,0x00,0x00,0x7f,0x00,0x00,0x55,0x7f,0x00,0x00,
    0xaa,0x7f,0x00,0x00,0xff,0x7f,0x00,0x00,0x00,0x9f,0x00,0x00,0x55,0x9f,0x00,0x00,0xaa,0x9f,0x00,0x00,
    0xff,0x9f,0x00,0x00,0x00,0xbf,0x00,0x00,0x55,0xbf,0x00,0x00,0xaa,0xbf,0x00,0x00,0xff,0xbf,0x00,0x00,
    0x00,0xdf,0x00,0x00,0x55,0xdf,0x00,0x00,0xaa,0xdf,0x00,0x00,0xff,0xdf,0x00,0x00,0x55,0xff,0x00,0x00,
    0xaa,0xff,0x00,0x00,0x00,0x00,0x2a,0x00,0x55,0x00,0x2a,0x00,0xaa,0x00,0x2a,0x00,0xff,0x00,0x2a,0x00,
    0x00,0x1f,0x2a,0x00,0x55,0x1f,0x2a,0x00,0xaa,0x1f,0x2a,0x00,0xff,0x1f,0x2a,0x00,0x00,0x3f,0x2a,0x00,
    0x55,0x3f,0x2a,0x00,0xf0,0xfb,0xff,0x00,0xa4,0xa0,0xa0,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0xff,0x00,
    0x00,0xff,0x00,0x00,0x00,0xff,0xff,0x00,0xff,0x00,0x00,0x00,0xff,0x00,0xff,0x00,0xff,0xff,0x00,0x00,
    0xff,0xff,0xff,0x00
};

const uint8_t yama_2_GFX_H_beta::web216_palette256_24_data[] = {
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





TFT000001::TFT000001(int32_t width, int32_t height)
                : yama_2_GFX_H_beta(width, height) {
    _init_width = width;
    _init_height = height;
}

void TFT000001::init() {

    int32_t width01, height01, colorWidth01;

    _bit18_flag = 1;             // 16bit = _bit18_flab = 0; 18bit = _bit18_flag = 1

    
    getFrameBufferSize(&width01, &height01, &colorWidth01);

    _init_width = width01;
    _init_height = height01;
    _init_color_width = colorWidth01;

    if (_bit18_flag == 1) {
      init_yama_2_GFX_beta(width01, height01, 24);
      setColorMode24();
    } else {
      init_yama_2_GFX_beta(width01, height01, 16);
      setColorMode16();
    }

    
    fd01 = open(framebuf01, O_RDWR);
    frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

}

void TFT000001::deinit() {
    msync(frameBuffer, _width * _height * 4, 0);
    munmap(frameBuffer, _width * _height * 4);
    close(fd01);
}

void TFT000001::getFrameBufferSize(int* width, int* height, int* colorWidth)
{
    int fd;
    int n;
    char str[64];

    /* Get Bit Per Pixel (usually 32-bit(4Byte) */
    fd = open("/sys/class/graphics/fb0/bits_per_pixel", O_RDONLY);
    n = read(fd, str, sizeof(str));
    str[n] = '\0';
    *colorWidth = atoi(str);
    close(fd);

    /* Get Frame Buffer Size */
    fd = open("/sys/class/graphics/fb0/virtual_size", O_RDONLY);
    n = read(fd, str, sizeof(str));
    str[n] = '\0';
    /* divide "800,600" into "800" and "600" */
    *width  = atoi(strtok(str, ","));
    *height = atoi(strtok(NULL, ","));
    close(fd);
}

void TFT000001::getFrameBufferSize2(int* width, int* height, int* colorWidth)
{
    struct fb_var_screeninfo var;
    int fd;
    fd = open("/dev/fb0", O_RDWR);
    ioctl(fd, FBIOGET_VSCREENINFO, &var);
    *colorWidth = var.bits_per_pixel;
    *width      = var.xres_virtual;
    *height     = var.yres_virtual;
    close(fd);
}

uint32_t TFT000001::get_bit18_flag() {
    return _bit18_flag;
}

void TFT000001::drawPixel_framebuff(uint32_t *buffer, int32_t x, int32_t y, uint32_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;

    int32_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = _init_width - 1 - y;
      y = t;
      break;
    case 2:
      x = _init_width - 1 - x;
      y = _init_height - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = _init_height - 1 - t;
      break;
    }
    if ((x >= 0) && (x < _init_width) && (y >= 0) && (y < _init_width))
        buffer[x + y * _init_width] = color;
  }
}

void TFT000001::writeLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                             uint32_t color) {
  int32_t steep = yama_2_GFX_H_beta_abs(y1 - y0) > yama_2_GFX_H_beta_abs(x1 - x0);
  if (steep) {
    yama_2_GFX_H_beta_swap_int32_t(x0, y0);
    yama_2_GFX_H_beta_swap_int32_t(x1, y1);
  }

  if (x0 > x1) {
    yama_2_GFX_H_beta_swap_int32_t(x0, x1);
    yama_2_GFX_H_beta_swap_int32_t(y0, y1);
  }

  int32_t dx, dy;
  dx = x1 - x0;
  dy = yama_2_GFX_H_beta_abs(y1 - y0);

  int32_t err = dx / 2;
  int32_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

//  int fd01 = open(framebuf01, O_RDWR);
//  uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

  for (; x0 <= x1; x0++) {
    if (steep) {
        drawPixel_framebuff(frameBuffer, y0, x0, color);
    } else {
        drawPixel_framebuff(frameBuffer, x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
  msync(frameBuffer, _width * _height * 4, 0);
//  munmap(frameBuffer, _width * _height * 4);
//  close(fd01);
}

void TFT000001::drawCircle(int32_t x0, int32_t y0, int32_t r,
                              uint32_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;

//  int fd01 = open(framebuf01, O_RDWR);
//  uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

  drawPixel_framebuff(frameBuffer, x0, y0 + r, color);
  drawPixel_framebuff(frameBuffer, x0, y0 - r, color);
  drawPixel_framebuff(frameBuffer, x0 + r, y0, color);
  drawPixel_framebuff(frameBuffer, x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel_framebuff(frameBuffer, x0 + x, y0 + y, color);
    drawPixel_framebuff(frameBuffer, x0 - x, y0 + y, color);
    drawPixel_framebuff(frameBuffer, x0 + x, y0 - y, color);
    drawPixel_framebuff(frameBuffer, x0 - x, y0 - y, color);
    drawPixel_framebuff(frameBuffer, x0 + y, y0 + x, color);
    drawPixel_framebuff(frameBuffer, x0 - y, y0 + x, color);
    drawPixel_framebuff(frameBuffer, x0 + y, y0 - x, color);
    drawPixel_framebuff(frameBuffer, x0 - y, y0 - x, color);
  }

  msync(frameBuffer, _width * _height * 4, 0);
//  munmap(frameBuffer, _width * _height * 4);
//  close(fd01);
}

void TFT000001::drawCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                    uint8_t cornername, uint32_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;

//  int fd01 = open(framebuf01, O_RDWR);
//  uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

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
      drawPixel_framebuff(frameBuffer, x0 + x, y0 + y, color);
      drawPixel_framebuff(frameBuffer, x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel_framebuff(frameBuffer, x0 + x, y0 - y, color);
      drawPixel_framebuff(frameBuffer, x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel_framebuff(frameBuffer, x0 - y, y0 + x, color);
      drawPixel_framebuff(frameBuffer, x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel_framebuff(frameBuffer, x0 - y, y0 - x, color);
      drawPixel_framebuff(frameBuffer, x0 - x, y0 - y, color);
    }
  }
  msync(frameBuffer, _width * _height * 4, 0);
//  munmap(frameBuffer, _width * _height * 4);
//  close(fd01);
}

void TFT000001::fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color) {

  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;
  int32_t px = x;
  int32_t py = y;

  delta++; // Avoid some +1's in the loop

//  int fd01 = open(framebuf01, O_RDWR);
//  uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

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
        writeFastVLine_framebuff(frameBuffer, x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        writeFastVLine_framebuff(frameBuffer, x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        writeFastVLine_framebuff(frameBuffer, x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        writeFastVLine_framebuff(frameBuffer, x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
  msync(frameBuffer, _width * _height * 4, 0);
//  munmap(frameBuffer, _width * _height * 4);
//  close(fd01);
}

void TFT000001::fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                                int32_t x2, int32_t y2, uint32_t color) {

  int32_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    yama_2_GFX_H_beta_swap_int32_t(y0, y1);
    yama_2_GFX_H_beta_swap_int32_t(x0, x1);
  }
  if (y1 > y2) {
    yama_2_GFX_H_beta_swap_int32_t(y2, y1);
    yama_2_GFX_H_beta_swap_int32_t(x2, x1);
  }
  if (y0 > y1) {
    yama_2_GFX_H_beta_swap_int32_t(y0, y1);
    yama_2_GFX_H_beta_swap_int32_t(x0, x1);
  }

  startWrite();
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
    writeFastHLine(a, y0, b - a + 1, color);
    endWrite();
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

//  int fd01 = open(framebuf01, O_RDWR);
//  uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

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
      yama_2_GFX_H_beta_swap_int32_t(a, b);
    writeFastHLine_framebuff(frameBuffer, a, y, b - a + 1, color);
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
      yama_2_GFX_H_beta_swap_int32_t(a, b);
    writeFastHLine_framebuff(frameBuffer, a, y, b - a + 1, color);
  }
  msync(frameBuffer, _width * _height * 4, 0);
//  munmap(frameBuffer, _width * _height * 4);
//  close(fd01);
}

void TFT000001::writeFastVLine_framebuff(uint32_t *frameBuffer, int32_t x, int32_t y, int32_t h, uint32_t color) {

    int32_t i, j;

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;

    if((y+h-1) >= _height) h = _height-y;

    for(j = 0; j < h; j++){
        i = 0;
        drawPixel_framebuff(frameBuffer, x + i, y + j, color);
    }
}

void TFT000001::writeFastHLine_framebuff(uint32_t *frameBuffer, int32_t x, int32_t y, int32_t w, uint32_t color) {
    int32_t i, j;

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;

    for(i = 0; i < w; i++){
        j = 0;
        drawPixel_framebuff(frameBuffer, x + i, y + j, color);
    }
}

void TFT000001::drawPixel(int32_t x, int32_t y, uint32_t color) {
    if (colorModeflag == 16) {
        drawPixel16(x, y, color);
    } else if (colorModeflag == 24) {
        drawPixel24(x, y, color);
    }
}

void TFT000001::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (colorModeflag == 16) {
        fillRect16(x, y, w, h, color);
    } else if (colorModeflag == 24) {
        fillRect24(x, y, w, h, color);
    }
}

void TFT000001::drawPixel16(int32_t x, int32_t y, uint32_t color)
{
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
 

}


void TFT000001::drawRGBBitmap16to16(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h)
{
    int32_t i, j;
    const uint16_t *p = (uint16_t *)bitmap;
 
}

void TFT000001::fill_color16(uint16_t color, uint32_t len)
{
}

void TFT000001::fillRect16(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= _width) || (y >= _height)) return;
        if((x + w - 1) >= _width)  w = _width  - x;
        if((y + h - 1) >= _height) h = _height - y;
 
    }
}

void TFT000001::drawPixel24(int32_t x, int32_t y, uint32_t color)
{
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

    drawPixel_framebuff(frameBuffer, x, y, color);

    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}

void TFT000001::drawRGBBitmap24to24(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h)
{
    int32_t i, j;
    uint32_t c;
    uint8_t *p = (uint8_t *)bitmap;

//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

    for(j = 0; j < h; j++){
        for(i = 0; i < w; i++) {
            c = ((*p++) << 16) | ((*p++) << 8) | (*p++);
            if(((x + i) < 0) ||((x + i) >= _width) || ((y + j) < 0) || ((y + j) >= _height)) {}
            else {
                drawPixel_framebuff(frameBuffer, x + i, y + j, c);
            }
        }
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}

void TFT000001::fillRect24(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    int32_t i, j;
    if(w > 0 && h > 0){
        // rudimentary clipping (drawChar w/big text requires this)
        if((x >= _width) || (y >= _height)) return;
        if((x + w - 1) >= _width)  w = _width  - x;
        if((y + h - 1) >= _height) h = _height - y;

//        int fd01 = open(framebuf01, O_RDWR);
//        uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

        for(j = 0; j < h; j++){
            for(i = 0; i < w; i++) {
                if(((x + i) < 0) ||((x + i) >= _width) || ((y + j) < 0) || ((y + j) >= _height)) {}
                else {
                    drawPixel_framebuff(frameBuffer, x + i, y + j, color);
                }
            }
        }
        msync(frameBuffer, _width * _height * 4, 0);
//        munmap(frameBuffer, _width * _height * 4);
//        close(fd01);
    }
}

void TFT000001::drawPalette256Bitmap16(int32_t x, int32_t y, const uint8_t bitmap[], uint32_t w, uint32_t h, uint16_t *palette_address) {
    int32_t i, j;
    uint16_t c, *palette01;
    uint8_t *p = (uint8_t *)bitmap;
    if (palette_address == NULL) palette01 = (uint16_t *)palette256_table;
    else                         palette01 = (uint16_t *)palette_address;
 
/*
    spi_begin();
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    for(j = 0; j < h; j++){
        for(i = 0; i < w; i++) {
            c = palette01[*p++];
            spi_write16_big((uint16_t)c);
        }
    }
    spi_end();
*/

}

void TFT000001::drawPalette256Bitmap24(int32_t x, int32_t y, const uint8_t bitmap[], uint32_t w, uint32_t h, uint32_t *palette_address) {
    int32_t i, j;
    uint32_t c1, *palette01;
    uint8_t *p = (uint8_t *)bitmap;
    if (palette_address == NULL) palette01 = (uint32_t *)palette256_24_table;
    else                         palette01 = (uint32_t *)palette_address;
    int32_t odd001, count01;

//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

    for(j = 0; j < h; j++){
        for(i = 0; i < w; i++) {
            if(((x + i) < 0) ||((x + i) >= _width) || ((y + j) < 0) || ((y + j) >= _height)) {}
            else {
                c1 = palette01[*p++];
                drawPixel_framebuff(frameBuffer, x + i, y + j, c1);
            }
        }
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}

void TFT000001::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {
    if (colorModeflag == 16) {
        drawFastVLine16(x, y, h, color);
    } else if (colorModeflag == 24) {
        drawFastVLine24(x, y, h, color);
    }
}

void TFT000001::drawFastVLine16(int32_t x, int32_t y, int32_t h, uint32_t color) {
 
    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
 
    if((y+h-1) >= _height) h = _height-y;
/*
    spi_begin();
    setAddrWindow(x, y, x, y+h-1);
    fill_color16((uint16_t)color,h);
    spi_end();
*/
}

void TFT000001::drawFastVLine24(int32_t x, int32_t y, int32_t h, uint32_t color) {

    int32_t i, j;

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
 
    if((y+h-1) >= _height) h = _height-y;

//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

    for(j = 0; j < h; j++){
        i = 0;
        drawPixel_framebuff(frameBuffer, x + i, y + j, color);
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}

void TFT000001::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {
    if (colorModeflag == 16) {
        drawFastHLine16(x, y, w, color);
    } else if (colorModeflag == 24) {
        drawFastHLine24(x, y, w, color);
    }
}

void TFT000001::drawBitmap(int32_t x, int32_t y,const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color, uint32_t bg) {
    if (getColorMode() == 16) {
        drawBitmap16(x, y, bitmap, w, h, color, bg);
    }
    if (getColorMode() == 24) {
        drawBitmap24(x, y, bitmap, w, h, color, bg);
    }
}

void TFT000001::drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                       int32_t w, int32_t h, uint32_t color) {
    if (getColorMode() == 16) {
        drawBitmap16(x, y, bitmap, w, h, color);
    }
    if (getColorMode() == 24) {
        drawBitmap24(x, y, bitmap, w, h, color);
    }
}

void TFT000001::drawBitmap(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg) {
    if (getColorMode() == 16) {
        drawBitmap16(x, y, bitmap01, bitmap_mask01, width, height, fg, bg);
    }
    if (getColorMode() == 24) {
        drawBitmap24(x, y, bitmap01, bitmap_mask01, width, height, fg, bg);
    }
}

void TFT000001::drawFastHLine16(int32_t x, int32_t y, int32_t w, uint32_t color) {
 
    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;
  
/*
    spi_begin();
    setAddrWindow(x, y, x+w-1, y);
    fill_color16((uint16_t)color, w);
    spi_end();
*/
}

void TFT000001::drawFastHLine24(int32_t x, int32_t y, int32_t w, uint32_t color) {
    int32_t i, j;
 
    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;
  
//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

    for(i = 0; i < w; i++){
        j = 0;
        drawPixel_framebuff(frameBuffer, x + i, y + j, color);
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}


void TFT000001::drawBitmap16(int32_t x, int32_t y,const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color, uint32_t bg)
{
    int16_t xi,yi;
    const uint8_t *p = bitmap;

/*
    spi_begin();
    setAddrWindow(x, y, x+w-1, y+h-1);
    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi+=8){
            uint8_t m = 0x80;
            uint8_t d = *p++;
            for(int32_t i=0; i<8 && xi+i < w; i++,m>>=1) {
               pixel_write16(m & d ? (uint16_t)color : (uint16_t)bg);
            }
        }
    }
    spi_end();
*/
}

void TFT000001::drawBitmap24(int32_t x, int32_t y,const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color, uint32_t bg)
{
    int16_t xi,yi;
    const uint8_t *p = bitmap;

//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);
 
    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi+=8){
            uint8_t m = 0x80;
            uint8_t d = *p++;
            for(int32_t i=0; i<8 && xi+i < w; i++,m>>=1) {
               drawPixel_framebuff(frameBuffer, x + xi + i, y + yi, (m & d) ? color : bg);
            }
        }
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}

void TFT000001::drawGrayscaleBitmap(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
    if (getColorMode() == 16) {
        drawGrayscaleBitmap16(x, y, bitmap, w, h, red_flag, green_flag, blue_flag, invert);
    }
    if (getColorMode() == 24) {
        drawGrayscaleBitmap24(x, y, bitmap, w, h, red_flag, green_flag, blue_flag, invert);
    }
}

void TFT000001::drawGrayscaleBitmap(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
    if (getColorMode() == 16) {
        drawGrayscaleBitmap16(x, y, bitmap, bitmap_mask, w, h, red_flag, green_flag, blue_flag, invert);
    }
    if (getColorMode() == 24) {
        drawGrayscaleBitmap24(x, y, bitmap, bitmap_mask, w, h, red_flag, green_flag, blue_flag, invert);
    }
}

void TFT000001::drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
    int32_t xi,yi;
    const uint8_t *p = bitmap;
    uint32_t red, green, blue;
    uint16_t c, col;

    if (red_flag)   red = 0x1f;
    else            red = 0;
    if (green_flag) green = 0x1f;
    else            green = 0;
    if (blue_flag)  blue = 0x1f;
    else            blue = 0;
/*
    spi_begin();
    setAddrWindow(x, y, x+w-1, y+h-1);
    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi++) {
            c = (uint16_t)*p++;
            if (invert)
                col = (uint16_t)(((((255 - c) >> 3) & red) << 11) | (((((255 - c) >> 3) & green) << 1) << 5) | (((255 - c) >> 3) & blue));
            else
                col = (uint16_t)((((c >> 3) & red) << 11) | ((((c >> 3) & green) << 1) << 5) | ((c >> 3) & blue));
            pixel_write16(col);
        }
    }
    spi_end();
*/
}

void TFT000001::drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[],
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
    int32_t xi,yi;
    const uint8_t *p = bitmap;
    uint32_t red, green, blue;
    uint32_t c, col;

    if (red_flag)   red = 0x1f;
    else            red = 0;
    if (green_flag) green = 0x1f;
    else            green = 0;
    if (blue_flag)  blue = 0x1f;
    else            blue = 0;

//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

    for(yi=0; yi<h; yi++){
        for(xi=0; xi<w; xi++) {
            c = (uint32_t)*p++;
            if (invert)
                col = (uint32_t)((((255 - c) & red) << 16) | (((255 - c) & green) << 8) | ((255 - c) & blue));
            else
                col = (uint32_t)(((c & red) << 16) | ((c & green) << 8) | (c & blue));
                drawPixel_framebuff(frameBuffer, x + xi, y + yi, col);

        }
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}


void TFT000001::drawGrayscaleBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
    yama_2_GFX_H_beta::drawGrayscaleBitmap16(x, y, bitmap, bitmap_mask, w, h, red_flag, green_flag, blue_flag, invert);
}

void TFT000001::drawGrayscaleBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[], 
                       int32_t w, int32_t h, bool red_flag, bool green_flag, bool blue_flag, bool invert) {
    yama_2_GFX_H_beta::drawGrayscaleBitmap24(x, y, bitmap, bitmap_mask, w, h, red_flag, green_flag, blue_flag, invert);
}

void TFT000001::drawRGBBitmap16to16(int32_t x, int32_t y,
                           const uint8_t bitmap[], const uint8_t bitmap_mask[],
                           int32_t w, int32_t h) {
    yama_2_GFX_H_beta::drawRGBBitmap16(x, y, bitmap, bitmap_mask, w, h);
}

void TFT000001::drawRGBBitmap24to24(int32_t x, int32_t y,
                           const uint8_t bitmap[], const uint8_t bitmap_mask[],
                           int32_t w, int32_t h) {
    yama_2_GFX_H_beta::drawRGBBitmap24(x, y, bitmap, bitmap_mask, w, h);
}


void TFT000001::drawBitmap16(int32_t x, int32_t y, const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color) {
    yama_2_GFX_H_beta::drawBitmap(x, y, bitmap, w, h, color);
}

void TFT000001::drawBitmap24(int32_t x, int32_t y, const uint8_t bitmap[],
                    int32_t w, int32_t h,
                    uint32_t color) {
    yama_2_GFX_H_beta::drawBitmap(x, y, bitmap, w, h, color);
}

void TFT000001::drawBitmap16(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg) {
    yama_2_GFX_H_beta::drawBitmap(x, y, bitmap01, bitmap_mask01, width, height, fg, bg);
}

void TFT000001::drawBitmap24(int32_t x, int32_t y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01,
                                  int32_t width, int32_t height,
                                  uint32_t fg, uint32_t bg) {
    yama_2_GFX_H_beta::drawBitmap(x, y, bitmap01, bitmap_mask01, width, height, fg, bg);
}

void TFT000001::drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t width, int32_t height) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, width, height);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, width, height);
    }
}

void TFT000001::drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t width, int32_t height) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, bitmap_mask, width, height);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, bitmap_mask, width, height);
    }
}

void TFT000001::drawPalette256Bitmap(int32_t x, int32_t y, const uint8_t bitmap[],
                                     int32_t w, int32_t h, void *palette_address) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, w, h, (uint16_t *)palette_address);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, w, h, (uint32_t *)palette_address);
    }
}

void TFT000001::drawPalette256Bitmap(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t w, int32_t h, void *palette_address) {
    if (getColorMode() == 16) {
        drawPalette256Bitmap16(x, y, bitmap, bitmap_mask, w, h, (uint16_t *)palette_address);
    }
    if (getColorMode() == 24) {
        drawPalette256Bitmap24(x, y, bitmap, bitmap_mask, w, h, (uint32_t *)palette_address);
    }
}

void TFT000001::drawPalette256Bitmap16(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t w, int32_t h, uint16_t *palette_address) {
    yama_2_GFX_H_beta::drawPalette256Bitmap16(x, y, bitmap, bitmap_mask, w, h, (uint16_t *)palette_address);
}

void TFT000001::drawPalette256Bitmap24(int32_t x, int32_t y, 
                                     const uint8_t bitmap[], const uint8_t bitmap_mask[],
                                     int32_t w, int32_t h, uint32_t *palette_address) {
    yama_2_GFX_H_beta::drawPalette256Bitmap24(x, y, bitmap, bitmap_mask, w, h, (uint32_t *)palette_address);
}


void TFT000001::drawRGBBitmap16(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    if (getColorMode() == 16) {
        drawRGBBitmap16to16(x, y, bitmap, w, h);
    }
    if (getColorMode() == 24) {
        drawRGBBitmap16to24(x, y, bitmap, w, h);
    }
}

void TFT000001::drawRGBBitmap24(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    if (getColorMode() == 16) {
        drawRGBBitmap24to16(x, y, bitmap, w, h);
    }
    if (getColorMode() == 24) {
        drawRGBBitmap24to24(x, y, bitmap, w, h);
    }
}

void TFT000001::drawRGBBitmap16(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[],
                       int32_t w, int32_t h) {
    if (getColorMode() == 16) {
        drawRGBBitmap16to16(x, y, bitmap, bitmap_mask, w, h);
    }
    if (getColorMode() == 24) {
        drawRGBBitmap16to24(x, y, bitmap, bitmap_mask, w, h);
    }
}

void TFT000001::drawRGBBitmap24(int32_t x, int32_t y,
                       const uint8_t bitmap[], const uint8_t bitmap_mask[],
                       int32_t w, int32_t h) {
    if (getColorMode() == 16) {
        drawRGBBitmap24to16(x, y, bitmap, bitmap_mask, w, h);
    }
    if (getColorMode() == 24) {
        drawRGBBitmap24to24(x, y, bitmap, bitmap_mask, w, h);
    }
}


void TFT000001::drawRGBBitmap24to16(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h)
{
    int32_t i, j;
    uint16_t c;
    uint8_t *p = (uint8_t *)bitmap;
 
/*
//    int fd01 = open(framebuf01, O_RDWR);
//    uint16_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 2, PROT_WRITE, MAP_SHARED, fd01, 0);

    for(j = 0; j < h; j++){
        for(i = 0; i < w; i++) {
            c = (uint16_t)((((uint16_t)(*p++) >> 3) << 11) | (((uint16_t)(*p++) >> 2) << 5) | ((uint16_t)(*p++) >> 3));
            drawPixel_framebuff16(frameBuffer, x + i, y + j, c);
        }
    }

    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
*/
}

void TFT000001::drawRGBBitmap16to24(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h)
{
    int32_t i, j;
    const uint16_t *p = (uint16_t *)bitmap;
 
//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
          drawPixel_framebuff(frameBuffer, x + i, y + j, color16to24inline(*p++));
        }
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}

void TFT000001::drawRGBBitmap24to16(int32_t x, int32_t y, const uint8_t bitmap01[],
                        const uint8_t bitmap_mask01[], int32_t w, int32_t h) {
  struct yama_2_GFX_H_beta_RGB *bitmap = (struct yama_2_GFX_H_beta_RGB *)bitmap01;
  int32_t n;
  int32_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = *((const uint8_t *)(&bitmap_mask01[j * bw + i / 8]));
      if (byte & 0x80) {
        n = j * w + i;
        drawPixel16(x + i, y, (uint16_t)(((((uint16_t)bitmap[n].r & 0xff) >> 3) << 11) | ((((uint16_t)bitmap[n].g & 0xff) >> 2) << 5) | (((uint16_t)bitmap[n].b & 0xff) >> 3)));
      }
    }
  }}

void TFT000001::drawRGBBitmap16to24(int32_t x, int32_t y, const uint8_t bitmap01[],
                        const uint8_t bitmap_mask01[], int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)bitmap01;
  int32_t bw = (w + 7) / 8; // Bitmask scanline pad = whole byte
  uint8_t byte = 0;

//  int fd01 = open(framebuf01, O_RDWR);
//  uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);

  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = bitmap_mask01[j * bw + i / 8];
      if (byte & 0x80) {
        drawPixel_framebuff(frameBuffer, x + i, y, color16to24inline(bitmap[j * w + i]));
      }
    }
  }
  msync(frameBuffer, _width * _height * 4, 0);
//  munmap(frameBuffer, _width * _height * 4);
//  close(fd01);
}










void TFT000001::resetPushColor24(int x1, int y1, int x2, int y2) {
    pushColor_x1 = x1;
    pushColor_left = x1;
    pushColor_y1 = y1;
    pushColor_x2 = x2;
    pushColor_y2 = y2;
}

void TFT000001::pushColors24(uint8_t *data, int32_t size) {
    uint8_t  *p;
    int      color01;

//    int fd01 = open(framebuf01, O_RDWR);
//    uint32_t *frameBuffer = (uint32_t *)mmap(NULL, _width * _height * 4, PROT_WRITE, MAP_SHARED, fd01, 0);
    p = data;
    for (int i = 0; i < size; i++) {
        color01 = ((int)*p++) << 16 | ((int)*p++) << 8 | ((int)*p++);
        drawPixel_framebuff(frameBuffer, pushColor_x1, pushColor_y1, color01);
        pushColor_x1++;
        if (pushColor_x1 == pushColor_x2 + 1) {
            pushColor_y1++;
            pushColor_x1 = pushColor_left;
        }
    }
    msync(frameBuffer, _width * _height * 4, 0);
//    munmap(frameBuffer, _width * _height * 4);
//    close(fd01);
}

// -------------------------------------------------------------------------
 
// yama_2_GFX_H_beta_Canvas1, yama_2_GFX_H_beta_Canvas8 and yama_2_GFX_H_beta_Canvas16 (currently a WIP, don't get too
// comfy with the implementation) provide 1-, 8- and 16-bit offscreen
// canvases, the address of which can be passed to drawBitmap() or
// pushColors() (the latter appears only in a couple of GFX-subclassed TFT
// libraries at this time).  This is here mostly to help with the recently-
// added proportionally-spaced fonts; adds a way to refresh a section of the
// screen without a massive flickering clear-and-redraw...but maybe you'll
// find other uses too.  VERY RAM-intensive, since the buffer is in MCU
// memory and not the display driver...GXFcanvas1 might be minimally useful
// on an Uno-class board, but this and the others are much more likely to
// require at least a Mega or various recent ARM-type boards (recommended,
// as the text+bitmap draw can be pokey).  yama_2_GFX_H_beta_Canvas1 requires 1 bit per
// pixel (rounded up to nearest byte per scanline), yama_2_GFX_H_beta_Canvas8 is 1 byte
// per pixel (no scanline pad), and yama_2_GFX_H_beta_Canvas16 uses 2 bytes per pixel (no
// scanline pad).
// NOT EXTENSIVELY TESTED YET.  MAY CONTAIN WORST BUGS KNOWN TO HUMANKIND.
 
/**************************************************************************/
/*!
   @brief    Instatiate a GFX 1-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas1::yama_2_GFX_H_beta_Canvas1(uint32_t w, uint32_t h) : yama_2_GFX_H_beta(w, h) {
  uint32_t bytes = ((w + 7) / 8) * h;
  if ((buffer = (uint8_t *)malloc(bytes))) {
    memset(buffer, 0, bytes);
  }
}
 
/**************************************************************************/
/*!
   @brief    Delete the canvas, free memory
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas1::~yama_2_GFX_H_beta_Canvas1(void) {
  if (buffer)
    free(buffer);
}
 
/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x     x coordinate
    @param  y     y coordinate
    @param  color Binary (on or off) color to fill with
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas1::drawPixel(int32_t x, int32_t y, uint32_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;
 
    int32_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }
 
    uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
#ifdef __AVR__
    if (color)
      *ptr |= *((uint8_t *)(&GFXsetBit[x & 7]));
    else
      *ptr &= *((uint8_t *)(&yama_2_GFX_H_beta_ClrBit[x & 7]));
#else
    if (color)
      *ptr |= 0x80 >> (x & 7);
    else
      *ptr &= ~(0x80 >> (x & 7));
#endif
  }
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's binary color value, either 0x1 (on) or 0x0
   (off)
*/
/**********************************************************************/
bool yama_2_GFX_H_beta_Canvas1::getPixel(int32_t x, int32_t y) const {
  int32_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
  return getRawPixel(x, y);
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's binary color value, either 0x1 (on) or 0x0
   (off)
*/
/**********************************************************************/
bool yama_2_GFX_H_beta_Canvas1::getRawPixel(int32_t x, int32_t y) const {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (this->getBuffer()) {
    uint8_t *buffer = this->getBuffer();
    uint8_t *ptr = &buffer[(x / 8) + y * ((WIDTH + 7) / 8)];
 
#ifdef __AVR__
    return ((*ptr) & *((uint8_t *)(&GFXsetBit[x & 7]))) != 0;
#else
    return ((*ptr) & (0x80 >> (x & 7))) != 0;
#endif
  }
  return 0;
}
 
/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color Binary (on or off) color to fill with
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas1::fillScreen(uint32_t color) {
  if (buffer) {
    uint32_t bytes = ((WIDTH + 7) / 8) * HEIGHT;
    memset(buffer, color ? 0xFF : 0x00, bytes);
  }
}
 
/**************************************************************************/
/*!
   @brief    Instatiate a GFX 8-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas8::yama_2_GFX_H_beta_Canvas8(uint32_t w, uint32_t h) : yama_2_GFX_H_beta(w, h) {
  uint32_t bytes = w * h;
  if ((buffer = (uint8_t *)malloc(bytes))) {
    memset(buffer, 0, bytes);
  }
}
 
/**************************************************************************/
/*!
   @brief    Delete the canvas, free memory
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas8::~yama_2_GFX_H_beta_Canvas8(void) {
  if (buffer)
    free(buffer);
}
 
/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 8-bit Color to fill with. Only lower byte of uint32_t is used.
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas8::drawPixel(int32_t x, int32_t y, uint32_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;
 
    int32_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }
 
    buffer[x + y * WIDTH] = color;
  }
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 8-bit color value
*/
/**********************************************************************/
uint8_t yama_2_GFX_H_beta_Canvas8::getPixel(int32_t x, int32_t y) const {
  int32_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
  return getRawPixel(x, y);
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 8-bit color value
*/
/**********************************************************************/
uint8_t yama_2_GFX_H_beta_Canvas8::getRawPixel(int32_t x, int32_t y) const {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (buffer) {
    return buffer[x + y * WIDTH];
  }
  return 0;
}
 
/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color 8-bit Color to fill with. Only lower byte of uint32_t is used.
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas8::fillScreen(uint32_t color) {
  if (buffer) {
    memset(buffer, color, WIDTH * HEIGHT);
  }
}
 
void yama_2_GFX_H_beta_Canvas8::writeFastHLine(int32_t x, int32_t y, int32_t w,
                                uint32_t color) {
 
  if ((x >= _width) || (y < 0) || (y >= _height))
    return;
  int32_t x2 = x + w - 1;
  if (x2 < 0)
    return;
 
  // Clip left/right
  if (x < 0) {
    x = 0;
    w = x2 + 1;
  }
  if (x2 >= _width)
    w = _width - x;
 
  int32_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
 
  memset(buffer + y * WIDTH + x, color, w);
}
 
/**************************************************************************/
/*!
   @brief    Instatiate a GFX 16-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas16::yama_2_GFX_H_beta_Canvas16(uint32_t w, uint32_t h) : yama_2_GFX_H_beta(w, h) {
  uint32_t bytes = w * h * 2;
  if ((buffer = (uint16_t *)malloc(bytes))) {
    memset(buffer, 0, bytes);
  }
  setColorMode16();
}
 
/**************************************************************************/
/*!
   @brief    Delete the canvas, free memory
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas16::~yama_2_GFX_H_beta_Canvas16(void) {
  if (buffer)
    free(buffer);
}
 
/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas16::drawPixel(int32_t x, int32_t y, uint32_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;
 
    int32_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }
 
    buffer[x + y * WIDTH] = color;
  }
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint16_t yama_2_GFX_H_beta_Canvas16::getPixel(int32_t x, int32_t y) const {
  int32_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
  return getRawPixel(x, y);
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint16_t yama_2_GFX_H_beta_Canvas16::getRawPixel(int32_t x, int32_t y) const {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (buffer) {
    return buffer[x + y * WIDTH];
  }
  return 0;
}
 
/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas16::fillScreen(uint32_t color) {
  if (buffer) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    if (hi == lo) {
      memset(buffer, lo, WIDTH * HEIGHT * 2);
    } else {
      uint32_t i, pixels = WIDTH * HEIGHT;
      for (i = 0; i < pixels; i++)
        buffer[i] = color;
    }
  }
}
 
/**************************************************************************/
/*!
    @brief  Reverses the "endian-ness" of each 16-bit pixel within the
            canvas; little-endian to big-endian, or big-endian to little.
            Most microcontrollers (such as SAMD) are little-endian, while
            most displays tend toward big-endianness. All the drawing
            functions (including RGB bitmap drawing) take care of this
            automatically, but some specialized code (usually involving
            DMA) can benefit from having pixel data already in the
            display-native order. Note that this does NOT convert to a
            SPECIFIC endian-ness, it just flips the bytes within each word.
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas16::byteSwap(void) {
  if (buffer) {
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++)
      buffer[i] = __builtin_bswap16(buffer[i]);
  }
}




/**************************************************************************/
/*!
   @brief    Instatiate a GFX 24-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas24::yama_2_GFX_H_beta_Canvas24(uint32_t w, uint32_t h) : yama_2_GFX_H_beta(w, h) {
  uint32_t bytes = w * h * 3;
  if ((buffer = (struct yama_2_GFX_H_beta_RGB *)malloc(bytes))) {
    memset(buffer, 0, bytes);
  }
  setColorMode24();
}
 
/**************************************************************************/
/*!
   @brief    Delete the canvas, free memory
*/
/**************************************************************************/
yama_2_GFX_H_beta_Canvas24::~yama_2_GFX_H_beta_Canvas24(void) {
  if (buffer)
    free(buffer);
}
 
/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas24::drawPixel(int32_t x, int32_t y, uint32_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;
 
    int32_t t;
    switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
    }
    struct yama_2_GFX_H_beta_RGB c;
    c.r = (uint8_t)(color >> 16);
    c.g = (uint8_t)(color >> 8);
    c.b = (uint8_t)color;
    buffer[x + y * WIDTH] = c;
  }
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint32_t yama_2_GFX_H_beta_Canvas24::getPixel(int32_t x, int32_t y) const {
  int32_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }
  return getRawPixel(x, y);
}
 
/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, unrotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint32_t yama_2_GFX_H_beta_Canvas24::getRawPixel(int32_t x, int32_t y) const {
  if ((x < 0) || (y < 0) || (x >= WIDTH) || (y >= HEIGHT))
    return 0;
  if (buffer) {
    int32_t n = x + y * WIDTH;
    uint32_t color = ((uint32_t)buffer[n].r << 16) | ((uint32_t)buffer[n].g << 8) | buffer[n].b;
    return color;
  }
  return 0;
}
 
/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas24::fillScreen(uint32_t color) {
  if (buffer) {
    struct yama_2_GFX_H_beta_RGB _color;
    _color.r = (uint8_t)(color >> 16);
    _color.g = (uint8_t)(color >> 8);
    _color.b = (uint8_t)color;
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++) {
      buffer[i] = _color;
    }
  }
}
 
/**************************************************************************/
/*!
    @brief  Reverses the "endian-ness" of each 16-bit pixel within the
            canvas; little-endian to big-endian, or big-endian to little.
            Most microcontrollers (such as SAMD) are little-endian, while
            most displays tend toward big-endianness. All the drawing
            functions (including RGB bitmap drawing) take care of this
            automatically, but some specialized code (usually involving
            DMA) can benefit from having pixel data already in the
            display-native order. Note that this does NOT convert to a
            SPECIFIC endian-ness, it just flips the bytes within each word.
*/
/**************************************************************************/
void yama_2_GFX_H_beta_Canvas24::byteSwap(void) {
  struct yama_2_GFX_H_beta_RGB c;
  uint8_t temp;
  if (buffer) {
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++) {
      c = buffer[i];
      temp = c.r;
      c.r = c.b;
      c.b = temp;
      buffer[i] = c;
    }
  }
}
