// https://i-satoh.hatenablog.com/entry/65438175
// ここのサイトのMotionJPEGの再生プログラムを参考にして
// FM TOWNSで動画再生のテストをしてみました。
// 動画のみで音声は再生されません
//
// 今回のはFM TOWNSのTOWNS OS用32768色版です
// OpenWatcom C++ Ver2.0でコンパイルしてください
//
// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder
// のJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cを参考にして手を加えました
// botmerさんがメンテナンスされているものがあるようですがかなり手を加えられているのでそれでは動作しません
// また、botmerさんがメンテナンスされているものはRGB565専用になってるようなので使いません
//
// HDDに保存されたMotionJPEGの動画を表示するものです。
// 動画は映像のみの再生です、音声は再生されません。
// 長時間動画の再生はできてます。
//
// このプログラムには動画ファイルは付属しません
// 356行目あたりのloop関数の中身を編集してご自分の動画ファイルを再生するように変更してください
// (拡張子は大文字の.MJPにしてください)
//
// 対応している動画形式はMotionJPEGです。
// 拡張子は大文字半角の.MJPで保存してください。
// MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
// ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
// ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
// ffmpeg -i test_in3.mp4 -b:v 750k  -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
// ffmpeg -i test_in4.mp4 -b:v 200k  -r 12.0 -s 120x90 -vcodec mjpeg -an test_o4.mov
// ffmpeg -i test_in5.mp6 -b:v 70k    -r 4.0 -s 120x90 -vcodec mjpeg -an test_o5.mov
// 拡張子は大文字の.MJPで保存してください。
//
// これらのプログラムの使用に当たってはご自分の責任において使用してください
// これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <i86.h>
#include <dos.h>

// JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoderのJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cに手を加えたものを使用しています
// https://i-satoh.hatenablog.com/entry/65438175
#include "JPEGDecoder/JPEGDecoder.h"



int work01[256*1024/4];

#define VRAM_SCREEN_WIDTH   (512)
#define VRAM_SCREEN_HIGHT   (256)
#define SCREEN_WIDTH        (320)
#define SCREEN_HEIGHT       (240)


#define ILI9486_BLACK      ((uint32_t)0x000000) ///<   0,   0,   0
#define ILI9486_NAVY       ((uint32_t)0x000080) ///<   0,   0, 128
#define ILI9486_DARKGREEN  ((uint32_t)0x008000) ///<   0, 128,   0
#define ILI9486_DARKCYAN   ((uint32_t)0x008080) ///<   0, 128, 128
#define ILI9486_MAROON     ((uint32_t)0x800000) ///< 128,   0,   0
#define ILI9486_PURPLE     ((uint32_t)0x800080) ///< 128,   0, 128
#define ILI9486_OLIVE      ((uint32_t)0x808000) ///< 128, 128,   0
#define ILI9486_LIGHTGREY  ((uint32_t)0xC6C3C6) ///< 198, 195, 198
#define ILI9486_DARKGREY   ((uint32_t)0x808080) ///< 128, 128, 128
#define ILI9486_BLUE       ((uint32_t)0x0000FF) ///<   0,   0, 255
#define ILI9486_GREEN      ((uint32_t)0x00FF00) ///<   0, 255,   0
#define ILI9486_CYAN       ((uint32_t)0x00FFFF) ///<   0, 255, 255
#define ILI9486_RED        ((uint32_t)0xFF0000) ///< 255,   0,   0
#define ILI9486_MAGENTA    ((uint32_t)0xFF00FF) ///< 255,   0, 255
#define ILI9486_YELLOW     ((uint32_t)0xFFFF00) ///< 255, 255,   0
#define ILI9486_WHITE      ((uint32_t)0xFFFFFF) ///< 255, 255, 255
#define ILI9486_ORANGE     ((uint32_t)0xFFA500) ///< 255, 165,   0
#define ILI9486_GREENYELLO ((uint32_t)0xADFF29) ///< 173, 255,  41
#define ILI9486_PINK       ((uint32_t)0xFF82C6) ///< 255, 130, 198

#define _width   SCREEN_WIDTH
#define _height  SCREEN_HEIGHT

#define DEFFRATE     10  // default frame rate

struct rgb01 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct rgb01 color01(uint32_t color) {
    struct rgb01 col;
    col.r = (color >> 16) & 0xff;
    col.g = (color >> 8) & 0xff;
    col.b = (color) & 0xff;
    return col;
}



#define FP2QWORD(x)   *((unsigned long long far *)((unsigned char far *)(x)))
#define FP2DWORD(x)   *((unsigned int far *)((unsigned char far *)(x)))
#define FP2WORD(x)    *((unsigned short far *)((unsigned char far *)(x)))
#define FP2BYTE(x)    *((unsigned char far *)((unsigned char far *)(x)))



inline static void dmaWait(void);

int kbhit(void);
bool drawJpgBmppicture(char *fname);
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate);
long long millis01(); 
long long time_us_64();
void delay01(int a);

int ginit(int mode1, int page1, char *workaddr01, int worksize01);
void set_color(int color);
void set_bgcolor(int color);
void set_color_GRB(int color);
void set_bgcolor_GRB(int color);
void clear_screen(void);
void draw_pixel(int x, int y, int color);
void set_palette(int pal_num01, int r01, int g01, int b01);
void draw_string(int x, int y, char *str1);
int set_screen_ratio(int x_ratio, int y_ratio);
int set_display_start(int mode, int x_start, int y_start);

#define drawPixel  draw_pixel_vram_16bit_02
void draw_pixel_vram_16bit(int x, int y, int color);
void draw_pixel_vram_16bit_02(int x, int y, int color);

void fillScreen(int color);
void fillRect(int x1, int y1, int x2, int y2, int color);
void resetPushColor(int x1, int y1, int x2, int y2);
void pushColor(uint32_t color);
void pushColors(uint8_t *data, int32_t size);
int color15(int color);

void int_to_str(char *str1, int a);
void uint_to_str(char *str1, int a);
void uint64_to_str(char *str1, long long a);
void hex64_to_str(char *str1, long long a);

void get_call_buff_addr(unsigned short *seg_prog01, unsigned short *off_prog01, 
                unsigned short *segment01, unsigned short *offset01, unsigned int *size01, unsigned short *prot_addr_seg01, unsigned int *prot_addr_off01);
int get_segment_attribute(unsigned short segment_selector01, unsigned char *access_right_byte01, unsigned char *use_type_bit01);
int allocate_msdos_memory_block(unsigned int seg_size01, unsigned short *segment01);
int free_msdos_memory_block(unsigned short segment01);
int call_realmode_procedure(unsigned int addr01, unsigned int copy_stack_number, struct rm_call_reg *p_param_rm_call);
int init_call_realmode_int(unsigned char far *protect_mode_addr);
int ready_call_realmode_int();
int call_realmode_int(int int_no, unsigned char far *protect_mode_addr, unsigned short realmode_segment01,
    struct rmi_call_reg *rmi_call_in, struct rmi_call_reg *rmi_call_out, int stack_num);



int init_timer01(unsigned int segment01, int *timer_num01);
unsigned int get_count_timer01(unsigned int segment01, int timer_num01);
int close_timer01(int segment01, int timer_num01);

// call_realmode_procedureで使うレジスタ定義    リアルモードの関数を呼び出す時に使う
struct rm_call_reg {
    unsigned short rmc_ds;
    unsigned short rmc_es;
    unsigned short rmc_fs;
    unsigned short rmc_gs;
    unsigned int   rmc_eax;
    unsigned int   rmc_ebx;
    unsigned int   rmc_ecx;
    unsigned int   rmc_edx;
};

// call_realmode_intで使う関数定義    リアルモードのBIOSコールを呼び出すときに使う
struct rmi_call_reg {
    unsigned char  zero1;
    unsigned char  zero2;
    unsigned short rmi_ds;
    unsigned short rmi_es;
    unsigned short rmi_fs;
    unsigned int   rmi_eax;
    unsigned int   rmi_ebx;
    unsigned int   rmi_ecx;
    unsigned int   rmi_edx;
    unsigned int   rmi_esi;
    unsigned int   rmi_edi;
    unsigned int   rmi_ebp;
};



void setup();
void loop();

unsigned short segment01;
int timer_num01;

char buf01[1024];
char buf02[256];

double fps0001 = 0.0;
int main(int argc, char *argv[]) {
    int i;

    struct SREGS sregs;
    union REGS regs;
    unsigned int   seg_size01;
    unsigned char far *prot_addr01;
    int ret01;



    setup();


    // DOSエクステンダーコールの0x25c0を呼んでメモリを確保してる
    // (DOSコールの0x48はDOSメモリではなくて、プロテクトモードメモリの確保になってる)
    // BXにパラグラフ単位のメモリ容量を入れる

    seg_size01 = 65536/16;
    if (allocate_msdos_memory_block(seg_size01 * 16, &segment01) != 0) {
        strcpy(buf01, "メモリ確保に失敗しました");
        draw_string(0, 20, buf01);

        // キーバッファのクリア
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);

        while (!kbhit());

        exit(1);
    }

    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000


    set_bgcolor_GRB(color15(ILI9486_BLACK));
    clear_screen();

    strcpy(buf01, "確保したメモリ = ");
    uint_to_str(buf02, (long long)seg_size01 * 16);
    strcat(buf01, buf02);
    strcat(buf01, " byte, segment01 = ");
    hex64_to_str(buf02, (long long)segment01);
    strcat(buf01, buf02);
    draw_string(0, 20, buf01);


    // timer関数で使うBIOSコールのための初期化(リアルモードBIOSを呼び出す時に使う)
    if (init_call_realmode_int(prot_addr01) != 0) {
        set_bgcolor_GRB(color15(ILI9486_RED));
        clear_screen();

        strcpy(buf01, "init_call_realmode_intが失敗しました");
        draw_string(0, 20, buf01);

        if (free_msdos_memory_block(segment01) != 0) {
            set_color_GRB(color15(ILI9486_WHITE));
            draw_string(0, 40, "segment01 メモリ解放に失敗しました");
        } else {
            set_color_GRB(color15(ILI9486_WHITE));
            draw_string(0, 40, "segment01 メモリ解放に成功しました");
        }

        // キーバッファのクリア
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);

        while (!kbhit());

        exit(1);
    }

    // タイマーを使うための期化
    if (init_timer01(segment01, &timer_num01)) {
        set_bgcolor_GRB(color15(ILI9486_RED));
        clear_screen();

        strcpy(buf01, "タイマの初期化に失敗しました");
        draw_string(0, 20, buf01);

        if (free_msdos_memory_block(segment01) != 0) {
            set_color_GRB(color15(ILI9486_WHITE));
            draw_string(0, 40, "segment01 メモリ解放に失敗しました");
        } else {
            set_color_GRB(color15(ILI9486_WHITE));
            draw_string(0, 40, "segment01 メモリ解放に成功しました");
        }

        // キーバッファのクリア
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);

        while (!kbhit());

        exit(1);
    }

    for(;;) {
        loop();
        if (kbhit()) break;
    }

    ret01 = close_timer01(segment01, timer_num01);

    set_color_GRB(color15(ILI9486_WHITE));
    set_bgcolor_GRB(color15(ILI9486_BLACK));
    clear_screen();

    if (ret01 == (char)0) {
        strcpy(buf01, "int 97h ah=01h OK!  タイマ解放OK");
        draw_string(0, 20, buf01);
    } else {
        strcpy(buf01, "int 97h ah=01h NG!  タイマ解放NG");
        draw_string(0, 20, buf01);
    }

    if (free_msdos_memory_block(segment01) != 0) {
        draw_string(0, 40, "segment01 メモリ解放に失敗しました");
    } else {
        draw_string(0, 40, "segment01 メモリ解放に成功しました");
    }
    delay(1000);

    set_bgcolor_GRB(color15(ILI9486_BLACK));
    clear_screen();

    exit(0);
}


void setup() {
  int ret01;

  // グラフィックの初期化
  if (ret01 = ginit(10, 0, (char *)work01, 256*1024) != 0) {
       puts("ginit failed!");
       while(!kbhit());
       exit(1);
  }
// 320x240 全画面設定
// 仮想画面中の表示開始位置を設定します。                                       
// モード　　0:画面の表示開始位置の設定                                         
// 　　　　　1:仮想画面中の移動                                                 
// 　　　　　2:画面の拡大                                                       
// 　　　　　3:表示画面の大きさ    
  set_display_start(3, 0, 0);
  set_display_start(2, 2, 2);
  set_display_start(0, 0, 0);
  set_display_start(1, 0, 0);
  set_display_start(3, 320, 240);

  set_color_GRB(color15(ILI9486_WHITE));
  set_bgcolor(color15(ILI9486_BLACK));
  clear_screen();

}

void loop() {
    // drawJpgBmppicture_mjpeg(動画ファイル名, FPS)で指定します。
    // MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
    // ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
    // ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
    // ffmpeg -i test_in3.mp4 -b:v 750k -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
    // ffmpeg -i test_in4.mp4 -b:v 200k -r 12.0 -s 120x90 -vcodec mjpeg -an test_o4.mov
    // ffmpeg -i test_in5.mp6 -b:v 70k   -r 4.0 -s 120x90 -vcodec mjpeg -an test_o5.mov
    // このプログラムはMotionJPEGのファイルの拡張子は.MJPとして認識するので拡張子を.MJPに変更してください。
    // JPEG画像やBMP画像の表示もできますが、表示されない場合は一度Windows付属のペイントで保存してから試してみてください。

    fillScreen(color15(ILI9486_BLACK));
    drawJpgBmppicture_mjpeg("SNAIL01.JPG", 0);
    delay01(1000);
    drawJpgBmppicture_mjpeg("SCENIC01.BMP", 0);
    delay01(1000);

/*
    fillScreen(ILI9486_BLUE);
    drawJpgBmppicture_mjpeg("test_o1.MJP", 13);
    delay01(5000);

    fillScreen(ILI9486_BLUE );
    drawJpgBmppicture_mjpeg("test_o2.MJP", 22);
    delay01(5000);

    fillScreen(ILI9486_BLUE);
    drawJpgBmppicture_mjpeg("test_o3.MJP", 38);
    delay01(5000);
*/

    fillScreen(color15(ILI9486_BLACK));
    drawJpgBmppicture_mjpeg("V1005FPS.MJP", 5);
    delay01(1000);

    fillScreen(color15(ILI9486_BLACK));
    drawJpgBmppicture_mjpeg("V5_05FPS.MJP", 5);
    delay01(1000);

}


// Jpeg/Motion-Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture_mjpeg(char *fname, uint8_t frate) {
  // variables
  bool rlt = false;
  FILE *fp;
  unsigned long nxtdt;
  char *ext;
  long picdly;
  long picdly01 = 0;

  set_color_GRB(color15(ILI9486_WHITE));
  set_bgcolor_GRB(color15(ILI9486_BLACK));
  clear_screen();


  // check BMP/JPEG file
  ext = strrchr(fname, '.');
  // if(!ext || (memcmp(ext + 1, "BMP", 3) && memcmp(ext + 1, "JPG", 3) && memcmp(ext + 1, "MJP", 3))) return false;
  if (!ext) {
    fillScreen(ILI9486_PURPLE);
    return false;
  }
  if (!(!memcmp(ext + 1, "BMP", 3) || !memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3))) {
    fillScreen(ILI9486_YELLOW);
    return false;
  }
  // check jpeg file
  if(!memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3)){
    // check & set mottion jpeg
    if(!memcmp(ext + 1, "MJP", 3)) {
      JpegDec.mjpegflag = 1;
    } else {
      JpegDec.mjpegflag = 0;
    }

    // variables
    uint8 *pImg;
    int x, y, bx, by;
    struct rgb01 col = color01(ILI9486_BLACK);
    bool inifrmset = true;

    // draw picture
    
    long long time01 = 0LL;
    long long time02 = 0LL;
    long long time00 = millis01();
    double fps_aver = 0.0;
    int count_fps = 0;
    
    do{
      if (kbhit()) break;

      time02 = time01;
      time01 = millis01();
      if (time02 > 0LL) {
        double time1 = (time01 - time02) / 1000.0;
        double fps01 = 1.0 / time1;
        fps_aver += fps01;
        count_fps++;
        if (((millis01() - time00) / 1000.0) > 5.0) {
          if(count_fps > 0) {
            fps0001 = fps_aver / count_fps;
            picdly01 = picdly;
//            printf("time = %.4f\r\n", time1);
//            printf("fps = %.4f\r\n", fps_aver / count_fps);
            fps_aver = 0.0;
            count_fps = 0;
          }
          time00 = millis01();
        }
      }

      // set next picture draw timing
      if(JpegDec.mjpegflag) nxtdt = millis01() + 1000 / ((frate > 0) ? frate : DEFFRATE);
      // check decording parameter
      if((JpegDec.decode(fname, 0) < 0) || !JpegDec.height || !JpegDec.width) return false;
      // set draw offset parameters
      long xofset = (_width - JpegDec.width) / 2;
      long yofset = (_height - JpegDec.height) / 2;
      if(inifrmset){
        // set top black belt
        dmaWait();

//        映像の上側を消去
//        if(yofset > 0) fillRect(0, 0, _width, yofset, ILI9486_BLACK);
        // set left & right black belt
        if(xofset > 0){
//          映像の左右を消去
//          fillRect(0, (yofset > 0) ? yofset : 0, xofset, (yofset > 0) ? JpegDec.height : _height, ILI9486_BLACK);
//          fillRect(xofset + JpegDec.width, (yofset > 0) ? yofset : 0, _width - JpegDec.width - xofset, (yofset > 0) ? JpegDec.height : _height, ILI9486_BLACK);
        }
        inifrmset = false;
      }
      // start jpeg decode
//      struct rgb01 clbuf[JpegDec.MCUWidth * JpegDec.MCUHeight];
      struct rgb01 *clbuf;
      clbuf = (struct rgb01 *)malloc(JpegDec.MCUWidth * JpegDec.MCUHeight * sizeof(struct rgb01));
      uint16_t clbufpt = 0;
      while(JpegDec.read()){
        // check draw range and get draw parameters
        pImg = JpegDec.pImage ;
        long dxstrt = JpegDec.MCUx * JpegDec.MCUWidth + xofset; if(dxstrt >= _width) continue;
        if(dxstrt < 0) dxstrt = 0;
        long dystrt = JpegDec.MCUy * JpegDec.MCUHeight + yofset; if(dystrt >= _height) continue;
        if(dystrt < 0) dystrt = 0;
        long dxend = (JpegDec.MCUx + 1) * JpegDec.MCUWidth - 1 + xofset; if(dxend < 0) continue;
        if(dxend >= _width) dxend = _width - 1;
        long dyend = (JpegDec.MCUy + 1) * JpegDec.MCUHeight - 1 + yofset; if(dyend < 0) continue;
        if(dyend >= _height) dyend = _height - 1;
        // draw princture
        bool drawstart = true;
        
        clbufpt = 0;
        for(by = 0 ; by < JpegDec.MCUHeight ; by++){
          // draw decoded image
          y = JpegDec.MCUy * JpegDec.MCUHeight + by;
          // check image end
          if(y >= JpegDec.height) break;
          if(((y + yofset) < 0) || ((y + yofset) >= _height)) continue;
          // draw image
          for(bx = 0 ; bx < JpegDec.MCUWidth ; bx++){
            x = JpegDec.MCUx * JpegDec.MCUWidth + bx;
            // check image draw range
            if(((x + xofset) < 0) || ((x + xofset) >= _width)) continue;
            // start drawing
            if(drawstart){
              // set draw window

              resetPushColor(dxstrt, dystrt, dxend, dyend);
              
              // clear draw start flag
              drawstart = false;
            }
            // get & set pixel data
            if(JpegDec.comps == 1) { // Grayscale
              col.r = pImg[0]; col.g = pImg[0]; col.b = pImg[0];
            } else {// RGB
              col.r = pImg[0]; col.g = pImg[1]; col.b = pImg[2];
            }
            // check picture draw range & set color
            if(x >= JpegDec.width) col = color01(ILI9486_BLACK);
            if(((xofset + x) >= 0) && ((yofset + y) >= 0) && ((xofset + x) < _width) && ((yofset + y) < _height))   clbuf[clbufpt++] = col;
            pImg += JpegDec.comps;
          }
        }
        // draw picture
        if(clbufpt) pushColors((uint8_t *)clbuf, clbufpt);
      }

      free(clbuf);

//      // 映像の下側を描画
//      // set bottom black belt
//      if(yofset > 0) fillRect(0, yofset + JpegDec.height, _width, _height - (yofset + JpegDec.height), ILI9486_BLACK);
      // check next picture wait timing
      if(JpegDec.mjpegflag){
        picdly = nxtdt - millis01();

        set_color_GRB(color15(ILI9486_WHITE));
        fillRect(0, 0, 50, 23, ILI9486_BLACK);
        int_to_str(buf01, (int)(fps0001 * 1000.0));
//        sprintf(buf01, "%.4f", fps0001);
        draw_string(0, 20, buf01);


        if(picdly > 0) delay01(picdly);
      }
      //
    } while(JpegDec.mjpegflag && JpegDec.nextjpgpos);
    // end of drawing
    rlt = true;

  // check bitmap file
  } else {
    // check file header
    #define RDPLEN   256   // read packets length
    unsigned char rdbuf[RDPLEN * 3];
	size_t ret01;
    fp = fopen(fname, "rb");
    ret01 = fread(rdbuf, 1, 34, fp);
    if((rdbuf[0] != 'B') || (rdbuf[1] != 'M')) goto rtnend;  // check ID
    if(rdbuf[30] || rdbuf[31] || rdbuf[32] || rdbuf[33]) goto rtnend;  // check no compressin
    if((rdbuf[28] | ((unsigned short)rdbuf[29] << 8)) != 24) goto rtnend;  // check 24bits mode
    // get file parameters
    unsigned long lwidth = rdbuf[18] | ((unsigned long)rdbuf[19] << 8) |
                           ((unsigned long)rdbuf[20] << 16) | ((unsigned long)rdbuf[21] << 24);
    unsigned long lheight = rdbuf[22] | ((unsigned long)rdbuf[23] << 8) |
                           ((unsigned long)rdbuf[24] << 16) | ((unsigned long)rdbuf[25] << 24);
    unsigned long offset = rdbuf[10] | ((unsigned long)rdbuf[11] << 8) |
                           ((unsigned long)rdbuf[12] << 16) | ((unsigned long)rdbuf[13] << 24);
    int drwidth = (lwidth <= _width) ? lwidth : _width;
    int drhight = (lheight <= _height) ? lheight : _height;
    int drxofset = (_width - lwidth) / 2;
    int dryofset = (_height - lheight) / 2;
    unsigned long lpwidth = (lwidth * 3) + ((((lwidth * 3) % 4) > 0) ? 4 - ((lwidth * 3) % 4) : 0);

    // draw screen
    struct rgb01 blkblt[_width];
	memset(blkblt, 0, sizeof(blkblt));
    struct rgb01 blkblt2[_width];
    for(int y = 0 ; y < _height ; y++){
      // draw blank line
      if((_height > lheight) && ((y < dryofset) || (y >= (dryofset + drhight)))){

        resetPushColor(0, y, _width-1, y);
        pushColors((uint8_t *)blkblt, _width);
        continue;
      }
      // draw y line picture
      for(int ly = 0 ; ly < drhight ; ly++){
        // draw x line
        for(int x = 0 ; x < _width ; x++){
          // draw left & right blank
          if((_width > lwidth) && ((x < drxofset) || (x >= (drxofset + drwidth)))){
            // pushColor(ILI9486_BLACK);
            blkblt2[x] = color01(ILI9486_BLACK);
            continue;
          }
          // draw x line picture
          unsigned long rowidx = (lheight - ly - 1 + ((dryofset < 0) ? dryofset : 0)) * lpwidth +
                                                 ((drxofset < 0) ? -drxofset * 3 : 0);
          fseek(fp, rowidx + offset, SEEK_SET);
          for(int lx = 0 ; lx < drwidth ; lx += RDPLEN){
            int rdpklen = ((lx + RDPLEN) < drwidth) ? RDPLEN : drwidth - lx;
            if(fread(rdbuf, (size_t)1, rdpklen * 3, fp) <= 0) goto rtnend;
//            struct rgb01 clbuf[rdpklen];
            for(int ln = 0 ; ln < rdpklen ; ln++) {
//              clbuf[ln].r = rdbuf[ln * 3 + 2];
//              clbuf[ln].g = rdbuf[ln * 3 + 1];
//              clbuf[ln].b = rdbuf[ln * 3];
              blkblt2[x + lx + ln].r = rdbuf[ln * 3 + 2];
              blkblt2[x + lx + ln].g = rdbuf[ln * 3 + 1];
              blkblt2[x + lx + ln].b = rdbuf[ln * 3 + 0];
            }
          }
          x += (drwidth - 1);
        }
        dmaWait();

        resetPushColor(0, y + ly, _width-1, y + ly);
        pushColors((uint8_t *)blkblt2, _width);

      }
      y += (drhight - 1);
    }
    rlt = true;
  }

  // process end
 rtnend:
  if(!memcmp(ext + 1, "BMP", 3)) fclose(fp);
  return rlt;
}

// Jpeg & Bitmap still picture drawing (true:OK, false:Error)
bool drawJpgBmppicture(char *fname) {
  return drawJpgBmppicture_mjpeg(fname, 10);
}

int pushColor_x1, pushColor_y1, pushColor_x2, pushColor_y2, pushColor_left;
void resetPushColor(int x1, int y1, int x2, int y2) {
    pushColor_x1 = x1;
    pushColor_left = x1;
    pushColor_y1 = y1;
    pushColor_x2 = x2;
    pushColor_y2 = y2;
}

void pushColor(uint32_t color) {

    drawPixel(pushColor_x1, pushColor_y1, color15(color));
	pushColor_x1++;
	if (pushColor_x1 == pushColor_x1 + 1) {
        pushColor_y1++;
		pushColor_x1 = 0;
	}
}

void pushColors(uint8_t *data, int32_t size) {
    uint8_t  *p;
    int      color01;

    p = data;
	for (int i = 0; i < size; i++) {
//	    color01 = ((int)*p++) << 16 | ((int)*p++) << 8 | ((int)*p++);
            color01 = (((int)*p++) >> 3) << 5 | (((int)*p++) >> 3) << 10 | (((int)*p++) >> 3);
	    drawPixel(pushColor_x1, pushColor_y1, color01);
	    pushColor_x1++;
	    if (pushColor_x1 == pushColor_x2 + 1) {
                pushColor_y1++;
                pushColor_x1 = pushColor_left;
	    }
	}
}

inline static void dmaWait(void) {
}

int second(void) {
    struct dostime_t time01;
    int hour, min, sec, millisec;
    _dos_gettime(&time01);

    return (double)(((time01.hour * 3600LL + time01.minute * 60LL + time01.second) * 1000 + time01.hsecond * 10) / 1000.0);
}

long long time_us_64() {
    return (long long)(get_count_timer01(segment01, timer_num01) * 10 * 1000);
}

long long millis01() {
    return (long long)get_count_timer01(segment01, timer_num01) * 10;
}

void delay01(int a) {
    double time0001, time0002;
    if (a <= 0) return;
    time0001 = millis01();
    time0002 = (double)a;
    while ((millis01() - time0001) < time0002);
}

int kbhit(void) {
    union REGS regs;
    int ret01;

    regs.h.ah = 0x07;
    int386(0x90, &regs, &regs);
    if (regs.h.dh == (char)0xff) {
        ret01 = 0;
    } else {
        ret01 = 1;
    }

    return ret01;
}

extern int ginit_page01;
void draw_pixel_vram_16bit(int x, int y, int color) {
    char *addr01;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    addr01 = (char *)(((y * VRAM_SCREEN_WIDTH + x) << 1) + ginit_page01 * 0x40000);

    __asm {
        push   fs
	push   ebx
        mov    ax, 0104h           ;16,32768color = 0x0104    256color = 0x010c
        mov    fs, ax
        mov    ebx, addr01
        mov    eax, color
        mov    fs:[ebx], ax
        pop    ebx
        pop    fs
    }
}

extern short far *vram01;
void draw_pixel_vram_16bit_02(int x, int y, int color) {
//    if (x < 0 || x >= SCREEN_WIDTH) return;
//    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    *(vram01 + (y * VRAM_SCREEN_WIDTH + x)) = (short)color;
}

void fillRect(int x1, int y1, int x2, int y2, int color) {
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

    for (j = y1; j <= y2; j++) {
        for (i = x1; i <= x2; i++) {
            draw_pixel_vram_16bit(i, j, color);
        }
    }
}

void fillScreen(int color) {
    fillRect(0, 0, _width-1, _height-1, color);
}

int color15(int color) {
    return ((((int)color >> 16) >> 3) << 5) | (((((int)color >> 8) & 0xff) >> 3) << 10) | (((int)color & 0xff) >> 3);
}

struct view_port0001 {
    short  x1;
    short  y1;
    short  x2;
    short  y2;
} view_port01;
short far *vram01;
char *workaddr;
int  worksize;
int ginit_page01 = 0;
int ginit(int mode1, int page1, char *workaddr01, int worksize01) {
    unsigned short mode01 = (unsigned short)mode1;
//    unsigned short mode02 = (unsigned short)mode2;
    unsigned char  page01 = (unsigned char)page1;
    char ret01;
    struct view_port0001 *view_port;

    view_port01.x1 = 0;
    view_port01.y1 = 0;
    view_port01.x2 = 319;
    view_port01.y2 = 239;
    view_port = &view_port01;

    ginit_page01 = page1;
    vram01 = (short far *)MK_FP(0x0104, ginit_page01 * 0x00040000); // 16,32768color = 0x0104    256color = 0x010c

    workaddr = workaddr01;
    worksize = worksize01;
    
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
	mov	edi,workaddr
        mov     ecx,worksize
        mov	ax,0110h
	mov	fs,ax
        mov     ah,00h
        call    pword ptr fs:0020h
        cmp     ah, 0
        jnz     gmode_end

        mov	ax,0110h           ;screen mode
        mov     fs,ax
        mov     ah,01h
        mov     al,0               ;0:page0 1:page1
        mov     dx, mode01
        call    pword ptr fs:0020h
        cmp     ah, 0
        jnz     gmode_end

;        mov	ax,0110h           ;screen mode
;        mov     fs,ax
;        mov     ah,01h
;        mov     al,1               ;0:page0 1:page1
;        mov     dx, mode01
;        call    pword ptr fs:0020h
;        cmp     ah, 0
;        jnz     gmode_end

        mov	ax,0110h           ;view port
        mov     fs,ax
        mov     ah,03h
        mov     esi, view_port
        call    pword ptr fs:0020h
        cmp     ah, 0
        jnz     gmode_end

        mov	ax,0110h          ;kakikomi page settei
        mov     fs,ax
        mov     ah,05h
        mov     al,page01         ;0:page0 1:page1
        call    pword ptr fs:0020h
        cmp     ah, 0
        jnz     gmode_end

        mov	ax,0110h          ;hyouji page settei
        mov     fs,ax
        mov     ah,06h
        mov     al,page01         ;0:page0 1:page1
        mov     edx,1
        call    pword ptr fs:0020h
        cmp     ah, 0

gmode_end:
        mov     ret01,ah

	pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
    return (int)ret01;
}

void set_color(int color01) {
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,07h
        mov     al,0
	mov	edi,workaddr
        mov     edx,color01
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
}

void set_bgcolor(int color01) {
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,07h
        mov     al,1
	mov	edi,workaddr
        mov     edx,color01
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
}

void set_color_GRB(int color) {
    int color01;
    color01 = (((color >> 10) & 0x1f) << (8 + 3)) | (((color >> 5) & 0x1f) << (16 + 3)) | ((color & 0x1f) << 3);
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,08h
        mov     al,0
	mov	edi,workaddr
        mov     edx,color01
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
}

void set_bgcolor_GRB(int color) {
    int color01;
    color01 = (((color >> 10) & 0x1f) << (16 + 3)) | (((color >> 5) & 0x1f) << (8 + 3)) | ((color & 0x1f) << 3);
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,08h
        mov     al,1
	mov	edi,workaddr
        mov     edx,color01
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
}

void clear_screen(void) {
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
	mov	edi,workaddr
        mov     ah,21h
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
}

void draw_pixel(int x, int y, int color) {

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

    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,40h
	mov	edi,workaddr
        mov     esi,p
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
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
    set_palette_data01.r = r01;
    set_palette_data01.g = g01;
    set_palette_data01.b = b01;
    set_palette_data01.zero = 0;

    p = &set_palette_data01;

    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,04h
        mov     al, 0
	mov	edi,workaddr
        mov     esi,p
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
}

void draw_string(int x, int y, char *str1) {
    struct draw_string_data {
        short     x;
        short     y;
        short       strlen;
        char      str[];
    };
    int strlen01;
    struct draw_string_data *draw_string_data01;

    strlen01 = strlen(str1);

    draw_string_data01 = (struct draw_string_data *)malloc(6 + strlen01 + 1);

    draw_string_data01->x = x;
    draw_string_data01->y = y;
    draw_string_data01->strlen = (short)strlen01;
    memcpy(draw_string_data01->str, str1, strlen01 + 1);

    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,60h
	mov	edi,workaddr
        mov     esi,draw_string_data01
        call    pword ptr fs:0020h

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }

    free(draw_string_data01);
}

int set_screen_ratio(int x_ratio, int y_ratio) {
    int ret01;
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,02h
        mov     al,02h
	mov	edi,workaddr
        mov     edx,x_ratio
	mov	ebx,y_ratio
        call    pword ptr fs:0020h

        xor	ebx, ebx
	mov	bl, ah
        mov     ret01, ebx

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
    return ret01;
}

// - 定義 -                                                                     
// #include <EGB.H>                                                             
// int 　　EGB_displayStart( work, mode, xstart, ystart );                      
// char　　*work;　　　/*　作業域のアドレス　　　　　　　　　　　　　　*/       
// int 　　mode; 　　　/*　モード(bit6=1ではVSYNC待ちなしで設定します) */       
// int 　　xstart; 　　/*　横方向パラメータ　　　　　　　　　　　　　　*/       
// int 　　ystart; 　　/*　縦方向パラメータ　　　　　　　　　　　　　　*/       
//                                                                              
// - 戻り値 -                                                                   
// 0:正常終了                                                                   
//                                                                              
// - 例 -                                                                       
// char　　work[1536];                                                          
// int 　　ret;   
// 
// ret = EGB_displayStart( work, 3, 0, 0 );                                     
// ret = EGB_displayStart( work, 2, 2, 2 );                                     
// ret = EGB_displayStart( work, 0, 0, 0 );                                     
// ret = EGB_displayStart( work, 1, 0, 0 );                                     
// ret = EGB_displayStart( work, 3, 320, 240 );  
// 
// 仮想画面中の表示開始位置を設定します。                                       
// モード　　0:画面の表示開始位置の設定                                         
// 　　　　　1:仮想画面中の移動                                                 
// 　　　　　2:画面の拡大                                                       
// 　　　　　3:表示画面の大きさ    
int set_display_start(int mode, int x_start, int y_start) {
    char mode01 = mode;
    int ret01;
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds
	pop	gs
        mov	ax,0110h
	mov	fs,ax
        mov     ah,02h
        mov     al,mode01
	mov	edi,workaddr
        mov     edx,x_start
	mov	ebx,y_start
        call    pword ptr fs:0020h

        xor	ebx, ebx
	mov	bl, ah
        mov     ret01, ebx

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
    return ret01;
}

void int_to_str(char *str1, int a) {
    char *p = str1;
    int  b = a;
    if (a < 0) {
        b = -a;
        p[0] = '-';
        p++;
    }
    uint_to_str(p, b);
}

void uint_to_str(char *str1, int a) {
    int i;
    char str[10];
    char *p, *p2;
    for (i = 0; i < 10; i++) {
        str[i] = (char)(a % 10 + 0x30);
        a = a / 10;
    }
    p = str+10;
    for (i = 0; i < 10; i++) {
        p--;
        if (*p != (char)0x30) break;
    }
    if (i == 10) {
        str1[0] = (char)(0x30 + 0);
        str1[1] = (char)0;
    } else {
        p2 = str1;
        for (; i < 10; i++) {
            *p2++ = (char)*p--;
        }
        *p2 = (char)0;
    }
}

//    使い方
//        uint64_to_str(buf01, 18446744073709551615ULL);
//        uint64_to_str(buf01, 0xffffffffffffffffULL);
void uint64_to_str(char *str1, long long a01) {
    unsigned long long a = a01;
    int i;
    char str[20];
    char *p, *p2;
    for (i = 0; i < 20; i++) {
        str[i] = (char)(a % 10ULL + 0x30);
        a = a / 10ULL;
    }
    p = str+20;
    for (i = 0; i < 20; i++) {
        p--;
        if (*p != (char)0x30) break;
    }
    if (i == 20) {
        str1[0] = (char)(0x30 + 0);
        str1[1] = (char)0;
    } else {
        p2 = str1;
        for (; i < 20; i++) {
            *p2++ = (char)*p--;
        }
        *p2 = (char)0;
    }
}

void hex64_to_str(char *str1, long long a01) {
    unsigned long long a = a01;
    int i;
    char str[16];
    char *p, *p2;
    char c;
    for (i = 0; i < 16; i++) {
        str[i] = (char)(a % 16ULL);
        a = a / 16ULL;
    }
    p = str + 16 - 1;
    p2 = str1;
    for (i = 0; i < 16; i++) {
        c = (char)*p--;
        if (c >= 10) c = (c - 10) + 'a';
        else         c = c + '0';
        *p2++ = c;
    }
    *p2 = (char)0;
}


// タイマーを使うための準備
int init_timer01(unsigned int segment01, int *timer_num01) {
    unsigned char far *prot_addr01;
    struct rmi_call_reg rmi_call_reg01;
    unsigned int timer_func_offset01;
    int ret01;

    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000

//      rmicall01でリアルモードBIOSをコール   タイマの登録 int 97h, 機能コード00h
    if (ready_call_realmode_int()) {
        rmi_call_reg01.rmi_ds = segment01;
        rmi_call_reg01.rmi_es = segment01;
        rmi_call_reg01.rmi_edi = 0x2000;
        rmi_call_reg01.rmi_eax = 0x00 << 8;    // ah = 0x01;
        timer_func_offset01 = 0x2010;

        FP2BYTE(prot_addr01 + 0x2010 + 0) = 0xcb;

        FP2BYTE(prot_addr01 + 0x2000 + 0) = 1;
        FP2BYTE(prot_addr01 + 0x2000 + 1) = 0;
        FP2WORD(prot_addr01 + 0x2000 + 2) = timer_func_offset01;
        FP2WORD(prot_addr01 + 0x2000 + 4) = segment01;
        FP2WORD(prot_addr01 + 0x2000 + 6) = 0xffff;
        FP2WORD(prot_addr01 + 0x2000 + 8) = 0xffff;

        if ((ret01 = call_realmode_int(0x97, prot_addr01, segment01, &rmi_call_reg01, &rmi_call_reg01, 16)) == 0) {
            *timer_num01 = rmi_call_reg01.rmi_eax & 0xff;
        }
    }

    return ret01;
}

// タイマのカウント値の読み取り
unsigned int get_count_timer01(unsigned int segment01, int timer_num01) {
    unsigned char far *prot_addr01;
    struct rmi_call_reg rmi_call_reg01;
    int ret01;

    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000

    //      rmicall01でリアルモードBIOSをコール   タイマ読み取り int 97h, 機能コード02h
    if (ready_call_realmode_int()) {

        rmi_call_reg01.rmi_ds = segment01;
        rmi_call_reg01.rmi_edi = 0x2000;
        rmi_call_reg01.rmi_eax = 0x02 << 8 | (timer_num01 & 0xff);    // ah = 0x01;

        if ((ret01 = call_realmode_int(0x97, prot_addr01, segment01, &rmi_call_reg01, &rmi_call_reg01, 16)) == 0) {
        }
    }
    return ((unsigned int)FP2WORD(prot_addr01 + 0x2000 + 6) << 16) | (unsigned int)FP2WORD(prot_addr01 + 0x2000 + 8);
}

// タイマの終了
int close_timer01(int segment01, int timer_num01) {
    unsigned char far *prot_addr01;
    struct rmi_call_reg rmi_call_reg01;
    int ret01;


    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000

//      rmicall01でリアルモードBIOSをコール   タイマ取り消し int 97h, 機能コード01h
    if (ready_call_realmode_int()) {

        rmi_call_reg01.rmi_ds = segment01;
        rmi_call_reg01.rmi_eax = 0x01 << 8 | timer_num01;    // ah = 0x01;

        if ((ret01 = call_realmode_int(0x97, prot_addr01, segment01, &rmi_call_reg01, &rmi_call_reg01, 16)) == 0) {
        }
    }
    return ret01;
}

//    calbuf領域の情報を取得するときに呼び出す
//       (seg_prog01:off_prog01 リアルモードプログラム領域のアドレス、segment01:offset01:calbufデータ領域のアドレス
//          size01:calbuf領域の大きさ、prot_addr_seg01:prot_addr_off01:プロテクトモードからアクセスするときのアドレス(MK_FP()でfarポインタを作れる)
void get_call_buff_addr(unsigned short *seg_prog01, unsigned short *off_prog01, 
                unsigned short *segment01, unsigned short *offset01, unsigned int *size01,
                unsigned short *prot_addr_seg01, unsigned int *prot_addr_off01) {
    unsigned short seg_prog, off_prog, segment1, offset1;
    unsigned int   size1;
    unsigned short prot_addr_seg;
    unsigned int   prot_addr_off;
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds

        mov     ax, 250dh
        int     21h

        mov     off_prog, ax        ;// プロテクトモードコール用プロシージャのアドレス
        shr     eax, 16             ;// ss:sp→PWORD PTR FAR address of procedure to call
        mov     seg_prog, ax

        mov     offset1, bx        ;//共有バッファのセグメントとオフセット
        shr     ebx, 16
        mov     segment1, bx

        mov     size1, ecx         ;//共有バッファのサイズ

        mov     ax, es
        mov     prot_addr_seg, ax
        mov     prot_addr_off, edx

        pop     ds
        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
    *off_prog01 = off_prog;
    *seg_prog01 = seg_prog;
    *offset01 = offset1;
    *segment01 = segment1;
    *size01 = size1;
    *prot_addr_seg01 = prot_addr_seg;
    *prot_addr_off01 = prot_addr_off;
}

int get_segment_attribute(unsigned short segment_selector01, unsigned char *access_right_byte01, unsigned char *use_type_bit01) {
    unsigned char access_right_byte;
    unsigned char use_type_bit;
    int ret01;
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds

        mov     bx, segment_selector01
        mov     ax, 2515h
        int     21h

	jc	get_segment_attribute_jmp01
        pop     ds
        mov     dword ptr ret01, 0                ;//成功
        mov     access_right_byte, cl
        mov     use_type_bit, ch

	jmp     get_segment_attribute_jmp02
get_segment_attribute_jmp01:

        pop     ds
        mov     dword ptr ret01, -1               ;//失敗
        mov     byte ptr access_right_byte, 0
        mov     byte ptr use_type_bit, 0

get_segment_attribute_jmp02:


        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
    *access_right_byte01 = access_right_byte;
    *use_type_bit01 = use_type_bit;

    return ret01;
}

// パラグラフ単位でMSDOS領域のメモリを確保する
// segment01:取得したメモリのセグメントの値を返す
int allocate_msdos_memory_block(unsigned int seg_size01, unsigned short *segment01) {
    unsigned int   seg_size = seg_size01 / 16;
    unsigned short segment1;
    int ret01;
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds

        mov     ebx, seg_size
        mov     ax, 25c0h
        int     21h

        jc      allocate_msdos_memory_block_jmp01

        pop     ds
        mov     dword ptr ret01, 0                  ;//成功
        mov     word ptr segment1, ax

        jmp     allocate_msdos_memory_block_jmp02
allocate_msdos_memory_block_jmp01:

        pop     ds
        mov     dword ptr ret01, -1                 ;//失敗
        mov     word ptr segment1, 0

allocate_msdos_memory_block_jmp02:

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }
    *segment01 = segment1;

    return ret01;
}

int free_msdos_memory_block(unsigned short segment01) {
    unsigned int segment1 = (unsigned int)segment01;
    int ret01;
    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds

        mov     ecx, segment1
        mov     ax, 25c1h
        int     21h

        jc      free_msdos_memory_block_jmp01

        pop     ds
        mov     dword ptr ret01, 0                  ;//成功

        jmp     free_msdos_memory_block_jmp02
free_msdos_memory_block_jmp01:

        pop     ds
        mov     dword ptr ret01, -1                 ;//失敗

free_msdos_memory_block_jmp02:

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }

    return ret01;
}

// p_param_rm_call:関数コール時のレジスタの値を入れる構造体のポインタ
int call_realmode_procedure(unsigned int call_realmode_addr, unsigned int copy_stack_number, struct rm_call_reg *p_param_rm_call) {
    int ret01;
    int error_no01;

    __asm {
        push    es
        push    fs
        push    gs

        push    eax
        push    ebx
        push    ecx
        push    edx
        push    esi
        push    edi
        push    ebp

	push	ds

        mov     ebx, call_realmode_addr
        mov     ecx, copy_stack_number
        mov     edx, p_param_rm_call
        mov     ax, 2510h
        int     21h

        jc      call_realmode_procedure_jmp01
        pop     ds
        mov     dword ptr ret01, 0              ;//成功
        jmp     call_realmode_procedure_jmp02
call_realmode_procedure_jmp01:
        pop     ds
        mov     dword ptr ret01, -1             ;//失敗
call_realmode_procedure_jmp02:

        pop     ebp
        pop     edi
        pop     esi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

        pop     gs
        pop     fs
        pop     es
    }

    return ret01;
}

// リアルモードBIOSを呼ぶための初期化(リアルモードBIOSを呼び出すために必要なリアルモードのプログラムを読み込んでいる)
// protect_mode_addr:call_realmode_intで使う作業領域のMSDOS領域のアドレス(プロテクトモードからアクセスする際のアドレス)
int rmi_call_ready_flag = 0;
int init_call_realmode_int(unsigned char far *protect_mode_addr) {
    FILE *fp1;
    int  i;
    int  ret01;
    int  ch;

    if ((fp1 = fopen("rmical01.com", "rb")) != NULL) {
        i = 0;
        while (feof(fp1) == 0) {
            ch = fgetc(fp1);
            *(protect_mode_addr + 0x100 + i) = (char)ch;
            i++;
            if (i >= (65536 - 0x100)) break;
        }
        fclose(fp1);

        rmi_call_ready_flag = 1;
        ret01 = 0;
    } else {
        ret01 = -1;
    }
    return ret01;
}

int ready_call_realmode_int() {
    return rmi_call_ready_flag;
}

// リアルモードBIOSを呼び出すための関数
// int_no:ソフトウェア割り込みintの番号、protect_mode_addr:MSDOS領域の作業領域のプロテクトモードのアドレス(このアドレスから684バイト使用する)
// protect_mode_addr:MSDOS領域の作業領域のセグメントの値(このアドレスから684バイト使用する)
// rmi_call_in:BIOSのレジスタのパラメータ(入力)
// rmi_call_in:BIOSのレジスタのパラメータ(出力)
// copy_stack_num:コピーするスタックパラメータ領域のサイズ
int call_realmode_int(int int_no, unsigned char far *protect_mode_addr, unsigned short realmode_segment01, struct rmi_call_reg *rmi_call_in, struct rmi_call_reg *rmi_call_out, int copy_stack_num) {
    struct rm_call_reg *rm_call01;
    struct rm_call_reg rm_call001;
    unsigned int call_realmode_addr01;
    unsigned short far *rmi_data_tbl;
    int ret01;

    rm_call01 = &rm_call001;
    rmi_data_tbl = (unsigned short far *)(protect_mode_addr + 0x180);    // rmical01のデータのアドレスが格納されているテーブルを定義
    call_realmode_addr01 = (realmode_segment01 << 16 | 0x0000) + 0x100 + 0;                // rmical01のアドレス

    if (rmi_call_ready_flag != 0){

        *(protect_mode_addr + rmi_data_tbl[0] + 0) = (unsigned char)int_no;                         // rmi_int
        *(protect_mode_addr + rmi_data_tbl[0] + 1) = 0;                                           // zero
        *((unsigned short far *)(protect_mode_addr + rmi_data_tbl[0] + 2)) = rmi_call_in->rmi_ds;  // rmi_ds
        *((unsigned short far *)(protect_mode_addr + rmi_data_tbl[0] + 4)) = rmi_call_in->rmi_es;  // rmi_es
        *((unsigned short far *)(protect_mode_addr + rmi_data_tbl[0] + 6)) = rmi_call_in->rmi_fs;  // rmi_fs
        *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] +  8))  = rmi_call_in->rmi_eax; // rmi_eax
        *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 12))  = rmi_call_in->rmi_ebx; // rmi_ebx
        *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 16))  = rmi_call_in->rmi_ecx; // rmi_ecx
        *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 20))  = rmi_call_in->rmi_edx; // rmi_edx
        *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 24))  = rmi_call_in->rmi_esi; // rmi_esi
        *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 28))  = rmi_call_in->rmi_edi; // rmi_edi
        *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 32))  = rmi_call_in->rmi_ebp; // rmi_ebp


        rm_call01->rmc_ds = realmode_segment01;
        rm_call01->rmc_es = realmode_segment01;
        rm_call01->rmc_fs = realmode_segment01;
        rm_call01->rmc_gs = realmode_segment01;
        rm_call01->rmc_eax = 0x00;
        rm_call01->rmc_ebx = 0x00;
        rm_call01->rmc_ecx = 0x00;
        rm_call01->rmc_edx = 0x00;

        if (call_realmode_procedure(call_realmode_addr01, copy_stack_num, rm_call01) == 0) {

            rmi_call_out->rmi_ds  = *((unsigned short far *)(protect_mode_addr + rmi_data_tbl[0] + 2));  // rmi_ds
            rmi_call_out->rmi_es  = *((unsigned short far *)(protect_mode_addr + rmi_data_tbl[0] + 4));  // rmi_es
            rmi_call_out->rmi_fs  = *((unsigned short far *)(protect_mode_addr + rmi_data_tbl[0] + 6));  // rmi_fs
            rmi_call_out->rmi_eax = *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] +  8));   // rmi_eax
            rmi_call_out->rmi_ebx = *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 12));   // rmi_ebx
            rmi_call_out->rmi_ecx = *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 16));   // rmi_ecx
            rmi_call_out->rmi_edx = *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 20));   // rmi_edx
            rmi_call_out->rmi_esi = *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 24));   // rmi_esi
            rmi_call_out->rmi_edi = *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 28));   // rmi_edi
            rmi_call_out->rmi_ebp = *((unsigned int far *)(protect_mode_addr + rmi_data_tbl[0] + 32));   // rmi_ebp

            ret01 = 0;
        } else {
            ret01 = -1;
        }
    } else {
        ret01 = -2;
    }

    return ret01;
}

unsigned int read_data_segment(void) {
    unsigned int ret01;
    __asm {
        push   eax
        mov    ax, ds
        and    eax, 0ffffh
        mov    ret01, eax
        pop    eax
    }
    return ret01;
}

unsigned int read_code_segment(void) {
    unsigned int ret01;
    __asm {
        push   eax
        mov    ax, cs
        and    eax, 0ffffh
        mov    ret01, eax
        pop    eax
    }
    return ret01;
}
