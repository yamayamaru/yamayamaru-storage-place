#include <stdio.h>
#include <stdlib.h>
#include <egb.h>
#include <mos.h>
#include <spr.h>
#include <snd.h>
#include <time.h>
#include <conio.h>
#include <string.h>

int egb_set();
int mos_set();
int spr_set();
int end();
int sprtest();

int snd_set();


int rnd(int a);
void setup1(void);
void random_raspberry(void);
void drawString(int x, int y, const char *str);

int sw, cx, cy;

#define SPRITE_NUM 200


#define snd_data01_ch   71                       //  音声出力チャンネル
#define snd_data01_note 60                       //  音声出力の音階
#define snd_data01_vol  127                       //  音声出力の音量 0～127
#define snd_data01_freq 19200                    //  サンプリングレート

#define SOUND_DATA_SIZE 3000000
unsigned char snd_data01[SOUND_DATA_SIZE + 32];

char buf02[1024];
char filebuf01[1024];

char snd_work[16384];
char egb_work01[1536];
char mos_work01[4096];

char spr_ptn01[] = {
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x18,0x63,0xd6,0x5a,0xf7,0x5e,0x00,0x80,
    0x00,0x80,0xf7,0x5e,0xd6,0x5a,0x39,0x67,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x18,0x63,0x46,0x49,0x88,0x55,0x88,0x55,0x6a,0x3d,
    0x6a,0x3d,0x88,0x55,0x88,0x55,0x46,0x49,0x18,0x63,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x5a,0x6b,0x26,0x41,0xa9,0x5d,0x47,0x4d,0x05,0x39,
    0x05,0x39,0x47,0x4d,0xa8,0x5d,0x26,0x41,0x5a,0x6b,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x94,0x52,0x26,0x45,0x26,0x45,0x41,0x04,
    0x41,0x04,0x26,0x45,0x26,0x45,0x94,0x52,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x93,0x4e,0x67,0x0a,0x23,0x05,0xc8,0x0e,
    0xe8,0x0e,0x43,0x05,0x67,0x0a,0x94,0x4e,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0xff,0x7f,0x66,0x15,0xa5,0x05,0xe5,0x05,0x43,0x05,
    0x44,0x05,0xe5,0x05,0xa5,0x05,0x66,0x15,0xff,0x7f,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x94,0x52,0xa5,0x05,0x87,0x0a,0x09,0x0f,0x26,0x0a,
    0x26,0x0a,0x09,0x0f,0x87,0x0a,0xa5,0x05,0x94,0x52,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0xff,0x7f,0xa8,0x1d,0x46,0x0a,0x87,0x0a,0x09,0x0f,0xa4,0x05,
    0xa4,0x05,0x09,0x0f,0x87,0x0a,0x46,0x0a,0xa9,0x21,0xff,0x7f,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x52,0x4a,0x64,0x05,0xc2,0x00,0x03,0x05,0xa8,0x0a,
    0xa8,0x0a,0x23,0x05,0xc2,0x00,0x64,0x05,0x52,0x4a,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x5a,0x6b,0x26,0x0a,0xc8,0x0e,0x64,0x05,0x29,0x0f,
    0x09,0x0f,0x84,0x05,0xc8,0x0e,0x26,0x0a,0x5a,0x6b,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0xff,0x7f,0xcc,0x31,0xc8,0x0a,0x23,0x05,0xe5,0x05,
    0xe5,0x05,0x23,0x05,0xc8,0x0a,0xcd,0x31,0xff,0x7f,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0xde,0x7b,0x73,0x4e,0xe5,0x10,0xa8,0x0a,
    0xa8,0x0a,0x05,0x11,0x73,0x4e,0xde,0x7b,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x9c,0x73,0x93,0x4e,
    0x73,0x4e,0x9c,0x73,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
    0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80
};

int main(int argc, char *argv[]) {

    srand((unsigned int)time(NULL)); 

    egb_set();
    mos_set();
    spr_set();
    snd_set();

    sprtest();

    end();

    return 0;
}

/* グラフィック初期化 */
int egb_set() {
    char para[64];

    EGB_init(egb_work01, 1536);
    EGB_resolution(egb_work01, 0, 10);          /* page 0, 画面モード10 */
    EGB_resolution(egb_work01, 1, 5);           /* page 1, 画面モード 5 */
    EGB_displayPage(egb_work01, 1, 3);          /* page 1を前,   page 0, page1を両方表示 */

    EGB_writePage(egb_work01, 0);               /* 書き込みページをpage 0 */
    EGB_displayStart(egb_work01, 2, 2, 2);      /* 画面の拡大率 横2倍、縦2倍 */
    EGB_displayStart(egb_work01, 3, 320, 240);  /* 表示画面の大きさ 320x240 */
    EGB_displayStart(egb_work01, 0, 0, 0);      /* 表示開始位置 x = 0, y = 0 */
    EGB_displayStart(egb_work01, 1, 0, 0);      /* 仮想画面の移動 x = 0, y = 0 */

    EGB_writePage(egb_work01, 1);               /* 書き込みページをpage 1 */
    EGB_displayStart(egb_work01, 2, 2, 2);      /* 画面の拡大率 横2倍、縦2倍 */
    EGB_displayStart(egb_work01, 3, 256, 240);  /* 表示画面の大きさ 256x240 */
    EGB_displayStart(egb_work01, 0, (320-256)/2, 0);  /* 表示開始位置 x = 0, y = 0 */
    EGB_displayStart(egb_work01, 1, 0, 0);      /* 仮想画面の移動 x = 0, y = 0 */

    /* page 0を塗る */
    EGB_writePage(egb_work01, 0);
    EGB_paintMode(egb_work01, 0x22);            /* 面塗モードの設定 (ベタ塗りに設定) */
    EGB_color(egb_work01, 0, 0x0000);           /* 前景色の設定 */
    EGB_color(egb_work01, 1, 0x0000);           /* 背景色の設定 */
    EGB_color(egb_work01, 2, 0x0000);           /* 面塗色の設定 */
    WORD(para + 0) = 0;
    WORD(para + 2) = 0;
    WORD(para + 4) = 319;
    WORD(para + 6) = 239;
    EGB_rectangle(egb_work01, para);            /* 0,0-319,239の矩形描画 */

    return 0;
}

int snd_set() {
    FILE *fp01;
    int ch, i, count01;
    char *filename;
    int file_size;

    SND_init(snd_work) ;                        // サウンドドライバの初期化
    SND_elevol_all_mute(-1) ;                   // サウンド全ミュートの解除
    SND_elevol_mute(0x01) ;                     // サウンドミュートの設定
    SND_pcm_mode_set(1) ;                       // サウンド音声モードチャンネル数の設定

    BYTE(snd_data01 + 0)   = 't';               // 8文字の名前
    BYTE(snd_data01 + 0)   = 'e';
    BYTE(snd_data01 + 0)   = 's';
    BYTE(snd_data01 + 0)   = 't';
    BYTE(snd_data01 + 0)   = '0';
    BYTE(snd_data01 + 0)   = '1';
    BYTE(snd_data01 + 0)   = ' ';
    BYTE(snd_data01 + 0)   = ' ';
    DWORD(snd_data01 +  8) = 1;                 // BIOS内のサウンド管理用識別用ID
    DWORD(snd_data01 + 12) = (int)(65536-32);   // ウェーブデータの総バイト数
    DWORD(snd_data01 + 16) = 0;                 // ループ開始点
    DWORD(snd_data01 + 20) = 0;                 // ループの長さ(0の時はループしない)
    WORD(snd_data01 + 24)  = (short)snd_data01_freq*0x62/1000;     // サンプリング周波数 (KHz * 0x62)
    WORD(snd_data01 + 26)  = (short)0;          // 原音の補正値(サンプリング周波数に対する加減算値)
    BYTE(snd_data01 + 28)  = (char)snd_data01_note; // 原音の音階(サウンドデータの基本音階)
    BYTE(snd_data01 + 29)  = (char)0;           // リザーブ
    WORD(snd_data01 + 30)  = (short)0;          // リザーブ

    EGB_color(egb_work01, 0, 0x7fff);           /* 前景色の設定 */
    EGB_color(egb_work01, 1, 0x0000);           /* 背景色の設定 */
    EGB_color(egb_work01, 2, 0x7fff);           /* 面塗色の設定 */

    sprintf(buf02, "Now loading!");
    drawString(0, 20, buf02);


    filename = "Wedding2.snd";

    fp01 = fopen(filename, "rb");
    if (fp01 == NULL) {
        SND_end();
        return 0;
    }
    file_size = _filelength(_fileno(fp01));
    fclose(fp01);



/*
    // rawデータの読み込み
    fp01 = fopen(filename, "rb");
    if (fp01 == NULL) {
        SND_end();
        return 0;
    }
    count01 = 0;
    if (file_size > SOUND_DATA_SIZE) file_size = SOUND_DATA_SIZE;
    for (i = 0; i < file_size / sizeof(filebuf01); i++) {
        fread(filebuf01, 1, sizeof(filebuf01), fp01);
        snd_data01[32 + count01] = (unsigned char)ch;
        memcpy(snd_data01 + 32 + count01, filebuf01, sizeof(filebuf01));
        count01 += sizeof(filebuf01);
    }
    for (i = count01; i < file_size; i++) {
        ch = fgetc(fp01);
        if (ch == EOF) break;
        snd_data01[32 + count01] = (unsigned char)ch;
        count01++;
    }
    fclose(fp01);
*/



    // sndデータの読み込み
    fp01 = fopen(filename, "rb");
    if (fp01 == NULL) {
        SND_end();
        return 0;
    }

    count01 = 0;
    if (file_size > SOUND_DATA_SIZE + 32) file_size = SOUND_DATA_SIZE + 32;
    for (i = 0; i < file_size / sizeof(filebuf01); i++) {
        fread(filebuf01, 1, sizeof(filebuf01), fp01);
        snd_data01[count01] = (unsigned char)ch;
        memcpy(snd_data01 + count01, filebuf01, sizeof(filebuf01));
        count01 += sizeof(filebuf01);
    }
    for (i = count01; i < file_size; i++) {
        ch = fgetc(fp01);
        if (ch == EOF) break;
        snd_data01[count01] = (unsigned char)ch;
        count01++;
    }
    fclose(fp01);



    EGB_clearScreen(egb_work01);



    DWORD(snd_data01 + 12) = count01;

    DWORD(snd_data01 + 20) = count01;
}

/* マウス初期化 */
int mos_set() {

    MOS_start(mos_work01, 4096);            /* マウス初期化 */
    MOS_horizon(0, 511);                    /* マウス水平移動範囲指定 */
    MOS_vertical(0, 511);                   /* マウス垂直移動範囲指定 */
    MOS_setpos(128, 120);                   /* マウス表示位置の指定 */

    return 0;
}

int spr_set() {
    int i;

    SPR_init();                             /* スプライトの初期化 */

    /* パターンデータの格納部にスプライトパターンデータを格納 */
    SPR_define(1, 128, 1, 1, spr_ptn01);   /* 第1パラメータ 0:16色 1:32k色 第2パラメータ:パターン番号 */
                                           /* 第3パラメータ 横スプライト数 第4パラメータ 縦スプライト数 */
                                           /* 第5パラメータ:パターンデータ配列 */

    for (i = 0; i < 200; i++) {
    /* インデックス格納部にアトリビュート設定 先頭スプライトは1023 */
        SPR_setAttribute(1023-i, 1, 1, 128, 0);  /* 第1パラメータ:先頭スプライト番号 */
                                           /* 第2パラメータ 横のスプライト数  第3パラメータ 縦のスプライト数 */
                                           /* 第4パラメータ パターン番号      第5パラメータ 色テーブル番号 */
    }


//    SPR_setPosition(0, 1023, 1, 1, 128, 120); /* 第1パラメータ:スプライトサイズ 第2パラメータ:先頭スプライト番号 */
                                              /* 第3パラメータ:横のスププライと数 第4パラメータ:縦のスプライト数 */
                                              /* 第5パラメータ:横表示位置 第6パラメータ:縦表示位置 */

    return 0;
}

/* マウス終了 */
int end() {

    MOS_end();

    return 0;
}

/* スプライトテスト */
int sprtest() {
    char para[64];
    time_t start_time, time01, time02;
    int count01;
    /* スプライト表示 */
    SPR_display(1, SPRITE_NUM);                        /* 第1パラメータ CRT制御 0:スプライト動作しない 1:スプライト動作する 2:スプライトREADYを待つ  */
                                              /* 第2パラメータ スプライトの数 */

    /* page 0を塗る */
    EGB_writePage(egb_work01, 0);
    EGB_paintMode(egb_work01, 0x22);        /* 面塗モードの設定 (ベタ塗りに設定) */
    EGB_color(egb_work01, 0, 0x0000);       /* 前景色の設定 */
    EGB_color(egb_work01, 1, 0x0000);       /* 背景色の設定 */
    EGB_color(egb_work01, 2, 0x0000);       /* 面塗色の設定 */
    WORD(para + 0) = 0;
    WORD(para + 2) = 0;
    WORD(para + 4) = 319;
    WORD(para + 6) = 239;
    EGB_rectangle(egb_work01, para);        /* 0,0-319,239の矩形描画 */

    EGB_color(egb_work01, 0, 0x0010);       /* 前景色の設定 */
    EGB_color(egb_work01, 1, 0x0000);       /* 背景色の設定 */
    EGB_color(egb_work01, 2, 0x0010);       /* 面塗色の設定 */
    WORD(para + 0) = (320-256)/2;
    WORD(para + 2) = 0;
    WORD(para + 4) = (320-256)/2+255;
    WORD(para + 6) = 239;
    EGB_rectangle(egb_work01, para);        /* (320-256)/2,0-(320-256)/2+255,239の矩形描画 */


    setup1();

    SND_pcm_play(snd_data01_ch, snd_data01_note, snd_data01_vol, snd_data01); // サウンド音声モード再生開始

    start_time = time(NULL);
    time02 = start_time;
    count01 = 0;
    /* マウスのボタンのどちらかが押されるまで繰り返す */
    while ((sw == 0) && (!kbhit())) {
        MOS_rdpos(&sw, &cx, &cy);               /* マウス読み取り */

        random_raspberry();
        SPR_display(2, SPRITE_NUM);
        time01 = time(NULL);
        if ((time01-time02) > 5) {

            EGB_color(egb_work01, 0, 0x0000);       /* 前景色の設定 */
            EGB_color(egb_work01, 1, 0x0000);       /* 背景色の設定 */
            EGB_color(egb_work01, 2, 0x0000);       /* 面塗色の設定 */
            WORD(para + 0) = 0;
            WORD(para + 2) = 239-16;
            WORD(para + 4) = 319;
            WORD(para + 6) = 239;
            EGB_rectangle(egb_work01, para);        /* (320-256)/2,0-(320-256)/2+255,239の矩形描画 */

            EGB_color(egb_work01, 0, 0x7fff);       /* 前景色の設定 */
            EGB_color(egb_work01, 1, 0x0000);       /* 背景色の設定 */
            EGB_color(egb_work01, 2, 0x7fff);       /* 面塗色の設定 */
            sprintf(buf02, "fps = %.4f", (double)count01 / (double)(time01 - start_time));
            drawString(0, 240, buf02);

            time02 = time01;
        }
        count01++;
    }


    SND_pcm_play_stop(snd_data01_ch) ;            // サウンド音声モード再生停止

    SND_end() ;                                   // サウンド動作終了の設定

    SPR_display(0, SPRITE_NUM);

    return 0;
}


#define WIDTH    256-16
#define HEIGHT   240-16-16
#define RASPBERRY_N   SPRITE_NUM
#define RDX        5
#define RDY        5


struct {
    int    x;
    int    y;
    int    dx;
    int    dy;
    int    signx;
    int    signy;
    int    c;
} raspberry_data01[RASPBERRY_N];

void setup1(void){
    for (int i = 0; i < RASPBERRY_N; i++) {
        raspberry_data01[i].x = rnd(WIDTH);
        raspberry_data01[i].y = rnd(HEIGHT);
        raspberry_data01[i].dx = rnd(RDX) + 1;
        raspberry_data01[i].dy = rnd(RDY) + 1;
        raspberry_data01[i].signx = rnd(2) ? 1 : -1;
        raspberry_data01[i].signy = rnd(2) ? 1 : -1;
    }
}

void random_raspberry(void) {

    int temp1, temp2;

    for (int i = 0; i < RASPBERRY_N; i++ ) {
        temp1 = raspberry_data01[i].dx * raspberry_data01[i].signx;
        temp2 = raspberry_data01[i].x + temp1;
        if (temp2 > WIDTH) {
            raspberry_data01[i].signx = -1;
            raspberry_data01[i].dx = rnd(RDX) + 1;
            raspberry_data01[i].x = WIDTH + raspberry_data01[i].signx * raspberry_data01[i].dx;
        } else if (temp2 < 0 ) {
            raspberry_data01[i].signx = 1;
            raspberry_data01[i].dx = rnd(RDX) + 1;
            raspberry_data01[i].x = 0 + raspberry_data01[i].signx * raspberry_data01[i].dx;
        } else {
            raspberry_data01[i].x = raspberry_data01[i].x + temp1;
        } 
        temp1 = raspberry_data01[i].dy * raspberry_data01[i].signy;
        temp2 = raspberry_data01[i].y + temp1;
        if (temp2 > HEIGHT) {
            raspberry_data01[i].signy = -1;
            raspberry_data01[i].dy = rnd(RDY) + 1;
            raspberry_data01[i].y = HEIGHT + raspberry_data01[i].signy * raspberry_data01[i].dy;
        } else if (temp2 < 0 ) {
            raspberry_data01[i].signy = 1;
            raspberry_data01[i].dy = rnd(RDY) + 1;
            raspberry_data01[i].y = 0 + raspberry_data01[i].signy * raspberry_data01[i].dy;
        } else {
            raspberry_data01[i].y = raspberry_data01[i].y + temp1;
        }
        SPR_setPosition(1, 1023 - i, 1, 1, raspberry_data01[i].x, raspberry_data01[i].y);
    }
}

int rnd(int a) {
    return (rand() % a);
}

char drawString_para1[1024 + 6];
void drawString(int x, int y, const char *str) {
    int len;
    len = strlen(str);

    if (len > 1024) len = 1024;

    WORD(drawString_para1 + 0) = x;
    WORD(drawString_para1 + 2) = y;
    WORD(drawString_para1 + 4) = (short)len;
    memcpy(drawString_para1 + 6, str, len);
    EGB_sjisString(egb_work01, drawString_para1);
}
