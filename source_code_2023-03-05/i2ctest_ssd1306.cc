/*
 * このプログラムはRISC-VのDebianのVisionFive2のI2Cテスト用に作ったものです。
 *
 * 表示に128x64のSSD1306のOLEDを使います。
 * 文字表示にlibfreetypeを使います。
 *
 * プログラム中で#define I2C_DEVICE "/dev/i2c-0"のようにデバイスを指定してます。
 * SSD1306のI2Cのデバイスを調べたい場合はi2cdetectを使います。
 * VisionFive2のDebianではi2cdetectで調べられないようなので他のLinuxマシンで調べてください。
 * i2cdetectはaptでi2c-toolsを入れると入ります。
 * 他のi2c関連のツールも入るのでi2c-toolsは入れるといいです。
 *
 * i2cdetectのパラメータに指定する番号はI2Cデバイスの番号になります。
 * /dev/i2c-0なら0を/dev/i2c-1なら1を指定します。
 * 下記のように0x3cが表示されればSSD1306が認識されています。
 * $ i2cdetect 0
 * WARNING! This program can confuse your I2C bus, cause data loss and worse!
 * I will probe file /dev/i2c-2.
 * I will probe address range 0x08-0x77.
 * Continue? [Y/n] y
 *      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
 * 00:                         -- -- -- -- -- -- -- --
 * 10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- --
 * 40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 70: -- -- -- -- -- -- 76 --
 *
 * このプログラムを実行するにはlibfreetype-devをインストールしてください。
 * sudo apt install libfreetype-dev
 *
 *
 * コンパイル方法(必ず、gccではなく、g++でコンパイルしてください)
 *
 * $ g++ -O2 -I/usr/include/freetype2 -c -o yama_ssd1306_library.o yama_ssd1306_library.cc
 * $ g++ -O2 -I/usr/include/freetype2 -o i2ctest_ssd1306 i2ctest_ssd1306.cc yama_ssd1306_library.o -lfreetype
 *
 *
 * 実行方法
 *
 * $ ./i2ctest_ssd1306
 *
 * (ここで、実行する前に自分が使ってるユーザがi2cグループに所属してるか確認してください。
 * $ cat /etc/group | grep i2c
 * i2cグループに所属してなければ次のようにi2cグループに所属させます。
 * $ sudo gpasswd -a 自分のユーザ名 i2c
 * )
 *
 * 120行目あたりのssd1306_set_font()の部分で日本語フォントを指定すれば日本語の表示も可能です。
 * 日本語はssd1306_word_print()ではなく、ssd1306_print()で表示してください。
 * 
 * VisionFive2とSSD1306 OLEDの接続はこんな感じです
 *
 * ピンはGPIO57 ピンヘッダの5番ピンが/dev/i2c-1のSCL
 * ピンはGPIO58 ピンヘッダの3番ピンが/dev/i2c-1のSDA
 *
 *
 * (下記のように実行するとピンの利用状況が表示されるようです。)
 * $ sudo cat /sys/kernel/debug/pinctrl/13040000.gpio/pinmux-pins
 * 
 */

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <csignal>


#include "yama_ssd1306_library.h"


#define I2C_DEVICE             "/dev/i2c-0"
#define SSD1306_I2C_ADDRESS    0x3c


int i2c_fd01;
FT_Library ssd1306_ft_library01;
FT_Face    ssd1306_ft_face01;

void sigint_handler(int sig);
void loop(); 
void delay(long milli_sec);


int main(int argc, char *argv[]) {

    // SIGINTハンドラの設定
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal SIGINT");
        exit(-1);
    }

    // I2Cをオープン
    if ((i2c_fd01 = i2c_open(I2C_DEVICE)) < 0) {
        return -1;
    }

    // SSD1306 OLEDの初期化
    ssd1306_oled_setup(i2c_fd01, SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);

    // ローテーションを0に設定
    ssd1306_set_rotation(0);
    // 画面を黒に塗りつぶす
    ssd1306_fillRect(0, 0, ssd1306_width, ssd1306_height, SSD1306_BLACK);
    // 描画した内容をSSD1306 OLEDに表示(ssd1306_display()を必ず実行しないとSSD1306 OLEDには反映されません)
    ssd1306_display();

    // libfreetypeの初期化
    int ret;
    if ((ret = init_ssd1306_freetype(&ssd1306_ft_library01)) != 0) {
        perror("main init_ssd1306_freetype");
        ssd1306_close();
        i2c_close(i2c_fd01);
	exit(-1);
    }

    // フォント設定
    if ((ret = ssd1306_set_font(&ssd1306_ft_library01, &ssd1306_ft_face01, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) != 0) {
        perror("main ssd1306_set_font /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
	ssd1306_close();
        i2c_close(i2c_fd01);
	exit(-1);
    }

    // フォントサイズと改行時の高さの設定
    ssd1306_set_font_size(&ssd1306_ft_face01, 22, 300, 300);
    ssd1306_set_font_line_height(20);




    for (int i = 0; i < 1000; i++) {
        loop();
    }




    ssd1306_close();
    i2c_close(i2c_fd01);

    return 0;
}


void random_circle_loop(int switch01);
extern const uint8_t raspberry_bitmap_mask01[];
extern const uint8_t raspberry_bitmap01[];

void loop() {

    ssd1306_set_rotation(0);

    // 画面を黒に塗りつぶす
    ssd1306_fillRect(0, 0, ssd1306_width, ssd1306_height, SSD1306_BLACK);
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();

    ssd1306_set_font_size(&ssd1306_ft_face01, 22, 300, 300);
    ssd1306_set_font_line_height(20);
    ssd1306_set_font_pos(0, 14);
    // 文字の表示
    ssd1306_word_print(&ssd1306_ft_face01, "VisionFive2\nHello World!!\nSSD1306\n");
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();
    delay(5000);


    // 画面を黒に塗りつぶす
    ssd1306_fillRect(0, 0, ssd1306_width, ssd1306_height, SSD1306_BLACK);
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();

    for (int j = 0; j < ssd1306_height / 32; j++) {
        for (int i = 0; i < ssd1306_width / 32; i++) {
            // 矩形を表示
            ssd1306_fillRect(i * 32 + 2, j * 32 + 2, 28, 28, SSD1306_WHITE);
        }
    }
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();
    delay(5000);


    // 画面を黒に塗りつぶす
    ssd1306_fillRect(0, 0, ssd1306_width, ssd1306_height, SSD1306_BLACK);
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();

    for (int j = 0; j < ssd1306_height / 32; j++) {
        for (int i = 0; i < ssd1306_width / 32; i++) {
            // 円を表示
            ssd1306_fillCircle(i * 32 + 16, j * 32 + 16, 16, SSD1306_WHITE);
        }
    }
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();
    delay(5000);


    // 画面を黒に塗りつぶす
    ssd1306_fillRect(0, 0, ssd1306_width, ssd1306_height, SSD1306_BLACK);
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();

    for (int j = 0; j < ssd1306_height / 32; j++) {
        for (int i = 0; i < ssd1306_width / 32; i++) {
            // bitmapを表示
            ssd1306_drawBitmap_mask(i * 32, j * 32, raspberry_bitmap01, raspberry_bitmap_mask01, 32, 32);
        }
    }
    // 描画した内容をSSD1306 OLEDに反映させる
    ssd1306_display();
    delay(5000);


    

    ssd1306_set_font_size(&ssd1306_ft_face01, 22, 300, 300);
    ssd1306_set_font_line_height(20);
    random_circle_loop(1); 




    random_circle_loop(0);
}



//  maskデータ (32x32ピクセルのmaskデータです。1ピクセル1ビットです。)
//  ピクセル列の最初がMSBから始まるデータ形式(0が透明、1が描画)
const uint8_t raspberry_bitmap_mask01[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x7c,0x3e,0x00,0x01,0xfe,0x7f,0x80,0x01,0xff,0xff,0x80,0x01,0xff,0xff,0x80,
    0x01,0xff,0xff,0x80,0x01,0xff,0xff,0x80,0x00,0xff,0xff,0x00,0x00,0x7f,0xfe,0x00,
    0x00,0x7f,0xfe,0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x01,0xff,0xff,0x80,
    0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,
    0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,0x01,0xff,0xff,0x80,0x01,0xff,0xff,0x80,
    0x01,0xff,0xff,0x80,0x00,0xff,0xff,0x00,0x00,0x7f,0xfe,0x00,0x00,0x3f,0xfc,0x00,
    0x00,0x0f,0xf0,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

//  画像データ (32x32ピクセルのマスクデータです。1ピクセル1ビットです。)
//  ピクセル列の最初がMSBから始まるデータ形式
const uint8_t raspberry_bitmap01[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x7c,0x3e,0x00,0x01,0xfe,0x7f,0x80,0x01,0xef,0xf7,0x80,0x01,0x83,0xc1,0x80,
    0x01,0x8b,0xd1,0x80,0x01,0xc7,0xe3,0x80,0x00,0xe7,0xe7,0x00,0x00,0x7f,0xfe,0x00,
    0x00,0x7e,0x7e,0x00,0x00,0xcc,0x33,0x00,0x00,0xde,0x7b,0x00,0x01,0xf3,0xcf,0x80,
    0x03,0xe1,0x87,0xc0,0x03,0x41,0x82,0xc0,0x03,0x41,0x82,0xc0,0x03,0x73,0xce,0xc0,
    0x03,0x7e,0x7e,0xc0,0x03,0xfc,0x3f,0xc0,0x01,0x98,0x19,0x80,0x01,0x8c,0x31,0x80,
    0x01,0xce,0x73,0x80,0x00,0xef,0xf7,0x00,0x00,0x7e,0x7e,0x00,0x00,0x3c,0x3c,0x00,
    0x00,0x0e,0x70,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};






// random_cirleの関数群と変数

void random_circle();
void random_circle_setup01(void);

int random_switch = 0;
void random_circle_loop(int switch01) {
    random_switch = switch01;
    static int r = 0;

    for (int j = 0; j < 4; j++) {
    ssd1306_set_rotation(r);
        random_circle_setup01();

        for (int i = 0; i < 100; i++) {
            random_circle();
        }
        r++;
        if (r >= 4) r = 0;
    }
}

#define RASPBERRY_N  20 
#define RDX           5
#define RDY           5 

int rnd(int max); 
int random_circle_width;
int random_circle_height;

struct {
    int    x;
    int    y;
    int    dx;
    int    dy;
    int    signx;
    int    signy;
    int    r;
    int    c;
} circle01[RASPBERRY_N];

void random_circle_setup01(void) {

    random_circle_width = ssd1306_width;
    random_circle_height = ssd1306_height;

    for (int i = 0; i < RASPBERRY_N; i++) {
        circle01[i].x = rnd(random_circle_width);
        circle01[i].y = rnd(random_circle_height);
        circle01[i].dx = rnd(RDX) + 1;
        circle01[i].r = rnd(10) + 3;
        circle01[i].c = SSD1306_WHITE;
        circle01[i].dy = rnd(RDY) + 1;
        circle01[i].signx = rnd(2) ? 1 : -1;
        circle01[i].signy = rnd(2) ? 1 : -1;
    }
}

void random_circle() {

    int temp1, temp2;
    ssd1306_fillRect(0, 0, ssd1306_width, ssd1306_height, SSD1306_BLACK);

    for (int i = 0; i < RASPBERRY_N; i++ ) {
        temp1 = circle01[i].dx * circle01[i].signx;
        temp2 = circle01[i].x + temp1;
        if (temp2 > random_circle_width) {
            circle01[i].signx = -1;
            circle01[i].dx = rnd(RDX) + 1;
            circle01[i].x = random_circle_width + circle01[i].signx * circle01[i].dx;
        } else if (temp2 < 0 ) {
            circle01[i].signx = 1;
            circle01[i].dx = rnd(RDX) + 1;
            circle01[i].x = 0 + circle01[i].signx * circle01[i].dx;
        } else {
            circle01[i].x = circle01[i].x + temp1;
        }
        temp1 = circle01[i].dy * circle01[i].signy;
        temp2 = circle01[i].y + temp1;
        if (temp2 > random_circle_height) {
            circle01[i].signy = -1;
            circle01[i].dy = rnd(RDY) + 1;
            circle01[i].y = random_circle_height + circle01[i].signy * circle01[i].dy;
        } else if (temp2 < 0 ) {
            circle01[i].signy = 1;
            circle01[i].dy = rnd(RDY) + 1;
            circle01[i].y = 0 + circle01[i].signy * circle01[i].dy;
        } else {
            circle01[i].y = circle01[i].y + temp1;
        }
	if (random_switch) 
             ssd1306_fillCircle(circle01[i].x, circle01[i].y, circle01[i].r, circle01[i].c);
	else
             ssd1306_drawBitmap_mask(circle01[i].x - 16, circle01[i].y - 16, raspberry_bitmap01, raspberry_bitmap_mask01, 32, 32);
    }
    ssd1306_set_font_pos(0, 16);
    ssd1306_word_print(&ssd1306_ft_face01, "VisionFive2");
    ssd1306_display();
    delay(10);
}

int rnd(int max) {
    return (rand() % max);
}




// SIGINT用のハンドラ
void sigint_handler(int sig) {
    i2c_close(i2c_fd01);
    exit(1);
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





