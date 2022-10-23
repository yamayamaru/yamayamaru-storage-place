/*
 * このプログラムはUbuntu Server 22.04.1のmango pi mq proのSPIテスト用に作ったものです。
 * LCDにILI9486のLCDを使います。
 * また、libgpiodとspidev、libfreetypeを使います。
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
 * (現在のUbuntu Server 22.04.1は最新版にすればCONFIG_GPIO_CDEV_V1がyになってますので必要ありません。)
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
 * このプログラムではNotoSansCJK-Regular.ttcを使っているのでインストールされていない場合はaptで入れるか、
 * 341行目のset_fontの部分で使いたいフォントを指定してください。
 * sudo apt install fonts-noto-cjk 
 *
 * また、libfreetype-devもインストールしてください。
 * sudo apt install libfreetype-dev
 * 
 * コンパイル方法
 * $ g++ -O2 -I/usr/include/freetype2 -o spitest_ili9486_double_buffer_draw_char spitest_ili9486_double_buffer_draw_char.cc -lgpiod -lfreetype
 *
 * libgpiodをソースコードから入れた人は下のようになります
 * $ g++ -O2 -I/usr/local/include -L/usr/local/lib -I/usr/include/freetype2 -o spitest_ili9486_double_buffer_draw_char spitest_ili9486_double_buffer_draw_char.cc -lgpiod -lfreetype
 *
 *  (undefined reference to `gpiod_chip_close(gpiod_chip*)'というエラーが出る場合は
 *   757行目あたりのgpiod_chip_close()をgpiod_chip_unref()に変更してください。)
 * 
 * 
 * 実行方法
 *
 * $ sudo chmod o+rw /dev/gpiochip0
 * $ sudo chmod o+rw /dev/spidev1.0
 * $ ./spitest_ili9486_double_buffer_draw_char
 *
 * libgpiodをソースコードから入れた人は下のようになります。
 * $ export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/lib
 * $ export PATH=/usr/local/bin:$PATH
 * $ sudo chmod o+rw /dev/gpiochip0
 * $ sudo chmod o+rw /dev/spidev1.0
 * $ ./spitest_ili9486_double_buffer_draw_char
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







extern FT_Library ft_library01;
extern FT_Face    ft_face01;
int init_freetype(FT_Library *ft_library01);
int set_font(FT_Library *ft_library01, FT_Face *ft_face01, const char *font_path);
int set_font_size(FT_Face *ft_face01, int32_t size, int32_t x_dpi, int32_t y_dpi);
void set_font_back_ground_fill(bool flag);
void set_font_color(int32_t fg, int32_t bg = -1);
int32_t drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, 
               const char *text, int32_t buf_size);
int32_t canvas24_drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, 
               const char *text, int32_t buf_size);
void set_font_pos(int32_t x, int32_t y);
void set_font_line_height(int32_t char_h);
void print(FT_Face *ft_face01, const char *text);
void canvas24_print(FT_Face *ft_face01, const char *text);
void word_print(FT_Face *ft_face01, std::string text);
void canvas24_word_print(FT_Face *ft_face01, std::string text);








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

    ili9486_init();
    fillRect (0, 0, _width, _height, ILI9486_BLACK);
    set_rotation_ILI9486(3);

    canvas24_begin(480, 321);
    canvas24_set_rotation(0);



    // freetypeの初期化
    int ret;
    if ((ret = init_freetype(&ft_library01)) != 0) {
        perror("init_freetype");
    main_end();
        exit(1);
    }

    // font設定
    if ((ret = set_font(&ft_library01, &ft_face01, "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc"))) {
        perror("/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc");
        main_end();
        exit(1);
    }

    // fontサイズ設定
    set_font_size(&ft_face01, 16, 300, 300);

    for (int i = 0; i < 20; i++) {
        loop();
    }

    canvas24_end();

    main_end();

    exit(0);

}

void main_end() {
    spi_close();
    gpiod_close();
}

void draw_func01(int32_t num);



// bitmapデータ 320x240 24bitRGB
extern const uint8_t bitmap01[];
// bitmapデータ 320x240 24bitRGB
extern const uint8_t bitmap02[];

void loop(){

    canvas24_set_rotation(0);


    canvas24_fillRect(0, 0, canvas24_width, canvas24_height, ILI9486_BLACK);
    canvas24_update(0, 0);



    // double buffer drawing
    draw_func01(0);

    set_font_size(&ft_face01, 40, 300, 300);
    set_font_color(color24(255, 255, 255), ILI9486_BLACK);
    canvas24_drawChars(&ft_face01, 0, 40, "abcdefg", sizeof("abcdefg"));
    canvas24_drawChars(&ft_face01, 0, 90, "あいうえお", sizeof("あいうえお"));
    canvas24_drawChars(&ft_face01, 0, 140, "ILI9486", sizeof("ILI9486"));
    canvas24_update(0,0);

    delay(5000);



    draw_func01(1);

    char text0001[] = " RISC-V（リスク ファイブ）は、確立された縮小命令セットコンピュータ (RISC) の原則に基づいたオープン標準の命令セットアーキテクチャ (ISA) である。他の多くのISA設計とは異なり、RISC-V ISAは、使用料のかからないオープンソースライセンスで提供されている。多くの企業がRISC-Vハードウェアを提供したり、発表したりしており、RISC-Vをサポートするオープンソースのオペレーティングシステムが利用可能であり、いくつかの一般的なソフトウェアツールチェーンで命令セットがサポートされている。";
    set_font_pos(0, 20);
    set_font_size(&ft_face01, 20, 300, 300);
    set_font_line_height(18);
    canvas24_print(&ft_face01, text0001);
    canvas24_update(0,0);
    delay(10000);



    draw_func01(0);

    std::string text0003 = " RISC-V (pronounced \"risk-five\"[1] where five refers to the number of generations of RISC architecture that were developed at the University of California, Berkeley since 1981[3]) is an open standard instruction set architecture (ISA) based on established RISC principles. Unlike most other ISA designs, RISC-V is provided under open source licenses that do not require fees to use. A number of companies are offering or have announced RISC-V hardware, open source operating systems with RISC-V support are available, and the instruction set is supported in several popular software toolchains. ";
    set_font_pos(0, 25);
    set_font_size(&ft_face01, 25, 300, 300);
    set_font_line_height(22);
    canvas24_word_print(&ft_face01, text0003);
    canvas24_update(0, 0);
    delay(10000);



    draw_func01(1);

    std::string text0005 = " RISC-V (pronounced \"risk-five\"[1] where five refers to the number of generations of RISC architecture that were developed at the University of California, Berkeley since 1981[3]) is an open standard instruction set architecture (ISA) based on established RISC principles. Unlike most other ISA designs, RISC-V is provided under open source licenses that do not require fees to use. A number of companies are offering or have announced RISC-V hardware, open source operating systems with RISC-V support are available, and the instruction set is supported in several popular software toolchains. ";
    set_font_pos(100, 25);
    set_font_size(&ft_face01, 25, 300, 300);
    set_font_line_height(22);
    canvas24_word_print(&ft_face01, text0005);
    canvas24_update(0, 0);
    delay(10000);





    // raspberry
    canvas24_fillRect(0, 0, canvas24_width, canvas24_height, ILI9486_BLACK);
    canvas24_update(0, 0);

    void random_raspberry_loop();
    random_raspberry_loop();




    canvas24_set_rotation(0);



    // ILI9486 LCD direct drawing
    draw_func01(0);

    set_font_size(&ft_face01, 40, 300, 300);
    set_font_color(color24(255, 255, 255), ILI9486_BLACK);
    drawChars(&ft_face01, 0, 40, "abcdefg", sizeof("abcdefg"));
    drawChars(&ft_face01, 0, 90, "あいうえお", sizeof("あいうえお"));
    drawChars(&ft_face01, 0, 140, "ILI9486", sizeof("ILI9486"));

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

    canvas24_fillRect(0, 0, canvas24_width, canvas24_height, ILI9486_BLACK);
    for (int32_t j = 0; j < canvas24_height; j++) {
        for (int32_t i = 0; i < canvas24_width; i++) {
            color_depth = (i + j) * 255 / (canvas24_height + canvas24_width);
            if (num == 0) {
                color = color24(color_depth, 0, 0);
            } else {
                color = color24(0, 0, color_depth);
            }
            canvas24_drawPixel(i, j, color);
        }
    }
    canvas24_update(0, 0);
}





void random_raspberry();
void random_raspberry_setup01(void); 

void random_raspberry_loop() {
    static int r = 0;

    for (int j = 0; j < 6; j++) {
    canvas24_set_rotation(r);
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

    random_raspberry_width = canvas24_width;
    random_raspberry_height = canvas24_height;

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
    canvas24_fillRect(0, 0, canvas24_width, canvas24_height, ILI9486_BLACK);

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
        canvas24_drawRGBBitmap_mask(raspberry_data01[i].x, raspberry_data01[i].y, raspberry_bitmap01, raspberry_bitmap_mask01, 16, 16);
    }
    set_font_size(&ft_face01, 32, 300, 300);
    set_font_color(ILI9486_PINK, ILI9486_BLACK);
    canvas24_drawChars(&ft_face01, 0, 34, "MangoPi Mq Pro", sizeof("MangoPi Mq Pro"));
    canvas24_update(0, 0);
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
    if (buffer_pos01 == SPI_WRITE_BUFFER_NUM) {
        spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
        buffer_pos01 = 0;
    }
}

void spi_write_buffer_flush() {
    spi_write_buffer_spi_write(spi_write_buffer_data01, spi_read_buffer_data01, buffer_pos01);
    buffer_pos01 = 0;
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





#define MAX_TEXT_SIZE01 32768
#define STR_BUFF01_LEN01 32768
FT_Library ft_library01;
FT_Face    ft_face01;

bool _back_ground_fill_flag = false;
int32_t freetype_fg_color = ILI9486_WHITE;
int32_t freetype_bg_color = ILI9486_BLACK;
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

int32_t canvas24_drawChars(FT_Face *ft_face01, int32_t char_x, int32_t char_y, const char *text, int32_t buf_size) {

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
                        canvas24_drawPixel(char_x + i + str_w,
                                  char_y - (*ft_face01)->glyph->bitmap_top + j, freetype_fg_color);
                    } else {
                        if (_back_ground_fill_flag) 
                       canvas24_drawPixel(char_x + (*ft_face01)->glyph->bitmap_left + i + str_w,
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

void canvas24_print(FT_Face *ft_face01, const char *text) { // does character wrap, compatible with stream output

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
                canvas24_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
            }
            curx = 0;
            written = pos_next;
            width1 = 0;
            cury = freetype_next_line(cury, char_h);
        } else {
            char_w = get_font_ch_char_w(ft_face01, ch);
            if ((curx + width1 + char_w) == canvas24_width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos_next - written);
                canvas24_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                written = pos_next;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
            
            } else if ((curx + width1 + char_w) > canvas24_width) {
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, written, pos - written);
                canvas24_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
                curx = 0;
                width1 = 0;
                cury = freetype_next_line(cury, char_h);
                freetype_substr01(text, freetype_print_str_buff01, STR_BUFF01_LEN01, pos, pos_next - pos);
                canvas24_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
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
        curx = canvas24_drawChars(ft_face01, curx, cury, freetype_print_str_buff01, STR_BUFF01_LEN01);
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
        if ((char)str1[i] != ' ') break;
    }

    return i;
}

std::string freetype_spaces(int32_t number) {
    std::string spaces01 = "";
    for (int i = 0; i < number; i++) {
        spaces01.push_back(' ');
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

void canvas24_word_print(FT_Face *ft_face01, std::string text) {

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
            if ((curx + word_width01) > canvas24_width) {
                if (curx != _freetype_x) cury = freetype_next_line(cury, char_h);
                curx = _freetype_x;
                while ((curx + get_font_word_w(ft_face01, word)) > canvas24_width) {
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
                        if ((curx + word_width01_next) > canvas24_width) {
                            over_flag01 = true;
                            break;
                        }  
                    }
                    if (over_flag01) {
                        canvas24_drawChars(ft_face01, curx, cury,
                                    (word.substr(0, ch_count01).c_str()), ch_count01);
                        word = word.substr(ch_count01, word.size() - ch_count01);
                        curx = _freetype_x;
                        cury = freetype_next_line(cury, char_h);
                    } else {
                        if ((curx + word_width01_next + freetype_font_space01) <= canvas24_width) {
                            curx = canvas24_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                        } else {
                            canvas24_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
                            curx = _freetype_x;
                            cury = freetype_next_line(cury, char_h);
                        }
                        break;
                    }
                }
            }
            if (word.length() > 0) {
                if ((curx + word_width01 +  + freetype_font_space01) <= canvas24_width) {
                    curx = canvas24_drawChars(ft_face01, curx, cury, (word + ' ').c_str(), word.size() + 1);
                } else {
                    canvas24_drawChars(ft_face01, curx, cury, word.c_str(), word.size());
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


// 16x16 ラズベリー各色8bit 24bitRGB画像ーデータ
const uint8_t raspberry_bitmap01[] = {
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xc7,0xc7,0xc6,0xb1,0xb3,0xb0,0xbd,0xbe,0xbc,0xff,0xff,0xff,
    0xff,0xff,0xff,0xbd,0xbe,0xbc,0xb1,0xb3,0xb0,0xc8,0xc9,0xc8,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xc3,0xc4,0xc3,0x51,0x92,0x37,0x61,0xad,0x42,0x61,0xad,0x42,
    0x5f,0x7b,0x57,0x5e,0x7b,0x56,0x61,0xad,0x42,0x61,0xad,0x42,0x51,0x92,0x37,
    0xc3,0xc4,0xc3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xd6,0xd6,0xd6,0x4a,0x84,0x33,0x69,0xba,0x48,
    0x56,0x98,0x3a,0x42,0x77,0x2d,0x42,0x77,0x2d,0x57,0x9a,0x3b,0x68,0xba,0x47,
    0x4a,0x84,0x33,0xd6,0xd6,0xd6,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xa5,0xa6,0xa5,
    0x4d,0x8b,0x35,0x4e,0x8c,0x35,0x15,0x0b,0x08,0x16,0x0b,0x08,0x4e,0x8c,0x35,
    0x4d,0x8a,0x35,0xa6,0xa7,0xa6,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xa1,0x9e,0x9e,0x98,0x14,0x39,0x4f,0x09,0x1d,0xb7,0x18,0x45,0xb9,0x19,0x45,
    0x53,0x0a,0x1f,0x98,0x14,0x39,0xa2,0x9f,0xa0,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xf8,0xf8,0xf8,0x5d,0x2f,0x36,0x6d,0x0d,0x28,0x79,0x0e,0x2c,0x52,0x09,0x1e,
    0x57,0x0a,0x20,0x78,0x0e,0x2c,0x6e,0x0d,0x29,0x5d,0x2f,0x37,0xf8,0xf8,0xf8,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xa4,0xa0,0xa1,0x6f,0x0d,0x29,0xa0,0x14,0x3c,0xc7,0x1a,0x4b,
    0x8b,0x11,0x33,0x8b,0x11,0x33,0xc7,0x1a,0x4b,0xa0,0x14,0x3b,0x6f,0x0d,0x28,
    0xa5,0xa1,0xa2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xfe,0xfe,0xfe,0x6f,0x3f,0x47,0x95,0x12,0x37,0xa2,0x15,0x3d,
    0xc4,0x1a,0x4a,0x68,0x0c,0x25,0x6a,0x0c,0x26,0xc4,0x1a,0x4a,0xa2,0x15,0x3c,
    0x95,0x12,0x37,0x6f,0x40,0x48,0xfe,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x94,0x90,0x91,0x5e,0x0a,0x22,
    0x37,0x06,0x13,0x47,0x08,0x19,0xab,0x16,0x40,0xaf,0x17,0x42,0x4f,0x09,0x1d,
    0x37,0x06,0x13,0x5e,0x0a,0x22,0x95,0x92,0x92,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xd2,0xd2,0xd2,
    0x8b,0x11,0x33,0xb5,0x18,0x44,0x5e,0x0b,0x23,0xc8,0x1a,0x4b,0xc7,0x1a,0x4b,
    0x67,0x0d,0x26,0xb6,0x18,0x44,0x8b,0x11,0x33,0xd3,0xd4,0xd3,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xfa,0xfa,0xfa,0x75,0x66,0x67,0xb2,0x17,0x42,0x4a,0x08,0x1a,0x7c,0x0f,0x2d,
    0x7d,0x0f,0x2e,0x4d,0x08,0x1c,0xb2,0x17,0x42,0x75,0x67,0x69,0xfa,0xfa,0xfa,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xf2,0xf2,0x9d,0x9c,0x9c,0x3d,0x26,0x29,
    0xad,0x16,0x41,0xaf,0x16,0x41,0x43,0x27,0x2c,0x9d,0x9c,0x9c,0xf1,0xf2,0xf2,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xe5,0xe5,0xe5,0xa1,0x9b,0x9b,0x9f,0x9a,0x9b,0xe5,0xe5,0xe5,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff
};


