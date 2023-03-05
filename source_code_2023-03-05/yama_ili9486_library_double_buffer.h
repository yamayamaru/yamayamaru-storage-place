#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>

#define SPI_MODE01_0        (0)
#define SPI_MODE01_1        (0 | SPI_CPHA)
#define SPI_MODE01_2        (SPI_CPOL | 0)
#define SPI_MODE01_3        (SPI_CPOL | SPI_CPHA)
#define SPI_SPEED_HZ01    20000000
#define SPI_DELAY_USECS01 0
#define SPI_BITS01        8
#define SPI_MODE          SPI_MODE01_0

#define ILI9486_BLACK      0x000000 ///<   0,   0,   0
#define ILI9486_NAVY       0x000080 ///<   0,   0, 128
#define ILI9486_DARKGREEN  0x008000 ///<   0, 128,   0
#define ILI9486_DARKCYAN   0x008080 ///<   0, 128, 128
#define ILI9486_MAROON     0x800000 ///< 128,   0,   0
#define ILI9486_PURPLE     0x800080 ///< 128,   0, 128
#define ILI9486_OLIVE      0x808000 ///< 128, 128,   0
#define ILI9486_LIGHTGREY  0xC6C3C6 ///< 198, 195, 198
#define ILI9486_DARKGREY   0x808080 ///< 128, 128, 128
#define ILI9486_BLUE       0x0000FF ///<   0,   0, 255
#define ILI9486_GREEN      0x00FF00 ///<   0, 255,   0
#define ILI9486_CYAN       0x00FFFF ///<   0, 255, 255
#define ILI9486_RED        0xFF0000 ///< 255,   0,   0
#define ILI9486_MAGENTA    0xFF00FF ///< 255,   0, 255
#define ILI9486_YELLOW     0xFFFF00 ///< 255, 255,   0
#define ILI9486_WHITE      0xFFFFFF ///< 255, 255, 255
#define ILI9486_ORANGE     0xFFA500 ///< 255, 165,   0
#define ILI9486_GREENYELLO 0xADFF29 ///< 173, 255,  41
#define ILI9486_PINK       0xFF82C6 ///< 255, 130, 198

#define ILI9486_WIDTH  320
#define ILI9486_HEIGHT 480

#define ILI9486_INIT_DELAY 0x80

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

#define ILI9486_MAD_MY  0x80
#define ILI9486_MAD_MX  0x40
#define ILI9486_MAD_MV  0x20
#define ILI9486_MAD_ML  0x10
#define ILI9486_MAD_RGB 0x00
#define ILI9486_MAD_BGR 0x08
#define ILI9486_MAD_MH  0x04
#define ILI9486_MAD_SS  0x02
#define ILI9486_MAD_GS  0x01

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
void gpiod_close();
void gpiod_set_out(struct gpiod_line *pin, int num);

void spi_open();
void spi_close();
extern int  spidev_fd01;
void spi_write(uint8_t spi_send_byte);
void spi_write_buffer(uint8_t byte);
void spi_write_buffer_flush();


void delay_ili9486(long milli_sec);
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

uint32_t color24(uint32_t colr, uint32_t colg, uint32_t colb);
void spi_init(char *spi_device, char *gpiod_device, int spi_cs, int spi_dc, int spi_reset);
void ili9486_init();
void set_rotation_ILI9486(int mode);
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




extern uint8_t *canvas24_buffer;
extern int32_t canvas24_rotation;
extern int32_t canvas24_width;
extern int32_t canvas24_height;
extern int32_t canvas24_init_width;
extern int32_t canvas24_init_height;
int canvas24_begin(int32_t w, int32_t h);
void canvas24_end();
void canvas24_set_rotation(int32_t x);
void canvas24_drawPixel(int32_t x, int32_t y, uint32_t color);
void canvas24_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
void canvas24_drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void canvas24_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color);
void canvas24_drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
void canvas24_drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
void canvas24_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void canvas24_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void canvas24_drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h);
void canvas24_drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap, const uint8_t *mask, int32_t w, int32_t h);
void canvas24_update(int32_t x, int32_t y);
