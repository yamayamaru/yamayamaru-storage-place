#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>
#include <time.h>

#define     SPI_CS       3
#define     SPI_DC       4
#define     SPI_RESET    5


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

int32_t _init_width, _init_height;
int32_t _width, _height;

struct gpiod_chip *gpiod_chip01;
struct gpiod_line_request *gpiod_dc_lineout;
struct gpiod_line_request *gpiod_reset_lineout;
struct gpiod_line_request *gpiod_cs_lineout;
const char *appname = "spitest01";
int gpiod_value01;
void gpiod_close();
void gpiod_set_out(struct gpiod_line_request *pin, int pin_num, int num);

void spi_open();
void spi_close();
int  spidev_fd01 = 0;
void spi_write(uint8_t spi_send_byte);
void spi_write_buffer(uint8_t byte);
void spi_write_buffer_flush();

void sigint_handler(int sig);
void delay(long milli_sec);
inline void spi_begin() {
    gpiod_set_out(gpiod_cs_lineout, SPI_CS, 0);
}

inline void spi_end() {
    gpiod_set_out(gpiod_cs_lineout, SPI_CS, 1);
}

inline void gpio_dc_on() {
    gpiod_set_out(gpiod_dc_lineout, SPI_DC, 0);
}

inline void gpio_dc_off() {
    gpiod_set_out(gpiod_dc_lineout, SPI_DC, 1);
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
void init();
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



double second();
void mandelbrot(int width, int height);
void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth);
int mand(double a1, double b1, int depth1);
void set_param(void);
struct _param01 {
    double xcorner;
    double ycorner;
    double length;
    int    depth;
} param01[28];


int main(int argc, char *argv[]) {

    // SIGINTハンドラの設定
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    init();

    ili9341_init();

    set_rotation_ILI9341(3);

    fillRect(0, 0, _width, _height, ILI9341_BLACK);

    set_param();

    for (int i = 0; i < 100; i++) {
        mandelbrot(_width, _height);
        delay(10000);
    }


    spi_close();
    gpiod_close();

    exit(0);
}

void set_param(void) {
    param01[0].xcorner =  -2.4;
    param01[0].ycorner =  -1.7;
    param01[0].length  =   3.2;
    param01[0].depth   =  1000;

    param01[1].xcorner = -6.1099999999999965e-001;
    param01[1].ycorner =  6e-001;
    param01[1].length  =  1.0000000000000001e-001;
    param01[1].depth   =  1000;

    param01[2].xcorner = -7.512120844523107e-001;
    param01[2].ycorner = 2.939359283447132e-002;
    param01[2].length = 2.441406250000000e-005;
    param01[2].depth = 1000;

    param01[3].xcorner = -1.7878664118448890e+000;
    param01[3].ycorner = -4.6994043985999932e-003;
    param01[3].length  =  1.5258789062500001e-006;
    param01[3].depth   =  1000;

    param01[4].xcorner = -1.255058807618605e+000;
    param01[4].ycorner =  3.834129844820908e-001;
    param01[4].length  =  5.960464477539063e-009;
    param01[4].depth   =  1000;

    param01[5].xcorner = -7.424999999999979e-002;
    param01[5].ycorner = -6.523749999999998e-001;
    param01[5].length = 3.125000000000000e-003;
    param01[5].depth   =  1000;

    param01[6].xcorner = -7.366145833333310e-002;
    param01[6].ycorner = -6.500052083333332e-001;
    param01[6].length = 3.125000000000000e-003;
    param01[6].depth   =  1000;

    param01[7].xcorner = -7.451562499999977e-002;
    param01[7].ycorner = -6.500117187500000e-001;
    param01[7].length = 7.812500000000000e-004;
    param01[7].depth   =  1000;

    param01[8].xcorner = -7.409765624999977e-002;
    param01[8].ycorner = -6.494752604166667e-001;
    param01[8].length = 1.953125000000000e-004;
    param01[8].depth   =  1000;

    param01[9].xcorner = -1.408903645833333e+000;
    param01[9].ycorner = -1.342989908854166e-001;
    param01[9].length = 2.441406250000000e-005;
    param01[9].depth   =  1000;

    param01[10].xcorner = -1.023473307291662e-001;
    param01[10].ycorner = 9.571370442708340e-001;
    param01[10].length = 4.882812500000000e-005;
    param01[10].depth   =  1000;

    param01[11].xcorner = -1.023286539713537e-001;
    param01[11].ycorner = 9.571538899739589e-001;
    param01[11].length = 1.220703125000000e-005;
    param01[11].depth   =  1000;

    param01[12].xcorner = -7.424999999999979e-002;
    param01[12].ycorner = -6.523749999999998e-001;
    param01[12].length = 3.125000000000000e-003;
    param01[12].depth   =  1000;

    param01[13].xcorner = -7.366145833333310e-002;
    param01[13].ycorner = -6.500052083333332e-001;
    param01[13].length = 3.125000000000000e-003;
    param01[13].depth   =  1000;

    param01[14].xcorner = -7.451562499999977e-002;
    param01[14].ycorner = -6.500117187500000e-001;
    param01[14].length = 7.812500000000000e-004;
    param01[14].depth   =  1000;

    param01[15].xcorner = -7.409765624999977e-002;
    param01[15].ycorner = -6.494752604166667e-001;
    param01[15].length = 1.953125000000000e-004;
    param01[15].depth   =  1000;

    param01[16].xcorner = -1.408903645833333e+000;
    param01[16].ycorner = -1.342989908854166e-001;
    param01[16].length = 2.441406250000000e-005;
    param01[16].depth   =  1000;

    param01[17].xcorner = -1.408903645833333e+000;
    param01[17].ycorner = -1.342989908854166e-001;
    param01[17].length = 2.441406250000000e-005;
    param01[17].depth   =  1000;

    param01[18].xcorner = -1.408886164585749e+000;
    param01[18].ycorner = -1.342802622318267e-001;
    param01[18].length = 1.192092895507813e-008;
    param01[18].depth   =  1000;

    param01[19].xcorner = -1.408886160294215e+000;
    param01[19].ycorner = -1.342802576224008e-001;
    param01[19].length = 2.980232238769531e-009;
    param01[19].depth   =  1000;

    param01[20].xcorner = -1.023473307291662e-001;
    param01[20].ycorner = 9.571370442708340e-001;
    param01[20].length = 4.882812500000000e-005;
    param01[20].depth   =  1000;

    param01[21].xcorner = -1.023286539713537e-001;
    param01[21].ycorner = 9.571538899739589e-001;
    param01[21].length = 1.220703125000000e-005;
    param01[21].depth   =  1000;

    param01[22].xcorner = -1.165292968750000e+000;
    param01[22].ycorner = 2.393867187500003e-001;
    param01[22].length = 3.906250000000000e-004;
    param01[22].depth   =  1000;

    param01[23].xcorner = -1.164973597208659e+000;
    param01[23].ycorner = 2.394546101888024e-001;
    param01[23].length = 1.525878906250000e-006;
    param01[23].depth   =  1000;

    param01[24].xcorner = -6.703997395833329e-001;
    param01[24].ycorner = -4.582591145833326e-001;
    param01[24].length = 3.906250000000000e-004;
    param01[24].depth   =  1000;

    param01[25].xcorner = -6.702213948567705e-001;
    param01[25].ycorner = -4.580732421874992e-001;
    param01[25].length = 2.441406250000000e-005;
    param01[25].depth   =  1000;

    param01[26].xcorner = -6.702324136098221e-001;
    param01[26].ycorner = -4.580734767913810e-001;
    param01[26].length = 1.907348632812500e-007;
    param01[26].depth   =  1000;

    param01[27].xcorner = -6.702323307991023e-001;
    param01[27].ycorner = -4.580733914375297e-001;
    param01[27].length = 2.384185791015625e-008;
    param01[27].depth   =  1000;
}

void mandelbrot(int width, int height) {

    double    xcorner, ycorner, length;
    int       xdot, ydot, depth;
    int       i;

    xdot = width;
    ydot = height;

    for (i = 0; i < 28; i++) {
        double start_time, end_time;

        start_time = second();
        mandel(param01[i].xcorner, param01[i].ycorner, param01[i].length, xdot, ydot, param01[i].depth);
        end_time = second();
        printf("time = %.4f\n", end_time - start_time);
    }

}

uint16_t pixel_buffer01[ILI9341_HEIGHT];

void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth) {
    double xgap, ygap;
    double xx, yy;
    int    i, j;
    int    col01, colr, colg, colb, color;

    // xgap = length / xdot;
    // ygap = length / xdot;
    xgap = length / ydot;
    ygap = length / ydot;

    //  x軸を240ドットから320ドットに広げる
    xcorner = xcorner - (xdot - ydot) / 2 * xgap;

    printf("xdot = %d    ydot = %d\r\n", xdot, ydot);
    printf("xcorner = %.15e\r\n", xcorner);
    printf("ycorner = %.15e\r\n", ycorner);
    printf("length  = %.15e\r\n", length);


    xx = xcorner;
    yy = ycorner;
    for(j = 0; j < ydot; j++) {
        for(i = 0; i < xdot; i++) {
            col01 = (short) mand( xx, yy, depth);
            if (col01 == 0){
                colr = 0;
                colg = 0;
                colb = 0;
            } else {
                colr = (col01 % 64) * 2 + 129;
                colg = (col01 % 64) * 4 + 3;
                colb = (col01 % 64) * 4 + 3;
            }
            color = color565(colr, colg, colb);
            pixel_buffer01[i] = color;

          xx = xx + xgap;
        }
        drawRGBBitmap(0, j, (uint8_t *)pixel_buffer01, _width, 1);

        yy = yy + ygap;
        xx = xcorner;
    }
}

int mand(double a1, double b1, int depth1) {
    int     i1;
    double  x1, y1, tmp1, x2, y2;

    i1 = 0;
    x1 = 0; y1 = 0;
    x2 = x1 * x1;   y2 = y1 * y1;
    while ((i1 < depth1) && (x2+y2 <=4)) {
        tmp1 = x2 - y2 + a1;
        y1 = 2 * x1 * y1 + b1;
        x1 = tmp1;
        x2 = x1 * x1;  y2 = y1 * y1;
        i1++;
    }

    if (i1 == depth1) i1 = 0;
    else            i1++;
    return i1;
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


void sigint_handler(int sig) {
    spi_close();
    gpiod_close();
    exit(1);
}


struct gpiod_line_settings *gline_settings_in, *gline_settings_out;
struct gpiod_line_config   *gline_config_in, *gline_config_out;
struct gpiod_request_config *gline_request_config_in, *gline_request_config_out;
struct gpiod_line_request   *gline_request_in, *gline_request_out;
int offset_out[3] = {SPI_CS, SPI_DC, SPI_RESET};
void init(){
    // GPIOデバイスを開く
    if ((gpiod_chip01=gpiod_chip_open("/dev/gpiochip4")) == NULL) {
        perror("gpiod_chip_open");
        exit(-1);
    }


    gline_settings_out = gpiod_line_settings_new();
    if (gpiod_line_settings_set_direction(gline_settings_out, GPIOD_LINE_DIRECTION_OUTPUT) != 0) {
      perror("gpiod_line_settings_set_direction");
    }
    gline_config_out = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(gline_config_out, offset_out, 3, gline_settings_out);
    gline_request_config_out = gpiod_request_config_new();
    gline_request_out = gpiod_chip_request_lines(gpiod_chip01, gline_request_config_out, gline_config_out);

    gpiod_reset_lineout = gline_request_out;
    gpiod_dc_lineout = gline_request_out;
    gpiod_cs_lineout = gline_request_out;

    gpiod_set_out(gline_request_out, SPI_RESET, 1);
    gpiod_set_out(gline_request_out, SPI_CS, 1);
    gpiod_set_out(gline_request_out, SPI_DC, 1);


/*
    // DCのピンのハンドラを取得する
    // ピンはhttps://mangopi.cc/_media/mq-pro-sch-v12.pdfで確認できます。
    if ((gpiod_dc_lineout=gpiod_chip_get_line(gpiod_chip01, SPI_DC)) == NULL) {
        perror("gpiod_chip_get_line");
        gpiod_close();
        exit(-1);
    }

    // DCを出力モードに設定する
    if (gpiod_line_request_output(gpiod_dc_lineout, appname, 0) != 0) {
        perror("gpiod_line_request_output");
        gpiod_close();
        exit(-1);
    }

    // DCの値を1に設定する
    gpiod_set_out(gpiod_dc_lineout, 1);

    // CSのピンのハンドラを取得する
    // ピンはhttps://mangopi.cc/_media/mq-pro-sch-v12.pdfで確認できます。
    if ((gpiod_cs_lineout=gpiod_chip_get_line(gpiod_chip01, SPI_CS)) == NULL) {
        perror("gpiod_chip_get_line");
        gpiod_close();
        exit(-1);
    }

    // CSを出力モードに設定する
    if (gpiod_line_request_output(gpiod_cs_lineout, appname, 0) != 0) {
        perror("gpiod_line_request_output");
        gpiod_close();
        exit(-1);
    }

    // CSの値を1に設定する
    gpiod_set_out(gpiod_cs_lineout, 1);

    // RESETのピンのハンドラを取得する
    // ピンはhttps://mangopi.cc/_media/mq-pro-sch-v12.pdfで確認できます。
    if ((gpiod_reset_lineout=gpiod_chip_get_line(gpiod_chip01, SPI_RESET)) == NULL) {
        perror("gpiod_chip_get_line");
        gpiod_close();
        exit(-1);
    }

    // RESETを出力モードに設定する
    if (gpiod_line_request_output(gpiod_reset_lineout, appname, 0) != 0) {
        perror("gpiod_line_request_output");
        gpiod_close();
        exit(-1);
    }
*/

    // RESETの値を1に設定する
    gpiod_set_out(gpiod_reset_lineout, SPI_RESET, 1);
    delay(100);
    gpiod_set_out(gpiod_reset_lineout, SPI_RESET, 0);
    delay(200);
    gpiod_set_out(gpiod_reset_lineout, SPI_RESET, 1);
    delay(200);


    spi_open();

    int ret;
    uint32_t mode;
    uint8_t  bits;
    uint32_t speed;

    mode = SPI_MODE;
    bits = SPI_BITS01;
    speed = SPI_SPEED_HZ01;

    ret = ioctl(spidev_fd01, SPI_IOC_RD_MODE32, &mode);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_WR_MODE32, &mode);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if(!ret)
        ret = ioctl(spidev_fd01, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

}


void gpiod_close() {
    if (gpiod_chip01 != 0) {
        gpiod_chip_close(gpiod_chip01);
    }
}

void gpiod_set_out(struct gpiod_line_request *pin, int pin_num, int num) {
    if ((gpiod_value01=gpiod_line_request_set_value(pin, pin_num, num)) == -1) {
        perror("gpiod_line_set_value");
    }
}


void spi_open() {

    spidev_fd01 = open("/dev/spidev2.0", O_RDWR);

}

void spi_close() {
    if (spidev_fd01 != 0) {
        close(spidev_fd01);
        spidev_fd01 = 0;
    }
}


struct spi_ioc_transfer transfer01;

void spi_write(uint8_t spi_send_byte) {

    uint8_t spi_read_byte = 0;
    int ret;

    transfer01.tx_buf        = (unsigned long)(&spi_send_byte);
    transfer01.rx_buf        = (unsigned long)(&spi_read_byte);
    transfer01.len           = 1;
    transfer01.delay_usecs   = SPI_DELAY_USECS01;
    transfer01.speed_hz      = SPI_SPEED_HZ01;
    transfer01.bits_per_word = SPI_BITS01;
    transfer01.cs_change     = 0;

    ret = ioctl(spidev_fd01, SPI_IOC_MESSAGE(1), &transfer01);
    if (ret < 1) perror("ioctl SPI_IOC_MESSAGE");
}

uint8_t spi_read(uint8_t spi_send_byte) {

    uint8_t spi_read_byte = 0;
    int     ret;

    transfer01.tx_buf        = (unsigned long)(&spi_send_byte);
    transfer01.rx_buf        = (unsigned long)(&spi_read_byte);
    transfer01.len           = 1;
    transfer01.delay_usecs   = SPI_DELAY_USECS01;
    transfer01.speed_hz      = SPI_SPEED_HZ01;
    transfer01.bits_per_word = SPI_BITS01;
    transfer01.cs_change     = 0;

    ret = ioctl(spidev_fd01, SPI_IOC_MESSAGE(1), &transfer01);
    if (ret < 1) perror("ioctl SPI_IOC_MESSAGE");

    return (spi_read_byte &  0xff);
}

void ili9341_init() {
    _width = ILI9341_WIDTH;
    _height = ILI9341_HEIGHT;
    _init_width = ILI9341_WIDTH;
    _init_height = ILI9341_HEIGHT;

    delay(150);

    spi_begin();
    writecommand(0xEF);
    writedata(0x03);
    writedata(0x80);
    writedata(0x02);
    spi_write_buffer_flush();

    writecommand(0xCF);
    writedata(0x00);
    writedata(0XC1);
    writedata(0X30);
    spi_write_buffer_flush();

    writecommand(0xED);
    writedata(0x64);
    writedata(0x03);
    writedata(0X12);
    writedata(0X81);
    spi_write_buffer_flush();

    writecommand(0xE8);
    writedata(0x85);
    writedata(0x00);
    writedata(0x78);
    spi_write_buffer_flush();

    writecommand(0xCB);
    writedata(0x39);
    writedata(0x2C);
    writedata(0x00);
    writedata(0x34);
    writedata(0x02);
    spi_write_buffer_flush();

    writecommand(0xF7);
    writedata(0x20);
    spi_write_buffer_flush();

    writecommand(0xEA);
    writedata(0x00);
    writedata(0x00);
    spi_write_buffer_flush();

    writecommand(ILI9341_PWCTR1);    //Power control
    writedata(0x23);   //VRH[5:0]
    spi_write_buffer_flush();

    writecommand(ILI9341_PWCTR2);    //Power control
    writedata(0x10);   //SAP[2:0];BT[3:0]
    spi_write_buffer_flush();

    writecommand(ILI9341_VMCTR1);    //VCM control
    writedata(0x3e); //¶O±E¶Eμ÷?U
    writedata(0x28);
    spi_write_buffer_flush();

    writecommand(ILI9341_VMCTR2);    //VCM control2
    writedata(0x86);  //--
    spi_write_buffer_flush();

    writecommand(ILI9341_MADCTL);    // Memory Access Control
    writedata(0x48);
    spi_write_buffer_flush();

    writecommand(ILI9341_PIXFMT);
    writedata(0x55);
    spi_write_buffer_flush();

    writecommand(ILI9341_FRMCTR1);
    writedata(0x00);
    writedata(0x18);
    spi_write_buffer_flush();

    writecommand(ILI9341_DFUNCTR);    // Display Function Control
    writedata(0x08);
    writedata(0x82);
    writedata(0x27);
    spi_write_buffer_flush();

    writecommand(0xF2);    // 3Gamma Function Disable
    writedata(0x00);
    spi_write_buffer_flush();

    writecommand(ILI9341_GAMMASET);    //Gamma curve selected
    writedata(0x01);
    spi_write_buffer_flush();

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
    spi_write_buffer_flush();

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
    spi_write_buffer_flush();

    writecommand(ILI9341_SLPOUT);    //Exit Sleep
    spi_end();
    delay(120);
    spi_begin();
    writecommand(ILI9341_DISPON);    //Display on
    delay(150);
    spi_end();

    set_rotation_ILI9341(0);
}

void set_rotation_ILI9341(int mode) {
    int rotation;

    spi_begin();
    rotation = mode % 8;

    writecommand(ILI9341_MADCTL);
    switch (rotation) {
      case 0:
        writedata(MADCTL_MX | MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 1:
        writedata(MADCTL_MV | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;
      case 2:
        writedata(MADCTL_MY | MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 3:
        writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;

      case 4:
        writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 5:
        writedata(MADCTL_MV | MADCTL_MX | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;
      case 6:
        writedata(MADCTL_BGR);
        _width  = _init_width;
        _height = _init_height;
        break;
      case 7:
        writedata(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
        _width  = _init_height;
        _height = _init_width;
        break;

    }
    spi_write_buffer_flush();
    spi_end();
}

void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    gpio_dc_on();
    spi_write(ILI9341_CASET);
    gpio_dc_off();
    spi_write_buffer((uint8_t)(x0 >> 8));
    spi_write_buffer((uint8_t)x0);
    spi_write_buffer((uint8_t)(x1 >> 8));
    spi_write_buffer((uint8_t)x1);
    spi_write_buffer_flush();

    gpio_dc_on();
    spi_write(ILI9341_PASET);
    gpio_dc_off();
    spi_write_buffer((uint8_t)(y0 >> 8));
    spi_write_buffer((uint8_t)y0);
    spi_write_buffer((uint8_t)(y1 >> 8));
    spi_write_buffer((uint8_t)y1);
    spi_write_buffer_flush();

    gpio_dc_on();
    spi_write(ILI9341_RAMWR);
    gpio_dc_off();
}

void drawPixel(int32_t x, int32_t y, uint32_t color)
{
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

    spi_begin();
    setAddrWindow(x, y, x + 1, y + 1);

    spi_write_buffer((uint8_t)(color >> 8));
    spi_write_buffer((uint8_t)color);
    spi_write_buffer_flush();
    spi_end();
}

void fill_color16(uint16_t color, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++) {
        spi_write_buffer((uint8_t)(color >> 8));
        spi_write_buffer((uint8_t)color);
    }
}

void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= _width) || (y >= _height)) return;
        if((x + w - 1) >= _width)  w = _width  - x;
        if((y + h - 1) >= _height) h = _height - y;

        spi_begin();
        setAddrWindow(x, y, x + w - 1, y + h - 1);
        fill_color16((uint16_t)color, w * h);
        spi_write_buffer_flush();
        spi_end();
    }
}

void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
  int32_t f = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x = 0;
  int32_t y = r;

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

int32_t test_spi_abs(int32_t num) {
    if (num < 0) num = -num;
    return num;
}

void test_spi_swap_int32_t(int32_t a, int32_t b) {
    int32_t t;
    t = a;
    a = b;
    b = t;
}

void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
  int32_t steep = test_spi_abs(y1 - y0) > test_spi_abs(x1 - x0);
  if (steep) {
    test_spi_swap_int32_t(x0, y0);
    test_spi_swap_int32_t(x1, y1);
  }

  if (x0 > x1) {
    test_spi_swap_int32_t(x0, x1);
    test_spi_swap_int32_t(y0, y1);
  }

  int32_t dx, dy;
  dx = x1 - x0;
  dy = test_spi_abs(y1 - y0);

  int32_t err = dx / 2;
  int32_t ystep;

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

void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;

    if((y+h-1) >= _height) h = _height-y;

    spi_begin();
    setAddrWindow(x, y, x, y+h-1);
    fill_color16((uint16_t)color,h);
    spi_write_buffer_flush();
    spi_end();
}

void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;

    spi_begin();
    setAddrWindow(x, y, x+w-1, y);
    fill_color16((uint16_t)color, w);
    spi_write_buffer_flush();
    spi_end();
}

void fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
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

void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
void drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint16_t *p = (uint16_t *)bitmap;
    uint16_t pixel01;

    spi_begin();
    setAddrWindow(x, y, x + w - 1, y + h - 1);
    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
            pixel01 = *p++;
            spi_write_buffer((uint8_t)(pixel01 >> 8));
            spi_write_buffer((uint8_t)(pixel01 & 0xff));
        }
    }
    spi_write_buffer_flush();
    spi_end();
}

/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8;
  uint8_t byte = 0;
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        drawPixel(x + i, y, bitmap[j * w + i]);
      }
    }
  }
}

uint16_t color565(uint32_t colr, uint32_t colg, uint32_t colb) {
    uint16_t color;
    color = (uint16_t)(((colr & 0xff) >> 3) << 11) | (((colg & 0xff) >> 2) << 5) | ((colb & 0xff) >> 3);
    return color;
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

void micro_delay(long micro_sec) {
    int nanosleep_ret;
    delay_nano_sleep_param01_req.tv_sec = micro_sec / 1000000;
    delay_nano_sleep_param01_req.tv_nsec = (micro_sec % 1000000) * 1000;
    nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    while (nanosleep_ret == -1) {
        delay_nano_sleep_param01_req.tv_sec = delay_nano_sleep_param01_rem.tv_sec;
        delay_nano_sleep_param01_req.tv_nsec = delay_nano_sleep_param01_rem.tv_nsec;
        nanosleep_ret = nanosleep(&delay_nano_sleep_param01_req, &delay_nano_sleep_param01_rem);
    }
}



#define SPI_WRITE_BUFFER_SIZE 512
#define SPI_WRITE_BUFFER_NUM  100

uint8_t spi_write_buffer_data01[SPI_WRITE_BUFFER_SIZE];
uint8_t spi_read_buffer_data01[SPI_WRITE_BUFFER_SIZE];

int32_t buffer_pos01;

void spi_write_buffer_spi_write(uint8_t *write_buf, uint8_t *read_buf, int32_t size) {

    uint8_t spi_read_byte = 0;
    int ret;

    transfer01.tx_buf        = (unsigned long)(write_buf);
    transfer01.rx_buf        = (unsigned long)(read_buf);
    transfer01.len           = size;
    transfer01.delay_usecs   = SPI_DELAY_USECS01;
    transfer01.speed_hz      = SPI_SPEED_HZ01;
    transfer01.bits_per_word = SPI_BITS01;
    transfer01.cs_change     = 0;

    ret = ioctl(spidev_fd01, SPI_IOC_MESSAGE(1), &transfer01);
    if (ret < 1) perror("ioctl SPI_IOC_MESSAGE");
}
void spi_write_buffer(uint8_t byte) {
    spi_write_buffer_data01[buffer_pos01++] = byte;
    if (buffer_pos01 > SPI_WRITE_BUFFER_NUM) {
        spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
        buffer_pos01 = 0;
    }
}

void spi_write_buffer_flush() {
    spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
    buffer_pos01 = 0;
}

