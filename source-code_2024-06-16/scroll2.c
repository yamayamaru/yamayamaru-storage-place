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

#define VRAM_SCREEN_WIDTH  (1024)
#define VRAM_SCREEN_HIGHT   (512)
#define SCREEN_WIDTH        (320)
#define SCREEN_HEIGHT       (240)
#define DBUFFER_WIDTH       (320)
#define DBUFFER_HEIGHT      (240)

int double_buffer01[DBUFFER_WIDTH * DBUFFER_HEIGHT / 4];
char *double_buffer;

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
inline void copy_mem_to_vram(void *mem_addr, void *vram_addr, int len);
inline void copy_vram_to_mem(void *vram_addr, void *mem_addr, int len);
inline void copy_vram_to_vram(void *src_addr, void *dst_addr, int len);
inline void draw_pixel_buffer(int x, int y, int color);
void screen_update(void);
void copy_vram(void);
void scroll_buffer_right(int num);

int set_screen_ratio(int x_ratio, int y_ratio);
int set_screen_move_virtual_screen(int x_start, int y_start, int mode);
extern int x_start01, y_start01, vram_page_addr01;    /* 仮想画面内の表示位置指定 */
void set_writepage(int page);
int  get_writepage();
void set_viewpage(int page);

int main(int argc, char *argv[]) {
    int i, j, count01;
    union REGS regs;
    FILE  *fp1;
    int r, g, b, c;

    double_buffer = (char *)double_buffer01;

    if (ginit(12, (char *)work01, 256*1024) != 0) {
        exit(1);
    }
    set_bgcolor(0);
    clear_screen();

    set_screen_ratio(2, 2);
    set_writepage(0);
    set_viewpage(0);

    // 画像の読み込み(パレットデータの読み込み)
    if ((fp1 = fopen("bmppale2.dat", "rb")) == NULL) {
        set_color(255);
        draw_string(0, 20, "ファイルが開けません");
        exit(1);
    }
    for (i = 0; i < 256; i++) {
        if (feof(fp1) != 0) break;
        r = fgetc(fp1);
        g = fgetc(fp1);
        b = fgetc(fp1);
        set_palette(i, r, g, b);
    }
    fclose(fp1);

    // 画像の読み込み(データの読み込み)
    if ((fp1 = fopen("bmpdata2.dat", "rb")) == NULL) {
        set_color(255);
        draw_string(0, 20, "ファイルが開けません");
        exit(1);
    }
    for (j = 0; j < 240; j++) {
        for (i = 0; i < 320; i++) {
            if (feof(fp1) != 0) break;
            c = fgetc(fp1);
            draw_pixel_vram(i, j, c);
        }
    }
    fclose(fp1);

    // キーバッファのクリア
    regs.h.ah = 0x06;
    int386(0x90, &regs, &regs);

    // VRAMの画像をダブルバッファにコピー
    copy_vram();

    count01 = 0;
    while(!kbhit()) {
        // 画像スクロール
        scroll_buffer_right(4);
        screen_update();
        count01++;
    }

   // キーバッファのクリア
    regs.h.ah = 0x06;
    int386(0x90, &regs, &regs);
    while (!kbhit());


    set_screen_ratio(1, 1);
    set_writepage(0);
    set_viewpage(0);

    ginit(3, (char *)work01, 256*1024);

    exit(0);
}

// VRAM直接アクセスの点の描画
inline void draw_pixel_vram(int x, int y, int color) {
    char *addr01;
    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    addr01 = (char *)(y * VRAM_SCREEN_WIDTH + x + vram_page_addr01);

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

// メモリからVRAMへの転送
inline void copy_mem_to_vram(void *mem_addr, void *vram_addr, int len) {
    __asm {
        push   es
        push   esi
        push   edi
        push   ecx

        mov    ax, 010ch
        mov    es, ax
        mov    esi, mem_addr
        mov    edi, vram_addr
        mov    ecx, len
        cld
        rep movsb

        pop    ecx
        pop    edi
        pop    esi
        pop    es
    }
}

// VRAMからメモリへの転送
inline void copy_vram_to_mem(void *vram_addr, void *mem_addr, int len) {
    __asm {
        push   es
        push   esi
        push   edi
        push   ecx

        push   ds
        mov    ax, ds
        mov    es, ax
        mov    esi, vram_addr
        mov    edi, mem_addr
        mov    ecx, len
        mov    ax, 010ch
        mov    ds, ax
        cld
        rep movsb
	pop    ds

        pop    ecx
        pop    edi
        pop    esi
        pop    es
    }
}

// VRAMからVRAMへの転送
inline void copy_vram_to_vram(void *src_addr, void *dst_addr, int len) {
    _asm {
        push   es
        push   esi
        push   edi
        push   ecx

        push   ds
        mov    ax, 010ch
        mov    es, ax
        mov    esi, src_addr
        mov    edi, dst_addr
        mov    ecx, len
        mov    ds, ax
        cld
        rep movsb
	pop    ds

        pop    ecx
        pop    edi
        pop    esi
        pop    es
    }
}

// ダブルバッファへの点の描画
inline void draw_pixel_buffer(int x, int y, int color) {
    char *addr01;
    if (x < 0 || x >= DBUFFER_WIDTH) return;
    if (y < 0 || y >= DBUFFER_HEIGHT) return ;

    *(double_buffer + y * DBUFFER_WIDTH + x) = (char)color;
}

// ダブルバッファの画像をVRAMに転送
void screen_update(void) {
    char *vram_addr, *mem_addr;
    int i;

    vram_addr = (char *)0;
    mem_addr = double_buffer;
    for (i = 0; i < DBUFFER_HEIGHT; i++) {
        copy_mem_to_vram(mem_addr, vram_addr + vram_page_addr01, DBUFFER_WIDTH);
        mem_addr += DBUFFER_WIDTH;
        vram_addr += VRAM_SCREEN_WIDTH;
    }
}

// ダブルバッファへVRAMの内容をコピー
void copy_vram(void) {
    char *vram_addr, *mem_addr;
    int i;

    vram_addr = (char *)0;
    mem_addr = double_buffer;
    for (i = 0; i < DBUFFER_HEIGHT; i++) {
        copy_vram_to_mem(vram_addr + vram_page_addr01, mem_addr, DBUFFER_WIDTH);
        mem_addr += DBUFFER_WIDTH;
        vram_addr += VRAM_SCREEN_WIDTH;
    }
}

// ダブルバッファの内容をnumドットだけ、右スクロール
char buf01[DBUFFER_WIDTH];
void scroll_buffer_right(int num) {
    int i, left, right, len;
    int line01;
    left = num;
    right = DBUFFER_WIDTH - num;
    line01 = 0;
    for (i = 0; i < DBUFFER_HEIGHT; i++) {
        memcpy(buf01, double_buffer + line01 + right, left);
        memcpy(buf01 + left, double_buffer + line01, right);
        memcpy(double_buffer + line01, buf01, DBUFFER_WIDTH);
        line01 += DBUFFER_WIDTH;
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

int set_screen_move_virtual_screen(int x_start, int y_start, int mode) {     /* mode = 1:vsync待ちなし、mode = 0:vsync待ちあり) */
    int ret01;
    char mode01 = ((char)mode & 0x01) << 6;
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
        mov     al,01h
	or	al, mode01
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

int x_start01, y_start01, writepage01, vram_page_addr01;    /* 仮想画面内の表示位置指定 */
void set_writepage(int page){
    writepage01 = page;
    switch(page) {
        case 0:
            x_start01 = 0;
            y_start01 = 0;
            vram_page_addr01 = x_start01 + y_start01 * VRAM_SCREEN_WIDTH;
            break;
        case 1:
            x_start01 = 320;
            y_start01 = 0;
            vram_page_addr01 = x_start01 + y_start01 * VRAM_SCREEN_WIDTH;
            break;
        case 2:
            x_start01 = 640;
            y_start01 = 0;
            vram_page_addr01 = x_start01 + y_start01 * VRAM_SCREEN_WIDTH;
            break;
        case 3:
            x_start01 = 0;
            y_start01 = 240;
            vram_page_addr01 = x_start01 + y_start01 * VRAM_SCREEN_WIDTH;
            break;
        case 4:
            x_start01 = 320;
            y_start01 = 240;
            vram_page_addr01 = x_start01 + y_start01 * VRAM_SCREEN_WIDTH;
            break;
        case 5:
            x_start01 = 640;
            y_start01 = 240;
            vram_page_addr01 = x_start01 + y_start01 * VRAM_SCREEN_WIDTH;
            break;

        default:
            x_start01 = 0;
            y_start01 = 0;
            writepage01 = 0;
            vram_page_addr01 = x_start01 + y_start01 * VRAM_SCREEN_WIDTH;
    }
}

int  get_writepage() {
    return writepage01;
}

void set_viewpage(int page) {
    int x_view_start01, y_view_start01;
    switch(page) {
        case 0:
            x_view_start01 = 0;
            y_view_start01 = 0;
            break;
        case 1:
            x_view_start01 = 320;
            y_view_start01 = 0;
            break;
        case 2:
            x_view_start01 = 640;
            y_view_start01 = 0;
            break;
        case 3:
            x_view_start01 = 0;
            y_view_start01 = 240;
            break;
        case 4:
            x_view_start01 = 320;
            y_view_start01 = 240;
            break;
        case 5:
            x_view_start01 = 640;
            y_view_start01 = 240;
            break;

        default:
            x_view_start01 = 0;
            y_view_start01 = 0;
    }
    set_screen_move_virtual_screen(x_view_start01, y_view_start01, 1);
}
