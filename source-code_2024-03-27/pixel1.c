/*
https://www.purose.net/befis/download/ito/l40ondos.ldt.txt

セグメントセレクタ値(LDT)
0104 : 80000000  7FFFF  0 1 0   0 1   0  1 1(RW-D ) 0 (T)VRAM(中解像度16/32K)
010C : 80100000  7FFFF  0 1 0   0 1   0  1 1(RW-D ) 0 (T)VRAM(中解像度256)
0114 : 81000000  1FFFF  0 1 0   0 1   0  1 1(RW-D ) 0 (T)パターン(スプライト)RAM

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <i86.h>

int work01[256*1024/4];

#define VIRTUAL_SCREEN_WIDTH  (1024)
#define VIRTUAL_SCREEN_HIGHT   (512)
#define SCREEN_WIDTH           (640)
#define SCREEN_HEIGHT          (480)
#define NUM_COLOR              (256)

int ginit(int mode, char *workaddr, int worksize);
void set_color(int color);
void set_bgcolor(int color);
void clear_screen(void);
void draw_pixel(int x, int y, int color);
void set_palette(int pal_num01, int r01, int g01, int b01);
void draw_string(int x, int y, char *str1);
int kbhit(void);

inline void draw_pixel_vram(int x, int y, int color);

int main(int argc, char *argv[]) {
    int i, x, y, color;
    union REGS regs;


    if (ginit(12, (char *)work01, 256*1024) != 0) {
        exit(1);
    }
    set_bgcolor(0);
    clear_screen();

    set_color(255);
    draw_string(0, 20, "BIOSでの描画");

    for(i = 0; i < 500000; i++) {
        x=rand() % SCREEN_WIDTH;
        y=rand() % (SCREEN_HEIGHT-20);
        color=rand() % (NUM_COLOR - 1) + 1;
        draw_pixel(x, y+20, color);
    }


    set_color(255);
    draw_string(0, 40, "何かキーを押してください");

    // キーバッファのクリア
    regs.h.ah = 0x06;
    int386(0x90, &regs, &regs);

    while (!kbhit());


    set_bgcolor(0);
    clear_screen();

    set_color(255);
    draw_string(0, 20, "VRAM直接アクセスでの描画");

    for(i = 0; i < 500000; i++) {
        x=rand() % SCREEN_WIDTH;
        y=rand() % (SCREEN_HEIGHT-20);
        color=rand() % (NUM_COLOR - 1) + 1;
        draw_pixel_vram(x, y+20, color);
    }

    set_color(255);
    draw_string(0, 40, "何かキーを押してください");

    // キーバッファのクリア
    regs.h.ah = 0x06;
    int386(0x90, &regs, &regs);

    while (!kbhit());

    ginit(3, (char *)work01, 256*1024);

    exit(0);
}




inline void draw_pixel_vram(int x, int y, int color) {
    char *addr01;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    addr01 = (char *)(y * VIRTUAL_SCREEN_WIDTH + x);

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
