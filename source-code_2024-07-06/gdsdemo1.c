#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>

struct simulate_real_mode_interrupt_regs {
    unsigned int   edi;
    unsigned int   esi;
    unsigned int   ebp;
    unsigned int   zero;
    unsigned int   ebx;
    unsigned int   edx;
    unsigned int   ecx;
    unsigned int   eax;
    unsigned short flags;
    unsigned short es;
    unsigned short ds;
    unsigned short fs;
    unsigned short gs;
    unsigned short ip;
    unsigned short cs;
    unsigned short sp;
    unsigned short ss;
};

int dpmi_simulate_real_mode_interrupt(int int_no, int copy_stack_data_size, struct simulate_real_mode_interrupt_regs *regs, unsigned int regs_seg);
int dpmi_allocate_dos_memory_block(int alloc_size, unsigned int *real_seg, unsigned int *prot_selector);
int dpmi_free_dos_memory_block(int prot_selector);
int dpmi_resize_dos_memory_block(int new_alloc_size, int prot_selector);
int gds_init(unsigned int workseg, unsigned int workaddr, void far *prot_workaddr, unsigned int dataseg, unsigned int dataaddr, void far *prot_dataaddr, int worksize);
int gds_actmode(int num);
int gds_circle(int cx, int cy, int r, int color, int mode, int fill);
int gds_rect(int x1, int y1, int x2, int y2, int color, int mode, int fill);
int gds_line(int x1, int y1, int x2, int y2, int color, int mode);
int gds_pset(int x1, int y1, int color, int mode);
int gds_palette(int color, int red, int green, int blue);
int gds_clear();
int gds_set_background_color(int color);
void hat();

int main(int argc, char *argv[]) {
    unsigned int seg01, prot_selector01;
    char *work, *data;
    int  i, j, count01, x1, y1, x2, y2, r, c;
    int  ret01;

    /* dpmi_allocate_dos_memory_block()Ç≈éwíËÇ∑ÇÈÉÅÉÇÉäÉTÉCÉYÇÕÉpÉâÉOÉâÉtíPà (16byte) */
    if(dpmi_allocate_dos_memory_block(65520 / 16, &seg01, &prot_selector01) != 0) {
        printf( "dpmi_allocate_dos_memory_block failed\n" );
        return 0;
    }

//    printf( "dpmi_allocate_dos_memory_block succeeded\n" );

//    printf("DOS SEGMENT = 0x%04x\n", seg01);
//    printf("PROTECTMODE SEGMENT SELECTOR = 0x%04x\n", prot_selector01);



    gds_init(seg01, 0x0000, MK_FP(prot_selector01, 0x0000), seg01, 0x2000, MK_FP(prot_selector01, 0x2000), 4096);
    gds_actmode(1);    // ägí£ÉÇÅ[Éh
    gds_set_background_color(0);

    // paletteÇÃê›íË(FMR-50(ägí£ÉÇÅ[Éh))
    for (i = 0; i < 16; i++) {
        if (i < 8)  gds_palette(i, ((i & 4) >> 2) * 128, ((i & 2) >> 1) * 128, (i & 1) * 128);
        if (i == 8) gds_palette(i, 64, 64, 64);
        if (i > 8)  gds_palette(i, ((i & 4) >> 2) * 255, ((i & 2) >> 1) * 255, (i & 1) * 255);
    }

    gds_clear();
    count01 = 0;
    for (j = 50; j < 400; j+=100) {
        for (i = 0; i < 640; i+=100) {
            if (j < 200)
                ret01 = gds_circle(i, j, 50, count01 % 15 + 1, 3, j < 100 ? 0: 1);
                ret01 = gds_line(i - 50, j - 50, i + 50 - 1, j + 50 - 1, (count01 + 1) % 15 + 1, 3);
                ret01 = gds_line(i + 50, j - 50, i - 50 - 1, j + 50 - 1, (count01 + 1) % 15 + 1, 3);
            if (j >= 200)
                ret01 = gds_rect(i - 50, j - 50, i + 50 - 1, j + 50 - 1, count01 % 15 + 1, 3, j < 300 ? 0: 1);
                ret01 = gds_line(i - 50, j - 50, i + 50 - 1, j + 50 - 1, (count01 + 1) % 15 + 1, 3);
                ret01 = gds_line(i + 50, j - 50, i - 50 - 1, j + 50 - 1, (count01 + 1) % 15 + 1, 3);

            count01++;
        }
    }
    delay(3000);


    gds_clear();
    for (i = 0; i < 60000; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        c = (rand() % 15) + 1;
        gds_pset(x1, y1, c, 3);
    }
    delay(3000);

    gds_clear();
    for (i = 0; i < 4000; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        x2 = rand() % 640;
        y2 = rand() % 400;
        c = (rand() % 15) + 1;
        gds_line(x1, y1, x2, y2, c, 3);
    }
    delay(3000);

    gds_clear();
    for (i = 0; i < 1500; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        x2 = rand() % 640;
        y2 = rand() % 400;
        c = (rand() % 15) + 1;
        gds_rect(x1, y1, x2, y2, c, 3, 1);
    }
    delay(3000);

    gds_clear();
    for (i = 0; i < 2000; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        r  = rand() % 50;
        c = (rand() % 15) + 1;
        ret01 = gds_circle(x1, y1, r, c, 3, 1);
    }
    delay(3000);


    gds_clear();
    hat();

    if (dpmi_free_dos_memory_block(prot_selector01) != 0) {
        printf( "dpmi_free_dos_memory_block failed\n" );
    } else {
//        printf( "dpmi_free_dos_memory_block success\n" );
    }

    return 0;
}

double d[160];
void hat() {
    int       i, j, x, y, zz, px, py, f, col;
    double    dr, r, z, sx, sy;
    int       xmax, ymax;

    xmax = 640;
    ymax = 400;

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }
    dr = 3.141592653589793 / 180.0;
    for (y = -180; y <= 180; y += 6) {
        for (x = -180; x < 180; x += 4) {
            r = dr * sqrt((double)x * (double)x + (double)y * (double)y);
            z = 100 * cos(r) - 30 * cos(3*r);
            sx = 80 + x / 3 - y / 6;
            sy = 40 - y / 6 - z / 4;
            if ((sx < 0) || (sx >= 160)) continue;
            if (d[(int)sx] <= sy) continue;
            zz = (int)((z + 100) * 0.035) + 1;
            col = 0;
            f = 0;
            if ((zz == 1) || (zz == 3) || (zz == 5) || (zz == 7)) {
                col = col | 1;
                f = 1;
            }
            if ((zz == 2) || (zz == 3) || (zz >= 6)) {
                col = col | 4;
                f = 1;
            }
            if ((zz >= 4)) {
                col = col | 2;
                f = 1;
            }

            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);
            if (f == 1) {
                gds_rect(px, py, px + 3, py + 3, col | 0x08, 3, 1);
            }
            d[(int)sx] = sy;
        }
    }
}




unsigned int gds_workseg01;
unsigned int gds_workaddr01;
unsigned int gds_dataseg01;
unsigned int gds_dataaddr01;
short far *gds_prot_workaddr01;
short far *gds_prot_dataaddr01;
int gds_init(unsigned int workseg, unsigned int workaddr, void far *prot_workaddr, unsigned int dataseg, unsigned int dataaddr, void far *prot_dataaddr, int worksize) {
    struct simulate_real_mode_interrupt_regs regs01;
    short far *p;

    gds_workseg01 = workseg;
    gds_workaddr01 = workaddr;
    gds_dataseg01 = dataseg;
    gds_dataaddr01 = dataaddr;
    gds_prot_workaddr01 = (short far *)prot_workaddr;
    gds_prot_dataaddr01 = (short far *)prot_dataaddr;
    p = gds_prot_workaddr01;
    *p = worksize - 2;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 00 << 8;
    regs01.ebx = workaddr;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);

    return (int)((regs01.eax >> 8) & 0xff);
}

// num = 0:FM16É¿ÉÇÅ[Éh  num != 0:ägí£ÉÇÅ[Éh
int gds_actmode(int num) {
    struct simulate_real_mode_interrupt_regs regs01;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x79 << 8 | (num & 0xff);
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);

    return (int)((regs01.eax >> 8) & 0xff);
}

// mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
// fill = 0:ìhÇÁÇ»Ç¢  fill = 1:ìhÇÈ
int gds_error_error = 0;
int gds_error_error_no = 0;
int gds_circle(int cx, int cy, int r, int color, int mode, int fill) {
    struct simulate_real_mode_interrupt_regs regs01;
    short far  *p;

    p = gds_prot_dataaddr01;
    p[0] = 36;
    p[1] = 0x6000 + ( 1 << 5) + 6;
    p[2] = (short)2;
    p[3] = (short)2;
    p[4] = (short)(mode & 0xff);         // ï`âÊÉÇÅ[Éh mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
    p[5] = 0x5000 + (26 << 5) + 2;
    p[6] = (short)1;                     // ê¸éÌ  1:é¿ê¸
    p[7] = 0x5000 + (29 << 5) + 2;
    p[8] = (short)1;                     // 0:ã´äEê¸Çï`âÊÇµÇ»Ç¢ 1:ã´äEê¸Çï`âÊÇ∑ÇÈ
    p[9] = 0x5000 + (28 << 5) + 2;
    p[10] = (short)color;                 // ã´äEê¸êF
    p[11] = 0x5000 + (22 << 5) + 2;
    p[12] = (short)(fill & 0x01);         // ñ ìhÉÇÅ[Éh  0:ìhÇÁÇ»Ç¢ 1:ìhÇËÇ¬Ç‘Çµ(ÉxÉ^ìhÇË) 2:É^ÉCÉäÉìÉOìhÇË 3:ÉnÉbÉ`ÉìÉOìhÇË
    p[13] = 0x5000 + (23 << 5) + 2;
    p[14] = (short)color;                 // ñ ìhÇËêF
    p[15] = (short)(0x4000 + (12 << 5) + 6);
    p[16] = (short)cx;
    p[17] = (short)cy;
    p[18] = (short)r;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x0f << 8;
    regs01.edi = gds_dataaddr01;
    regs01.ds  = (unsigned short)gds_dataseg01;
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);
    gds_error_error = (regs01.edx & 0xffff);
    gds_error_error_no = (regs01.ecx & 0xffff);

    return (int)((regs01.eax >> 8) & 0xff);
}

// mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
// fill = 0:ìhÇÁÇ»Ç¢  fill = 1:ìhÇÈ
int gds_rect(int x1, int y1, int x2, int y2, int color, int mode, int fill) {
    struct simulate_real_mode_interrupt_regs regs01;
    short far  *p;

    p = gds_prot_dataaddr01;
    p[0] = 38;
    p[1] = 0x6000 + ( 1 << 5) + 6;
    p[2] = (short)2;
    p[3] = (short)2;
    p[4] = (short)(mode & 0xff);         // ï`âÊÉÇÅ[Éh mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
    p[5] = 0x5000 + (26 << 5) + 2;
    p[6] = (short)1;                     // ê¸éÌ  1:é¿ê¸
    p[7] = 0x5000 + (29 << 5) + 2;
    p[8] = (short)1;                     // 0:ã´äEê¸Çï`âÊÇµÇ»Ç¢ 1:ã´äEê¸Çï`âÊÇ∑ÇÈ
    p[9] = 0x5000 + (28 << 5) + 2;
    p[10] = (short)color;                 // ã´äEê¸êF
    p[11] = 0x5000 + (22 << 5) + 2;
    p[12] = (short)(fill & 0x01);         // ñ ìhÉÇÅ[Éh  0:ìhÇÁÇ»Ç¢ 1:ìhÇËÇ¬Ç‘Çµ(ÉxÉ^ìhÇË) 2:É^ÉCÉäÉìÉOìhÇË 3:ÉnÉbÉ`ÉìÉOìhÇË
    p[13] = 0x5000 + (23 << 5) + 2;
    p[14] = (short)color;                 // ñ ìhÇËêF
    p[15] = (short)(0x4000 + (11 << 5) + 8);
    p[16] = (short)x1;
    p[17] = (short)y1;
    p[18] = (short)x2;
    p[19] = (short)y2;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x0f << 8;
    regs01.edi = gds_dataaddr01;
    regs01.ds  = (unsigned short)gds_dataseg01;
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);
    gds_error_error = (regs01.edx & 0xffff);
    gds_error_error_no = (regs01.ecx & 0xffff);

    return (int)((regs01.eax >> 8) & 0xff);
}

// mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
int gds_line(int x1, int y1, int x2, int y2, int color, int mode) {
    struct simulate_real_mode_interrupt_regs regs01;
    short far  *p;

    p = gds_prot_dataaddr01;
    p[0] = 26;
    p[1] = 0x6000 + ( 1 << 5) + 6;
    p[2] = (short)2;
    p[3] = (short)2;
    p[4] = (short)(mode & 0xff);          // ï`âÊÉÇÅ[Éh mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
    p[5] = 0x5000 + (2 << 5) + 2;
    p[6] = (short)(1 & 0xff);             // ê¸éÌ  1:é¿ê¸
    p[7] = 0x5000 + (4 << 5) + 2;
    p[8] = (short)color;                 // ã´äEê¸êF
    p[9] = (short)(0x4000 + (2 << 5) + 8);
    p[10] = (short)x1;
    p[11] = (short)y1;
    p[12] = (short)x2;
    p[13] = (short)y2;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x0f << 8;
    regs01.edi = gds_dataaddr01;
    regs01.ds  = (unsigned short)gds_dataseg01;
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);
    gds_error_error = (regs01.edx & 0xffff);
    gds_error_error_no = (regs01.ecx & 0xffff);

    return (int)((regs01.eax >> 8) & 0xff);
}

// mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
int gds_pset(int x1, int y1, int color, int mode) {
    struct simulate_real_mode_interrupt_regs regs01;
    short far  *p;

    p = gds_prot_dataaddr01;
    p[0] = 18;
    p[1] = 0x6000 + ( 1 << 5) + 6;
    p[2] = (short)2;
    p[3] = (short)2;
    p[4] = (short)(mode & 0xff);          // ï`âÊÉÇÅ[Éh mode : 0:preset 1:and  3:pset 6:xor 7:or 12:not
    p[5] = 0x5000 + (8 << 5) + 2;
    p[6] = (short)color;                  // É|ÉCÉìÉgêF
    p[7] = (short)(0x4000 + (3 << 5) + 4);
    p[8] = (short)x1;
    p[9] = (short)y1;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x0f << 8;
    regs01.edi = gds_dataaddr01;
    regs01.ds  = (unsigned short)gds_dataseg01;
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);
    gds_error_error = (regs01.edx & 0xffff);
    gds_error_error_no = (regs01.ecx & 0xffff);

    return (int)((regs01.eax >> 8) & 0xff);
}

int gds_clear() {
    struct simulate_real_mode_interrupt_regs regs01;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x04 << 8;
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);

    return (int)((regs01.eax >> 8) & 0xff);
}

int gds_set_background_color(int color) {
    struct simulate_real_mode_interrupt_regs regs01;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x10 << 8;
    regs01.edx = color & 0xffff;
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);

    return (int)((regs01.eax >> 8) & 0xff);
}

int gds_palette(int color, int red, int green, int blue) {
    struct simulate_real_mode_interrupt_regs regs01;
    short far  *p;

    p = gds_prot_dataaddr01;
    p[0] = 8;
    p[1] = color;
    p[2] = (red & 0xff) << 8;
    p[3] = (green & 0xff) | (blue & 0xff) << 8;

    regs01.ss = 0;
    regs01.sp = 0;
    regs01.eax = 0x03 << 8;
    regs01.edi = gds_dataaddr01;
    regs01.ds  = (unsigned short)gds_dataseg01;
    regs01.ebx = gds_workaddr01;
    regs01.es  = (unsigned short)gds_workseg01;
    dpmi_simulate_real_mode_interrupt(0x92, 16, &regs01, 0);
    gds_error_error = (regs01.edx & 0xffff);
    gds_error_error_no = (regs01.ecx & 0xffff);

    return (int)((regs01.eax >> 8) & 0xff);
}

/* alloc_size : block size in (16-byte) paragraphs */
/*
error code:
0007H	memory control blocks damaged (also returned by DPMI 0.9 hosts)
0008H	insufficient memory (also returned by DPMI 0.9 hosts).
8011H	descriptor unavailable
*/
int dpmi_allocate_dos_memory_block(int alloc_size, unsigned int *real_seg, unsigned int *prot_selector) {
    unsigned short real_seg01, prot_selector01;
    int ret01;
    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp

        mov     ebx, alloc_size
        mov     eax, 0100h
        int     31h
        jnc      dpmi_allocate_dos_memory_block_jmp01
        mov     ret01, eax                    ; failed
        jmp     dpmi_allocate_dos_memory_block_jmp02

dpmi_allocate_dos_memory_block_jmp01:         ; success
        mov     real_seg01, ax
        mov     prot_selector01, dx
        xor     eax, eax
        mov     ret01, eax

dpmi_allocate_dos_memory_block_jmp02:
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    *real_seg = (int)real_seg01;
    *prot_selector = (int)prot_selector01;
    return ret01;
}

/*
error code:
0007H	memory control blocks damaged (also returned by DPMI 0.9 hosts).
0009H	incorrect memory segment specified (also returned by DPMI 0.9 hosts).
8022H	invalid selector
*/
int dpmi_free_dos_memory_block(int prot_selector) {
    unsigned short prot_selector01;
    int ret01;
    prot_selector01 = (unsigned short)prot_selector;
    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp

        mov     dx, prot_selector01;
        mov     eax, 0101h
        int     31h
        jnc     dpmi_free_dos_memory_block_jmp01
        mov     ret01, eax                    ; failed
        jmp     dpmi_free_dos_memory_block_jmp02

dpmi_free_dos_memory_block_jmp01:             ; success
        xor     eax, eax
        mov     ret01, eax

dpmi_free_dos_memory_block_jmp02:
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    return ret01;
}

/*
error code :
0007H	memory control blocks damaged (also returned by DPMI 0.9 hosts).
0008H	insufficient memory (also returned by DPMI 0.9 hosts).
0009H	incorrect memory segment specified (also returned by DPMI 0.9 hosts).
8011H	descriptor unavailable
8022H	invalid selector

BX = maximum possible block size (paragraphs)
*/
int dpmi_resize_dos_memory_block(int new_alloc_size, int prot_selector) {
    unsigned short prot_selector01;
    int ret01;
    prot_selector01 = (unsigned short)prot_selector;

    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp

        mov     ebx, new_alloc_size
        mov     dx, prot_selector01;
        mov     eax, 0102h
        int     31h
        jnc     dpmi_resize_dos_memory_block_jmp01
        mov     ret01, eax                      ; failed
        jmp     dpmi_resize_dos_memory_block_jmp02

dpmi_resize_dos_memory_block_jmp01:             ; success
        xor     eax, eax
        mov     ret01, eax

dpmi_resize_dos_memory_block_jmp02:
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    return ret01;
}

/*
regs_seg == 0 then regs_seg = ds

error code :
8012H	linear memory unavailable (stack)
8013H	physical memory unavailable (stack)
8014H	backing store unavailable (stack)
8021H	invalid value (CX too large)
*/
int dpmi_simulate_real_mode_interrupt(int int_no, int copy_stack_data_size, struct simulate_real_mode_interrupt_regs *regs, unsigned int regs_seg) {
    int ret01;
    char int_no01;
    unsigned short es01;
    unsigned int addr01;

    es01 = (unsigned short)regs_seg;
    int_no01 = (char)int_no;
    regs->zero = 0;

    __asm {
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    edi
        push    esi
        push    ebp
        push    ds
        push    es
        push    fs
        push    gs

        mov	ax, es01             ;if (es01 == 0) es = ds else es = es01;
        cmp     ax, 0
        jz      simulate_real_mode_interrupt_jmp03
        mov	ax, es01
        mov     es, ax
        jmp     simulate_real_mode_interrupt_jmp04
simulate_real_mode_interrupt_jmp03:
	push	ds
	pop	es
simulate_real_mode_interrupt_jmp04:

        mov     eax, 0300h
        mov     bl, int_no01
        mov     bh, 0
        mov     ecx, copy_stack_data_size
        mov     edi, regs
        int     31h
        jnc     simulate_real_mode_interrupt_jmp01
        mov     ret01, eax                ; failed

        jmp     simulate_real_mode_interrupt_jmp02
simulate_real_mode_interrupt_jmp01:       ; success
        mov     ax, es
        mov     es01, ax
        mov     addr01, edi
        xor     eax, eax
        mov     ret01, eax

simulate_real_mode_interrupt_jmp02:
        pop     gs
        pop     fs
        pop     es
        pop     ds
        pop     ebp
        pop     esi
        pop     edi
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax

    }
//    ret_regs = (struct simulate_real_mode_interrupt_regs far *)MK_FP(es01, addr01);

    return ret01;
}
