// https://i-satoh.hatenablog.com/entry/65438175
// �����̃T�C�g��MotionJPEG�̍Đ��v���O�������Q�l�ɂ���
// FM TOWNS�œ���Đ��̃e�X�g�����Ă݂܂����B
// ����݂̂ŉ����͍Đ�����܂���
//
// ����̂�FM TOWNS��TOWNS OS�p�ł�
//
// JPegDecorder��emIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder
// ��JPEGDecoder.h��JPEGDecoder.cpp��picojpeg.h��picojpeg.c���Q�l�ɂ��Ď�������܂���
// botmer���񂪃����e�i���X����Ă�����̂�����悤�ł������Ȃ����������Ă���̂ł���ł͓��삵�܂���
// �܂��Abotmer���񂪃����e�i���X����Ă�����̂�RGB565��p�ɂȂ��Ă�悤�Ȃ̂Ŏg���܂���
//
// HDD�ɕۑ����ꂽMotionJPEG�̓����\��������̂ł��B
// ����͉f���݂̂̍Đ��ł��A�����͍Đ�����܂���B
// �����ԓ���̍Đ��͂ł��Ă܂��B
//
// ���̃v���O�����ɂ͓���t�@�C���͕t�����܂���
// 366�s�ڂ������loop�֐��̒��g��ҏW���Ă������̓���t�@�C�����Đ�����悤�ɕύX���Ă�������
// (�g���q�͑啶����.MJP�ɂ��Ă�������)
//
// �Ή����Ă��铮��`����MotionJPEG�ł��B
// �g���q�͑啶�����p��.MJP�ŕۑ����Ă��������B
// MotionJPEG�̓���t�@�C���̓p�\�R����ffmpeg���g���ĉ��L�̂悤�ɍ쐬�ł��܂��B
// ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
// ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
// ffmpeg -i test_in3.mp4 -b:v 750k  -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
// ffmpeg -i test_in4.mp4 -b:v 200k  -r 12.0 -s 120x90 -vcodec mjpeg -an test_o4.mov
// ffmpeg -i test_in5.mp6 -b:v 70k    -r 4.0 -s 120x90 -vcodec mjpeg -an test_o5.mov
// �g���q�͑啶����.MJP�ŕۑ����Ă��������B
//
// �����̃v���O�����̎g�p�ɓ������Ă͂������̐ӔC�ɂ����Ďg�p���Ă�������
// �����̃v���O�����Ŕ������������Ȃ鑹�Q�A�f�[�^�̏����A���K���̐ӔC�͈�ؕ����܂���

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <i86.h>
#include <dos.h>

// JPegDecorder��emIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder��JPEGDecoder.h��JPEGDecoder.cpp��picojpeg.h��picojpeg.c�Ɏ�����������̂��g�p���Ă��܂�
// https://i-satoh.hatenablog.com/entry/65438175
#include "JPEGDecoder/JPEGDecoder.h"



int work01[256*1024/4];

#define VRAM_SCREEN_WIDTH   (1024)
#define VRAM_SCREEN_HIGHT   (512)
#define SCREEN_WIDTH        (320)
#define SCREEN_HEIGHT       (240)

#define BLACK  0

#define WEB216_PALETTE_BLACK      0xd7     ///< 0x0000    0,   0,   0
#define WEB216_PALETTE_NAVY       0xda     ///< 0x0010    0,   0, 128
#define WEB216_PALETTE_DARKGREEN  0xdc     ///< 0x0400    0, 128,   0
#define WEB216_PALETTE_DARKCYAN   0xdb     ///< 0x0410    0, 125, 123
#define WEB216_PALETTE_MAROON     0xdf     ///< 0x8000  128,   0,   0
#define WEB216_PALETTE_PURPLE     0xde     ///< 0x8010  128,   0, 128
#define WEB216_PALETTE_OLIVE      0xdd     ///< 0x8400  128, 128,   0
#define WEB216_PALETTE_LIGHTGREY  0xd9     ///< 0xc618  198, 195, 198
#define WEB216_PALETTE_DARKGREY   0xd8     ///< 0x8410  128, 128, 128
#define WEB216_PALETTE_BLUE       0xd2     ///< 0x001f    0,   0, 255
#define WEB216_PALETTE_GREEN      0xd2     ///< 0x07e0    0, 255,   0
#define WEB216_PALETTE_CYAN       0xb4     ///< 0x07ff    0, 255, 255
#define WEB216_PALETTE_RED        0x23     ///< 0xf800  255,   0,   0
#define WEB216_PALETTE_MAGENTA    0x1e     ///< 0xf81f  255,   0, 255
#define WEB216_PALETTE_YELLOW     0x05     ///< 0xffe0  255, 255,   0
#define WEB216_PALETTE_WHITE      0x00     ///< 0xffff  255, 255, 255
#define WEB216_PALETTE_ORANGE     0x11     ///< 0xfcc0  255, 152,   0
#define WEB216_PALETTE_GREENYELLOW  0x4c   ///< 0x9fe6  152, 255,  48
#define WEB216_PALETTE_PINK       0x13     ///< 0xfb59  255, 104, 200

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
void clear_screen(void);
void draw_pixel(int x, int y, int color);
void set_palette(int pal_num01, int r01, int g01, int b01);
void draw_string(int x, int y, char *str1);
int set_screen_ratio(int x_ratio, int y_ratio);

#define drawPixel  draw_pixel_vram_16bit_02
void draw_pixel_vram_16bit(int x, int y, int color);
void draw_pixel_vram_16bit_02(int x, int y, int color);

void fillScreen(int color);
void fillRect(int x1, int y1, int x2, int y2, int color);
void resetPushColor(int x1, int y1, int x2, int y2);
void pushColor(uint32_t color);
void pushColors(uint8_t *data, int32_t size);
int color_web216(int color);

void int_to_str(char *str1, int a);
void uint_to_str(char *str1, unsigned int a);
void uint64_to_str(char *str1, unsigned long long a);
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

// call_realmode_procedure�Ŏg�����W�X�^��`    ���A�����[�h�̊֐����Ăяo�����Ɏg��
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

// call_realmode_int�Ŏg���֐���`    ���A�����[�h��BIOS�R�[�����Ăяo���Ƃ��Ɏg��
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


extern const uint8_t palette256_web216[];

void setup();
void loop();

char str0001[1000];
double fps0001 = 0.0;

char buf01[1024];
char buf02[256];

unsigned short segment01;
int timer_num01;
int main(int argc, char *argv[]) {
    int i;
    struct SREGS sregs;
    union REGS regs;
    unsigned int   seg_size01;
    unsigned char far *prot_addr01;
    int ret01;

    setup();

    // DOS�G�N�X�e���_�[�R�[����0x25c0���Ă�Ń��������m�ۂ��Ă�
    // (DOS�R�[����0x48��DOS�������ł͂Ȃ��āA�v���e�N�g���[�h�������̊m�ۂɂȂ��Ă�)
    // BX�Ƀp���O���t�P�ʂ̃������e�ʂ�����

    seg_size01 = 65536/16;
    if (allocate_msdos_memory_block(seg_size01 * 16, &segment01) != 0) {
        strcpy(buf01, "�������m�ۂɎ��s���܂���");
        draw_string(0, 20, buf01);

        // �L�[�o�b�t�@�̃N���A
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);

        while (!kbhit());

        exit(1);
    }

    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000


    set_bgcolor(12);
    clear_screen();

    strcpy(buf01, "�m�ۂ��������� = ");
    uint_to_str(buf02, (long long)seg_size01 * 16);
    strcat(buf01, buf02);
    strcat(buf01, " byte, segment01 = ");
    hex64_to_str(buf02, (long long)segment01);
    strcat(buf01, buf02);
    draw_string(0, 20, buf01);


    // timer�֐��Ŏg��BIOS�R�[���̂��߂̏�����
    if (init_call_realmode_int(prot_addr01) != 0) {
        set_bgcolor(12);
        clear_screen();

        strcpy(buf01, "init_call_realmode_int�����s���܂���");
        draw_string(0, 20, buf01);

        if (free_msdos_memory_block(segment01) != 0) {
            set_color(255);
            draw_string(0, 40, "segment01 ����������Ɏ��s���܂���");
        } else {
            set_color(255);
            draw_string(0, 40, "segment01 ����������ɐ������܂���");
        }

        // �L�[�o�b�t�@�̃N���A
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);

        while (!kbhit());

        exit(1);
    }

    // �^�C�}�[���g�����߂̊���
    if (init_timer01(segment01, &timer_num01)) {
        set_bgcolor(12);
        clear_screen();

        strcpy(buf01, "�^�C�}�̏������Ɏ��s���܂���");
        draw_string(0, 20, buf01);

        if (free_msdos_memory_block(segment01) != 0) {
            set_color(255);
            draw_string(0, 40, "segment01 ����������Ɏ��s���܂���");
        } else {
            set_color(255);
            draw_string(0, 40, "segment01 ����������ɐ������܂���");
        }

        // �L�[�o�b�t�@�̃N���A
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);

        while (!kbhit());

        exit(1);
    }

    //���C�����[�v
    for(;;) {
        loop();
        if (kbhit()) break;
    }


    ret01 = close_timer01(segment01, timer_num01);

    clear_screen();
    if (ret01 == (char)0) {
        strcpy(buf01, "int 97h ah=01h OK!  �^�C�}���OK");
        draw_string(0, 20, buf01);
    } else {
        strcpy(buf01, "int 97h ah=01h NG!  �^�C�}���NG");
        draw_string(0, 20, buf01);
    }

    if (free_msdos_memory_block(segment01) != 0) {
        set_color(255);
        draw_string(0, 40, "segment01 ����������Ɏ��s���܂���");
    } else {
        set_color(255);
        draw_string(0, 40, "segment01 ����������ɐ������܂���");
    }
    delay(1000);

    set_bgcolor(0);
    clear_screen();


    ginit(3, 0, (char *)work01, 256*1024);

    exit(0);
}


void setup() {
  int ret01, i;

  // �O���t�B�b�N�̏�����
  if (ret01 = ginit(12, 0, (char *)work01, 256*1024) != 0) {
       puts("ginit failed!");
       while(!kbhit());
       exit(1);
  }

  // �O���t�B�b�N�\���̔{����ݒ�(��2�{�A�c2�{�ɂ��邱�Ƃ�320x240������)
  set_screen_ratio(2, 2);

  set_bgcolor(0);
  clear_screen();

  // �p���b�g��Web�Z�[�t�J���[216�F�ɐݒ�
  for (i = 0; i < 256; i++) {
      set_palette(i, palette256_web216[i * 4 + 2], palette256_web216[i * 4 + 1], palette256_web216[i * 4 + 0]);
  }
}

void loop() {
    // drawJpgBmppicture_mjpeg(����t�@�C����, FPS)�Ŏw�肵�܂��B
    // MotionJPEG�̓���t�@�C���̓p�\�R����ffmpeg���g���ĉ��L�̂悤�ɍ쐬�ł��܂��B
    // ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
    // ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
    // ffmpeg -i test_in3.mp4 -b:v 750k -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
    // ffmpeg -i test_in4.mp4 -b:v 200k  -r 12.0 -s 120x90 -vcodec mjpeg -an test_o4.mov
    // ffmpeg -i test_in5.mp6 -b:v 70k    -r 4.0 -s 120x90 -vcodec mjpeg -an test_o5.mov
    // ���̃v���O������MotionJPEG�̃t�@�C���̊g���q��.MJP�Ƃ��ĔF������̂Ŋg���q��.MJP�ɕύX���Ă��������B
    // JPEG�摜��BMP�摜�̕\�����ł��܂����A�\������Ȃ��ꍇ�͈�xWindows�t���̃y�C���g�ŕۑ����Ă��玎���Ă݂Ă��������B

    // �Î~����\���ł��܂�
    fillScreen(WEB216_PALETTE_GREEN);
    drawJpgBmppicture_mjpeg("SNAIL01.JPG", 0);
    delay(1000);
    drawJpgBmppicture_mjpeg("SCENIC01.BMP", 0);
    delay(1000);

/*
    fillScreen(WEB216_PALETTE_BLUE);
    drawJpgBmppicture_mjpeg("test_o1.MJP", 13);
    delay(5000);

    fillScreen(WEB216_PALETTE_BLUE );
    drawJpgBmppicture_mjpeg("test_o2.MJP", 22);
    delay(5000);

    fillScreen(WEB216_PALETTE_BLUE);
    drawJpgBmppicture_mjpeg("test_o3.MJP", 38);
    delay(5000);
*/

    fillScreen(WEB216_PALETTE_BLACK);

    // ����̕\��(�t�@�C������FPS��ݒ肵�܂�
    drawJpgBmppicture_mjpeg("V1005FPS.MJP", 5);
    delay(5000);


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

  set_color(WEB216_PALETTE_WHITE);
  set_bgcolor(WEB216_PALETTE_BLACK);
  clear_screen();

  // check BMP/JPEG file
  ext = strrchr(fname, '.');
  // if(!ext || (memcmp(ext + 1, "BMP", 3) && memcmp(ext + 1, "JPG", 3) && memcmp(ext + 1, "MJP", 3))) return false;
  if (!ext) {
    fillScreen(WEB216_PALETTE_PURPLE);
    return false;
  }
  if (!(!memcmp(ext + 1, "BMP", 3) || !memcmp(ext + 1, "JPG", 3) || !memcmp(ext + 1, "MJP", 3))) {
    fillScreen(WEB216_PALETTE_YELLOW);
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
    struct rgb01 col = color01(BLACK);
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

//        �f���̏㑤������
//        if(yofset > 0) fillRect(0, 0, _width, yofset, color_web216(BLACK));
        // set left & right black belt
        if(xofset > 0){
//          �f���̍��E������
//          fillRect(0, (yofset > 0) ? yofset : 0, xofset, (yofset > 0) ? JpegDec.height : _height, color_web216(BLACK));
//          fillRect(xofset + JpegDec.width, (yofset > 0) ? yofset : 0, _width - JpegDec.width - xofset, (yofset > 0) ? JpegDec.height : _height, color_web216(BLACK));
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
            if(x >= JpegDec.width) col = color01(BLACK);
            if(((xofset + x) >= 0) && ((yofset + y) >= 0) && ((xofset + x) < _width) && ((yofset + y) < _height))   clbuf[clbufpt++] = col;
            pImg += JpegDec.comps;
          }
        }
        // draw picture
        if(clbufpt) pushColors((uint8_t *)clbuf, clbufpt);
      }

      free(clbuf);

       // �f���̉�����`��
//      // set bottom black belt
//      if(yofset > 0) fillRect(0, yofset + JpegDec.height, _width, _height - (yofset + JpegDec.height), color_web216(BLACK));

      // check next picture wait timing
      if(JpegDec.mjpegflag){
        picdly = nxtdt - millis01();


        fillRect(0, 0, 50, 23, WEB216_PALETTE_BLACK);
        int_to_str(str0001, (int)(fps0001 * 1000.0));
//        sprintf(str0001, "%.4f", fps0001);
        draw_string(0, 20, str0001);


        if(picdly > 0) delay(picdly);
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
            // pushColor(BLACK);
            blkblt2[x] = color01(BLACK);
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

    drawPixel(pushColor_x1, pushColor_y1, color_web216(color));
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
            color01 = 215 - ((((int)*p++) / 51) * 36 + (((int)*p++) / 51) * 6 + (((int)*p++) / 51));
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

    addr01 = (char *)(((y * VRAM_SCREEN_WIDTH + x)));

    __asm {
        push   fs
	push   ebx
        mov    ax, 010ch           ;16,32768color = 0x0104    256color = 0x010c
        mov    fs, ax
        mov    ebx, addr01
        mov    eax, color
        mov    fs:[ebx], al
        pop    ebx
        pop    fs
    }
}

extern char far *vram01;
void draw_pixel_vram_16bit_02(int x, int y, int color) {
//    if (x < 0 || x >= SCREEN_WIDTH) return;
//    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    *(vram01 + (y * VRAM_SCREEN_WIDTH + x)) = (char)color;
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

int color_web216(int color) {
    int r, g, b, color01;
    r = (color >> 16) / 51;            // 255 / 5 = 51
    g = (color >> 8) / 51;
    b = (color) / 51;
    color01 = 215 - (r * 36 + g * 6 + b);
    return color01;
}

struct view_port0001 {
    short  x1;
    short  y1;
    short  x2;
    short  y2;
} view_port01;
char far *vram01;
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
    view_port01.x2 = 639;
    view_port01.y2 = 479;
    view_port = &view_port01;

    ginit_page01 = page1;
    vram01 = (char far *)MK_FP(0x010c, 0x00000000); // 16,32768color = 0x0104    256color = 0x010c

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

void uint_to_str(char *str1, unsigned int a) {
    int i;
    char str[10];
    char *p, *p2;
    for (i = 0; i < 10; i++) {
        str[i] = (char)0;
    }
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

//    �g����
//        uint64_to_str(buf01, 18446744073709551615ULL);
//        uint64_to_str(buf01, 0xffffffffffffffffULL);
void uint64_to_str(char *str1, unsigned long long a01) {
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

const uint8_t palette256_web216[] = {
    0xff,0xff,0xff,0x00,0xcc,0xff,0xff,0x00,0x99,0xff,0xff,0x00,0x66,0xff,0xff,0x00,0x33,0xff,0xff,0x00,
    0x00,0xff,0xff,0x00,0xff,0xcc,0xff,0x00,0xcc,0xcc,0xff,0x00,0x99,0xcc,0xff,0x00,0x66,0xcc,0xff,0x00,
    0x33,0xcc,0xff,0x00,0x00,0xcc,0xff,0x00,0xff,0x99,0xff,0x00,0xcc,0x99,0xff,0x00,0x99,0x99,0xff,0x00,
    0x66,0x99,0xff,0x00,0x33,0x99,0xff,0x00,0x00,0x99,0xff,0x00,0xff,0x66,0xff,0x00,0xcc,0x66,0xff,0x00,
    0x99,0x66,0xff,0x00,0x66,0x66,0xff,0x00,0x33,0x66,0xff,0x00,0x00,0x66,0xff,0x00,0xff,0x33,0xff,0x00,
    0xcc,0x33,0xff,0x00,0x99,0x33,0xff,0x00,0x66,0x33,0xff,0x00,0x33,0x33,0xff,0x00,0x00,0x33,0xff,0x00,
    0xff,0x00,0xff,0x00,0xcc,0x00,0xff,0x00,0x99,0x00,0xff,0x00,0x66,0x00,0xff,0x00,0x33,0x00,0xff,0x00,
    0x00,0x00,0xff,0x00,0xff,0xff,0xcc,0x00,0xcc,0xff,0xcc,0x00,0x99,0xff,0xcc,0x00,0x66,0xff,0xcc,0x00,
    0x33,0xff,0xcc,0x00,0x00,0xff,0xcc,0x00,0xff,0xcc,0xcc,0x00,0xcc,0xcc,0xcc,0x00,0x99,0xcc,0xcc,0x00,
    0x66,0xcc,0xcc,0x00,0x33,0xcc,0xcc,0x00,0x00,0xcc,0xcc,0x00,0xff,0x99,0xcc,0x00,0xcc,0x99,0xcc,0x00,
    0x99,0x99,0xcc,0x00,0x66,0x99,0xcc,0x00,0x33,0x99,0xcc,0x00,0x00,0x99,0xcc,0x00,0xff,0x66,0xcc,0x00,
    0xcc,0x66,0xcc,0x00,0x99,0x66,0xcc,0x00,0x66,0x66,0xcc,0x00,0x33,0x66,0xcc,0x00,0x00,0x66,0xcc,0x00,
    0xff,0x33,0xcc,0x00,0xcc,0x33,0xcc,0x00,0x99,0x33,0xcc,0x00,0x66,0x33,0xcc,0x00,0x33,0x33,0xcc,0x00,
    0x00,0x33,0xcc,0x00,0xff,0x00,0xcc,0x00,0xcc,0x00,0xcc,0x00,0x99,0x00,0xcc,0x00,0x66,0x00,0xcc,0x00,
    0x33,0x00,0xcc,0x00,0x00,0x00,0xcc,0x00,0xff,0xff,0x99,0x00,0xcc,0xff,0x99,0x00,0x99,0xff,0x99,0x00,
    0x66,0xff,0x99,0x00,0x33,0xff,0x99,0x00,0x00,0xff,0x99,0x00,0xff,0xcc,0x99,0x00,0xcc,0xcc,0x99,0x00,
    0x99,0xcc,0x99,0x00,0x66,0xcc,0x99,0x00,0x33,0xcc,0x99,0x00,0x00,0xcc,0x99,0x00,0xff,0x99,0x99,0x00,
    0xcc,0x99,0x99,0x00,0x99,0x99,0x99,0x00,0x66,0x99,0x99,0x00,0x33,0x99,0x99,0x00,0x00,0x99,0x99,0x00,
    0xff,0x66,0x99,0x00,0xcc,0x66,0x99,0x00,0x99,0x66,0x99,0x00,0x66,0x66,0x99,0x00,0x33,0x66,0x99,0x00,
    0x00,0x66,0x99,0x00,0xff,0x33,0x99,0x00,0xcc,0x33,0x99,0x00,0x99,0x33,0x99,0x00,0x66,0x33,0x99,0x00,
    0x33,0x33,0x99,0x00,0x00,0x33,0x99,0x00,0xff,0x00,0x99,0x00,0xcc,0x00,0x99,0x00,0x99,0x00,0x99,0x00,
    0x66,0x00,0x99,0x00,0x33,0x00,0x99,0x00,0x00,0x00,0x99,0x00,0xff,0xff,0x66,0x00,0xcc,0xff,0x66,0x00,
    0x99,0xff,0x66,0x00,0x66,0xff,0x66,0x00,0x33,0xff,0x66,0x00,0x00,0xff,0x66,0x00,0xff,0xcc,0x66,0x00,
    0xcc,0xcc,0x66,0x00,0x99,0xcc,0x66,0x00,0x66,0xcc,0x66,0x00,0x33,0xcc,0x66,0x00,0x00,0xcc,0x66,0x00,
    0xff,0x99,0x66,0x00,0xcc,0x99,0x66,0x00,0x99,0x99,0x66,0x00,0x66,0x99,0x66,0x00,0x33,0x99,0x66,0x00,
    0x00,0x99,0x66,0x00,0xff,0x66,0x66,0x00,0xcc,0x66,0x66,0x00,0x99,0x66,0x66,0x00,0x66,0x66,0x66,0x00,
    0x33,0x66,0x66,0x00,0x00,0x66,0x66,0x00,0xff,0x33,0x66,0x00,0xcc,0x33,0x66,0x00,0x99,0x33,0x66,0x00,
    0x66,0x33,0x66,0x00,0x33,0x33,0x66,0x00,0x00,0x33,0x66,0x00,0xff,0x00,0x66,0x00,0xcc,0x00,0x66,0x00,
    0x99,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x33,0x00,0x66,0x00,0x00,0x00,0x66,0x00,0xff,0xff,0x33,0x00,
    0xcc,0xff,0x33,0x00,0x99,0xff,0x33,0x00,0x66,0xff,0x33,0x00,0x33,0xff,0x33,0x00,0x00,0xff,0x33,0x00,
    0xff,0xcc,0x33,0x00,0xcc,0xcc,0x33,0x00,0x99,0xcc,0x33,0x00,0x66,0xcc,0x33,0x00,0x33,0xcc,0x33,0x00,
    0x00,0xcc,0x33,0x00,0xff,0x99,0x33,0x00,0xcc,0x99,0x33,0x00,0x99,0x99,0x33,0x00,0x66,0x99,0x33,0x00,
    0x33,0x99,0x33,0x00,0x00,0x99,0x33,0x00,0xff,0x66,0x33,0x00,0xcc,0x66,0x33,0x00,0x99,0x66,0x33,0x00,
    0x66,0x66,0x33,0x00,0x33,0x66,0x33,0x00,0x00,0x66,0x33,0x00,0xff,0x33,0x33,0x00,0xcc,0x33,0x33,0x00,
    0x99,0x33,0x33,0x00,0x66,0x33,0x33,0x00,0x33,0x33,0x33,0x00,0x00,0x33,0x33,0x00,0xff,0x00,0x33,0x00,
    0xcc,0x00,0x33,0x00,0x99,0x00,0x33,0x00,0x66,0x00,0x33,0x00,0x33,0x00,0x33,0x00,0x00,0x00,0x33,0x00,
    0xff,0xff,0x00,0x00,0xcc,0xff,0x00,0x00,0x99,0xff,0x00,0x00,0x66,0xff,0x00,0x00,0x33,0xff,0x00,0x00,
    0x00,0xff,0x00,0x00,0xff,0xcc,0x00,0x00,0xcc,0xcc,0x00,0x00,0x99,0xcc,0x00,0x00,0x66,0xcc,0x00,0x00,
    0x33,0xcc,0x00,0x00,0x00,0xcc,0x00,0x00,0xff,0x99,0x00,0x00,0xcc,0x99,0x00,0x00,0x99,0x99,0x00,0x00,
    0x66,0x99,0x00,0x00,0x33,0x99,0x00,0x00,0x00,0x99,0x00,0x00,0xff,0x66,0x00,0x00,0xcc,0x66,0x00,0x00,
    0x99,0x66,0x00,0x00,0x66,0x66,0x00,0x00,0x33,0x66,0x00,0x00,0x00,0x66,0x00,0x00,0xff,0x33,0x00,0x00,
    0xcc,0x33,0x00,0x00,0x99,0x33,0x00,0x00,0x66,0x33,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x33,0x00,0x00,
    0xff,0x00,0x00,0x00,0xcc,0x00,0x00,0x00,0x99,0x00,0x00,0x00,0x66,0x00,0x00,0x00,0x33,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,0xc0,0xc0,0xc0,0x00,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,
    0x00,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0xff,0xff,0xff,0x00,
    0xf6,0xf6,0xf6,0x00,0xee,0xee,0xee,0x00,0xe6,0xe6,0xe6,0x00,0xde,0xde,0xde,0x00,0xd5,0xd5,0xd5,0x00,
    0xcd,0xcd,0xcd,0x00,0xc5,0xc5,0xc5,0x00,0xbd,0xbd,0xbd,0x00,0xb4,0xb4,0xb4,0x00,0xac,0xac,0xac,0x00,
    0xa4,0xa4,0xa4,0x00,0x9c,0x9c,0x9c,0x00,0x94,0x94,0x94,0x00,0x8b,0x8b,0x8b,0x00,0x83,0x83,0x83,0x00,
    0x7b,0x7b,0x7b,0x00,0x73,0x73,0x73,0x00,0x6a,0x6a,0x6a,0x00,0x62,0x62,0x62,0x00,0x5a,0x5a,0x5a,0x00,
    0x52,0x52,0x52,0x00,0x4a,0x4a,0x4a,0x00,0x41,0x41,0x41,0x00,0x39,0x39,0x39,0x00,0x31,0x31,0x31,0x00,
    0x29,0x29,0x29,0x00,0x20,0x20,0x20,0x00,0x18,0x18,0x18,0x00,0x10,0x10,0x10,0x00,0x08,0x08,0x08,0x00,
    0x00,0x00,0x00,0x00
};





// �^�C�}�[���g�����߂̏���
int init_timer01(unsigned int segment01, int *timer_num01) {
    unsigned char far *prot_addr01;
    struct rmi_call_reg rmi_call_reg01;
    unsigned int timer_func_offset01;
    int ret01;

    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000

//      rmicall01�Ń��A�����[�hBIOS���R�[��   �^�C�}�̓o�^ int 97h, �@�\�R�[�h00h
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

// �^�C�}�̃J�E���g�l�̓ǂݎ��
unsigned int get_count_timer01(unsigned int segment01, int timer_num01) {
    unsigned char far *prot_addr01;
    struct rmi_call_reg rmi_call_reg01;
    int ret01;

    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000

    //      rmicall01�Ń��A�����[�hBIOS���R�[��   �^�C�}�ǂݎ�� int 97h, �@�\�R�[�h02h
    if (ready_call_realmode_int()) {

        rmi_call_reg01.rmi_ds = segment01;
        rmi_call_reg01.rmi_edi = 0x2000;
        rmi_call_reg01.rmi_eax = 0x02 << 8 | (timer_num01 & 0xff);    // ah = 0x01;

        if ((ret01 = call_realmode_int(0x97, prot_addr01, segment01, &rmi_call_reg01, &rmi_call_reg01, 16)) == 0) {
        }
    }
    return ((unsigned int)FP2WORD(prot_addr01 + 0x2000 + 6) << 16) | (unsigned int)FP2WORD(prot_addr01 + 0x2000 + 8);
}

// �^�C�}�̏I��
int close_timer01(int segment01, int timer_num01) {
    unsigned char far *prot_addr01;
    struct rmi_call_reg rmi_call_reg01;
    int ret01;


    prot_addr01 = (unsigned char far *)MK_FP(0x34, segment01 * 16 + 0);     //  segment01:0000

//      rmicall01�Ń��A�����[�hBIOS���R�[��   �^�C�}������ int 97h, �@�\�R�[�h01h
    if (ready_call_realmode_int()) {

        rmi_call_reg01.rmi_ds = segment01;
        rmi_call_reg01.rmi_eax = 0x01 << 8 | timer_num01;    // ah = 0x01;

        if ((ret01 = call_realmode_int(0x97, prot_addr01, segment01, &rmi_call_reg01, &rmi_call_reg01, 16)) == 0) {
        }
    }
    return ret01;
}

//    calbuf�̈�̏����擾����Ƃ��ɌĂяo��
//       (seg_prog01:off_prog01 ���A�����[�h�v���O�����̈�̃A�h���X�Asegment01:offset01:calbuf�f�[�^�̈�̃A�h���X
//          size01:calbuf�̈�̑傫���Aprot_addr_seg01:prot_addr_off01:�v���e�N�g���[�h����A�N�Z�X����Ƃ��̃A�h���X(MK_FP()��far�|�C���^������)
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

        mov     off_prog, ax        ;// �v���e�N�g���[�h�R�[���p�v���V�[�W���̃A�h���X
        shr     eax, 16             ;// ss:sp��PWORD PTR FAR address of procedure to call
        mov     seg_prog, ax

        mov     offset1, bx        ;//���L�o�b�t�@�̃Z�O�����g�ƃI�t�Z�b�g
        shr     ebx, 16
        mov     segment1, bx

        mov     size1, ecx         ;//���L�o�b�t�@�̃T�C�Y

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
        mov     dword ptr ret01, 0                ;//����
        mov     access_right_byte, cl
        mov     use_type_bit, ch

	jmp     get_segment_attribute_jmp02
get_segment_attribute_jmp01:

        pop     ds
        mov     dword ptr ret01, -1               ;//���s
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

// �p���O���t�P�ʂ�MSDOS�̈�̃��������m�ۂ���
// segment01:�擾�����������̃Z�O�����g�̒l��Ԃ�
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
        mov     dword ptr ret01, 0                  ;//����
        mov     word ptr segment1, ax

        jmp     allocate_msdos_memory_block_jmp02
allocate_msdos_memory_block_jmp01:

        pop     ds
        mov     dword ptr ret01, -1                 ;//���s
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
        mov     dword ptr ret01, 0                  ;//����

        jmp     free_msdos_memory_block_jmp02
free_msdos_memory_block_jmp01:

        pop     ds
        mov     dword ptr ret01, -1                 ;//���s

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

// p_param_rm_call:�֐��R�[�����̃��W�X�^�̒l������\���̂̃|�C���^
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
        mov     dword ptr ret01, 0              ;//����
        jmp     call_realmode_procedure_jmp02
call_realmode_procedure_jmp01:
        pop     ds
        mov     dword ptr ret01, -1             ;//���s
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

// ���A�����[�hBIOS���ĂԂ��߂̏�����(���A�����[�hBIOS���Ăяo�����߂ɕK�v�ȃ��A�����[�h�̃v���O������ǂݍ���ł���)
// protect_mode_addr:call_realmode_int�Ŏg����Ɨ̈��MSDOS�̈�̃A�h���X(�v���e�N�g���[�h����A�N�Z�X����ۂ̃A�h���X)
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

// ���A�����[�hBIOS���Ăяo�����߂̊֐�
// int_no:�\�t�g�E�F�A���荞��int�̔ԍ��Aprotect_mode_addr:MSDOS�̈�̍�Ɨ̈�̃v���e�N�g���[�h�̃A�h���X(���̃A�h���X����684�o�C�g�g�p����)
// protect_mode_addr:MSDOS�̈�̍�Ɨ̈�̃Z�O�����g�̒l(���̃A�h���X����684�o�C�g�g�p����)
// rmi_call_in:BIOS�̃��W�X�^�̃p�����[�^(����)
// rmi_call_in:BIOS�̃��W�X�^�̃p�����[�^(�o��)
// copy_stack_num:�R�s�[����X�^�b�N�p�����[�^�̈�̃T�C�Y
int call_realmode_int(int int_no, unsigned char far *protect_mode_addr, unsigned short realmode_segment01, struct rmi_call_reg *rmi_call_in, struct rmi_call_reg *rmi_call_out, int copy_stack_num) {
    struct rm_call_reg *rm_call01;
    struct rm_call_reg rm_call001;
    unsigned int call_realmode_addr01;
    unsigned short far *rmi_data_tbl;
    int ret01;

    rm_call01 = &rm_call001;
    rmi_data_tbl = (unsigned short far *)(protect_mode_addr + 0x180);    // rmical01�̃f�[�^�̃A�h���X���i�[����Ă���e�[�u�����`
    call_realmode_addr01 = (realmode_segment01 << 16 | 0x0000) + 0x100 + 0;                // rmical01�̃A�h���X

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
