#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>
#include <cmath>
#include <cstring>
#include <ctime>

#include <string>
#include <codecvt>
#include <iostream>
#include <locale>
#include <vector>
#include <sstream> // std::stringstream
#include <istream> // std::getline

#include <ft2build.h>
#include FT_FREETYPE_H


#define SPI_MODE01_0        (0)
#define SPI_MODE01_1        (0 | SPI_CPHA)
#define SPI_MODE01_2        (SPI_CPOL | 0)
#define SPI_MODE01_3        (SPI_CPOL | SPI_CPHA)
#define SPI_SPEED_HZ01    20000000
#define SPI_DELAY_USECS01 0
#define SPI_BITS01        8
#define SPI_MODE          SPI_MODE01_0

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

#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
 
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

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04


extern int32_t _init_width, _init_height;
extern int32_t _width, _height;

extern struct gpiod_chip *gpiod_chip01;
extern struct gpiod_line *gpiod_dc_lineout;
extern struct gpiod_line *gpiod_reset_lineout;
extern struct gpiod_line *gpiod_cs_lineout;
extern const char *appname;
extern int gpiod_value01;
extern int  spidev_fd01;

void gpiod_close();
void gpiod_set_out(struct gpiod_line *pin, int num);

int  spi_open();
void spi_close();
void spi_write(uint8_t spi_send_byte);
void spi_write_buffer(uint8_t byte);
void spi_write_buffer_flush();

void delay_ili9341(long milli_sec);

uint64_t diff_rdcycle(uint64_t last);
inline void spi_begin() {
    gpiod_set_out(gpiod_cs_lineout, 0);
}

inline void spi_end() {
    gpiod_set_out(gpiod_cs_lineout, 1);
}

inline void gpio_dc_on() {
    gpiod_set_out(gpiod_dc_lineout, 0);
}

inline void gpio_dc_off() {
    gpiod_set_out(gpiod_dc_lineout, 1);
}

inline void writedata(uint8_t byte) {
    spi_write_buffer(byte);
}

inline void writecommand(uint8_t byte) {
    gpio_dc_on();
    spi_write(byte);
    gpio_dc_off();
}

uint16_t color565(uint32_t colr, uint32_t colg, uint32_t colb);
void spi_init(const char *spi_device, const char *gpiod_device, int spi_cs, int spi_dc, int spi_reset);
void ili9341_init();
void set_rotation_ILI9341(int mode);
void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void drawPixel(int32_t x, int32_t y, uint32_t color);
void drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap, const uint8_t *mask, int32_t w, int32_t h);




extern uint16_t *canvas16_buffer;
extern int32_t canvas16_rotation;
extern int32_t canvas16_width;
extern int32_t canvas16_height;
extern int32_t canvas16_init_width;
extern int32_t canvas16_init_height;

int canvas16_begin(int32_t w, int32_t h);
void canvas16_end();
void canvas16_set_rotation(int32_t x);
void canvas16_drawPixel(int32_t x, int32_t y, uint32_t color);
void canvas16_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
void canvas16_drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void canvas16_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void canvas16_drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
void canvas16_drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
void canvas16_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void canvas16_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void canvas16_drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void canvas16_drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap, const uint8_t *mask, int32_t w, int32_t h);
void canvas16_update(int32_t x, int32_t y);




int init_freetype(FT_Library *ft_library01);
int set_font(FT_Library *ft_library01, FT_Face *ft_face01, const char *font_path);
int set_font_size(FT_Face *ft_face01, int32_t size, int32_t x_dpi, int32_t y_dpi);
void set_font_back_ground_fill(bool flag);
void set_font_color(int32_t fg, int32_t bg = -1);
int32_t drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, 
               const char *text, int32_t buf_size);
int32_t canvas16_drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, 
               const char *text, int32_t buf_size);
void set_font_pos(int32_t x, int32_t y);
void set_font_line_height(int32_t char_h);
void print(FT_Face *ft_face01, const char *text);
void canvas16_print(FT_Face *ft_face01, const char *text);
void word_print(FT_Face *ft_face01, std::string text);
void canvas16_word_print(FT_Face *ft_face01, std::string text);
