/*

�R���p�C�����@�F
wcc386 -3r -zq -zk0 -ox -fo=testrmicall01.obj trmical1.c
wlink system pharlap runtime CALLBUFS=65536 option quiet name trmical1 file{ trmical1.obj }

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include <dos.h>

#include <conio.h>      // outp()�Ainp()�ɕK�v

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

    // CALLBUFS�̈�̃��A�����[�h�Ŏg���Z�O�����g(segment01)�A�I�t�Z�b�g(offset01)��
    // CALLBUFS�̈�̃T�C�Y(size01)�ƃv���e�N�g���[�h����g���Z�O�����g(prot_addr_seg01)�A�I�t�Z�b�g���擾(prot_addr_off01)
    get_call_buff_addr(&seg_prog01, &off_prog01, &segment01, &offset01, &size01, &prot_addr_seg01, &prot_addr_off01);

    // CALLBUFS�ɂ̓v���e�N�g���[�h�����far�|�C���^���g���ăA�N�Z�X���邽�߂���far�|�C���^���쐬
    prot_addr01 = MK_FP(prot_addr_seg01, prot_addr_off01);

    // RUN386�ł̓Z�O�����g0x34�ł����A�����[�h�̃A�h���X�ɃA�N�Z�X�ł��܂�
//    prot_addr01 = MK_FP(0x34, segment01 * 16 + offset01);        //  segment01:offset01


    // ���A�����[�hDOS�R�[���⃊�A�����[�hBIOS���Ăяo�����߂̃��A�����[�h�̃v���O������ǂݍ���ł܂�
    // ���̃��A�����[�h�̃v���O�����Ƀ��W�X�^�l��n���āA���ۂɃ\�t�g�E�F�A���荞�݂����s�����Ă܂�
    if (init_rmi_call(prot_addr01, size01) != 0) {
        set_bgcolor(12);
        clear_screen();

        set_color(255);
        draw_string(0, 40, "rmical01.com�̃��[�h�Ɏ��s���܂���");
        draw_string(0, 60, "rmical01.com��{�v���O�����Ɠ����ꏊ�ɒu���Ă�������");

        draw_string(0, 100, "�����L�[�������Ă�������");

        // �L�[�o�b�t�@�̃N���A
        regs.h.ah = 0x06;
        int386(0x90, &regs, &regs);
        // �L�[���͂�����܂őҋ@
        while (!kbhit());

        exit(1);
    }


    set_color(255);
    draw_string(0, 60, "�I������ɂ͉����L�[�������Ă�������");

    // �L�[�o�b�t�@�̃N���A
    regs.h.ah = 0x06;
    int386(0x90, &regs, &regs);

    while (!kbhit()) {

        // ���t�A�����ǂݎ��̃��A�����[�h��TBIOS�R�[��(�ꉞ�O�̂���DS�Z�O�����g�̒l���ݒ肵�Ă܂�)
        rmi_call01.rmi_ds = segment01;
        rmi_call01.rmi_edi = 0x1000;
        rmi_call01.rmi_eax = 0x01 << 8;    // ah = 0x01;

        // CALBUFS�̈�ɒu�������A�����[�h�̃v���O�������Ăяo���ă��A�����[�hBIOS���Ăяo���Ă܂�
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

    //  ���t�A�����ǂݎ��̃��A�����[�h��TBIOS�R�[���Ŏ擾���������̃f�[�^��\�����Ă܂�
    //  sprintf()���g����TOWNS�̃G�~�����[�^�Ō݊�BIOS���g���Ă����ꍇ�ɗ�����̂�sprintf�͎g���Ă܂���
        set_color(12);
        draw_fillrect(0, 0, 639, 40);
        set_color(255);
        strcpy(buf01, "���݂̓��� = ");
        uint_to_str(buf02, year01);
        strcat(buf01, buf02);
        strcat(buf01, "�N");
        uint_to_str(buf02, mon01);
        strcat(buf01, buf02);
        strcat(buf01, "��");
        uint_to_str(buf02, day01);
        strcat(buf01, buf02);
        strcat(buf01, "��");
        uint_to_str(buf02, week01);
        strcat(buf01, buf02);
        strcat(buf01, "�j�� ");
        
        uint_to_str(buf02, hour01);
        strcat(buf01, buf02);
        strcat(buf01, "��");
        uint_to_str(buf02, min01);
        strcat(buf01, buf02);
        strcat(buf01, "��");
        uint_to_str(buf02, sec01);
        strcat(buf01, buf02);
        strcat(buf01, "�b");

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
    // 8251������
    outp(0xa02, 0x0);
    outp(0xa02, 0x0);
    outp(0xa02, 0x0);
    outp(0xa02, 0x40);     // ���Z�b�g
    outp(0xa02, 0x4e);     // 0b01001110    bit7,bit6=�f�[�^�r�b�g1�Abit5=�p���e�B��Abit4=�p���e�B�Ȃ��Abit3,bit2=�f�[�^8bit�Abit1,bit0=�ʐM���xx16
    outp(0xa02, 0x37);     // 0b00110111    bit5:RTS=1�Abit4:�G���[���Z�b�g�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
    // 8253������(4800bps) RS232C�p�^�C�}��2�ڂ�8253�̃`���l��1(�܂�`�����l��4)
    outp(0x56, 0x76);      // 2�ڂ�8253�R���g���[�����W�X�^  0b01110110 bit7,bit6=�`���l��1(4)�Abit5,bit4=lsb,msb�A�����[�h���C�g�Abit3,bit2,bit1=���[�h3�Abit0=�o�C�i���J�E���g
    outp(0x52, 16);        // 2�ڂ�8253 �`�����l��1(4)�J�E���g���W�X�^ �J�E���g�lLSB 16    1/16�̂Ƃ�4800bps
    outp(0x52, 0x00);      // 2�ڂ�8253 �`�����l��1(4)�J�E���g���W�X�^ �J�E���g�lMSB  0
    
}

// RS232C�ɏo�͉\���m�F
int rs232c_write_ready(void) {
    int ret01;

    if ((inp(0xa02) & 0x01) == 1) ret01 = 1;
    else                                 ret01 = 0;

    return ret01;
}

// RS232C�œ��̓f�[�^�����Ă��邩�m�F
int rs232c_read_ready(void) {
    int ret01;

    if ((inp(0xa02) & 0x02) == 2) ret01 = 1;
    else                                 ret01 = 0;

    return ret01;
}

// RS232C�f�[�^���́A�ǂݍ��߂Ȃ��Ă������߂��Ă���A�ǂݍ��݃r�W�[ : -1�A�ǂݍ��ݐ��� : �ǂݍ��݃f�[�^
int rs232c_read(void) {
    int ret01, ch;

    outp(0xa02, 0x37);     // 0b00110111    bit5:RTS=1�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
    if (((ch = inp(0xa02)) & 0x02) == 2) {
        ret01 = ((int)inp(0xa00) & 0xff);
    } else {
        ret01 = -1;
    }

    return ret01;
}

// RS232C�f�[�^�������݁A�������߂Ȃ��Ă������߂��Ă���A�������݃r�W�[ : -1�A�������ݐ��� : 0
int rs232c_write(int send_data) {
    int ch, i, ret01;

    if (((ch = inp(0xa02)) & 0x1) == 1) {
        outp(0xa02, 0x17);     // 0b00010111    bit5:RTS=0�Abit4:�G���[���Z�b�g�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
        outp(0xa00, (char)send_data);
        outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
        ret01 = 0;
    } else {
        ret01 = -1;
    }

    return ret01;
}

// �f�[�^�̏�������(�������ݏI���܂ň�莞�Ԗ߂��Ă��Ȃ�)
int rs232c_send_data(int send_data) {
    int ch, i, wait01;

    outp(0xa02, 0x17);     // 0b00010111    bit5:RTS=0�Abit4:�G���[���Z�b�g�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
    wait01 = 1000000;
    for (i = 0; i < wait01; i++) {
        if (((ch = inp(0xa02)) & 0x1) == 1) break;
    }
    if (i >= wait01) {
        outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
        return -1;
    }
    outp(0xa00, (char)send_data);
    outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��

    return 0;
}

int rs232c_send_data_not_ready(int send_data, int wait) {
    int ch, i, wait01;

    outp(0xa02, 0x17);     // 0b00010111    bit5:RTS=0�Abit4:�G���[���Z�b�g�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
    wait01 = wait;
    for (i = 0; i < wait01; i++) {
        if (((ch = inp(0xa02)) & 0x1) == 1) break;
    }

    outp(0xa00, (char)send_data);
    outp(0xa02, 0x27);     // 0b00100111    bit5:RTS=1�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��

    return 0;
}

// �f�[�^�̓ǂݍ���(�ǂݍ��ݏI���܂ň�莞�Ԗ߂��Ă��Ȃ�)
int rs232c_recv_data(void) {
    int ch, i, wait01;

    outp(0xa02, 0x37);     // 0b00110111    bit5:RTS=1�Abit2:���[�h�G�l�[�u���Abit1:DTR=0�Abit0:���C�g�C�l�[�u��
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

// unsigned int�^�f�[�^�𕶎���ɕϊ�
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

// unsigned long long�^�f�[�^�𕶎���ɕϊ�
//    �g����
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

// unsigned long long�^�f�[�^��16�i��������ɕϊ�
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

// 256�F���[�h��VRAM�A�N�Z�X�œ_��łv���O����(�C�����C���A�Z���u����)
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

// 256�F���[�h��VRAM�A�N�Z�X�œ_��łv���O����
void draw_pixel_vram(int x, int y, int color) {
    __segment           seg;
    char __based( seg ) *pixel01;

    if (x < 0 || x >= SCREEN_WIDTH) return;
    if (y < 0 || y >= SCREEN_HEIGHT) return ;

    seg = 0x010c;
    pixel01 = (char __based( seg ) *)0;

    *(pixel01 + y * 1024 + x) = (char)color;
}

// �O���t�B�b�N�XBIOS������
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

// �`�悷��F�̐ݒ�
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

// �`�悷��F�̐ݒ�(�o�b�N�O���E���h)
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

// �o�b�N�O���E���h�Ɏw�肵���F�ŉ�ʂ�h��Ԃ�
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

// �O���t�B�b�NBIOS���g���ē_��ł֐�
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

// �O���t�B�b�NBIOS�Ńp���b�g��ݒ肷��֐�
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

// �O���t�B�b�NBIOS�ŕ������\������֐�
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

// �O���t�B�b�NBIOS�ŋ�`��\������֐�
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

// �O���t�B�b�NBIOS�Ő��������֐�
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

// ���������ēh��Ԃ��̋�`��\�����Ă܂�
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

// �L�[�{�[�hBIOS�ŃL�[�{�[�h�o�b�t�@���󂩂ǂ����̔���
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

// CALLBUFS�̃A�h���X���擾
// seg_prog01 : �v���e�N�g���[�h�֐����Ăяo�����߂̃v���O�����̃A�h���X(�Z�O�����g)
// off_prog01 : �v���e�N�g���[�h�֐����Ăяo�����߂̃v���O�����̃A�h���X(�I�t�Z�b�g)
// segment01  : CALLBUFS�̈�̃��A�����[�h�ł̃Z�O�����g�̒l
// offset01   : CALLBUFS�̈�̃��A�����[�h�ł̃I�t�Z�b�g�̒l
// size01     : CALLBUFS�̈�̃T�C�Y
// prot_addr_seg01 : CALLBUFS�̈�̃v���e�N�g���[�h����A�N�Z�X����ꍇ�̃Z�O�����g
// prot_addr_off01 : CALLBUFS�̈�̃v���e�N�g���[�h����A�N�Z�X����ꍇ�̃I�t�Z�b�g
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

        mov     ax, 250dh           ;RUN386�̃V�X�e���R�[���̌Ăяo��
        int     21h

        mov     off_prog, ax        ;�v���e�N�g���[�h�R�[���p�v���V�[�W���̃A�h���X
        shr     eax, 16             ; ss:sp��PWORD PTR FAR address of procedure to call
        mov     seg_prog, ax

        mov     offset1, bx         ;���L�o�b�t�@�̃Z�O�����g�ƃI�t�Z�b�g
        shr     ebx, 16
        mov     segment1, bx

        mov     size1, ecx          ;���L�o�b�t�@�̃T�C�Y

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

// �v���e�N�g���[�h���烊�A�����[�h�̃v���O�������Ăяo�����߂�RUN386�̃V�X�e���R�[�����Ăяo���Ă܂�
// call_realmode_addr : ���A�����[�h�v���O�����̃A�h���X(���16bit�����A�����[�h�̃Z�O�����g�A����16bit�����A�����[�h�̃I�t�Z�b�g
// copy_stack_number  : ���A�����[�h�̃v���O�������Ăяo���Ƃ��ɃR�s�[����X�^�b�N�e�ʂ��w��
// p_param_rm_call    : ���A�����[�h�̃v���O�������Ăяo���Ƃ��ɐݒ肷�郌�W�X�^�̓��e���w��(struct rm_call�̃|�C���^���w��)
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
        mov     dword ptr ret01, 0              ;����
        jmp     call_realmode_procedure_jmp02
call_realmode_procedure_jmp01:
        pop     ds
        mov     dword ptr ret01, -1             ;���s
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

// ���A�����[�hBIOS���Ăяo�����߂ɕK�v�ȃ��A�����[�h�̃v���O������CALLBUFS�̈�ɓǂݍ���ŏ��������܂�
// protect_mode_addr : CALLBUFS�̈�̐擪�A�h���X(CALLBUFS�̈��0x0100�Ƀv���O���������[�h����܂�)
// protect_mode_addr : CALLBUFS�̈�̃T�C�Y���w��
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

// ���A�����[�hBIOS���Ăяo���܂�
// int_no      : BIOS�R�[����int�̒l
// protect_mode_addr : CALLBUFS�̈�̐擪�̃v���e�N�g���[�h�p��far�|�C���^���w��
// realmode_segment01: CALLBUFS�̈�̐擪�̃��A�����[�h�Z�O�����g���w��
// rmi_call_in       : ���A�����[�hBIOS���Ăяo���Ƃ��̃��W�X�^�l��ݒ�(struct rmi_call�̃|�C���^)
// rmi_call_out      : ���A�����[�hBIOS�����s������̃��W�X�^�l��ۑ�����ꏊ�̐ݒ�(struct rmi_call�̃|�C���^)
// copy_stack_num    : ���A�����[�h�̃v���O�������Ăяo���Ƃ��ɃR�s�[����X�^�b�N�̗e�ʂ�ݒ�
int rmi_call(int int_no, unsigned char far *protect_mode_addr, unsigned short realmode_segment01, struct rmi_call *rmi_call_in, struct rmi_call *rmi_call_out, int copy_stack_num) {
    struct rm_call *rm_call01;
    struct rm_call rm_call001;
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

// ���݂̃v���e�N�g���[�h�̃f�[�^�Z�O�����g�̃Z���N�^�l���擾
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

// ���݂̃v���e�N�g���[�h�̃R�[�h�Z�O�����g�̃Z���N�^�l���擾
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
