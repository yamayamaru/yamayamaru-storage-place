#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cstdbool>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <string>
#include <codecvt>
#include <iostream>
#include <locale>
#include <vector>
#include <sstream> // std::stringstream
#include <istream> // std::getline

#include <ft2build.h>
#include FT_FREETYPE_H


// i2c読み書き用のプロトタイプ宣言
int32_t i2c_open(const char *i2c_device);
void    i2c_close(int fd);
int i2c_read(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, uint16_t len);
int i2c_read_byte(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *bytedata);
int i2c_write(int fd, uint8_t i2c_address, uint8_t reg, uint8_t *data, uint16_t len);
int i2c_write_byte(int fd, uint8_t i2c_address, uint8_t reg, uint8_t bytedata);
int i2c_write_nonreg(int fd, uint8_t i2c_address, uint8_t *data, uint16_t len);





// ssd1306用のプロトタイプ宣言
#define SSD1306_BLACK 0   ///< Draw 'off' pixels
#define SSD1306_WHITE 1   ///< Draw 'on' pixels
#define SSD1306_INVERSE 2 ///< Invert pixels

#define SSD1306_EXTERNALVCC 0x01  ///< External display voltage source
#define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V

int  ssd1306_oled_setup(int fd, uint8_t vcs, uint8_t addr);
void ssd1306_close();
void ssd1306_display(void);
void clearDisplay(void);
void ssd1306_invertDisplay(int i);
void ssd1306_dim(int dim);
void ssd1306_drawPixel(int16_t x, int16_t y, uint16_t color);
void ssd1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void ssd1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void ssd1306_startscrollright(uint8_t start, uint8_t stop);
void ssd1306_startscrollleft(uint8_t start, uint8_t stop);
void ssd1306_startscrolldiagright(uint8_t start, uint8_t stop);
void ssd1306_startscrolldiagleft(uint8_t start, uint8_t stop);
void ssd1306_stopscroll(void);
void ssd1306_setContrast(uint8_t _contrast);
void ssd1306_command(uint8_t c);
bool ssd1306_getPixel(int16_t x, int16_t y);
uint8_t *ssd1306_getBuffer(void);

void ssd1306_clearDisplay(void);
void ssd1306_set_rotation(int32_t x);
void ssd1306_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
void ssd1306_fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
                                    uint8_t corners, int32_t delta,
                                    uint32_t color);
void ssd1306_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void ssd1306_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void ssd1306_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void ssd1306_drawBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void ssd1306_drawBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h);
extern int32_t    ssd1306_width;
extern int32_t    ssd1306_height;
extern int32_t    ssd1306_init_width;
extern int32_t    ssd1306_init_height;
extern int32_t    ssd1306_rotation;





// libfreetypeを使ったssd1306の文字表示用のプロトタイプ宣言
int init_ssd1306_freetype(FT_Library *ft_library01);
int ssd1306_set_font(FT_Library *ft_library01, FT_Face *ft_face01, const char *font_path);
int ssd1306_set_font_size(FT_Face *ft_face01, int32_t size, int32_t x_dpi, int32_t y_dpi);
void ssd1306_set_font_back_ground_fill(bool flag);
void ssd1306_set_font_color(int32_t fg, int32_t bg = -1);
int32_t ssd1306_drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y,
               const char *text, int32_t buf_size);
void ssd1306_set_font_pos(int32_t x, int32_t y);
void ssd1306_set_font_line_height(int32_t char_h);
void ssd1306_print(FT_Face *ft_face01, const char *text);
void ssd1306_word_print(FT_Face *ft_face01, std::string text);
