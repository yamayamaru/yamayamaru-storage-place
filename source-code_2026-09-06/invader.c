#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * 架空のグラフィック・入力関数
 *
 * DrawPixel(x, y, color);
 * fillrect(x1, y1, x2, y2, color);
 * draw_image(x, y, data);
 * clear_screen(color);
 * key_pressed(key);
 * wait_vsync();
 */

#define WORD(x)  (*((unsigned short *)((unsigned char *)x)))
#define BYTE(x)  (*((unsigned char  *)((unsigned char *)x)))
#define DWORD(x) (*((unsigned int   *)((unsigned char *)x)))

char work01[1536];

int ginit(char *workaddr01, int worksize01);
void fillrect(int x1, int y1, int x2, int y2, int color);
void draw_image(int x, int y, unsigned char *data, int color);
void clear_screen(int color);
void wait_vsync();
void screenterm(char *workaddr);
int key_pressed(int key);
int read_key();
void reset_key();
void set_palette(int pal_num01, int r01, int g01, int b01);
void draw_string(int x, int y, char *str1);
void set_color(int color01);
void set_bgcolor(int color01);
void set_vram_write_page(int page);
void set_vram_view_page(int page);
void draw_text(int x, int y, const char *s, int color);
void draw_number(int x, int y, int number, int color);
void draw_text_expansion(int x, int y, const char *s, int color, int size);
void draw_pixel_vram_16bit_02_fast(int x, int y, int color);
void draw_pixel_vram_16bit_02_fast2(int x, int y, int color);
void DrawPixel_bios(int x, int y, int color);
void view_sprite();
void end_sprite();
int set_sprite();

#define DrawPixel_fast draw_pixel_vram_16bit_02_fast
#define DrawPixel draw_pixel_vram_16bit_02

/* 画面サイズ */
#define SCREEN_W 256
#define SCREEN_H 240
#define VRAM_SCREEN_WIDTH 256

/* 色 */
#define BLACK   0
#define WHITE   0x7fff
#define RED     0x03e0
#define GREEN   0x7c00
#define YELLOW  0x7fe0
#define CYAN    0x7c1f
#define PURPLE  0x03ff

/* キー */
#define KEY_LEFT   29
#define KEY_RIGHT  28
#define KEY_SPACE  0x20
#define KEY_ESC    0x1b

/* 敵 */
#define ENEMY_ROWS  5
#define ENEMY_COLS  8
#define ENEMY_W     16
#define ENEMY_H     10

/* 弾 */
#define MAX_ENEMY_BULLETS 8

#define BARRIER_COUNT 4
#define BARRIER_W     28
#define BARRIER_H     16

#define SPRITE_NUM ENEMY_ROWS*ENEMY_COLS

struct Bullet01 {
    int x;
    int y;
    int active;
};

struct Enemy01 {
    int alive;
};

/* -------------------------------------------------- */
/* ゲームの状態                                       */
/* -------------------------------------------------- */

static int player_x;
static int player_y;

static struct Bullet01 player_bullet;
static struct Bullet01 enemy_bullet[MAX_ENEMY_BULLETS];

static struct Enemy01  enemies[ENEMY_ROWS][ENEMY_COLS];

static int enemy_x;
static int enemy_y;
static int enemy_dir;

static int enemy_move_counter;
static int enemy_move_delay;

static int score;
static int game_over;

static int player_lives;

static unsigned char barriers[BARRIER_COUNT][BARRIER_W * BARRIER_H];

/* -------------------------------------------------- */
/* 簡単な画像データ                                   */
/* 実際の draw_image() の形式は環境依存という想定      */
/* -------------------------------------------------- */
/*
static char player_image[] = {
    "    XX    "
    "   XXXX   "
    "XXXXXXXXXX"
    "XXXXXXXXXX"
};

static char enemy_image1[] = {
    "  XX  XX  "
    " XXXXXXXX "
    "XXXXXXXXXX"
    "XX XXXX XX"
    "XX      XX"
};

static char enemy_image2[] = {
    "   XXXX   "
    " XXXXXXXX "
    "XXXXXXXXXX"
    "XX XXXX XX"
    "  XX  XX  "
};
*/

/* draw_image()用データ
/* 16x16 = 256 bytes
 *
 * 0 = transparent
 * 1 = sprite color
 */

/* 自機 */

/* 敵キャラクタ2 */
static unsigned char enemy_image2[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff, 0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,

    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,

    0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,
    0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff, 0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,

    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,

    0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00, 0x00,0x00,0xff,0xff,0xff,0x00,0x00,0x00,
    0x00,0x00,0x00,0xff,0xff,0xff,0x00,0x00, 0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00, 0x00,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff, 0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static unsigned char player_image[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff, 0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff, 0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff, 0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,
    0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,

    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,

    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,
    0x00,0x00,0x00,0xff,0xff,0x00,0xff,0xff, 0xff,0xff,0x00,0xff,0xff,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


/* 敵キャラクタ1 */
static unsigned char enemy_image1[] = {
    0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00, 0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
    0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,
    0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,

    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,

    0xff,0xff,0x00,0x00,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,
    0xff,0xff,0x00,0x00,0x00,0xff,0xff,0xff, 0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,

    0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,
    0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,

    0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00,
    0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00, 0x00,0x00,0x00,0xff,0xff,0x00,0x00,0x00,

    0x00,0x00,0x00,0x00,0xff,0xff,0x00,0x00, 0x00,0x00,0xff,0xff,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x00, 0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

/* スプライト用データ */
short spr_ptn01[] = {
    0x0000,0x0000,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x0000, 0x0000,0x03ff,0x03ff,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x03ff,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x0000,0x0000,
    0x0000,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x0000,

    0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,
    0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,
    0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,0x03ff,

    0x03ff,0x03ff,0x0000,0x0000,0x03ff,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x03ff,0x0000,0x0000,0x03ff,0x03ff,
    0x03ff,0x03ff,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x03ff, 0x03ff,0x03ff,0x03ff,0x0000,0x0000,0x03ff,0x03ff,0x03ff,

    0x03ff,0x03ff,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x03ff,
    0x0000,0x03ff,0x03ff,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x03ff,0x0000,

    0x0000,0x0000,0x03ff,0x03ff,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x03ff,0x03ff,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x03ff,0x03ff,0x0000,0x0000,0x0000,

    0x0000,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x0000,0x0000, 0x0000,0x0000,0x03ff,0x03ff,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x03ff,0x03ff,0x0000, 0x0000,0x03ff,0x03ff,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

short spr_ptn02[] = {
    0x0000,0x0000,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x0000, 0x0000,0x7fff,0x7fff,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x7fff,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x0000,0x0000,
    0x0000,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x0000,

    0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,
    0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,
    0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,0x7fff,

    0x7fff,0x7fff,0x0000,0x0000,0x7fff,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x7fff,0x0000,0x0000,0x7fff,0x7fff,
    0x7fff,0x7fff,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x7fff, 0x7fff,0x7fff,0x7fff,0x0000,0x0000,0x7fff,0x7fff,0x7fff,

    0x7fff,0x7fff,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x7fff,
    0x0000,0x7fff,0x7fff,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x7fff,0x0000,

    0x0000,0x0000,0x7fff,0x7fff,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x7fff,0x7fff,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x7fff,0x7fff,0x0000,0x0000,0x0000,

    0x0000,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x0000,0x0000, 0x0000,0x0000,0x7fff,0x7fff,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x7fff,0x7fff,0x0000, 0x0000,0x7fff,0x7fff,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

short spr_ptn03[] = {
    0x0000,0x0000,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x0000, 0x0000,0x03e0,0x03e0,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x03e0,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x0000,0x0000,
    0x0000,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x0000,

    0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,
    0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,
    0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,0x03e0,

    0x03e0,0x03e0,0x0000,0x0000,0x03e0,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x03e0,0x0000,0x0000,0x03e0,0x03e0,
    0x03e0,0x03e0,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x03e0, 0x03e0,0x03e0,0x03e0,0x0000,0x0000,0x03e0,0x03e0,0x03e0,

    0x03e0,0x03e0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x03e0,
    0x0000,0x03e0,0x03e0,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x03e0,0x0000,

    0x0000,0x0000,0x03e0,0x03e0,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x03e0,0x03e0,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x03e0,0x03e0,0x0000,0x0000,0x0000,

    0x0000,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x0000,0x0000, 0x0000,0x0000,0x03e0,0x03e0,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x03e0,0x03e0,0x0000, 0x0000,0x03e0,0x03e0,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

short spr_ptn04[] = {
    0x0000,0x0000,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x0000, 0x0000,0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x0000,0x0000,
    0x0000,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x0000,

    0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,
    0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,
    0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x7fe0,

    0x7fe0,0x7fe0,0x0000,0x0000,0x7fe0,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x7fe0,0x0000,0x0000,0x7fe0,0x7fe0,
    0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x7fe0, 0x7fe0,0x7fe0,0x7fe0,0x0000,0x0000,0x7fe0,0x7fe0,0x7fe0,

    0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x7fe0,
    0x0000,0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x7fe0,0x0000,

    0x0000,0x0000,0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x0000,0x0000,0x0000,

    0x0000,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x0000,0x0000, 0x0000,0x0000,0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x7fe0,0x7fe0,0x0000, 0x0000,0x7fe0,0x7fe0,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

short spr_ptn05[] = {
    0x0000,0x0000,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x0000, 0x0000,0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x0000,0x0000,
    0x0000,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x0000,

    0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,
    0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,
    0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x7c1f,

    0x7c1f,0x7c1f,0x0000,0x0000,0x7c1f,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x7c1f,0x0000,0x0000,0x7c1f,0x7c1f,
    0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x7c1f, 0x7c1f,0x7c1f,0x7c1f,0x0000,0x0000,0x7c1f,0x7c1f,0x7c1f,

    0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x7c1f,
    0x0000,0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x7c1f,0x0000,

    0x0000,0x0000,0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x0000,0x0000,0x0000,

    0x0000,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x0000,0x0000, 0x0000,0x0000,0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x7c1f,0x7c1f,0x0000, 0x0000,0x7c1f,0x7c1f,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

/* -------------------------------------------------- */

void init_barriers(void);

void init_game(void) {
    int x, y;

    player_x = SCREEN_W / 2 - 8;
    player_y = SCREEN_H - 20;

    player_lives = 3;       /* 残機3機 */

    player_bullet.active = 0;

    for (x = 0; x < MAX_ENEMY_BULLETS; x++) {
        enemy_bullet[x].active = 0;
    }

    for (y = 0; y < ENEMY_ROWS; y++) {
        for (x = 0; x < ENEMY_COLS; x++) {
            enemies[y][x].alive = 1;
        }
    }

    init_barriers();

    enemy_x = 30;
    enemy_y = 30;

    enemy_dir = 1;

    enemy_move_counter = 0;
    enemy_move_delay = 20;

    score = 0;
    game_over = 0;
}

/* -------------------------------------------------- */
/* プレイヤー                                         */
/* -------------------------------------------------- */

void move_player(void)
{
    if (key_pressed(KEY_LEFT)) {
        player_x -= 2;

        if (player_x < 0)
            player_x = 0;
    }

    if (key_pressed(KEY_RIGHT)) {
        player_x += 2;

        if (player_x > SCREEN_W - 16)
            player_x = SCREEN_W - 16;
    }

    if (key_pressed(KEY_SPACE)) {

        if (!player_bullet.active) {
            player_bullet.x = player_x + 7;
            player_bullet.y = player_y - 4;
            player_bullet.active = 1;
        }
    }
}

/* -------------------------------------------------- */
/* プレイヤーの弾                                     */
/* -------------------------------------------------- */

void move_player_bullet(void)
{
    if (!player_bullet.active)
        return;

    player_bullet.y -= 4;

    if (player_bullet.y < 0) {
        player_bullet.active = 0;
    }
}

/* -------------------------------------------------- */
/* 敵の座標                                           */
/* -------------------------------------------------- */

void get_enemy_pos(int row, int col, int *x, int *y)
{
    *x = enemy_x + col * 24;
    *y = enemy_y + row * 18;
}

/* -------------------------------------------------- */
/* 敵移動                                             */
/* -------------------------------------------------- */

void move_enemies(void)
{
    int row, col;
    int ex, ey;
    int left = SCREEN_W;
    int right = 0;

    enemy_move_counter++;

    if (enemy_move_counter < enemy_move_delay)
        return;

    enemy_move_counter = 0;

    /*
     * 生きている敵の左右端を調べる
     */

    for (row = 0; row < ENEMY_ROWS; row++) {

        for (col = 0; col < ENEMY_COLS; col++) {

            if (!enemies[row][col].alive)
                continue;

            get_enemy_pos(row, col, &ex, &ey);

            if (ex < left)
                left = ex;

            if (ex + ENEMY_W > right)
                right = ex + ENEMY_W;
        }
    }

    /*
     * 画面端まで行ったら下へ
     */

    if ((enemy_dir > 0 && right >= SCREEN_W - 4) ||
        (enemy_dir < 0 && left <= 4)) {

        enemy_dir = -enemy_dir;
        enemy_y += 8;
    }
    else {
        enemy_x += enemy_dir * 4;
    }
}

/* -------------------------------------------------- */
/* 敵弾発射                                           */
/* -------------------------------------------------- */

void enemy_fire(void)
{
    int i;
    int row, col;
    int ex, ey;

    /*
     * 適当な確率で発射
     */

    if ((rand() % 100) > 3)
        return;

    /*
     * 空いている弾スロットを探す
     */

    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {

        if (!enemy_bullet[i].active)
            break;
    }

    if (i == MAX_ENEMY_BULLETS)
        return;

    /*
     * ランダムな列を選ぶ
     */

    col = rand() % ENEMY_COLS;

    /*
     * その列で一番下にいる敵を探す
     */

    for (row = ENEMY_ROWS - 1; row >= 0; row--) {

        if (enemies[row][col].alive) {

            get_enemy_pos(row, col, &ex, &ey);

            enemy_bullet[i].x = ex + ENEMY_W / 2;
            enemy_bullet[i].y = ey + ENEMY_H;
            enemy_bullet[i].active = 1;

            return;
        }
    }
}

/* -------------------------------------------------- */

void move_enemy_bullets(void)
{
    int i;

    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {

        if (!enemy_bullet[i].active)
            continue;

        enemy_bullet[i].y += 2;

        if (enemy_bullet[i].y >= SCREEN_H) {
            enemy_bullet[i].active = 0;
        }
    }
}

/* -------------------------------------------------- */
/* 当たり判定                                         */
/* -------------------------------------------------- */

int hit_rect(
    int ax1, int ay1, int ax2, int ay2,
    int bx1, int by1, int bx2, int by2)
{
    if (ax2 < bx1)
        return 0;

    if (ax1 > bx2)
        return 0;

    if (ay2 < by1)
        return 0;

    if (ay1 > by2)
        return 0;

    return 1;
}

/* -------------------------------------------------- */
/* プレイヤー弾 vs 敵                                 */
/* -------------------------------------------------- */

void check_player_bullet_hit(void)
{
    int row, col;
    int ex, ey;

    if (!player_bullet.active)
        return;

    for (row = 0; row < ENEMY_ROWS; row++) {

        for (col = 0; col < ENEMY_COLS; col++) {

            if (!enemies[row][col].alive)
                continue;

            get_enemy_pos(row, col, &ex, &ey);

            if (hit_rect(
                    player_bullet.x,
                    player_bullet.y,
                    player_bullet.x + 1,
                    player_bullet.y + 4,

                    ex,
                    ey,
                    ex + ENEMY_W,
                    ey + ENEMY_H)) {

                enemies[row][col].alive = 0;
                player_bullet.active = 0;

                score += 10;

                /*
                 * 敵が少なくなるほど速くする
                 */
                if (enemy_move_delay > 3)
                    enemy_move_delay--;

                return;
            }
        }
    }
}

/* -------------------------------------------------- */
/* 敵弾 vs プレイヤー                                 */
/* -------------------------------------------------- */

void check_enemy_bullet_hit(void)
{
    int i;
    int j;

    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {

        if (!enemy_bullet[i].active)
            continue;

        if (hit_rect(
                enemy_bullet[i].x,
                enemy_bullet[i].y,
                enemy_bullet[i].x + 2,
                enemy_bullet[i].y + 5,

                player_x,
                player_y,
                player_x + 16,
                player_y + 8)) {

            enemy_bullet[i].active = 0;
            
            /* 1機失う */
            player_lives--;

            if (player_lives <= 0) {
                game_over = 1;
                return;
            }

            /*
             * 自機を初期位置に戻す
             */
            player_x = SCREEN_W / 2 - 8;
            player_y = SCREEN_H - 20;

            /*
             * プレイヤーの弾を消す
             */
            player_bullet.active = 0;

            /*
             * 敵の弾も全部消す
             */
            for (j = 0; j < MAX_ENEMY_BULLETS; j++)
                enemy_bullet[j].active = 0;

            return;
        }
    }
}

/* -------------------------------------------------- */
/* 敵が下まで来たか                                   */
/* -------------------------------------------------- */

void check_enemy_bottom(void)
{
    int row, col;
    int ex, ey;

    for (row = 0; row < ENEMY_ROWS; row++) {

        for (col = 0; col < ENEMY_COLS; col++) {

            if (!enemies[row][col].alive)
                continue;

            get_enemy_pos(row, col, &ex, &ey);

            if (ey + ENEMY_H >= player_y) {
                game_over = 1;
                return;
            }
        }
    }
}

/* -------------------------------------------------- */
/* 全滅判定                                           */
/* -------------------------------------------------- */

int all_enemies_dead(void)
{
    int row, col;

    for (row = 0; row < ENEMY_ROWS; row++) {

        for (col = 0; col < ENEMY_COLS; col++) {

            if (enemies[row][col].alive)
                return 0;
        }
    }

    return 1;
}

/* -------------------------------------------------- */
/* 残機表示                                           */
/* -------------------------------------------------- */

void draw_lives(void)
{
    int i;
    int x, y;

    /*
     * 現在操作している1機を除いた予備機を表示
     */

    for (i = 0; i < player_lives - 1; i++) {
        x = 8 + i * 20;
        y = SCREEN_H - 18;
        
        fillrect(
            x,
            y + 3,
            x + 15,
            y + 7,
            GREEN);

        fillrect(
            x + 6,
            y,
            x + 9,
            y + 3,
            GREEN);
/*
        draw_image(
            x,
            y,
            player_image
        );
*/
    }

}


/* -------------------------------------------------- */
/* 描画                                               */
/* -------------------------------------------------- */

void draw_player(void)
{
    /*
     * draw_image() の代わりに fillrect() だけでも構わない
     */

    fillrect(
        player_x,
        player_y + 3,
        player_x + 15,
        player_y + 7,
        GREEN);

    fillrect(
        player_x + 6,
        player_y,
        player_x + 9,
        player_y + 3,
        GREEN);

/*    draw_image(player_x, player_y, player_image, GREEN); */
}

/* -------------------------------------------------- */

void draw_enemies(void)
{
    int row, col;
    int ex, ey;

    for (row = 0; row < ENEMY_ROWS; row++) {

        for (col = 0; col < ENEMY_COLS; col++) {

            if (!enemies[row][col].alive) {
                SPR_setPosition(0, 1023-(ENEMY_COLS * row + col), 1, 1, 0, -16);
                continue;
            }

            get_enemy_pos(row, col, &ex, &ey);

            /*
             * 本来なら
             *
             * draw_image(ex, ey, enemy_image1);
             *
             * などで描画してもよい
             */
             
            /* fillrect()で敵キャラクタを描画 */
/*
            if ((row & 7) == 0) {

                fillrect(
                    ex + 2,
                    ey,
                    ex + 13,
                    ey + 2,
                    PURPLE);

                fillrect(
                    ex,
                    ey + 3,
                    ex + 15,
                    ey + 7,
                    PURPLE);
            }
            else if ((row & 7) == 1) {

                fillrect(
                    ex + 4,
                    ey,
                    ex + 11,
                    ey + 2,
                    WHITE);

                fillrect(
                    ex + 1,
                    ey + 3,
                    ex + 14,
                    ey + 7,
                    WHITE);
            }
            else if ((row & 7) == 2) {

                fillrect(
                    ex + 4,
                    ey,
                    ex + 11,
                    ey + 2,
                    RED);

                fillrect(
                    ex + 1,
                    ey + 3,
                    ex + 14,
                    ey + 7,
                    RED);
            }
            else if ((row & 7) == 3) {

                fillrect(
                    ex + 4,
                    ey,
                    ex + 11,
                    ey + 2,
                    YELLOW);

                fillrect(
                    ex + 1,
                    ey + 3,
                    ex + 14,
                    ey + 7,
                    YELLOW);
            }
            else if ((row & 7) == 4) {

                fillrect(
                    ex + 4,
                    ey,
                    ex + 11,
                    ey + 2,
                    CYAN);

                fillrect(
                    ex + 1,
                    ey + 3,
                    ex + 14,
                    ey + 7,
                    CYAN);
            }
*/
            /* draw_image()で敵キャラクタを描画 */
/*
            if ((row & 7) == 0) {
                draw_image(ex, ey, enemy_image2, PURPLE);
            }
            else if ((row & 7) == 1) {
                draw_image(ex, ey, enemy_image2, WHITE);
            }
            else if ((row & 7) == 2) {
                draw_image(ex, ey, enemy_image2, RED);
            }
            else if ((row & 7) == 3) {
                draw_image(ex, ey, enemy_image2, YELLOW);
            }
            else if ((row & 7) == 4) {
                draw_image(ex, ey, enemy_image2, CYAN);
            }
*/

            /* スプライトで敵キャラクタを描画 */
            SPR_setPosition(0, 1023-(ENEMY_COLS * row + col), 1, 1, ex, ey);
        }
    }
}

/* -------------------------------------------------- */

void draw_bullets(void)
{
    int i;

    if (player_bullet.active) {

        fillrect(
            player_bullet.x,
            player_bullet.y,
            player_bullet.x + 1,
            player_bullet.y + 4,
            WHITE);
    }

    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {

        if (!enemy_bullet[i].active)
            continue;

        fillrect(
            enemy_bullet[i].x,
            enemy_bullet[i].y,
            enemy_bullet[i].x + 1,
            enemy_bullet[i].y + 4,
            RED);
    }
}

/* -------------------------------------------------- */
/* 防御壁初期化                                       */
/* -------------------------------------------------- */

void init_barriers(void)
{
    int i;
    int x, y;

    for (i = 0; i < BARRIER_COUNT; i++) {

        for (y = 0; y < BARRIER_H; y++) {
            for (x = 0; x < BARRIER_W; x++) {

                /*
                 * 下側中央を切り欠く
                 * インベーダー風のバリアー
                 */
                if (y >= 10 &&
                    x >= 8 &&
                    x < 20) {
                    barriers[i][y * BARRIER_W + x] = 0;
                }
                else {
                    barriers[i][y * BARRIER_W + x] = 1;
                }
            }
        }
    }
}

/* -------------------------------------------------- */
/* 防御壁当たり判定                                             */
/* -------------------------------------------------- */

void check_barrier_hit(void)
{
    int i;
    int x, y;
    int bx, by;

    /*
     * プレイヤーの弾
     */
    if (player_bullet.active) {

        for (i = 0; i < BARRIER_COUNT; i++) {

/*            bx = 35 + i * 70;  */
            bx = 28 + i * 56;
            by = 185;

            x = player_bullet.x - bx;
            y = player_bullet.y - by;

            if (x >= 0 && x < BARRIER_W &&
                y >= 0 && y < BARRIER_H) {

                if (barriers[i][y * BARRIER_W + x]) {

                    /*
                     * 弾が当たった部分を破壊
                     */
                    barriers[i][y * BARRIER_W + x] = 0;
                    barriers[i][(y + 1) * BARRIER_W + x] = 0;
                    barriers[i][(y + 2) * BARRIER_W + x] = 0;
                    barriers[i][(y - 1) * BARRIER_W + x] = 0;
                    barriers[i][(y - 2) * BARRIER_W + x] = 0;

                    player_bullet.active = 0;

                    return;
                }
            }
        }
    }
}

void check_enemy_bullet_barrier_hit(void)
{
    int i;
    int b;
    int x, y;
    int bx, by;

    for (i = 0; i < MAX_ENEMY_BULLETS; i++) {

        if (!enemy_bullet[i].active)
            continue;

        for (b = 0; b < BARRIER_COUNT; b++) {

/*            bx = 35 + b * 70; */
            bx = 28 + b * 56;
            by = 185;

            x = enemy_bullet[i].x - bx;
            y = enemy_bullet[i].y - by;

            if (x >= 0 && x < BARRIER_W &&
                y >= 0 && y < BARRIER_H) {

                if (barriers[b][y * BARRIER_W + x]) {

                    barriers[b][y * BARRIER_W + x] = 0;
                    barriers[b][(y-1) * BARRIER_W + x] = 0;

                    enemy_bullet[i].active = 0;

                    return;
                }
            }
        }
    }
}

/* -------------------------------------------------- */
/* 防御壁                                             */
/* -------------------------------------------------- */

/* 防御壁をただ描画しているだけのバージョン */
/*
void draw_barriers(void)
{
    int i;
    int x;

    for (i = 0; i < 4; i++) {

        x = 35 + i * 70;

        fillrect(
            x,
            185,
            x + 30,
            195,
            GREEN);

        fillrect(
            x,
            195,
            x + 8,
            201,
            GREEN);

        fillrect(
            x + 22,
            195,
            x + 30,
            201,
            GREEN);
    }
}
*/

/* 防御壁に弾が当たると削れるバージョン */
void draw_barriers(void)
{
    int i;
    int x, y;
    int bx;

    for (i = 0; i < BARRIER_COUNT; i++) {

/*        bx = 35 + i * 70;  */
        bx = 28 + i * 56;

        for (y = 0; y < BARRIER_H; y++) {
            for (x = 0; x < BARRIER_W; x++) {

                if (barriers[i][y * BARRIER_W + x]) {
/*                    DrawPixel(   */
                    DrawPixel_fast(
                        bx + x,
                        185 + y,
                        GREEN
                    );
                }
            }
        }
    }
}

/* -------------------------------------------------- */

char str1[20];
void draw_game(void)
{
    static vram_page01 = 1;

    vram_page01 = (vram_page01 + 1) & 1;
    set_vram_write_page(vram_page01);

    clear_screen(0x8000);

    draw_enemies();
    draw_barriers();
    draw_player();
    draw_bullets();

    draw_lives();
    
    /*
     * 文字描画関数があるなら
     *
     * draw_text(4, 4, "SCORE", WHITE);
     *
     * などを追加できる
     */

/*
     sprintf(str1, "SCORE %6d", score);
     set_color(WHITE);
     set_vram_view_page(vram_page01);
     draw_string(4, 14, str1);
*/

     draw_text(4, 4, "SCORE ", WHITE);
     draw_number(60, 4, score, WHITE);
     set_vram_view_page(vram_page01);
}

/* -------------------------------------------------- */
/* GAMEOVERの表示                                     */
/* -------------------------------------------------- */

int draw_gameover(void) {
    int key;
    time_t time01;
    int i;
    int ret01;
    int color;
    
    key = 0xffff;
    i = 0;
    ret01 = 0;
/*    while (key == 0xffff) { */
    while (key == 0xffff) {        /* 何も入力されない時は0xfffff */
        key = read_key();
        if (key_pressed(KEY_ESC)) ret01 = 1;
        color = (((i % 7 + 1) & 4) ? (31 << 10) : 0) | (((i % 7 + 1) & 2) ? (31 << 5) : 0) | (((i % 7 + 1) & 1) ? (31) : 0);
        draw_text_expansion((SCREEN_W-(9*8)*3)-16, 96, "GAMEOVER", color, 3);
        
        time01 = time(NULL);
        while ((time(NULL) - time01) < 1);
        i++;
        if (i > 100000000) i = 0;
    }
    
    return ret01;
}

/* -------------------------------------------------- */
/* メイン                                             */
/* -------------------------------------------------- */

int main(void)
{
    int i;
    int ret01;
    int end01;
    
    if ((ret01 = ginit((char *)work01, 256*1024)) != 0) {
         puts("ginit failed!");
         exit(1);
    }

    KYB_init();
    KYB_clic(1);

	for (i = 0; i < 8; i++) {
	    set_palette(i, (i & 4) ? 255 : 0, (i & 2) ? 255 : 0, (i & 1) ? 255 : 0);
	}
    set_screen_ratio(2, 2);
    clear_screen(0);

    set_sprite();

    end01 = 0;
    

    while (end01 == 0) {
    clear_screen(0);

    init_game();

    view_sprite();

    while (!game_over) {

        /*
         * 入力
         */
        read_key();
        move_player();

        if (key_pressed(KEY_ESC)) {
            end01 = 1;
            break;
        }
        reset_key();
        
        /*
         * ゲーム処理
         */
        move_player_bullet();

        move_enemies();

        enemy_fire();
        move_enemy_bullets();

        check_barrier_hit();
        check_enemy_bullet_barrier_hit();

        check_player_bullet_hit();
        check_enemy_bullet_hit();
        check_enemy_bottom();

        /*
         * 敵全滅
         */
        if (all_enemies_dead()) {
            break;
        }

        /*
         * 描画
         */
        draw_game();

        /*
         * 1フレーム待つ
         */
        wait_vsync();
    }
        if (end01 == 1) break;
        end01 = draw_gameover();
    }

    clear_screen(BLACK);

    end_sprite();

    screenterm(work01);

    return 0;
}

extern char *workaddr;

int vram_write_page = 0;
int vram_view_page = 0;
int vram_page_x[2] = { 0, 0 };
int vram_page_y[2] = { 0, 240 };

void draw_pixel_vram_16bit_02(int x, int y, int color) {
    int  index;
    if (x < 0 || x >= SCREEN_W) return;
    if (y < 0 || y >= SCREEN_H) return ;

    index = (((y + vram_page_y[vram_write_page]) * VRAM_SCREEN_WIDTH) + ((x + vram_page_x[vram_write_page])) << 1);

    __asm__ volatile (
    		"pushw	%%fs\n\t"
    		"pushw	$0x0104\n\t"
    		"popw	%%fs\n\t"
       		"movl	%0, %%ebx\n\t"
    		"movl	%1, %%eax\n\t"
    		"movw	%%ax, %%fs:(%%ebx)\n\t"
    		"popw	%%fs\n\t"
    		:
    		:"r"(index), "r"(color)
    		:"ax", "bx"
    );
}

void draw_pixel_vram_16bit_02_fast(int x, int y, int color) {
    int index;
    
    index = (((y + vram_page_y[vram_write_page]) * VRAM_SCREEN_WIDTH) + ((x + vram_page_x[vram_write_page])) << 1);
    __asm__ volatile (
    		"pushw	%%fs\n\t"
    		"pushw	$0x0104\n\t"
    		"popw	%%fs\n\t"
       		"movl	%0, %%ebx\n\t"
    		"movl	%1, %%eax\n\t"
    		"movw	%%ax, %%fs:(%%ebx)\n\t"
    		"popw	%%fs\n\t"
    		:
    		:"r"(index), "r"(color)
    		:"ax", "bx"
    );
}

void draw_pixel_vram_16bit_02_fast2(int x, int y, int color) {
    int index;
    index = (((y + vram_page_y[vram_write_page]) * VRAM_SCREEN_WIDTH) + ((x + (320-256)/2 + vram_page_x[vram_write_page])) << 1);
    __asm__ volatile (
    		"pushw	%%fs\n\t"
    		"pushw	$0x0104\n\t"
    		"popw	%%fs\n\t"
       		"movl	%0, %%ebx\n\t"
    		"movl	%1, %%eax\n\t"
    		"movw	%%ax, %%fs:(%%ebx)\n\t"
    		"popw	%%fs\n\t"
    		:
    		:"r"(index), "r"(color)
    		:"ax", "bx"
    );
}

void draw_hline_vram(int x, int y, int width, int color) {
    int index;
    int i;
    if (x < 0 || x >= SCREEN_W) return;
    if ((x + width) >= SCREEN_W) width = SCREEN_W - x;
    if (y < 0 || y >= SCREEN_H) return;
    
    index = (((y + vram_page_y[vram_write_page]) * VRAM_SCREEN_WIDTH) + ((x + vram_page_x[vram_write_page])) << 1);   /* 16bitcolorなので2倍にする */
    __asm__ volatile (
    		"pushw	%%es\n\t"
    		"pushw	$0x0104\n\t"
    		"popw	%%es\n\t"
       		"movl	%0, %%edi\n\t"
    		"movl	%1, %%eax\n\t"
    		"movl	%2, %%ecx\n\t"
    		"loop01_deraw_hline_vram: movl	%%ax, %%es:(%%edi)\n\t"
    		"addl	$2, %%edi\n\t"
    		"loop	loop01_deraw_hline_vram\n\t"
    		"popw	%%es\n\t"
    		:
    		:"r"(index), "r"(color), "r"(width)
    		:"ax", "di", "cx"
    );
}

void set_vram_write_page(int page) {
    if (page >= 2 || page < 0) return;
    vram_write_page = page;
}

void set_vram_view_page(int page) {
    if (page >= 2 || page < 0) return;
    vram_view_page = page;
    EGB_displayStart(workaddr, 1, vram_page_x[vram_view_page], vram_page_y[vram_view_page]);
}

void fillrect(int x1, int y1, int x2, int y2, int color) {
    int xx1, yy1, xx2, yy2, i, j;

    if (x1 <= x2) {
        xx1 = x1;
        xx2 = x2;
    } else {
        xx1 = x2;
        xx2 = x1;
    }
    if (y1 <= y2) {
        yy1 = y1;
        yy2 = y2;
    } else {
        yy1 = y2;
        yy2 = y1;
    }

    for (j = yy1; j <= yy2; j++) {
/*
        for (i = xx1; i <= xx2; i++) {
            draw_pixel_vram_16bit_02_fast(i, j, color);
        }
*/
        draw_hline_vram(xx1, j, xx2 - xx1, color);
    }
}

void clear_screen(int color) {
/*    fillrect(0, 0, SCREEN_W-1, SCREEN_H-1, color);  */

    int index, index1;
    int count;
    int i, j;
    int color2;
    union {
        int   i;
        short w[2];
    } color1;
    
    color1.w[0] = color;
    color1.w[1] = color;
    color2 = color1.i;
    index = (((vram_page_y[vram_write_page]) * VRAM_SCREEN_WIDTH) + ((vram_page_x[vram_write_page])) << 1);   /* 16bitcolorなので2倍にする */
    for (j = 0; j < SCREEN_H; j++) {
        index1 = index + (((VRAM_SCREEN_WIDTH) * j) << 1);   /* 16bitcolorなので2倍にする */
        count = VRAM_SCREEN_WIDTH >> 1;                      /* 4バイトごとのfillで16bitカラーなので1/2にする */
        __asm__ volatile (
        		"pushw	%%es\n\t"
    	    	"pushw	$0x0104\n\t"
    		    "popw	%%es\n\t"
          		"movl	%0, %%edi\n\t"
        		"movl	%1, %%eax\n\t"
        		"movl	%2, %%ecx\n\t"
    	    	"loop01_clear_screen: movl	%%eax, %%es:(%%edi)\n\t"
    	    	"addl	$4, %%edi\n\t"
    	    	"loop	loop01_clear_screen\n\t"
    		    "popw	%%es\n\t"
        		:
        		:"r"(index1), "r"(color2), "r"(count)
    	    	:"ax", "di", "cx"
        );
    }
}


char *workaddr;
int  worksize;
int ginit(char *workaddr01, int worksize01) {
    char ret01;
    char para01[64];
    
    workaddr = workaddr01;
    worksize = worksize01;
    
    EGB_init(workaddr, 1536);
    EGB_resolution(workaddr, 0, 5);        /* page 0, 画面モード 5 */
    EGB_resolution(workaddr, 1, 5);        /* page 1, 画面モード 5 */

    EGB_displayPage(workaddr, 0, 3);        /* page 0を前,   page 0, page1を両方表示 */

    /* page 0の設定 */
    EGB_writePage(workaddr, 0);                     /* 書き込みページをpage 0 */
    EGB_color(workaddr, 1, 0x7fff);
    EGB_displayStart(workaddr, 0, 64, 0);           /* 表示開始位置 x = 0, y = 0 */
    EGB_displayStart(workaddr, 1, 0, 0);            /* 仮想画面の移動 x = 0, y = 0 */
    EGB_displayStart(workaddr, 2, 2, 2);            /* 画面の拡大率 横2倍、縦2倍 */
    EGB_displayStart(workaddr, 3, 256, 240);        /* 表示画面の大きさ 256x240 */

    /* page 1の設定 */
    EGB_writePage(workaddr, 1);                     /* 書き込みページをpage 1 */
    EGB_displayStart(workaddr, 0, 64, 0);           /* 表示開始位置 x = 0, y = 0 */
    EGB_displayStart(workaddr, 1, 0, 0);            /* 仮想画面の移動 x = 0, y = 0 */
    EGB_displayStart(workaddr, 2, 2, 2);            /* 画面の拡大率 横2倍、縦2倍 */
    EGB_displayStart(workaddr, 3, 256, 240);        /* 表示画面の大きさ 256x240 */
    EGB_color(workaddr, 1, 0x8000);

    /* page 1の画面消去 */
    EGB_paintMode(workaddr, 0x20);     /* 面塗モード設定 */
    EGB_color(workaddr, 0, 0x8000);    /* 0x8000は透明色 */
    EGB_color(workaddr, 2, 0x8000);    /* 面塗色設定  */

    WORD(para01 + 0) = 0;
    WORD(para01 + 2) = 0;
    WORD(para01 + 4) = 256;
    WORD(para01 + 6) = 240;
    EGB_rectangle(workaddr, para01);
    EGB_color(workaddr, 0, 0x7fff);
    EGB_color(workaddr, 2, 0x7fff);
    EGB_paintMode(workaddr, 0x22);
    
    
    /* page 0の画面消去 */
    EGB_writePage(workaddr, 0);

    EGB_paintMode(workaddr, 0x20);     /* 面塗モード設定 */
    EGB_color(workaddr, 0, 0x8000);    /* 0x8000は透明色 */
    EGB_color(workaddr, 2, 0x8000);    /* 面塗色設定  */

    WORD(para01 + 0) = 0;
    WORD(para01 + 2) = 0;
    WORD(para01 + 4) = 320;
    WORD(para01 + 6) = 240;
    EGB_rectangle(workaddr, para01);
    EGB_color(workaddr, 0, 0x7fff);
    EGB_color(workaddr, 2, 0x7fff);
    EGB_paintMode(workaddr, 0x22);
    
    return 0;
}

/* TBIOSコールによるfillrect */
void fillrect2(int x1, int y1, int x2, int y2, int col)
{
    char para01[64];

    EGB_paintMode(workaddr, 0x20);  /* 面塗モード設定 */
    EGB_color(workaddr, 0, col);
    EGB_color(workaddr, 2, col);    /* 面塗色設定   */

    WORD(para01 + 0) = x1;
    WORD(para01 + 2) = y1;
    WORD(para01 + 4) = x2;
    WORD(para01 + 6) = y2;
    EGB_rectangle(workaddr, para01);

    EGB_color(workaddr, 0, 0x7fff);
    EGB_color(workaddr, 2, 0x7fff);
    EGB_paintMode(workaddr, 0x22);
}

void set_color(int color01) {
    EGB_color(workaddr, 0, color01);
}

void set_bgcolor(int color01) {
    EGB_color(workaddr, 1, color01);
}

void clear_screen1(void) {
    EGB_clearScreen(workaddr);
}

void clear_screen2(int color) {
    EGB_color(workaddr, 0, color);
    EGB_clearScreen(workaddr);
}

int set_screen_ratio(int x_ratio, int y_ratio) {
    int ret01;
    ret01 = EGB_displayStart(workaddr, 2, x_ratio, y_ratio);
    return ret01;
}

void DrawPixel_bios(int x, int y, int color) {

    struct draw_pixel_data {
        short   num;
        short   x;
        short   y;
    };
    struct draw_pixel_data draw_pixel_data01;
    struct draw_pixel_data *p;

    set_color(color);

    draw_pixel_data01.num = 1;
    draw_pixel_data01.x = (short)x;
    draw_pixel_data01.y = (short)y;
    p = &draw_pixel_data01;

    EGB_pset(workaddr, (char *)p);
}

void set_palette(int pal_num01, int r01, int g01, int b01) {
    struct set_palette_data {
        int     num;
        int     pal_num;
        char    b;
        char    r;
        char    g;
        char    zero;
    };
    struct set_palette_data set_palette_data01;
    struct set_palette_data *p;

    set_palette_data01.num = 1;
    set_palette_data01.pal_num = pal_num01;
    set_palette_data01.b = b01;
    set_palette_data01.r = r01;
    set_palette_data01.g = g01;
    set_palette_data01.zero = 0;

    p = &set_palette_data01;
    EGB_palette(workaddr, 0, (char *)p);
}

void wait_vsync(void)
{
    int    par = 0;
    EGB_palette(workaddr, 0, (char *)&par);
}

/* イメージ描画関数 */
void draw_image(int x, int y, unsigned char *data, int color)
{
    int px, py;
    unsigned short point;

    for (py = 0; py < 16; py++) {
        for (px = 0; px < 16; px++) {

            point = data[py * 16 + px] ? 65535 : 0;

            if (color != 0)
/*                DrawPixel(x + px, y + py, point & color);  */
                DrawPixel_fast(x + px, y + py, point & color);
        }
    }
}

void _outb(unsigned int addr, unsigned char data) {
    __asm__ volatile (
    		"movl	%0, %%edx\n\t"
    		"movb	%1, %%al\n\t"
    		"outb	%%al, %%dx\n\t"
    		:
    		:"r"(addr), "r"(data)
    		:"ax", "dx"
    );
}

void _outw(unsigned int addr, unsigned short data) {
    __asm__ volatile (
    		"movl	%0, %%edx\n\t"
    		"movw	%1, %%ax\n\t"
    		"outw	%%ax, %%dx\n\t"
    		:
    		:"r"(addr), "r"(data)
    		:"ax", "dx"
    );
}

void CRTC(unsigned char reg_addr, unsigned short data) {
    _outb(0x440, reg_addr);
    _outw(0x442, data);
}

void screenterm(char *workaddr)
{
    EGB_init(workaddr, 1536);
    EGB_resolution(workaddr, 0, 1);
    EGB_resolution(workaddr, 1, 1);
    EGB_displayPage(workaddr, 1, 3);
    CRTC(0x18, 0x80);
    set_palette(7, 0xbf, 0xbf, 0xbf);
    return;
}

int keycode1;
unsigned int ene;
int read_key() {
    keycode1 = KYB_read(1, &ene);
    return keycode1;
}
void reset_key() {
    keycode1 = 0xffffffff;
}

int key_pressed(int key) {
    unsigned int ene, ky, ret01;

    ret01 = 0;
    if (key == keycode1) ret01 = 1;
    return ret01;
}

/* TBIOSによるグラフィック画面への文字表示 */
void draw_string(int x, int y, char *str1) {
    struct draw_string_data {
        short     x01;
        short     y01;
        short     strlen;
        char      str[0];
    };
    int strlen01;
    struct draw_string_data *draw_string_data01;

    strlen01 = strlen(str1);

    draw_string_data01 = (struct draw_string_data *)malloc(6 + strlen01 + 1);

    draw_string_data01->x01 = x;
    draw_string_data01->y01 = y;
    draw_string_data01->strlen = (short)strlen01;
    memcpy(draw_string_data01->str, str1, strlen01 + 1);
    EGB_sjisString(workaddr, (char *)draw_string_data01);
}

#define FONT_W 8
#define FONT_H 8

static unsigned char font_digit[10][FONT_W * FONT_H] = {

    /* 0 */
    {
        0,1,1,1,1,1,0,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,1,1,1,1,1,0,0
    },

    /* 1 */
    {
        0,0,0,1,1,0,0,0,
        0,1,1,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        1,1,1,1,1,1,1,0
    },

    /* 2 */
    {
        0,1,1,1,1,1,0,0,
        1,1,0,0,0,1,1,0,
        0,0,0,0,0,1,1,0,
        0,0,0,0,1,1,0,0,
        0,0,0,1,1,0,0,0,
        0,0,1,1,0,0,0,0,
        0,1,1,0,0,0,0,0,
        1,1,1,1,1,1,1,0
    },

    /* 3 */
    {
        1,1,1,1,1,1,0,0,
        0,0,0,0,0,1,1,0,
        0,0,0,0,1,1,0,0,
        0,0,1,1,1,0,0,0,
        0,0,0,0,1,1,0,0,
        0,0,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,1,1,1,1,1,0,0
    },

    /* 4 */
    {
        0,0,0,1,1,0,0,0,
        0,0,1,1,1,0,0,0,
        0,1,1,1,1,0,0,0,
        1,1,0,1,1,0,0,0,
        1,1,1,1,1,1,1,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0,
        0,0,0,1,1,0,0,0
    },

    /* 5 */
    {
        1,1,1,1,1,1,1,0,
        1,1,0,0,0,0,0,0,
        1,1,1,1,1,1,0,0,
        0,0,0,0,0,1,1,0,
        0,0,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,1,1,0,1,1,0,0,
        0,0,1,1,1,0,0,0
    },

    /* 6 */
    {
        0,0,1,1,1,1,0,0,
        0,1,1,0,0,0,0,0,
        1,1,0,0,0,0,0,0,
        1,1,1,1,1,1,0,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,1,1,0,0,1,1,0,
        0,0,1,1,1,1,0,0
    },

    /* 7 */
    {
        1,1,1,1,1,1,1,0,
        0,0,0,0,0,1,1,0,
        0,0,0,0,1,1,0,0,
        0,0,0,1,1,0,0,0,
        0,0,1,1,0,0,0,0,
        0,1,1,0,0,0,0,0,
        0,1,1,0,0,0,0,0,
        0,1,1,0,0,0,0,0
    },

    /* 8 */
    {
        0,1,1,1,1,1,0,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,1,1,1,1,1,0,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,1,1,1,1,1,0,0
    },

    /* 9 */
    {
        0,1,1,1,1,1,0,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,1,1,1,1,1,1,0,
        0,0,0,0,0,1,1,0,
        0,0,0,0,1,1,0,0,
        0,0,0,1,1,0,0,0,
        0,0,1,1,0,0,0,0
    }
};

/* draw_numberで使われる数値1文字表示関数 */
void draw_digit(int x, int y, int digit, int color)
{
    int px, py;

    for (py = 0; py < FONT_H; py++) {

        for (px = 0; px < FONT_W; px++) {

            if (font_digit[digit][py * FONT_W + px])
                DrawPixel(x + px, y + py, color);
        }
    }
}

/* グラフィック画面への数値表示 */
void draw_number(int x, int y, int number, int color)
{
    int divisor;
    int digit;
    int started = 0;

    divisor = 10000;

    while (divisor > 0) {

        digit = number / divisor;
        number %= divisor;

        if (digit != 0 || started || divisor == 1) {
            draw_digit(x, y, digit, color);
            x += 9;
            started = 1;
        }

        divisor /= 10;
    }
}

/*
 * 8x8 英字フォント
 *
 * 1バイト = 横8ピクセル
 * bit = 0 : 背景
 * bit = 1 : 描画
 *
 * font_alpha[0]  = A
 * font_alpha[1]  = B
 * ...
 * font_alpha[25] = Z
 */

static unsigned char font_alpha[26][8] = {

    /* A */
    {
        0x18,
        0x3C,
        0x66,
        0x66,
        0x7E,
        0x66,
        0x66,
        0x66
    },

    /* B */
    {
        0x7C,
        0x66,
        0x66,
        0x7C,
        0x66,
        0x66,
        0x66,
        0x7C
    },

    /* C */
    {
        0x3C,
        0x66,
        0x60,
        0x60,
        0x60,
        0x60,
        0x66,
        0x3C
    },

    /* D */
    {
        0x78,
        0x6C,
        0x66,
        0x66,
        0x66,
        0x66,
        0x6C,
        0x78
    },

    /* E */
    {
        0x7E,
        0x60,
        0x60,
        0x7C,
        0x60,
        0x60,
        0x60,
        0x7E
    },

    /* F */
    {
        0x7E,
        0x60,
        0x60,
        0x7C,
        0x60,
        0x60,
        0x60,
        0x60
    },

    /* G */
    {
        0x3C,
        0x66,
        0x60,
        0x60,
        0x6E,
        0x66,
        0x66,
        0x3C
    },

    /* H */
    {
        0x66,
        0x66,
        0x66,
        0x7E,
        0x66,
        0x66,
        0x66,
        0x66
    },

    /* I */
    {
        0x3C,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x3C
    },

    /* J */
    {
        0x1E,
        0x0C,
        0x0C,
        0x0C,
        0x0C,
        0x6C,
        0x6C,
        0x38
    },

    /* K */
    {
        0x66,
        0x6C,
        0x78,
        0x70,
        0x78,
        0x6C,
        0x66,
        0x66
    },

    /* L */
    {
        0x60,
        0x60,
        0x60,
        0x60,
        0x60,
        0x60,
        0x60,
        0x7E
    },

    /* M */
    {
        0x63,
        0x77,
        0x7F,
        0x6B,
        0x63,
        0x63,
        0x63,
        0x63
    },

    /* N */
    {
        0x66,
        0x76,
        0x7E,
        0x7E,
        0x6E,
        0x66,
        0x66,
        0x66
    },

    /* O */
    {
        0x3C,
        0x66,
        0x66,
        0x66,
        0x66,
        0x66,
        0x66,
        0x3C
    },

    /* P */
    {
        0x7C,
        0x66,
        0x66,
        0x7C,
        0x60,
        0x60,
        0x60,
        0x60
    },

    /* Q */
    {
        0x3C,
        0x66,
        0x66,
        0x66,
        0x66,
        0x6E,
        0x3C,
        0x0E
    },

    /* R */
    {
        0x7C,
        0x66,
        0x66,
        0x7C,
        0x78,
        0x6C,
        0x66,
        0x66
    },

    /* S */
    {
        0x3C,
        0x66,
        0x60,
        0x3C,
        0x06,
        0x06,
        0x66,
        0x3C
    },

    /* T */
    {
        0x7E,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x18
    },

    /* U */
    {
        0x66,
        0x66,
        0x66,
        0x66,
        0x66,
        0x66,
        0x66,
        0x3C
    },

    /* V */
    {
        0x66,
        0x66,
        0x66,
        0x66,
        0x66,
        0x66,
        0x3C,
        0x18
    },

    /* W */
    {
        0x63,
        0x63,
        0x63,
        0x6B,
        0x6B,
        0x7F,
        0x77,
        0x63
    },

    /* X */
    {
        0x66,
        0x66,
        0x3C,
        0x18,
        0x18,
        0x3C,
        0x66,
        0x66
    },

    /* Y */
    {
        0x66,
        0x66,
        0x66,
        0x3C,
        0x18,
        0x18,
        0x18,
        0x18
    },

    /* Z */
    {
        0x7E,
        0x06,
        0x0C,
        0x18,
        0x30,
        0x60,
        0x60,
        0x7E
    }
};


/*
 * A～Zを1文字描画
 */
/* draw_textで使われる文字1文字表示関数 */
void draw_char(int x, int y, char c, int color)
{
    int i;
    int j;
    unsigned char line;

    if (c >= 'a' && c <= 'z')
        c = c - 'a' + 'A';

    if (c < 'A' || c > 'Z')
        return;

    line = 0;

    /*
     * A=0, B=1, ..., Z=25
     */
    line = c - 'A';

    for (i = 0; i < 8; i++) {

        for (j = 0; j < 8; j++) {

            if (font_alpha[line][i] & (0x80 >> j)) {
                DrawPixel(
                    x + j,
                    y + i,
                    color
                );
            }
        }
    }
}


/*
 * 文字列を描画
 */
/* グラフィックへ文字を表示する関数 */
void draw_text(int x, int y, const char *s, int color)
{
    while (*s != '\0') {

        if (*s == ' ') {
            x += 8;
        }
        else {
            draw_char(x, y, *s, color);
            x += 9;
        }

        s++;
    }
}

/*
 * A～Zを1文字描画
 */
void draw_char_expansion(int x, int y, char c, int color, int size)
{
    int i;
    int j;
    unsigned char line;

    if (c >= 'a' && c <= 'z')
        c = c - 'a' + 'A';

    if (c < 'A' || c > 'Z')
        return;

    line = 0;

    /*
     * A=0, B=1, ..., Z=25
     */
    line = c - 'A';

    for (i = 0; i < 8; i++) {

        for (j = 0; j < 8; j++) {

            if (font_alpha[line][i] & (0x80 >> j)) {
                fillrect(
                    x + j * size,
                    y + i * size,
                    x + (j + 1) * size - 1,
                    y + (i + 1) * size - 1,
                    color
                );
            }
        }
    }
}


/*
 * 文字列を描画
 */
/* グラフィックへ文字を表示する関数(文字の大きさの整数倍の倍率指定ができるバージョン */
void draw_text_expansion(int x, int y, const char *s, int color, int size)
{
    while (*s != '\0') {

        if (*s == ' ') {
            x += 8 * size;
        }
        else {
            draw_char_expansion(x, y, *s, color, size);
            x += 9 * size;
        }

        s++;
    }
}

void view_sprite() {
    SPR_display(1, SPRITE_NUM);                        /* 第1パラメータ CRT制御 0:スプライト動作しない 1:スプライト動作する 2:スプライトREADYを待つ  */
}

void end_sprite() {
    SPR_display(0, SPRITE_NUM);                        /* 第1パラメータ CRT制御 0:スプライト動作しない 1:スプライト動作する 2:スプライトREADYを待つ  */
}

/* スプライト初期化関数 */
int set_sprite() {
    int i, j;

    SPR_init();                             /* スプライトの初期化 */

    /* 32768色の場合、128からのパターン番号は4の倍数 */
    /* パターンデータの格納部にスプライトパターンデータを格納 */
    SPR_define(1, 128, 1, 1, (char *)spr_ptn01);   /* 第1パラメータ 0:16色 1:32k色 第2パラメータ:パターン番号 */
                                           /* 第3パラメータ 横スプライト数 第4パラメータ 縦スプライト数 */
                                           /* 第5パラメータ:パターンデータ配列 */
    /* パターンデータの格納部にスプライトパターンデータを格納 */
    SPR_define(1, 128+1*4, 1, 1, (char *)spr_ptn02);   /* 第1パラメータ 0:16色 1:32k色 第2パラメータ:パターン番号 */
                                           /* 第3パラメータ 横スプライト数 第4パラメータ 縦スプライト数 */
                                           /* 第5パラメータ:パターンデータ配列 */
    /* パターンデータの格納部にスプライトパターンデータを格納 */
    SPR_define(1, 128+2*4, 1, 1, (char *)spr_ptn03);   /* 第1パラメータ 0:16色 1:32k色 第2パラメータ:パターン番号 */
                                           /* 第3パラメータ 横スプライト数 第4パラメータ 縦スプライト数 */
                                           /* 第5パラメータ:パターンデータ配列 */
    /* パターンデータの格納部にスプライトパターンデータを格納 */
    SPR_define(1, 128+3*4, 1, 1, (char *)spr_ptn04);   /* 第1パラメータ 0:16色 1:32k色 第2パラメータ:パターン番号 */
                                           /* 第3パラメータ 横スプライト数 第4パラメータ 縦スプライト数 */
                                           /* 第5パラメータ:パターンデータ配列 */
    /* パターンデータの格納部にスプライトパターンデータを格納 */
    SPR_define(1, 128+4*4, 1, 1, (char *)spr_ptn05);   /* 第1パラメータ 0:16色 1:32k色 第2パラメータ:パターン番号 */
                                           /* 第3パラメータ 横スプライト数 第4パラメータ 縦スプライト数 */
                                           /* 第5パラメータ:パターンデータ配列 */
    /* 個々のスプライトの初期化 */
    for (j = 0; j < ENEMY_ROWS; j++) {
        for (i = 0; i < ENEMY_COLS; i++) {
            SPR_setAttribute(1023-(ENEMY_COLS*j+i), 1, 1, 128+j*4, 0);  /* 第1パラメータ:先頭スプライト番号 */
                                           /* 第2パラメータ 横のスプライト数  第3パラメータ 縦のスプライト数 */
                                           /* 第4パラメータ パターン番号      第5パラメータ 色テーブル番号 */
        }
    }

    /* 使わないスプライトも一応初期化 */
    for (i = ENEMY_ROWS*ENEMY_COLS; i < 200; i++) {
    /* インデックス格納部にアトリビュート設定 先頭スプライトは1023 */
        SPR_setAttribute(1023-i, 1, 1, 128, 0);  /* 第1パラメータ:先頭スプライト番号 */
                                           /* 第2パラメータ 横のスプライト数  第3パラメータ 縦のスプライト数 */
                                           /* 第4パラメータ パターン番号      第5パラメータ 色テーブル番号 */
    }

/*    スプライトの位置を指定する関数 */
/*    SPR_setPosition(0, 1023, 1, 1, 0, -16); *//* 第1パラメータ:スプライトサイズ 第2パラメータ:先頭スプライト番号 */
                                              /* 第3パラメータ:横のスププライと数 第4パラメータ:縦のスプライト数 */
                                              /* 第5パラメータ:横表示位置 第6パラメータ:縦表示位置 */

    return 0;
}

