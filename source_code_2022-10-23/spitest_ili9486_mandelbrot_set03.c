/*
 * このプログラムはUbuntu Server 22.04.1のmango pi mq proのSPIテスト用に作ったものです。
 * LCDにILI9486のLCDを使います。
 * また、libgpiodとspidevを使います。
 *
 * libgpiodのインストールはaptで入ります
 * $ sudo apt install libgpiod2 libgpiod-dev gpiod
 *
 * ただし、Linuxの中ではパッケージで入るlibgpiod2に対応してないものもあります。
 * (LinuxのカーネルコンフィグでCONFIG_GPIO_CDEV_V1がyになっておらず、
 * コンパイルしたプログラムやgpiodのコマンドを実行するとInvalid argumentとエラーが
 * 出ることがあります。
 * その場合は最新のlibgpiodをソースコードから入れる必要があります。
 * やり方はこちらを参照してください。)
 *
 * Ubuntu Server 22.04.1のmangp pi mq proでlibgpiodでGPIOを使う
 * https://pastebin.com/yJxcTwp9
 * (現在のUbuntu Server 22.04.1は最新版にすればCONFIG_GPIO_CDEV_V1がyになっているので必要ないです。)
 *
 *
 * 下記をを実行してみてspidevのデバイスファイルができてない場合はspidevを有効にする必要があります。
 * $ ls /dev/spidev*
 * 
 * Ubuntu Server 22.04.1でのspidevを有効にする方法は下記を参照してください。
 * (ただし、Linuxカーネルが5.19以降の場合はこの方法ではできません。
 *  現在のUbuntu Server 22.04.1のLinuxカーネルは5.17.0です。
 * また、Linuxによっては専用のspidevを有効にする方法があるようです。
 * Raspberry Pi OSならraspi-config、Armbianならarmbian-configなど)
 *
 * RISC-VのUbuntu Server 22.04.1のmango pi mq proでspidevを有効にする
 * https://pastebin.com/MfFf6MHA
 *
 *
 * コンパイル方法
 *
 * $ gcc -O2 -o spitest_ili9486_mandelbrot_set03 spitest_ili9486_mandelbrot_set03.c -lgpiod
 *
 * libgpiodをソースコードから入れた人は下のようになります。
 * $ gcc -O2 -I/usr/local/include -L/usr/local/lib -o spitest_ili9486_mandelbrot_set03 spitest_ili9486_mandelbrot_set03.c -lgpiod
 *
 *  (undefined reference to `gpiod_chip_close(gpiod_chip*)'というエラーが出る場合は
 *   679行目あたりのgpiod_chip_close()をgpiod_chip_unref()に変更してください。)
 *
 * 実行方法
 *
 * $ sudo chmod o+rw /dev/gpiochip0
 * $ sudo chmod o+rw /dev/spidev1.0
 * $ ./spitest_ili9486_mandelbrot_set03
 *
 * libgpiodをソースコードから入れた人は下のようになります。
 * $ export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/lib
 * $ export PATH=/usr/local/bin:$PATH
 * $ sudo chmod o+rw /dev/gpiochip0
 * $ sudo chmod o+rw /dev/spidev1.0
 * $ ./spitest_ili9486_mandelbrot_set03
 * (LD_LIBRARY_PATHとPATHのexportは~/.profileに追記した方が便利です。)
 *
 * mango pi mq priとLCDの接続はこんな感じです
 * (libgpiodではピンヘッダのピン番号ではなくpin xxxと書かれてるxxxの部分の番号を指定します)
 *
 * CS    ピンヘッダのピン番号24(pin 106 PD10) : device 4026000.spi function spi1 group PD10
 * MOSI  ピンヘッダのピン番号19(pin 108 PD12) : device 4026000.spi function spi1 group PD12
 * MISO  ピンヘッダのピン番号21(pin 109 PD13) : device 4026000.spi function spi1 group PD13
 * SCLK  ピンヘッダのピン番号23(pin 107 PD11) : device 4026000.spi function spi1 group PD11
 *
 * RESET ピンヘッダのピン番号22(pin  65 PC1) : UNCLAIMED
 * CS    ピンヘッダのピン番号27(pin 145 PE17): UNCLAIMED
 * DC    ピンヘッダのピン番号28(pin 144 PE16): UNCLAIMED
 *
 * ILI9346にRDピンがある場合はVCCに接続してください。
 * 
 * (下記のように実行するとピンの利用状況が表示されるようです。)
 * $ sudo cat /sys/kernel/debug/pinctrl/2000000.pinctrl/pinmux-pins
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>

// mangopi mq proのピン設定
#define     SPI_DC       144    //ピンヘッダのピン番号28(pin 144 PE16): UNCLAIMED
#define     SPI_CS       145    //ピンヘッダのピン番号27(pin 145 PE17): UNCLAIMED
#define     SPI_RESET     65    //ピンヘッダのピン番号22(pin  65 PC1) : UNCLAIMED
#define     SPIDEV_DEVICE "/dev/spidev1.0"
#define     GPIOD_DEVICE  "/dev/gpiochip0"


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

struct gpiod_chip *gpiod_chip01;
struct gpiod_line *gpiod_dc_lineout;
struct gpiod_line *gpiod_reset_lineout;
struct gpiod_line *gpiod_cs_lineout;
int gpiod_value01;
void gpiod_close();
void gpiod_set_out(struct gpiod_line *pin, int num);

int  spi_open();
void spi_close();
void spi_write(uint8_t spi_send_byte);
void spi_write_buffer(uint8_t byte);
void spi_write_buffer_flush();

void sigint_handler(int sig);
void delay(long milli_sec);
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
void init();
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

    ili9486_init();

    set_rotation_ILI9486(3);

    fillRect(0, 0, _width, _height, ILI9486_BLACK);

    set_param();

    mandelbrot(_width, _height);

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
    int       i, ii;

    xdot = width;
    ydot = height;

    for (ii = 0; ii < 2800; ii++) {
        double start_time, end_time;

        i = ii % 28;
        start_time = second();
        mandel(param01[i].xcorner, param01[i].ycorner, param01[i].length, xdot, ydot, param01[i].depth);
        end_time = second();
        printf("time = %.4f\n", end_time - start_time);
    }

}


uint8_t pixel_buffer01[ILI9486_HEIGHT * 3];

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
            color = color24(colr, colg, colb);
//            drawPixel(i, j, color);
            pixel_buffer01[i * 3] = colr;
            pixel_buffer01[i * 3 + 1] = colg;
            pixel_buffer01[i * 3 + 2] = colb;

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





void init(){

    const char *appname = "spitest01";


    // GPIOデバイスを開く
    if ((gpiod_chip01=gpiod_chip_open(GPIOD_DEVICE)) == NULL) {
        perror("gpiod_chip_open");
        exit(-1);
    }

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

    // RESETの値を1に設定する
    gpiod_set_out(gpiod_reset_lineout, 1);
    delay(5);
    gpiod_set_out(gpiod_reset_lineout, 0);
    delay(20);
    gpiod_set_out(gpiod_reset_lineout, 1);
    delay(150);


    // SPIのオープン
    int spi_fd01;
    if((spi_fd01 = spi_open()) < 0) {
        perror("spi_open");
        gpiod_close();
        exit(-1);
    }

    int ret;
    uint32_t mode;
    uint8_t  bits;
    uint32_t speed;

    mode = SPI_MODE;
    bits = SPI_BITS01;
    speed = SPI_SPEED_HZ01;

    ret = ioctl(spi_fd01, SPI_IOC_RD_MODE32, &mode);
    if(!ret)
        ret = ioctl(spi_fd01, SPI_IOC_WR_MODE32, &mode);
    if(!ret)
        ret = ioctl(spi_fd01, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if(!ret)
        ret = ioctl(spi_fd01, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if(!ret)
        ret = ioctl(spi_fd01, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if(!ret)
        ret = ioctl(spi_fd01, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
}


void gpiod_close() {
    if (gpiod_chip01 != 0) {
        void gpiod_chip_close(struct gpiod_chip *chip);
        gpiod_chip_close(gpiod_chip01);
        //void gpiod_chip_unref(struct gpiod_chip *chip);
        //gpiod_chip_unref(gpiod_chip01);
        return ;
    }
}

void gpiod_set_out(struct gpiod_line *pin, int num) {
    if ((gpiod_value01=gpiod_line_set_value(pin, num)) == -1) {
        perror("gpiod_line_set_value");
    }
}



 
int  spidev_fd01 = 0;

int spi_open() {

    if ((spidev_fd01 = open(SPIDEV_DEVICE, O_RDWR)) < 0) {
        perror("spi_open open");
        return spidev_fd01;
    }
    return spidev_fd01;
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






int32_t _init_width, _init_height;
int32_t _width, _height;


void ili9486_init() {
    _width = ILI9486_WIDTH;
    _height = ILI9486_HEIGHT;
    _init_width = ILI9486_WIDTH;
    _init_height = ILI9486_HEIGHT;

    spi_begin();

    writecommand(0x11);
    delay(120);

    writecommand(0x3A);
    writedata(0x66);                      // 18 bit colour interface
    spi_write_buffer_flush();

    writecommand(0xC2);
    writedata(0x44);
    spi_write_buffer_flush();

    writecommand(0xC5);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00); 
    spi_write_buffer_flush();

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
    spi_write_buffer_flush();
 
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
    spi_write_buffer_flush();
 
    writecommand(ILI9486_INVOFF);
//    writecommand(ILI9486_INVON);
 
    writecommand(0x36);
    writedata(0x48);
    spi_write_buffer_flush();

    writecommand(0x29);                     // display on
    delay(150);
    spi_end();

    set_rotation_ILI9486(0);
}

void set_rotation_ILI9486(int mode) {
    int rotation;

    spi_begin();
    rotation = mode % 8;

    writecommand(ILI9486_MADCTL);
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
    spi_write(ILI9486_CASET);
    gpio_dc_off();    
    spi_write_buffer((uint8_t)(x0 >> 8));
    spi_write_buffer((uint8_t)x0);
    spi_write_buffer((uint8_t)(x1 >> 8));
    spi_write_buffer((uint8_t)x1);
    spi_write_buffer_flush();

    gpio_dc_on();
    spi_write(ILI9486_PASET);
    gpio_dc_off();
    spi_write_buffer((uint8_t)(y0 >> 8));
    spi_write_buffer((uint8_t)y0);    
    spi_write_buffer((uint8_t)(y1 >> 8));
    spi_write_buffer((uint8_t)y1);
    spi_write_buffer_flush();

    gpio_dc_on();
    spi_write(ILI9486_RAMWR);
    gpio_dc_off();
}

void drawPixel(int32_t x, int32_t y, uint32_t color)
{
    if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

    spi_begin();
    setAddrWindow(x, y, x + 1, y + 1);

    spi_write_buffer((uint8_t)(color >> 16));
    spi_write_buffer((uint8_t)(color >> 8));
    spi_write_buffer((uint8_t)color);
    spi_write_buffer_flush();
    spi_end();
}

void fill_color24(uint32_t color, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++) {
        spi_write_buffer((uint8_t)(color >> 16));
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
        fill_color24(color, w * h);
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
    fill_color24(color,h);
    spi_write_buffer_flush();
    spi_end();
}

void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;

    spi_begin();
    setAddrWindow(x, y, x+w-1, y);
    fill_color24(color, w);
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

/* bitmapデータは各色8bitの24bitRGB形式です */
void drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint8_t *p = bitmap;
    uint8_t r, g, b;

    if(w > 0 && h > 0){
        if((x >= _width) || (y >= _height)) return;
        if((x + w - 1) >= _width)  w = _width  - x;
        if((y + h - 1) >= _height) h = _height - y;

        spi_begin();
        setAddrWindow(x, y, x + w - 1, y + h - 1);
        for(j=0; j < h; j++){
            for(i = 0; i < w; i++) {
                r = *p++;
                g = *p++;
                b = *p++;
                spi_write_buffer(r);
                spi_write_buffer(g);
                spi_write_buffer(b);
            }
        }
        spi_write_buffer_flush();
        spi_end();
    }
}

/* bitmapデータは各色8bitの24bitRGB形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8;
  uint8_t byte = 0;
  int32_t index = 0;
  uint8_t r, g, b;
  for (int32_t j = 0; j < h; j++, y++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        index = (j * w + i)  * 3;
        r = bitmap[index];
        g = bitmap[index + 1];
        b = bitmap[index + 2];
        drawPixel(x + i, y, ((uint32_t)r << 16) | ((uint32_t)g) << 8 | ((uint32_t)b));
      }
    }
  }
}

uint32_t color24(uint32_t colr, uint32_t colg, uint32_t colb) {
    uint32_t color;
    color = ((colr & 0xff) << 16) | ((colg & 0xff) << 8) | (colb & 0xff);
    return color;
}






int32_t canvas24_rotation = 0;
int32_t canvas24_width = 320;
int32_t canvas24_height = 480;
int32_t canvas24_init_width = 320;
int32_t canvas24_init_height = 480;
uint8_t *canvas24_buffer = NULL;

int canvas24_begin(int32_t w, int32_t h) {
  int ret = 0;
  uint32_t bytes = w * h * 3;
  canvas24_init_width = w;
  canvas24_init_height = h;
  canvas24_width = w;
  canvas24_height = h;
  if ((canvas24_buffer = (uint8_t *)malloc(bytes)) != NULL) {
      memset(canvas24_buffer, 0, bytes);
      ret = 0;
  } else {
      ret = -1;
  }

  return ret;
}

void canvas24_end() {
  if (canvas24_buffer != NULL)
    free(canvas24_buffer);
}

void canvas24_set_rotation(int32_t x) {
  canvas24_rotation = (x & 3);
  switch (canvas24_rotation) {
  case 0:
  case 2:
    canvas24_width = canvas24_init_width;
    canvas24_height = canvas24_init_height;
    break;
  case 1:
  case 3:
    canvas24_width = canvas24_init_height;
    canvas24_height = canvas24_init_width;
    break;
  }
}

void canvas24_drawPixel(int32_t x, int32_t y, uint32_t color) {

  int32_t index;

  if (canvas24_buffer) {
    if ((x < 0) || (y < 0) || (x >= canvas24_width) || (y >= canvas24_height))
      return;

    int32_t t;
    switch (canvas24_rotation) {
    case 1:
      t = x;
      x = canvas24_init_width - 1 - y;
      y = t;
      break;
    case 2:
      x = canvas24_init_width - 1 - x;
      y = canvas24_init_height - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = canvas24_init_height - 1 - t;
      break;
    }

    index = ((x + y * canvas24_init_width) * 3);
    canvas24_buffer[index] = (uint8_t)(color >> 16);
    canvas24_buffer[index + 1] = (uint8_t)(color >> 8);
    canvas24_buffer[index + 2] = (uint8_t)color;
  }
}

void canvas24_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
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
      canvas24_drawPixel(y0, x0, color);
    } else {
      canvas24_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void canvas24_drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas24_width) || (y >= canvas24_height)) return;

    if((y+h-1) >= canvas24_height) h = canvas24_height-y;

    for (int yy = y; yy < (y + h); yy++) {
        canvas24_drawPixel(x, yy, color);
    }
}

void canvas24_drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas24_width) || (y >= canvas24_height)) return;
    if((x+w-1) >= canvas24_width)  w = canvas24_width-x;

    for (int xx = x; xx < (x + w); xx++) {
        canvas24_drawPixel(xx, y, color);
    }
}

void canvas24_fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
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
        canvas24_drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        canvas24_drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        canvas24_drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        canvas24_drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void canvas24_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    canvas24_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    canvas24_fillCircleHelper(x0, y0, r, 3, 0, color);
}

void canvas24_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  canvas24_drawFastHLine(x, y, w, color);
  canvas24_drawFastHLine(x, y + h - 1, w, color);
  canvas24_drawFastVLine(x, y, h, color);
  canvas24_drawFastVLine(x + w - 1, y, h, color);
}

void canvas24_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= canvas24_width) || (y >= canvas24_height)) return;
        if((x + w - 1) >= canvas24_width)  w = canvas24_width  - x;
        if((y + h - 1) >= canvas24_height) h = canvas24_height - y;

    for (int j = y; j < (y + h); j++) {
            for (int i = x; i < (x + w); i++) {
                canvas24_drawPixel(i, j, color);
        }
    }
    }
}

/* bitmapデータは各色8bitの24bitRGB形式です */
void canvas24_drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint8_t *p = (uint8_t *)bitmap;
    uint32_t color;
    uint32_t r, g, b;

    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
            r = *p++;
            g = *p++;
            b = *p++;
            color = (r << 16) | (g << 8) | b;
            canvas24_drawPixel(x + i, y + j, color);
        }
    }
}

/* bitmapデータは各色8bitの24bitRGB形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void canvas24_drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint8_t *bitmap = (uint8_t *)_bitmap;
  int32_t bw = (w + 7) / 8;
  uint8_t byte = 0;
  uint32_t color;
  uint32_t r, g, b;
  int32_t index;

  for (int32_t j = 0; j < h; j++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
          index = (j * w + i) * 3;
          r = bitmap[index];
          g = bitmap[index + 1];
          b = bitmap[index + 2];
          color = (r << 16) | (g << 8) | b;
          canvas24_drawPixel(x + i, y + j, color);
      }
    }
  }
}

void canvas24_update(int32_t x, int32_t y) {
    drawRGBBitmap(x, y, (uint8_t *)canvas24_buffer, canvas24_init_width, canvas24_init_height);
}






/*
// https://stackoverflow.com/questions/52187221/how-to-calculate-the-no-of-clock-cycles-in-riscv-clang
extern __inline unsigned long __attribute__((__gnu_inline__, __always_inline__, __artificial__)) rdcycle(void)
{
    unsigned long dst;
    // output into any register, likely a0
    // regular instruction:
    asm volatile ("csrrs %0, 0xc00, x0" : "=r" (dst) );
    // regular instruction with symbolic csr and register names
    // asm volatile ("csrrs %0, cycle, zero" : "=r" (dst) );
    // pseudo-instruction:
    // asm volatile ("csrr %0, cycle" : "=r" (dst) );
    // pseudo-instruction:
    //asm volatile ("rdcycle %0" : "=r" (dst) );
    return dst;
}

void delay(long milli_sec) {

    unsigned long time01 = 1000000 * milli_sec;
    unsigned long cycle01, cycle02;

    cycle01 = rdcycle();
    while (1) {
        cycle02 = rdcycle();
        if (cycle02 < cycle01) {
            if ((0 - cycle01 + cycle02) > time01) break;
        } else {
            if ((cycle02 - cycle01) > time01) break;
        }
    }
}
*/

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










#define SPI_WRITE_BUFFER_SIZE 512
#define SPI_WRITE_BUFFER_NUM  500

uint8_t spi_write_buffer_data01[SPI_WRITE_BUFFER_SIZE];
uint8_t spi_read_buffer_data01[SPI_WRITE_BUFFER_SIZE];

int32_t buffer_pos01 = 0;

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
    if (buffer_pos01 == SPI_WRITE_BUFFER_NUM) {
        spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
        buffer_pos01 = 0;
    }
}

void spi_write_buffer_flush() {
    spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
    buffer_pos01 = 0;
}

