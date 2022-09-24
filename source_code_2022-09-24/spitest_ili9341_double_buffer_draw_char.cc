/*
 * このプログラムはUbuntu Server 22.04.1のmango pi mq proのSPIテスト用に作ったものです。
 * LCDにILI9341のLCDを使います。
 * また、libgpiodとspidev、libfreetypeを使います。
 * 
 * libgpiodのインストール方法やspidevを有効にする方法は下記を参照してください
 * (RISC-VのUbuntu Server 22.04.1のAllwinner Nezhaイメージでは
 * aptで入れられるlibgpiodでは正常に動作しないようです。
 * また、デフォルトの状態ではspidevが有効になってません。)
 * 
 * Ubuntu Server 22.04.1のmangp pi mq proでlibgpiodでGPIOを使う
 * https://pastebin.com/yJxcTwp9
 * 
 * RISC-VのUbuntu Server 22.04.1のmango pi mq proでspidevを有効にする
 * https://pastebin.com/MfFf6MHA
 * 
 * 
 * このプログラムではfonts-takaoを使っているのでaptでインストールするか、
 * 334行目のset_fontの部分で使いたいフォントを指定してください。
 * sudo apt install fonts-takao
 *
 * また、libfreetype-devもインストールしてください。
 * sudo apt install libfreetype-dev
 * 
 * コンパイル方法
 * $ g++ -O2 -I/usr/local/include -L/usr/local/lib -I/usr/include/freetype2 -o spitest_ili9341_double_buffer_draw_char spitest_ili9341_double_buffer_draw_char.cc -lgpiod -lfreetype
 * 
 * 
 * 実行方法
 * 
 * $ export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/lib
 * $ export PATH=/usr/local/bin:$PATH
 * $ sudo chmod o+rw /dev/gpiochip0
 * $ sudo chmod o+rw /dev/spidev1.0
 * $ ./spitest_ili9341_double_buffer_draw_char
 *
 * (LD_LIBRARY_PATHとPATHのexportは~/.profileに追記した方が便利です。)
 * 
 * 
 * mango pi mq priとLCDの接続はこんな感じです
 * (libgpiodではピンヘッダのピン番号ではなくpin xxxと書かれてるxxxの部分の番号を指定します
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
 * (下記のように実行するとピンの利用状況が表示されるようです。)
 * $ sudo cat /sys/kernel/debug/pinctrl/2000000.pinctrl/pinmux-pins
 * 
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <csignal>
#include <linux/spi/spidev.h>
#include <gpiod.h>
#include <cmath>
#include <cstring>

#include <string>
#include <codecvt>
#include <iostream>
#include <locale>
#include <vector>
#include <sstream> // std::stringstream
#include <istream> // std::getline

#include <ft2build.h>
#include FT_FREETYPE_H


#define     SPI_DC       144    //ピンヘッダのピン番号28(pin 144 PE16): UNCLAIMED
#define     SPI_CS       145    //ピンヘッダのピン番号27(pin 145 PE17): UNCLAIMED
#define     SPI_RESET     65    //ピンヘッダのピン番号22(pin  65 PC1) : UNCLAIMED

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
struct gpiod_line *gpiod_dc_lineout;
struct gpiod_line *gpiod_reset_lineout;
struct gpiod_line *gpiod_cs_lineout;
const char *appname = "spitest01";
int gpiod_value01;
void gpiod_close();
void gpiod_set_out(struct gpiod_line *pin, int num);

void spi_open();
void spi_close();
int  spidev_fd01 = 0;
void spi_write(uint8_t spi_send_byte);
void spi_write_buffer(uint8_t byte);
void spi_write_buffer_flush();

void sigint_handler(int sig);
void delay(long milli_sec);
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




uint16_t *canvas16_buffer;
int32_t canvas16_rotation = 0;
int32_t canvas16_width;
int32_t canvas16_height;
int32_t canvas16_init_width;
int32_t canvas16_init_height;
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







FT_Library ft_library01;
FT_Face    ft_face01;
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








int rnd(int num);
double second();


void loop();
void main_end();

int main(int argc, char *argv[]) {

    // SIGINTハンドラの設定
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    init();

    ili9341_init();
    fillRect (0, 0, _width, _height, ILI9341_BLACK);
    set_rotation_ILI9341(3);

    canvas16_begin(320, 240);
    canvas16_set_rotation(0);



    // freetypeの初期化
    int ret;
    if ((ret = init_freetype(&ft_library01)) != 0) {
        perror("init_freetype");
    main_end();
        exit(1);
    }

    // font設定
    if ((ret = set_font(&ft_library01, &ft_face01, "/usr/share/fonts/truetype/takao-gothic/TakaoPGothic.ttf"))) {
        perror("/usr/share/fonts/truetype/takao-gothic/TakaoPGothic.ttf");
        main_end();
        exit(1);
    }

    // fontサイズ設定
    set_font_size(&ft_face01, 16, 300, 300);

    for (int i = 0; i < 20; i++) {
        loop();
    }

    canvas16_end();

    main_end();

    exit(0);

}

void main_end() {
    spi_close();
    gpiod_close();
}

void draw_func01(int32_t num);

void loop(){

    canvas16_set_rotation(0);

    // bitmapデータ 320x240 16bit RGB565(Little Endian) 
    extern const uint8_t bitmap01[];
    // bitmapデータ 320x240 16bit RGB565(Little Endian)
    extern const uint8_t bitmap02[];


    canvas16_fillRect(0, 0, canvas16_width, canvas16_height, ILI9341_BLACK);
    canvas16_update(0, 0);



    // double buffer drawing
    draw_func01(0);

    set_font_size(&ft_face01, 40, 300, 300);
    set_font_color(color565(255, 255, 255), ILI9341_BLACK);
    canvas16_drawChars(&ft_face01, 0, 40, "abcdefg", sizeof("abcdefg"));
    canvas16_drawChars(&ft_face01, 0, 90, "あいうえお", sizeof("あいうえお"));
    canvas16_drawChars(&ft_face01, 0, 140, "ILI9341", sizeof("ILI9341"));
    canvas16_update(0,0);

    delay(5000);



    draw_func01(1);

    char text0001[] = " RISC-V（リスク ファイブ）は、確立された縮小命令セットコンピュータ (RISC) の原則に基づいたオープン標準の命令セットアーキテクチャ (ISA) である。他の多くのISA設計とは異なり、RISC-V ISAは、使用料のかからないオープンソースライセンスで提供されている。多くの企業がRISC-Vハードウェアを提供したり、発表したりしており、RISC-Vをサポートするオープンソースのオペレーティングシステムが利用可能であり、いくつかの一般的なソフトウェアツールチェーンで命令セットがサポートされている。";
    set_font_pos(0, 20);
    set_font_size(&ft_face01, 20, 300, 300);
    set_font_line_height(18);
    canvas16_print(&ft_face01, text0001);
    canvas16_update(0,0);
    delay(10000);



    draw_func01(0);

    std::string text0003 = " RISC-V (pronounced \"risk-five\"[1] where five refers to the number of generations of RISC architecture that were developed at the University of California, Berkeley since 1981[3]) is an open standard instruction set architecture (ISA) based on established RISC principles. Unlike most other ISA designs, RISC-V is provided under open source licenses that do not require fees to use. A number of companies are offering or have announced RISC-V hardware, open source operating systems with RISC-V support are available, and the instruction set is supported in several popular software toolchains. ";
    set_font_pos(0, 25);
    set_font_size(&ft_face01, 25, 300, 300);
    set_font_line_height(22);
    canvas16_word_print(&ft_face01, text0003);
    canvas16_update(0, 0);
    delay(10000);



    draw_func01(1);

    std::string text0005 = " RISC-V (pronounced \"risk-five\"[1] where five refers to the number of generations of RISC architecture that were developed at the University of California, Berkeley since 1981[3]) is an open standard instruction set architecture (ISA) based on established RISC principles. Unlike most other ISA designs, RISC-V is provided under open source licenses that do not require fees to use. A number of companies are offering or have announced RISC-V hardware, open source operating systems with RISC-V support are available, and the instruction set is supported in several popular software toolchains. ";
    set_font_pos(100, 25);
    set_font_size(&ft_face01, 25, 300, 300);
    set_font_line_height(22);
    canvas16_word_print(&ft_face01, text0005);
    canvas16_update(0, 0);
    delay(10000);





    // raspberry
    canvas16_fillRect(0, 0, canvas16_width, canvas16_height, ILI9341_BLACK);
    canvas16_update(0, 0);

    void random_raspberry_loop();
    random_raspberry_loop();




    canvas16_set_rotation(0);



    // ILI9341 LCD direct drawing
    draw_func01(0);

    set_font_size(&ft_face01, 40, 300, 300);
    set_font_color(color565(255, 255, 255), ILI9341_BLACK);
    drawChars(&ft_face01, 0, 40, "abcdefg", sizeof("abcdefg"));
    drawChars(&ft_face01, 0, 90, "あいうえお", sizeof("あいうえお"));
    drawChars(&ft_face01, 0, 140, "ILI9341", sizeof("ILI9341"));

    delay(5000);



    draw_func01(1);

    char text0000[] = " RISC-V（リスク ファイブ）は、確立された縮小命令セットコンピュータ (RISC) の原則に基づいたオープン標準の命令セットアーキテクチャ (ISA) である。他の多くのISA設計とは異なり、RISC-V ISAは、使用料のかからないオープンソースライセンスで提供されている。多くの企業がRISC-Vハードウェアを提供したり、発表したりしており、RISC-Vをサポートするオープンソースのオペレーティングシステムが利用可能であり、いくつかの一般的なソフトウェアツールチェーンで命令セットがサポートされている。";
    set_font_pos(0, 20);
    set_font_size(&ft_face01, 20, 300, 300);
    set_font_line_height(18);
    print(&ft_face01, text0000);
    delay(10000);



    draw_func01(0);

    std::string text0002 = " RISC-V (pronounced \"risk-five\"[1] where five refers to the number of generations of RISC architecture that were developed at the University of California, Berkeley since 1981[3]) is an open standard instruction set architecture (ISA) based on established RISC principles. Unlike most other ISA designs, RISC-V is provided under open source licenses that do not require fees to use. A number of companies are offering or have announced RISC-V hardware, open source operating systems with RISC-V support are available, and the instruction set is supported in several popular software toolchains. ";
    set_font_pos(0, 25);
    set_font_size(&ft_face01, 25, 300, 300);
    set_font_line_height(22);
    word_print(&ft_face01, text0002);
    delay(10000);



    draw_func01(1);

    std::string text0004 = " RISC-V (pronounced \"risk-five\"[1] where five refers to the number of generations of RISC architecture that were developed at the University of California, Berkeley since 1981[3]) is an open standard instruction set architecture (ISA) based on established RISC principles. Unlike most other ISA designs, RISC-V is provided under open source licenses that do not require fees to use. A number of companies are offering or have announced RISC-V hardware, open source operating systems with RISC-V support are available, and the instruction set is supported in several popular software toolchains. ";
    set_font_pos(100, 25);
    set_font_size(&ft_face01, 25, 300, 300);
    set_font_line_height(22);
    word_print(&ft_face01, text0004);
    delay(10000);
}

void draw_func01(int32_t num) {
    int32_t color_depth = 0;
    int32_t color = 0;

    canvas16_fillRect(0, 0, canvas16_width, canvas16_height, ILI9341_BLACK);
    for (int32_t j = 0; j < canvas16_height; j++) {
        for (int32_t i = 0; i < canvas16_width; i++) {
            color_depth = (i + j) * 255 / (canvas16_height + canvas16_width);
            if (num == 0) {
                color = color565(color_depth, 0, 0);
            } else {
                color = color565(0, 0, color_depth);
            }
            canvas16_drawPixel(i, j, color);
        }
    }
    canvas16_update(0, 0);
}





void random_raspberry();
void random_raspberry_setup01(void); 

void random_raspberry_loop() {
    static int r = 0;

    for (int j = 0; j < 6; j++) {
    canvas16_set_rotation(r);
        random_raspberry_setup01();

        for (int i = 0; i < 20; i++) {
            random_raspberry();
        }
        r++;
        if (r >= 4) r = 0;
    }
}

#define RASPBERRY_N  60 
#define RDX        10
#define RDY        10

int random_raspberry_width;
int random_raspberry_height;
extern const uint8_t raspberry_bitmap01[];
extern const uint8_t raspberry_bitmap_mask01[];

struct {
    int    x;
    int    y;
    int    dx;
    int    dy;
    int    signx;
    int    signy;
    int    c;
} raspberry_data01[RASPBERRY_N];

void random_raspberry_setup01(void) {

    random_raspberry_width = canvas16_width;
    random_raspberry_height = canvas16_height;

    for (int i = 0; i < RASPBERRY_N; i++) {
        raspberry_data01[i].x = rnd(random_raspberry_width);
        raspberry_data01[i].y = rnd(random_raspberry_height);
        raspberry_data01[i].dx = rnd(RDX) + 1;
        raspberry_data01[i].dy = rnd(RDY) + 1;
        raspberry_data01[i].signx = rnd(2) ? 1 : -1;
        raspberry_data01[i].signy = rnd(2) ? 1 : -1;
    }
}

void random_raspberry() {

    int temp1, temp2;
    canvas16_fillRect(0, 0, canvas16_width, canvas16_height, ILI9341_BLACK);

    for (int i = 0; i < RASPBERRY_N; i++ ) {
        temp1 = raspberry_data01[i].dx * raspberry_data01[i].signx;
        temp2 = raspberry_data01[i].x + temp1;
        if (temp2 > random_raspberry_width) {
            raspberry_data01[i].signx = -1;
            raspberry_data01[i].dx = rnd(RDX) + 1;
            raspberry_data01[i].x = random_raspberry_width + raspberry_data01[i].signx * raspberry_data01[i].dx;
        } else if (temp2 < 0 ) {
            raspberry_data01[i].signx = 1;
            raspberry_data01[i].dx = rnd(RDX) + 1;
            raspberry_data01[i].x = 0 + raspberry_data01[i].signx * raspberry_data01[i].dx;
        } else {
            raspberry_data01[i].x = raspberry_data01[i].x + temp1;
        } 
        temp1 = raspberry_data01[i].dy * raspberry_data01[i].signy;
        temp2 = raspberry_data01[i].y + temp1;
        if (temp2 > random_raspberry_height) {
            raspberry_data01[i].signy = -1;
            raspberry_data01[i].dy = rnd(RDY) + 1;
            raspberry_data01[i].y = random_raspberry_height + raspberry_data01[i].signy * raspberry_data01[i].dy;
        } else if (temp2 < 0 ) {
            raspberry_data01[i].signy = 1;
            raspberry_data01[i].dy = rnd(RDY) + 1;
            raspberry_data01[i].y = 0 + raspberry_data01[i].signy * raspberry_data01[i].dy;
        } else {
            raspberry_data01[i].y = raspberry_data01[i].y + temp1;
        }

        // ラズベリーのアイコンの描画
        canvas16_drawRGBBitmap_mask(raspberry_data01[i].x, raspberry_data01[i].y, raspberry_bitmap01, raspberry_bitmap_mask01, 16, 16);
    }
    set_font_size(&ft_face01, 32, 300, 300);
    set_font_color(ILI9341_PINK, ILI9341_BLACK);
    canvas16_drawChars(&ft_face01, 0, 34, "MangoPi Mq Pro", sizeof("MangoPi Mq Pro"));
    canvas16_update(0, 0);
    delay(10);
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

int rnd(int num) {
    return (rand() % num);
}


void sigint_handler(int sig) {
    spi_close();
    gpiod_close();
    exit(1);
}

void init(){
    // GPIOデバイスを開く
    if ((gpiod_chip01=gpiod_chip_open("/dev/gpiochip0")) == NULL) {
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
    delay(100);
    gpiod_set_out(gpiod_reset_lineout, 0);
    delay(200);
    gpiod_set_out(gpiod_reset_lineout, 1);
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
        gpiod_chip_unref(gpiod_chip01);
    }
}

void gpiod_set_out(struct gpiod_line *pin, int num) {
    if ((gpiod_value01=gpiod_line_set_value(pin, num)) == -1) {
        perror("gpiod_line_set_value");
    }
}

void spi_open() {

    spidev_fd01 = open("/dev/spidev1.0", O_RDWR);

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

#define SPI_WRITE_BUFFER_SIZE 512
#define SPI_WRITE_BUFFER_NUM  400

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









int canvas16_begin(int32_t w, int32_t h) {
  int ret = 0;
  uint32_t bytes = w * h * 2;
  canvas16_init_width = w;
  canvas16_init_height = h;
  canvas16_width = w;
  canvas16_height = h;
  if ((canvas16_buffer = (uint16_t *)malloc(bytes)) != NULL) {
      memset(canvas16_buffer, 0, bytes);
      ret = 0;
  } else {
      ret = -1;
  }

  return ret;
}

void canvas16_end() {
  if (canvas16_buffer != NULL)
    free(canvas16_buffer);
}

void canvas16_set_rotation(int32_t x) {
  canvas16_rotation = (x & 3);
  switch (canvas16_rotation) {
  case 0:
  case 2:
    canvas16_width = canvas16_init_width;
    canvas16_height = canvas16_init_height;
    break;
  case 1:
  case 3:
    canvas16_width = canvas16_init_height;
    canvas16_height = canvas16_init_width;
    break;
  }
}

void canvas16_drawPixel(int32_t x, int32_t y, uint32_t color) {
  if (canvas16_buffer) {
    if ((x < 0) || (y < 0) || (x >= canvas16_width) || (y >= canvas16_height))
      return;

    int32_t t;
    switch (canvas16_rotation) {
    case 1:
      t = x;
      x = canvas16_init_width - 1 - y;
      y = t;
      break;
    case 2:
      x = canvas16_init_width - 1 - x;
      y = canvas16_init_height - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = canvas16_init_height - 1 - t;
      break;
    }

    canvas16_buffer[x + y * canvas16_init_width] = color;
  }
}

void canvas16_drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
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
      canvas16_drawPixel(y0, x0, color);
    } else {
      canvas16_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void canvas16_drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas16_width) || (y >= canvas16_height)) return;

    if((y+h-1) >= canvas16_height) h = canvas16_height-y;

    for (int yy = y; yy < (y + h); yy++) {
        canvas16_drawPixel(x, yy, color);
    }
}

void canvas16_drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {

    // Rudimentary clipping
    if((x >= canvas16_width) || (y >= canvas16_height)) return;
    if((x+w-1) >= canvas16_width)  w = canvas16_width-x;

    for (int xx = x; xx < (x + w); xx++) {
        canvas16_drawPixel(xx, y, color);
    }
}

void canvas16_fillCircleHelper(int32_t x0, int32_t y0, int32_t r,
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
        canvas16_drawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        canvas16_drawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        canvas16_drawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        canvas16_drawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

void canvas16_fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) {
    canvas16_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
    canvas16_fillCircleHelper(x0, y0, r, 3, 0, color);
}

void canvas16_drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
  canvas16_drawFastHLine(x, y, w, color);
  canvas16_drawFastHLine(x, y + h - 1, w, color);
  canvas16_drawFastVLine(x, y, h, color);
  canvas16_drawFastVLine(x + w - 1, y, h, color);
}

void canvas16_fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if(w > 0 && h > 0){
        if((x >= canvas16_width) || (y >= canvas16_height)) return;
        if((x + w - 1) >= canvas16_width)  w = canvas16_width  - x;
        if((y + h - 1) >= canvas16_height) h = canvas16_height - y;

    for (int j = y; j < (y + h); j++) {
            for (int i = x; i < (x + w); i++) {
                canvas16_drawPixel(i, j, color);
        }
    }
    }
}

/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
void canvas16_drawRGBBitmap(int32_t x, int32_t y, const uint8_t bitmap[], int32_t w, int32_t h) {
    int32_t i, j;
    const uint16_t *p = (uint16_t *)bitmap;
    uint16_t pixel01;

    for(j=0; j < h; j++){
        for(i = 0; i < w; i++) {
            canvas16_drawPixel(x + i, y + j, *p++);
        }
    }
}

/* bitmapデータは16bitのrgb565のリトルエンディアン形式です */
/* maskデータは1bitモノクロのビットマップデータです(MSBから開始します) */
void canvas16_drawRGBBitmap_mask(int32_t x, int32_t y, const uint8_t *_bitmap,
                                 const uint8_t *mask, int32_t w, int32_t h) {
  uint16_t *bitmap = (uint16_t *)_bitmap;
  int32_t bw = (w + 7) / 8;
  uint8_t byte = 0;
  for (int32_t j = 0; j < h; j++) {
    for (int32_t i = 0; i < w; i++) {
      if (i & 7)
        byte <<= 1;
      else
        byte = mask[j * bw + i / 8];
      if (byte & 0x80) {
        canvas16_drawPixel(x + i, y + j, bitmap[j * w + i]);
      }
    }
  }
}

void canvas16_update(int32_t x, int32_t y) {
    drawRGBBitmap(x, y, (uint8_t *)canvas16_buffer, canvas16_init_width, canvas16_init_height);
}





#define MAX_TEXT_SIZE01 32768
#define STR_BUFF01_LEN01 32768
bool _back_ground_fill_flag = false;
int32_t freetype_fg_color = ILI9341_WHITE;
int32_t freetype_bg_color = ILI9341_BLACK;
int32_t freetype_font_size01 = 16 * 34;
int32_t freetype_font_height01 = 34;
int32_t freetype_font_line_height01 = 34;
int32_t freetype_font_space01 = 16;
int32_t _freetype_x = 0;
int32_t _freetype_y = 0;
char freetype_print_str_buff01[STR_BUFF01_LEN01];

int init_freetype(FT_Library *ft_library01) {
    int ret = 0;
    if ((ret = FT_Init_FreeType(ft_library01 )) != 0) {
        perror("FT_Init_FreeType ft_library01");
        return ret;
    }

    return ret;
}

int set_font(FT_Library *ft_library01, FT_Face *ft_face01, const char *font_path) {

    int ret = 0;
    if ((ret = FT_New_Face(*ft_library01, font_path, 0, ft_face01)) != 0) {
        perror("FT_New_Face ft_library01 ft_face01");
        return ret;
    }

    if ((ret = FT_Set_Char_Size(*ft_face01, 0,
                      16 * 34,    // 文字の幅 * 高さ
                      300, 300)   // X方向のdpi、Y方向のdpi
                      ) != 0) {
        perror("FT_Set_Char_Size ft_face01");
        return ret;
    }
    int32_t size = 16;
    freetype_font_size01 = (size / 2) * size;
    freetype_font_height01 = size;
    freetype_font_line_height01 = size;
    int32_t get_font_ch_char_w(FT_Face *ft_face01, const char *text);
    freetype_font_space01 = get_font_ch_char_w(ft_face01, u8"a");

    return ret;
}

int set_font_size(FT_Face *ft_face01, int32_t size, int32_t x_dpi, int32_t y_dpi) {
    int ret = 0;

    if ((ret = FT_Set_Char_Size(*ft_face01, 0,
                      ((size - 2) / 2) * size,    // 文字の幅 * 高さ
                      x_dpi, y_dpi)   // X方向のdpi、Y方向のdpi
                      ) != 0) {
        perror("FT_Set_Char_Size ft_face01");
        return ret;
    }
    freetype_font_size01 = (size / 2) * size;
    freetype_font_height01 = size;
    freetype_font_line_height01 = size;
    int32_t get_font_ch_char_w(FT_Face *ft_face01, const char *text);
    freetype_font_space01 = get_font_ch_char_w(ft_face01, u8"a");

    return ret;
}

int32_t utf8_byte_count(char ch) {

    uint32_t a = (uint32_t)ch;
    if (0 <= a && a < 0x80) {
        return 1;
    } else if (0xc2 <= a && a < 0xe0) {
        return 2;
    } else if (0xe0 <= a && a < 0xf0) {
        return 3;
    } else if (0xf0 <= a && a < 0xf8) {
        return 4;
    }

    return 0;
}

std::u32string utf8_to_utf32(const char *text, int32_t buf_size) {

    int i;
    uint32_t a, byte_count01;
    std::u32string str1 = U"";
    for (i = 0; i < buf_size; i++) {
        if (text[i] == '\0') break;
        byte_count01 = utf8_byte_count(text[i]);
        switch (byte_count01) {
            case 1:
                str1.push_back((char32_t)text[i]);
                break;
            case 2:
                a = (((uint32_t)text[i] & 0x1f) << 6);
                a |= ((uint32_t)text[++i] & 0x3f);
                str1.push_back((char32_t)a);
                break;
            case 3:
                a = ((uint32_t)text[i] & 0x0f) << 12;
                a |= ((uint32_t)text[++i] & 0x3f) << 6;
                a |= ((uint32_t)text[++i] & 0x3f);
                str1.push_back((char32_t)a);
                break;
            case 4:
                a = ((uint32_t)text[i] & 0x07) << 18;
                a |= ((uint32_t)text[++i] & 0x3f) << 12;
                a |= ((uint32_t)text[++i] & 0x3f) << 6;
                a |= ((uint32_t)text[++i] & 0x3f);
                str1.push_back((char32_t)a);
            default:
        break;
        }
    }

    return str1;
}

void set_font_back_ground_fill(bool flag) {
    _back_ground_fill_flag = flag;
}

void set_font_color(int32_t fg, int32_t bg) {
    freetype_fg_color = fg;
    if (bg != -1) {
        freetype_bg_color = bg;
    }
}

int32_t drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, const char *text, int32_t buf_size) {

    int ret = 0;
    int32_t str_w = 0;
    std::u32string  u32_string01 = utf8_to_utf32(text, buf_size);

    for (int k = 0; k < u32_string01.size(); k++) {
        if ((u32_string01[k] == 0x0000000a) || (u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0) || (u32_string01[k] == 0x00003000) ) {
            if (u32_string01[k] == 0x00003000) {
                str_w += freetype_font_space01 * 2;
            } else if ((u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0)){
                str_w += freetype_font_space01;
            }
        } else {
            FT_UInt glyph_char_index01 = FT_Get_Char_Index(*ft_face01, u32_string01[k]);

            if((ret = FT_Load_Glyph(*ft_face01, glyph_char_index01, FT_LOAD_DEFAULT )) != 0) {
                perror("FT_Load_Glyph ft_face01 glyph_char_index01 FT_LOAD_DEFAULT");
            }
            FT_Render_Glyph((*ft_face01)->glyph, FT_RENDER_MODE_NORMAL );// 1文字レンダリング

            int index;
            for (int j = 0; j < (*ft_face01)->glyph->bitmap.rows; j++) {   // 文字のbitmapの描画
                for (int i = 0; i < (*ft_face01)->glyph->bitmap.width; i++) {
                    index = j * (*ft_face01)->glyph->bitmap.width + i;
                    if ((*ft_face01)->glyph->bitmap.buffer[index]) {
                        drawPixel(char_x + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_fg_color);
                    } else {
                        if (_back_ground_fill_flag) 
                            drawPixel(char_x + (*ft_face01)->glyph->bitmap_left + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_bg_color);
                    }
                }
            }
            str_w += (*ft_face01)->glyph->bitmap.width + (*ft_face01)->glyph->bitmap_left;
        }
    }
    return (char_x + str_w);
}

int32_t canvas16_drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, const char *text, int32_t buf_size) {

    int ret = 0;
    int32_t str_w = 0;
    std::u32string  u32_string01 = utf8_to_utf32(text, buf_size);

    for (int k = 0; k < u32_string01.size(); k++) {
        if ((u32_string01[k] == 0x0000000a) || (u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0) || (u32_string01[k] == 0x00003000) ) {
            if (u32_string01[k] == 0x00003000) {
                str_w += freetype_font_space01 * 2;
            } else if ((u32_string01[k] == 0x00000020) || (u32_string01[k] == 0x000000a0)){
                str_w += freetype_font_space01;
            }
        } else {
            FT_UInt glyph_char_index01 = FT_Get_Char_Index(*ft_face01, u32_string01[k]);

            if((ret = FT_Load_Glyph(*ft_face01, glyph_char_index01, FT_LOAD_DEFAULT )) != 0) {
                perror("FT_Load_Glyph ft_face01 glyph_char_index01 FT_LOAD_DEFAULT");
            }
            FT_Render_Glyph((*ft_face01)->glyph, FT_RENDER_MODE_NORMAL );// 1文字レンダリング

            int index;
            for (int j = 0; j < (*ft_face01)->glyph->bitmap.rows; j++) {   // 文字のbitmapの描画
                for (int i = 0; i < (*ft_face01)->glyph->bitmap.width; i++) {
                    index = j * (*ft_face01)->glyph->bitmap.width + i;
                    if ((*ft_face01)->glyph->bitmap.buffer[index]) {
                        canvas16_drawPixel(char_x + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_fg_color);
                    } else {
                        if (_back_ground_fill_flag) 
                       canvas16_drawPixel(char_x + (*ft_face01)->glyph->bitmap_left + i + str_w,
                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_bg_color);
                    }
                }
            }
        str_w += (*ft_face01)->glyph->bitmap.width + (*ft_face01)->glyph->bitmap_left;
        }
    }
    return (char_x + str_w);
}

int32_t freetype_next_line(int32_t cury, int32_t char_h) {
    int32_t res;
    res = cury + char_h;
    return res;
}

void set_font_line_height(int32_t char_h) {
    freetype_font_line_height01 = char_h;
}

int freetype_substr01(const char *source, char *dest, int buffsize, int start, int length) {
    char *sp, *ep, size;
    size = buffsize - 1;
    int32_t source_len01 = strnlen(source,  MAX_TEXT_SIZE01);

    sp = (char *)source + start;
    ep = (char *)source + start + length;
    if (sp > (source + source_len01)) {
        dest[0] = '\0';
        return -1;
    }
    if (ep > (source + source_len01)) {
        ep = ((char *)source + source_len01);
        length = ep - sp;
    }
    if (length > size) {
        length = size;
    }
    if (length < 0) {
        dest[0] = '\0';
        return -1;
    }
    memcpy(dest, sp, length);
    dest[length] = '\0';
    return 0;
}

int32_t get_font_ch_char_w(FT_Face *ft_face01, const char *text) {
    int32_t width = 0;
    std::u32string u32_string01 = utf8_to_utf32(text, 9);
    if ((u32_string01[0] == 0x0000000a) || (u32_string01[0] == 0x00000020) || (u32_string01[0] == 0x000000a0) || (u32_string01[0] == 0x00003000) ) {
        if (u32_string01[0] == 0x00003000) {
            width = freetype_font_space01 * 2;
        } else if ((u32_string01[0] == 0x00000020) || (u32_string01[0] == 0x000000a0)){
            width = freetype_font_space01;
        } else if(u32_string01[0] == 0x0000000a) {
            width = 0;
        }
    } else {
        int ret;
        FT_UInt glyph_char_index01 = FT_Get_Char_Index(*ft_face01, u32_string01[0]);
        if((ret = FT_Load_Glyph(*ft_face01, glyph_char_index01, FT_LOAD_DEFAULT )) != 0) {
            perror("FT_Load_Glyph ft_face01 glyph_char_index01 FT_LOAD_DEFAULT");
        }
        FT_Render_Glyph((*ft_face01)->glyph, FT_RENDER_MODE_NORMAL );
        width = (*ft_face01)->glyph->bitmap.width + (*ft_face01)->glyph->bitmap_left;
    }
    return width;
}

void set_font_pos(int32_t x, int32_t y) {
    _freetype_x = x;
    _freetype_y = y;
}

void print(FT_Face *ft_face01, const char *text) { // does character wrap, compatible with stream output

    int32_t curx, cury, char_w, char_h, width1, written;
    int32_t str_len01;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    width1 = 0;
    written = 0;

    str_len01 = strnlen(text, MAX_TEXT_SIZE01);
    if (str_len01 > STR_BUFF01_LEN01) return ;

    int32_t pos_next = 0;
    int32_t ch_byte_count = 0;
    for (int32_t pos = 0; pos < str_len01; pos = pos_next) {
        char ch[9];
        ch_byte_count = utf8_byte_count(text[pos]);
        for (int i = 0; i < ch_byte_count; i++) {
            ch[i] = text[pos_next++];
        }
        ch[ch_byte_count] = '\0';
        if ((ch[0] == '\n') && (ch_byte_count == 1)) {
            if (pos > 0) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
            }
            curx = 0;
            written = pos_next;
            width1 = 0;
            cury = freetype_next_line(cury, char_h);
        } else {
            char_w = get_font_ch_char_w(ft_face01, ch);
            if ((curx + width1 + char_w) == _width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos_next - written);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                written = pos_next;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
            
            } else if ((curx + width1 + char_w) > _width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, pos, pos_next - pos);
                drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx += char_w;
                width1 += char_w;
                written = pos_next;
            } else {
                width1 += char_w;
            }
        }
    }
    if (written < str_len01) {
        freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, str_len01 - written);
        curx = drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
    }
    _freetype_x = curx;
    _freetype_y = cury;
}

void canvas16_print(FT_Face *ft_face01, const char *text) { // does character wrap, compatible with stream output

    int32_t curx, cury, char_w, char_h, width1, written;
    int32_t str_len01;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    width1 = 0;
    written = 0;

    str_len01 = strnlen(text, MAX_TEXT_SIZE01);
    if (str_len01 > STR_BUFF01_LEN01) return ;

    int32_t pos_next = 0;
    int32_t ch_byte_count = 0;
    for (int32_t pos = 0; pos < str_len01; pos = pos_next) {
        char ch[9];
        ch_byte_count = utf8_byte_count(text[pos]);
        for (int i = 0; i < ch_byte_count; i++) {
            ch[i] = text[pos_next++];
        }
        ch[ch_byte_count] = '\0';
        if ((ch[0] == '\n') && (ch_byte_count == 1)) {
            if (pos > 0) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
            }
            curx = 0;
            written = pos_next;
            width1 = 0;
            cury = freetype_next_line(cury, char_h);
        } else {
            char_w = get_font_ch_char_w(ft_face01, ch);
            if ((curx + width1 + char_w) == canvas16_width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos_next - written);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                written = pos_next;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
            
            } else if ((curx + width1 + char_w) > canvas16_width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, pos, pos_next - pos);
                canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx += char_w;
                width1 += char_w;
                written = pos_next;
            } else {
                width1 += char_w;
            }
        }
    }
    if (written < str_len01) {
        freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, str_len01 - written);
        curx = canvas16_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
    }
    _freetype_x = curx;
    _freetype_y = cury;
}

int32_t get_font_word_w(FT_Face *ft_face01, std::string word) {
    int32_t word_width = 0;
    int32_t ch_byte_count02 = 0;
    for (int i = 0; i < word.size(); i += ch_byte_count02) {
        ch_byte_count02 = utf8_byte_count(word[i]);
        word_width += get_font_ch_char_w(ft_face01, word.substr(i, ch_byte_count02).c_str());
    }
    return word_width;
}

int32_t freetype_check_leading_spaces(std::string str1) {
    int i;
    for (i = 0; i < str1.size(); i++) {
        if ((char)str1[i] != u8' ') break;
    }

    return i;
}

std::string freetype_spaces(int32_t number) {
    std::string spaces01 = u8"";
    for (int i = 0; i < number; i++) {
        spaces01.push_back(u8' ');
    }

    return spaces01;
}

void word_print(FT_Face *ft_face01, std::string text) {

    int32_t curx, cury, char_h, char_w;
    std::stringstream text_s{text};
    std::string lines;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    char_w = freetype_font_height01 * 2;      // 文字の横幅の最大値
    while (std::getline(text_s, lines, '\n')) {

        int32_t spaces_num = freetype_check_leading_spaces(lines);
        std::stringstream lines_s{lines};
        std::string buf;
        std::vector<std::string> words;
        while (std::getline(lines_s, buf, ' ')) {
            words.push_back(buf);
        }

        if (spaces_num > 0) {
            words[0].insert(0, freetype_spaces(spaces_num));
        }
        for (int i = 0; i < words.size(); i++) {
            std::string word = words[i];
            int32_t word_width01 = get_font_word_w(ft_face01, word);
            if ((curx + word_width01) > _width) {
                if (curx != _freetype_x) cury = freetype_next_line(cury, char_h);
                curx = _freetype_x;
                while ((curx + get_font_word_w(ft_face01, word)) > _width) {
                    int32_t ch_byte_count = 0;
                    int32_t ch_count01 = 0;
                    int32_t ch_count01_next = 0;
                    int32_t word_width01 = 0;
                    int32_t word_width01_next = 0;
                    char ch01[9];
                    int32_t ch_copy_byte_count = 0;
                    bool over_flag01 = false;
                    for (ch_count01_next = 0; ch_count01_next < word.size();) {
                        ch_byte_count = utf8_byte_count((char)word[ch_count01_next]);
                        ch_count01 = ch_count01_next;
                        ch_count01_next += ch_byte_count;
                        word_width01 = word_width01_next;
                        for (ch_copy_byte_count = 0; ch_copy_byte_count < ch_byte_count; ch_copy_byte_count++) {
                            ch01[ch_copy_byte_count] = (char)word[ch_count01_next + ch_copy_byte_count];
                        }
                        ch01[ch_copy_byte_count] = '\0';
                        word_width01_next += get_font_ch_char_w(ft_face01, ch01);
                        if ((curx + word_width01_next) > _width) {
                            over_flag01 = true;
                            break;
                        }  
                    }
                    if (over_flag01) {
                        drawChars(ft_face01, curx, cury,
                                    (word.substr(0, ch_count01).c_str()), ch_count01);
                        word = word.substr(ch_count01, word.size() - ch_count01);
                        curx = _freetype_x;
                        cury = freetype_next_line(cury, char_h);
                    } else {
                        if ((curx + word_width01_next + freetype_font_space01) <= _width) {
                            curx = drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                        } else {
                            drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                            curx = _freetype_x;
                            cury = freetype_next_line(cury, char_h);
                        }
                        break;
                    }
                }
            }
            if (word.length() > 0) {
                if ((curx + word_width01 +  + freetype_font_space01) <= _width) {
                    curx = drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                } else {
                    drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                    curx = _freetype_x;
                    cury = freetype_next_line(cury, char_h);
                }
            }
        }
        curx = _freetype_x;
        cury = freetype_next_line(cury, char_h);
    }
    _freetype_y = cury;
}

void canvas16_word_print(FT_Face *ft_face01, std::string text) {

    int32_t curx, cury, char_h, char_w;
    std::stringstream text_s{text};
    std::string lines;

    curx = _freetype_x;
    cury = _freetype_y;
    char_h = freetype_font_line_height01;
    char_w = freetype_font_height01 * 2;      // 文字の横幅の最大値
    while (std::getline(text_s, lines, '\n')) {

        int32_t spaces_num = freetype_check_leading_spaces(lines);
        std::stringstream lines_s{lines};
        std::string buf;
        std::vector<std::string> words;
        while (std::getline(lines_s, buf, ' ')) {
            words.push_back(buf);
        }

        if (spaces_num > 0) {
            words[0].insert(0, freetype_spaces(spaces_num));
        }
        for (int i = 0; i < words.size(); i++) {
            std::string word = words[i];
            int32_t word_width01 = get_font_word_w(ft_face01, word);
            if ((curx + word_width01) > canvas16_width) {
                if (curx != _freetype_x) cury = freetype_next_line(cury, char_h);
                curx = _freetype_x;
                while ((curx + get_font_word_w(ft_face01, word)) > canvas16_width) {
                    int32_t ch_byte_count = 0;
                    int32_t ch_count01 = 0;
                    int32_t ch_count01_next = 0;
                    int32_t word_width01 = 0;
                    int32_t word_width01_next = 0;
                    char ch01[9];
                    int32_t ch_copy_byte_count = 0;
                    bool over_flag01 = false;
                    for (ch_count01_next = 0; ch_count01_next < word.size();) {
                        ch_byte_count = utf8_byte_count((char)word[ch_count01_next]);
                        ch_count01 = ch_count01_next;
                        ch_count01_next += ch_byte_count;
                        word_width01 = word_width01_next;
                        for (ch_copy_byte_count = 0; ch_copy_byte_count < ch_byte_count; ch_copy_byte_count++) {
                            ch01[ch_copy_byte_count] = (char)word[ch_count01_next + ch_copy_byte_count];
                        }
                        ch01[ch_copy_byte_count] = '\0';
                        word_width01_next += get_font_ch_char_w(ft_face01, ch01);
                        if ((curx + word_width01_next) > _width) {
                            over_flag01 = true;
                            break;
                        }  
                    }
                    if (over_flag01) {
                        canvas16_drawChars(ft_face01, curx, cury,
                                    (word.substr(0, ch_count01).c_str()), ch_count01);
                        word = word.substr(ch_count01, word.size() - ch_count01);
                        curx = _freetype_x;
                        cury = freetype_next_line(cury, char_h);
                    } else {
                        if ((curx + word_width01_next + freetype_font_space01) <= canvas16_width) {
                            curx = canvas16_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                        } else {
                            canvas16_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                            curx = _freetype_x;
                            cury = freetype_next_line(cury, char_h);
                        }
                        break;
                    }
                }
            }
            if (word.length() > 0) {
                if ((curx + word_width01 +  + freetype_font_space01) <= canvas16_width) {
                    curx = canvas16_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                } else {
                    canvas16_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                    curx = _freetype_x;
                    cury = freetype_next_line(cury, char_h);
                }
            }
        }
        curx = _freetype_x;
        cury = freetype_next_line(cury, char_h);
    }
    _freetype_y = cury;
}



// 16x16 ラズベリーマスクデータ(1ピクセル1ビット MSBが先頭)
const uint8_t raspberry_bitmap_mask01[] = {
    0x00,0x00,0x00,0x00,0x0e,0x70,0x1f,0xf8,0x1f,0xf8,0x0f,0xf0,0x0f,0xf0,0x1f,0xf8,
    0x1f,0xf8,0x3f,0xfc,0x1f,0xf8,0x1f,0xf8,0x1f,0xf8,0x0f,0xf0,0x03,0xc0,0x00,0x00,
};


// 16x16 ラズベリー16bitピクセルRGB565画像ーデータ(リトルエンディアン)
const uint8_t raspberry_bitmap01[] = {
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x38,0xc6,0x96,0xb5,0xf7,0xbd,0xff,0xff,
    0xff,0xff,0xf7,0xbd,0x96,0xb5,0x59,0xce,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0x38,0xc6,0x86,0x54,0x68,0x65,0x68,0x65,0xca,0x5b,
    0xca,0x5b,0x68,0x65,0x68,0x65,0x86,0x54,0x38,0xc6,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xba,0xd6,0x26,0x4c,0xc9,0x6d,0xc7,0x54,0xa5,0x43,
    0xa5,0x43,0xc7,0x54,0xc8,0x6d,0x26,0x4c,0xba,0xd6,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x34,0xa5,0x46,0x4c,0x66,0x4c,0x41,0x10,
    0x41,0x10,0x66,0x4c,0x46,0x4c,0x34,0xa5,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf3,0xa4,0xa7,0x98,0x43,0x48,0xc8,0xb0,
    0xc8,0xb8,0x43,0x50,0xa7,0x98,0xf4,0xa4,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xdf,0xff,0x66,0x59,0x65,0x68,0x65,0x78,0x43,0x50,
    0x44,0x50,0x65,0x78,0x65,0x68,0x66,0x59,0xdf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0x14,0xa5,0x65,0x68,0xa7,0xa0,0xc9,0xc0,0x86,0x88,
    0x86,0x88,0xc9,0xc0,0xa7,0xa0,0x65,0x68,0x14,0xa5,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xe8,0x69,0x86,0x90,0xa7,0xa0,0xc9,0xc0,0x64,0x68,
    0x64,0x68,0xc9,0xc0,0xa7,0xa0,0x86,0x90,0x09,0x6a,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0x92,0x94,0x44,0x58,0x22,0x30,0x43,0x40,0xa8,0xa8,
    0xa8,0xa8,0x43,0x48,0x22,0x30,0x44,0x58,0x92,0x94,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0x9a,0xd6,0x86,0x88,0xc8,0xb0,0x44,0x58,0xc9,0xc8,
    0xc9,0xc0,0x64,0x60,0xc8,0xb0,0x86,0x88,0xba,0xd6,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xdf,0xff,0x2c,0x73,0xa8,0xb0,0x43,0x48,0x65,0x78,
    0x65,0x78,0x43,0x48,0xa8,0xb0,0x2d,0x73,0xdf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x9e,0xf7,0xf3,0x9c,0x25,0x39,0xa8,0xa8,
    0xa8,0xa8,0x25,0x41,0xf3,0x9c,0x9e,0xf7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3c,0xe7,0xd3,0xa4,
    0xd3,0x9c,0x3c,0xe7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

