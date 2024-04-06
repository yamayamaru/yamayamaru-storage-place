/*

コンパイル方法：
wcc386 -3r -zq -zk0 -ox -fo=testrmicall01.obj trmical1.c
wlink system pharlap runtime CALLBUFS=65536 option quiet name trmical1 file{ trmical1.obj }

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include <dos.h>

#include <conio.h>      // outp()、inp()に必要

int work01[256*1024/4];

#define VRAM_SCREEN_WIDTH  (1024)
#define VRAM_SCREEN_HIGHT   (512)
#define SCREEN_WIDTH        (640)
#define SCREEN_HEIGHT       (480)

#define NUM_COLOR           (256)

int ginit(int mode, char *workaddr, int worksize);
void set_color(int color);
void set_bgcolor(int color);
void clear_screen(void);
void draw_pixel(int x, int y, int color);
void set_palette(int pal_num01, int r01, int g01, int b01);
void draw_string(int x, int y, char *str1);
void draw_rect(int x1, int y1, int x2, int y2);
void draw_line(int x1, int y1, int x2, int y2);
void draw_fillrect(int x1, int y1, int x2, int y2);
int kbhit(void);

void draw_pixel_vram(int x, int y, int color);
void draw_pixel_vram02(int x, int y, int color);

void uint_to_str(char *str1, int a);
void uint64_to_str(char *str1, long long a);
void hex64_to_str(char *str1, long long a);

void get_call_buff_addr(unsigned short *seg_prog01, unsigned short *off_prog01, 
                unsigned short *segment01, unsigned short *offset01, unsigned int *size01,
                unsigned short *prot_addr_seg01, unsigned int *prot_addr_off01);
int call_realmode_procedure(unsigned int call_realmode_addr, unsigned int copy_stack_number, struct rm_call *p_param_rm_call);

int init_rmi_call(unsigned char far *protect_mode_addr, int size);
int rmi_call(int int_no, unsigned char far *protect_mode_addr, unsigned short segment01,
    struct rmi_call *rmi_call_in, struct rmi_call *rmi_call_out, int stack_num);

unsigned int read_data_segment(void);
unsigned int read_code_segment(void);

void rs232c_print_str(char *str1);
int rs232c_write_ready(void);
int rs232c_read_ready(void);
int rs232c_read(void);
int rs232c_write(int send_data);
int rs232c_send_data(int send_data);
int rs232c_send_data_not_ready(int send_data, int wait);


struct rm_call {
    unsigned short rmc_ds;
    unsigned short rmc_es;
    unsigned short rmc_fs;
    unsigned short rmc_gs;
    unsigned int   rmc_eax;
    unsigned int   rmc_ebx;
    unsigned int   rmc_ecx;
    unsigned int   rmc_edx;
};

struct rmi_call {
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

char buf01[1024];
char buf02[256];

unsigned short seg_prog01, off_prog01, segment01, offset01;
unsigned int   size01;
unsigned short prot_addr_seg01;
unsigned int   prot_addr_off01;
unsigned char far *prot_addr01;
struct rmi_call rmi_call01;

int wait0001(long wait);

int main(int argc, char *argv[]) {
    union REGS regs;

    int year01, mon01, day01, week01;
    int hour01, min01, sec01, hsec01;


    if (ginit(12, (char *)work01, 256*1024) != 0) {
        exit(1);
    }
    set_bgcolor(12);
    clear_screen();

    // CALLBUFS領域のリアルモードで使うセグメント(segment01)、オフセット(offset01)と
    // CALLBUFS領域のサイズ(size01)とプロテクトモードから使うセグメント(prot_addr_seg01)、オフセットを取得(prot_addr_off01)
    get_call_buff_addr(&seg_prog01, &off_prog01, &segment01, &offset01, &size01, &prot_addr_seg01, &prot_addr_off01);

    // CALLBUFSにはプロテクトモードからはfarポインタを使ってアクセスするためそのfarポインタを作成
    prot_addr01 = MK_FP(prot_addr_seg01, prot_addr_off01);

    // RUN386ではセグメント0x34でもリアルモードのアドレスにアクセスできます
//    prot_addr01 = MK_FP(0x34, segment01 * 16 + offset01);        //  segment01:offset01


    // リアルモードDOSコールやリアルモードBIOSを呼び出すためのリアルモードのプログラムを読み込んでます
    // このリアルモードのプログラムにレジスタ値を渡して、実際にソフトウェア割り込みを実行させてます
    if (init_rmi_call(prot_addr01, size01) != 0) {
        set_bgcolor(12);
        clear_screen();

        set_color(255);
        draw_string(0, 40, "rmical01.comのロードに失敗しました");
        draw_string(0, 60, "rmical01.comを本プログラムと同じ場所に置いてください");

        draw_string(0, 100, "何かキーを押してください");

        // キーバッファのクリア
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);
        // キー入力があるまで待機
        while (!kbhit());

        exit(1);
    }


    set_color(255);
    draw_string(0, 60, "終了するには何かキーを押してください");

    // キーバッファのクリア
    regs.h.ah = 0x06;
    int386(0x90, &regs, &regs);

    while (!kbhit()) {

        // 日付、時刻読み取りのリアルモードのTBIOSコール(一応念のためDSセグメントの値も設定してます)
        rmi_call01.rmi_ds = segment01;
        rmi_call01.rmi_edi = 0x1000;
        rmi_call01.rmi_eax = 0x01 << 8;    // ah = 0x01;

        // CALBUFS領域に置いたリアルモードのプログラムを呼び出してリアルモードBIOSを呼び出してます
        if (rmi_call(0x96, prot_addr01, segment01, &rmi_call01, &rmi_call01, 16) == 0) {
            if (((rmi_call01.rmi_eax >> 8) & 0xff) == 0) {
                year01 = *((unsigned short far *)(prot_addr01 + 0x1000));
                mon01  = *(prot_addr01 + 0x1000 + 2);
                day01  = *(prot_addr01 + 0x1000 + 3);
                week01 = *(prot_addr01 + 0x1000 + 4);
                hour01 = *(prot_addr01 + 0x1000 + 5);
                min01  = *(prot_addr01 + 0x1000 + 6);
                sec01  = *(prot_addr01 + 0x1000 + 7);
                hsec01 = *(prot_addr01 + 0x1000 + 8);
            }
        }

    //  日付、時刻読み取りのリアルモードのTBIOSコールで取得した日時のデータを表示してます
    //  sprintf()を使うとTOWNSのエミュレータで互換BIOSを使っていた場合に落ちるのでsprintfは使ってません
        set_color(12);
        draw_fillrect(0, 0, 639, 40);
        set_color(255);
        strcpy(buf01, "現在の日時 = ");
        uint_to_str(buf02, year01);
        strcat(buf01, buf02);
        strcat(buf01, "年");
        uint_to_str(buf02, mon01);
        strcat(buf01, buf02);
        strcat(buf01, "月");
        uint_to_str(buf02, day01);
        strcat(buf01, buf02);
        strcat(buf01, "日");
        uint_to_str(buf02, week01);
        strcat(buf01, buf02);
        strcat(buf01, "曜日 ");
        
        uint_to_str(buf02, hour01);
        strcat(buf01, buf02);
        strcat(buf01, "時");
        uint_to_str(buf02, min01);
        strcat(buf01, buf02);
        strcat(buf01, "分");
        uint_to_str(buf02, sec01);
        strcat(buf01, buf02);
        strcat(buf01, "秒");

        draw_string(0,20, buf01);

        wait0001(1000000);
    }

    ginit(3, (char *)work01, 256*1024);


    exit(0);
}

int wait0001(long wait) {
    long i, a;
    a = 0;
    for (i = 0; i < wait; i++) {
        a = i + i;
    }
    return (int)a;
}

void rs232c_init(void) {
    // 8251初期化
    outp(0xa02, 0x0);
    outp(0xa02, 0x0);
    outp(0xa02, 0x0);
    outp(0xa02, 0x40);     // リセット
    outp(0xa02, 0x4e);     // 0b01001110    bit7,bit6=データビット1、bit5=パリティ奇数、bit4=パリティなし、bit3,bit2=データ8bit、bit1,bit0=通信速度x16
    outp(0xa02, 0x37);     // 0b00110111    bit5:RTS=1、bit4:エラーリセット、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
    // 8253初期化(4800bps) RS232C用タイマは2個目の8253のチャネル1(つまりチャンネル4)
    outp(0x56, 0x76);      // 2個目の8253コントロールレジスタ  0b01110110 bit7,bit6=チャネル1(4)、bit5,bit4=lsb,msb連続リードライト、bit3,bit2,bit1=モード3、bit0=バイナリカウント
    outp(0x52, 16);        // 2個目の8253 チャンネル1(4)カウントレジスタ カウント値LSB 16    1/16のとき4800bps
    outp(0x52, 0x00);      // 2個目の8253 チャンネル1(4)カウントレジスタ カウント値MSB  0
    
}

// RS232Cに出力可能か確認
int rs232c_write_ready(void) {
    int ret01;

    if ((inp(0xa02) & 0x01) == 1) ret01 = 1;
    else                                 ret01 = 0;

    return ret01;
}

// RS232Cで入力データが来ているか確認
int rs232c_read_ready(void) {
    int ret01;

    if ((inp(0xa02) & 0x02) == 2) ret01 = 1;
    else                                 ret01 = 0;

    return ret01;
}

// RS232Cデータ入力、読み込めなくてもすぐ戻ってくる、読み込みビジー : -1、読み込み成功 : 読み込みデータ
int rs232c_read(void) {
    int ret01, ch;

    outp(0xa02, 0x37);     // 0b00110111    bit5:RTS=1、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
    if (((ch = inp(0xa02)) & 0x02) == 2) {
        ret01 = ((int)inp(0xa00) & 0xff);
    } else {
        ret01 = -1;
    }

    return ret01;
}

// RS232Cデータ書き込み、書き込めなくてもすぐ戻ってくる、書き込みビジー : -1、書き込み成功 : 0
int rs232c_write(int send_data) {
    int ch, i, ret01;

    if (((ch = inp(0xa02)) & 0x1) == 1) {
        outp(0xa02, 0x17);     // 0b00010111    bit5:RTS=0、bit4:エラーリセット、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
        outp(0xa00, (char)send_data);
        outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
        ret01 = 0;
    } else {
        ret01 = -1;
    }

    return ret01;
}

// データの書き込み(書き込み終了まで一定時間戻ってこない)
int rs232c_send_data(int send_data) {
    int ch, i, wait01;

    outp(0xa02, 0x17);     // 0b00010111    bit5:RTS=0、bit4:エラーリセット、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
    wait01 = 1000000;
    for (i = 0; i < wait01; i++) {
        if (((ch = inp(0xa02)) & 0x1) == 1) break;
    }
    if (i >= wait01) {
        outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
        return -1;
    }
    outp(0xa00, (char)send_data);
    outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル

    return 0;
}

int rs232c_send_data_not_ready(int send_data, int wait) {
    int ch, i, wait01;

    outp(0xa02, 0x17);     // 0b00010111    bit5:RTS=0、bit4:エラーリセット、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
    wait01 = wait;
    for (i = 0; i < wait01; i++) {
        if (((ch = inp(0xa02)) & 0x1) == 1) break;
    }

    outp(0xa00, (char)send_data);
    outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル

    return 0;
}

// データの読み込み(読み込み終了まで一定時間戻ってこない)
int rs232c_recv_data(void) {
    int ch, i, wait01;

    outp(0xa02, 0x37);     // 0b00110111    bit5:RTS=1、bit2:リードエネーブル、bit1:DTR=0、bit0:ライトイネーブル
    wait01 = 1000000;
    for (i = 0; i < wait01; i++) {
        if (((ch = inp(0xa02)) & 0x02) == 2) break;
    }
    if (i >= wait01) return -1;

    ch = inp(0xa00);

    return ch;

}

void rs232c_print_str(char *str1) {
    char *p1;

    p1 = str1;
    while (*p1 != (char)0) {
        while (rs232c_write_ready() == 0);
        rs232c_send_data(*p1);
        ++p1;
    }
}

// unsigned int型データを文字列に変換
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

// unsigned long long型データを文字列に変換
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

// unsigned long long型データを16進数文字列に変換
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

// 256色モードでVRAMアクセスで点を打つプログラム(インラインアセンブラ版)
void draw_pixel_vram02(int x, int y, int color) {
    char *addr01;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    addr01 = (char *)(y * VRAM_SCREEN_WIDTH + x);

    __asm {
        push   fs
	push   ebx
        mov    ax, 010ch
        mov    fs, ax
        mov    ebx, addr01
        mov    eax, color
        mov    fs:[ebx], al
        pop    ebx
        pop    fs
    }
}

// 256色モードでVRAMアクセスで点を打つプログラム
void draw_pixel_vram(int x, int y, int color) {
    __segment           seg;
    char __based( seg ) *pixel01;

    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    seg = 0x010c;
    pixel01 = (char __based( seg ) *)0;

    *(pixel01 + y * 1024 + x) = (char)color;
}

// グラフィックスBIOS初期化
char *workaddr;
int  worksize;
int ginit(int mode, char *workaddr01, int worksize01) {
    short mode01 = (char)mode;
    char ret01;

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

        mov	ax,0110h
        mov     fs,ax
        mov     ah,01h
        mov     al,0
        mov     dx, mode01
        call    pword ptr fs:0020h
        cmp     ah, 0
        jnz     gmode_end

        mov	ax,0110h
        mov     fs,ax
        mov     ah,05h
        mov     al,0
        call    pword ptr fs:0020h
        cmp     ah, 0

        mov	ax,0110h
        mov     fs,ax
        mov     ah,06h
        mov     al,0
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

// 描画する色の設定
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

// 描画する色の設定(バックグラウンド)
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

// バックグラウンドに指定した色で画面を塗りつぶし
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

// グラフィックBIOSを使って点を打つ関数
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

// グラフィックBIOSでパレットを設定する関数
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

// グラフィックBIOSで文字列を表示する関数
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

// グラフィックBIOSで矩形を表示する関数
void draw_rect(int x1, int y1, int x2, int y2) {
    struct draw_rect_data {
        short   x1;
        short   y1;
        short   x2;
        short   y2;
    };
    struct draw_rect_data draw_rect_data01;
    struct draw_rect_data *p;

    draw_rect_data01.x1 = (short)x1;
    draw_rect_data01.y1 = (short)y1;
    draw_rect_data01.x2 = (short)x2;
    draw_rect_data01.y2 = (short)y2;

    p = &draw_rect_data01;

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
        mov     ah,46h
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

// グラフィックBIOSで線を引く関数
void draw_line(int x1, int y1, int x2, int y2) {
    struct draw_line_data {
        short   num;
        short   x1;
        short   y1;
        short   x2;
        short   y2;
    };
    struct draw_line_data draw_line_data01;
    struct draw_line_data *p;

    draw_line_data01.num = (short)2;
    draw_line_data01.x1 = (short)x1;
    draw_line_data01.y1 = (short)y1;
    draw_line_data01.x2 = (short)x2;
    draw_line_data01.y2 = (short)y2;

    p = &draw_line_data01;

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
        mov     ah,42h
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

// 線を引いて塗りつぶしの矩形を表示してます
void draw_fillrect(x1, y1, x2, y2) {
    int i, y01, y02;
    if ((y2 - y1) >= 0) {
        y01 = y1;
        y02 = y2;
    } else {
        y01 = y2;
        y02 = y1;
    }               
    for (i = y01; i <= y02; i++) {
        draw_line(x1, i, x2, i);
    }
}

// キーボードBIOSでキーボードバッファが空かどうかの判定
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

// CALLBUFSのアドレスを取得
// seg_prog01 : プロテクトモード関数を呼び出すためのプログラムのアドレス(セグメント)
// off_prog01 : プロテクトモード関数を呼び出すためのプログラムのアドレス(オフセット)
// segment01  : CALLBUFS領域のリアルモードでのセグメントの値
// offset01   : CALLBUFS領域のリアルモードでのオフセットの値
// size01     : CALLBUFS領域のサイズ
// prot_addr_seg01 : CALLBUFS領域のプロテクトモードからアクセスする場合のセグメント
// prot_addr_off01 : CALLBUFS領域のプロテクトモードからアクセスする場合のオフセット
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

        mov     ax, 250dh           ;RUN386のシステムコールの呼び出し
        int     21h

        mov     off_prog, ax        ;プロテクトモードコール用プロシージャのアドレス
        shr     eax, 16             ; ss:sp→PWORD PTR FAR address of procedure to call
        mov     seg_prog, ax

        mov     offset1, bx         ;共有バッファのセグメントとオフセット
        shr     ebx, 16
        mov     segment1, bx

        mov     size1, ecx          ;共有バッファのサイズ

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

// プロテクトモードからリアルモードのプログラムを呼び出すためのRUN386のシステムコールを呼び出してます
// call_realmode_addr : リアルモードプログラムのアドレス(上位16bitがリアルモードのセグメント、下位16bitがリアルモードのオフセット
// copy_stack_number  : リアルモードのプログラムを呼び出すときにコピーするスタック容量を指定
// p_param_rm_call    : リアルモードのプログラムを呼び出すときに設定するレジスタの内容を指定(struct rm_callのポインタを指定)
int call_realmode_procedure(unsigned int call_realmode_addr, unsigned int copy_stack_number, struct rm_call *p_param_rm_call) {
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
        mov     dword ptr ret01, 0              ;成功
        jmp     call_realmode_procedure_jmp02
call_realmode_procedure_jmp01:
        pop     ds
        mov     dword ptr ret01, -1             ;失敗
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

// リアルモードBIOSを呼び出すために必要なリアルモードのプログラムをCALLBUFS領域に読み込んで初期化します
// protect_mode_addr : CALLBUFS領域の先頭アドレス(CALLBUFS領域の0x0100にプログラムがロードされます)
// protect_mode_addr : CALLBUFS領域のサイズを指定
int rmi_call_ready_flag = 0;
int init_rmi_call(unsigned char far *protect_mode_addr, int size) {
    FILE *fp1;
    int  i;
    int  ret01;
    int  ch;

    if (size < 680) return -1;

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

// リアルモードBIOSを呼び出します
// int_no      : BIOSコールのintの値
// protect_mode_addr : CALLBUFS領域の先頭のプロテクトモード用のfarポインタを指定
// realmode_segment01: CALLBUFS領域の先頭のリアルモードセグメントを指定
// rmi_call_in       : リアルモードBIOSを呼び出すときのレジスタ値を設定(struct rmi_callのポインタ)
// rmi_call_out      : リアルモードBIOSを実行した後のレジスタ値を保存する場所の設定(struct rmi_callのポインタ)
// copy_stack_num    : リアルモードのプログラムを呼び出すときにコピーするスタックの容量を設定
int rmi_call(int int_no, unsigned char far *protect_mode_addr, unsigned short realmode_segment01, struct rmi_call *rmi_call_in, struct rmi_call *rmi_call_out, int copy_stack_num) {
    struct rm_call *rm_call01;
    struct rm_call rm_call001;
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

// 現在のプロテクトモードのデータセグメントのセレクタ値を取得
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

// 現在のプロテクトモードのコードセグメントのセレクタ値を取得
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
