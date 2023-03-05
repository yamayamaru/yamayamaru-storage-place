/*
 * このプログラムはVision Five2 DebianのSPIテスト用に作ったものです。
 * LCDにILI9341のLCDを使います。
 * また、libgpiodとspidevを使います。
 *
 * libgpiodのインストールはaptで入ります
 * $ sudo apt install libgpiod2 libgpiod-dev gpiod
 *
 *
 * コンパイル方法
 * $ gcc -O2 -c -o yama_ili9341_library_double_buffer.o yama_ili9341_library_double_buffer.c
 * $ gcc -O2 -o spitest_ili9341_mandelbrot_set03 spitest_ili9341_mandelbrot_set03.c yama_ili9341_library_double_buffer.o -lgpiod
 *
 * 実行方法
 *
 * $ sudo chmod o+rw /dev/gpiochip0
 * $ sudo chmod o+rw /dev/spidev1.0
 * $ ./spitest_ili9341_mandelbrot_set03
 *
 *
 * VisionFive2とLCDの接続はこんな感じです
 * (libgpiodではピンヘッダのピン番号ではなくpin xxxと書かれてるxxxの部分の番号を指定します
 * 
 * CS    ピンヘッダのピン番号24(pin 49 PAD_GPIO49)    注意：今回はこのピンは使いません
 * MOSI  ピンヘッダのピン番号19(pin 52 PAD_GPIO52)
 * MISO  ピンヘッダのピン番号21(pin 53 PAD_GPIO53)
 * SCLK  ピンヘッダのピン番号23(pin 48 PAD_GPIO48)
 * 
 * RESET ピンヘッダのピン番号31(pin 39 PAD_GPIO39)
 * CS    ピンヘッダのピン番号27(pin 45 PAD_GPIO45)
 * DC    ピンヘッダのピン番号29(pin 37 PAD_GPIO37)
 *
 * (下記のように実行するとピンの利用状況が表示されるようです。)
 * $ sudo cat /sys/kernel/debug/pinctrl/13040000.gpio/pinmux-pins
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "yama_ili9341_library_double_buffer.h"

// visionfive2のピン設定
#define     SPI_CS        45    //ピンヘッダのピン番号27(pin 45 GPIO45) : UNCLAIMED
#define     SPI_DC        37    //ピンヘッダのピン番号29(pin 37 GPIO37) : UNCLAIMED
#define     SPI_RESET     39    //ピンヘッダのピン番号31(pin 39 GPIO39) : UNCLAIMED
#define     SPIDEV_DEVICE "/dev/spidev1.0"
#define     GPIOD_DEVICE  "/dev/gpiochip0"


// Raspberry Pi 4のピン設定
// MOSI  ピンヘッダーの19番ピン(GPIO10)
// MISO  ピンヘッダーの21番ピン(GPIO9)
// SCLK  ピンヘッダーの23番ピン(GPIO11)
//#define     SPI_DC        25    //ピンヘッダのピン番号22(pin 25 GPIO25): UNCLAIMED
//#define     SPI_CS         0    //ピンヘッダのピン番号27(pin  0 GPIO0) : UNCLAIMED
//#define     SPI_RESET     24    //ピンヘッダのピン番号18(pin 24 GPIO24): UNCLAIMED
//#define     SPIDEV_DEVICE "/dev/spidev0.0"
//#define     GPIOD_DEVICE  "/dev/gpiochip0"


void delay(long milli_sec);
double second();
void sigint_handler(int sig);

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

    spi_init(SPIDEV_DEVICE, GPIOD_DEVICE, SPI_CS, SPI_DC, SPI_RESET);

    ili9341_init();

    set_rotation_ILI9341(3);

    fillRect(0, 0, _width, _height, ILI9341_BLACK);

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

void sigint_handler(int sig) {
    spi_close();
    gpiod_close();
    exit(1);
}
