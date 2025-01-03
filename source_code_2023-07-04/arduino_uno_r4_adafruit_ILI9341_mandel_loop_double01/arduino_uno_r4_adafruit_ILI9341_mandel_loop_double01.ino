/*
   Reqire Adafruit_GFX, Adafuit_ILI9341, Adafruit_BusIO

  今のところArduino UNO R4でAdafuite_ILI9341を使うには
  C:\Users\USERNAME\AppData\Local\Arduino15\packages\arduino\hardware\renesas_uno\1.0.1\libraries
    注：USERNAMEの部分はご自分のWindowsのユーザ名です
  ここにwiring_privateというフォルダを作り、その下にwiring_private.hという名前で空のファイルを作るとコンパイルできるようです
*/

#include "Arduino.h"

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define SPI0 SPI

#define TFT_RST   8
#define TFT_DC    9
#define TFT_CS   10
#define TFT_MOSI 11
#define TFT_MISO 12
#define TFT_CLK  13

Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI0, TFT_DC, TFT_CS, TFT_RST);
const int tft_frequency = 20000000;

void printf1(const char* format, ...);
void mandelbrot(void);
void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth);
int mand(double a1, double b1, int depth1);

struct m_param {
    double    xcorner;
    double    ycorner;
    double    length;
    int       depth;
} m_param01[15];

void setup() {
    Serial.begin(115200);

    tft.begin(tft_frequency);      
    tft.setRotation(3);
    tft.fillRect(0, 0, 320, 240, ILI9341_BLACK);

    set_m_param01();
}

void loop(){
    mandelbrot();
    delay(60000);
}

#define va_start(v,l)  __builtin_va_start(v,l)
#define va_end(v) __builtin_va_end(v)
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

uint16_t color01(int colr, int colg, int colb) {
    uint16_t color;
    color = (uint16_t)((colr * 32 / 256) << 11) | ((colg * 64 / 256) << 5) | (colb * 32 / 256);

    return color;
}


void mandelbrot(void) {

    double    xcorner, ycorner, length;
    int       xdot, ydot, depth;

    xdot = 320;
    ydot = 240;

    for (int number01 = 0; number01 < 15; number01++) {
      xcorner =  m_param01[number01].xcorner;
      ycorner =  m_param01[number01].ycorner;
      length  =  m_param01[number01].length;
      depth   =  m_param01[number01].depth;
      mandel(xcorner, ycorner, length, xdot, ydot, depth);
//      delay(10000);
    }
}

void mandel(double xcorner, double ycorner, double length, int xdot, int ydot, int depth) {
    double xgap, ygap;
    double xx, yy;
    int    i, j;
    int    col01, colr, colg, colb;
    double xcorner1;

    // xgap = length / xdot;
    // ygap = length / xdot;
    xgap = length / ydot;
    ygap = length / ydot;
    //  x軸を240ドットから320ドットに広げる
    xcorner1 = xcorner;
    xcorner = xcorner - (xdot - ydot) / 2 * xgap;
    printf1("xdot = %d    ydot = %d\r\n", xdot, ydot);
    printf1("xcorner = "); print_double(xcorner, 15);
    printf1("  :  "); print_double(xcorner1, 15); printf1(" - (%d - %d) / 2 * ", xdot, ydot); print_double_ln(xgap, 15);
    printf1("ycorner = "); print_double_ln(ycorner, 15);
    printf1("length  = "); print_double_ln(length, 15);
    printf1("depth   = %d\r\n", depth);
    printf1("\r\n");

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
            tft.fillRect(i, j, 2, 1, color01(colr, colg, colb));

          xx = xx + xgap;
        }
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

void set_m_param01(){

        m_param01[0].xcorner =  -2.4;
        m_param01[0].ycorner =  -1.7;
        m_param01[0].length  =   3.2;
        m_param01[0].depth   =  1000;

        m_param01[1].xcorner = -6.1099999999999965e-001;
        m_param01[1].ycorner =  6e-001;
        m_param01[1].length  =  1.0000000000000001e-001;
        m_param01[1].depth   =  1000;

        m_param01[2].xcorner = -7.512120844523107e-001;
        m_param01[2].ycorner = 2.939359283447132e-002;
        m_param01[2].length = 2.441406250000000e-005;
        m_param01[2].depth = 1000;

        m_param01[3].xcorner = -7.424999999999979e-002;
        m_param01[3].ycorner = -6.523749999999998e-001;
        m_param01[3].length = 3.125000000000000e-003;
        m_param01[3].depth   =  1000;

        m_param01[4].xcorner = -7.366145833333310e-002;
        m_param01[4].ycorner = -6.500052083333332e-001;
        m_param01[4].length = 3.125000000000000e-003;
        m_param01[4].depth   =  1000;

        m_param01[5].xcorner = -7.451562499999977e-002;
        m_param01[5].ycorner = -6.500117187500000e-001;
        m_param01[5].length = 7.812500000000000e-004;
        m_param01[5].depth   =  1000;

        m_param01[6].xcorner = -7.409765624999977e-002;
        m_param01[6].ycorner = -6.494752604166667e-001;
        m_param01[6].length = 1.953125000000000e-004;
        m_param01[6].depth   =  1000;

        m_param01[7].xcorner = -1.023473307291662e-001;
        m_param01[7].ycorner = 9.571370442708340e-001;
        m_param01[7].length = 4.882812500000000e-005;
        m_param01[7].depth   =  1000;

        m_param01[8].xcorner = -7.424999999999979e-002;
        m_param01[8].ycorner = -6.523749999999998e-001;
        m_param01[8].length = 3.125000000000000e-003;
        m_param01[8].depth   =  1000;

        m_param01[9].xcorner = -7.366145833333310e-002;
        m_param01[9].ycorner = -6.500052083333332e-001;
        m_param01[9].length = 3.125000000000000e-003;
        m_param01[9].depth   =  1000;

        m_param01[10].xcorner = -7.451562499999977e-002;
        m_param01[10].ycorner = -6.500117187500000e-001;
        m_param01[10].length = 7.812500000000000e-004;
        m_param01[10].depth   =  1000;

        m_param01[11].xcorner = -7.409765624999977e-002;
        m_param01[11].ycorner = -6.494752604166667e-001;
        m_param01[11].length = 1.953125000000000e-004;
        m_param01[11].depth   =  1000;

        m_param01[12].xcorner = -1.023473307291662e-001;
        m_param01[12].ycorner = 9.571370442708340e-001;
        m_param01[12].length = 4.882812500000000e-005;
        m_param01[12].depth   =  1000;

        m_param01[13].xcorner = -1.165292968750000e+000;
        m_param01[13].ycorner = 2.393867187500003e-001;
        m_param01[13].length = 3.906250000000000e-004;
        m_param01[13].depth   =  1000;

        m_param01[14].xcorner = -6.703997395833329e-001;
        m_param01[14].ycorner = -4.582591145833326e-001;
        m_param01[14].length = 3.906250000000000e-004;
        m_param01[14].depth   =  1000;
}

void print_double(double a, int digits001) {
    double exp01, exp02, a01, b01, c01, c_exp01, d01;
    int sign01 = 0;
//    int digits01 = 15 - 1;
    int digits01;
    if (digits001 < 1) {
        digits01 = 1 - 1;
    } else {
        digits01 = digits001 - 1;
    }

    a01 = a;
    if (a01 < 0) {
        sign01 = 1;
        a01 = -a01;
    }

    if (a01 > 5.0e-324) {
        b01 = log(a01)/log(10.0);
        c_exp01 = floor(b01);
        c01 = pow(10.0, (b01 - c_exp01));
    } else {
        c01 = 0.0;
        c_exp01 = 0.0;
    }

    c01 = c01 + 0.5 * pow(10.0, (double)(-digits01));

    if (sign01) printf1("-");
    d01 = floor(c01);
    c01 = c01 - d01;
    printf1("%1d.", (int)d01);
    for (int i = 0; i < digits01; i++) {
        c01 = c01 * 10.0;
        d01 = floor(c01);
        c01 = c01 - d01;
        printf1("%1d", (int)d01);
    }
    if (c_exp01 >= 0) {
        printf1("e+");
    } else {
        printf1("e-");
        c_exp01 = -c_exp01;
    }

    printf1("%03d", (int)c_exp01);
}

void print_double_ln(double a, int digits001) {
    print_double(a, digits001);
    printf1("\r\n");
}
