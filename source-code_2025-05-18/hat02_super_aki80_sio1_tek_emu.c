#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define CTC0PORT        0x10
#define CTC1PORT        0x11
#define CTC2PORT        0x12
#define CTC3PORT        0x13
#define PIOAPORT_DATA   0x1c
#define PIOAPORT_CMD    0x1d
#define PIOBPORT_DATA   0x1e
#define PIOBPORT_CMD    0x1f
#define SIOAPORT_DATA   0x18
#define SIOAPORT_CMD    0x19
#define SIOBPORT_DATA   0x1a
#define SIOBPORT_CMD    0x1b
#define SIOVECT         0xf800
#define PIO1VECT        0xf810
#define CTC0VECT        0xf818
#define SIOVECT_PTR  ((volatile uint16_t *) SIOVECT)
#define PIO1VECT_PTR ((volatile uint16_t *) PIO1VECT)
#define CTC0VECT_PTR ((volatile uint16_t *) CTC0VECT)


void outp(uint8_t port, uint8_t data);
uint8_t inp(uint8_t port);
void z80_ei(void);
void z80_di(void);
void super_aki80_init(void);
void init_sio(void);
int sioaout_ok(void);
void init_ctc0(void);
void stop_ctc0(void);
unsigned long millis(void);
void delay(unsigned long millisec);
extern void ctc0_intcall(void);
extern void sioa_intcall(void);
extern void iretmon(void);
int sioa_read_available(void);
int sioa_read(void);




void init_tek_emu();
void tek_set_color(int color);
void tek_draw_pixel(int px, int py);
void tek_draw_line(int px1, int py1, int px2, int py2);

void hat02(void);
int main(int argc, char *argv[]) {
	int ch, a;

    z80_di();

    super_aki80_init();
    init_sio();
    init_ctc0();

    z80_ei();

    while(1) {

        hat02();

	    delay(60000);
	}

	return 0;
}



float d[160];
int xmax, ymax;
void hat02() {
    int      i, j, x, y, zz, px, py, f, sxi;
    int      col;
    float    dr, r, z, sx, sy, a;
    unsigned long start_time, end_time;

    xmax = 1024;
    ymax =  780;

    start_time = millis();

    init_tek_emu(); 

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }
    dr = 3.141592653589793 / 180.0;
    for (y = -180; y <= 180; y += 6) {
        for (x = -180; x < 180; x += 4) {
            a = (float)x * (float)x + (float)y * (float)y;
            r = dr * sqrtf(a);
            z = 100 * cosf(r) - 30 * cosf(3*r);
            sx = 80 + x / 3 - y / 6;
            sy = 40 - y / 6 - z / 4;
                        sxi = sx;
            if ((sxi < 0) || (sxi >= 160)) continue;
            if (d[sxi] <= sy) continue;
            zz = (int)((z + 100) * 0.035) + 1;

            col = 0;
            f = 0;
            if ((zz == 1) || (zz == 3) || (zz == 5) || (zz == 7)) {
                col = col | 4;
                f = 1;
            }
            if ((zz == 2) || (zz == 3) || (zz >= 6)) {
                col = col | 1;
                f = 1;
            }
            if ((zz >= 4)) {
                col = col | 2;
                f = 1;
            }
            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);
            if (f == 1) {
                tek_set_color(col);
                for (j = 0; j < 7; j++) {
                    tek_draw_line(px, py, px + 6, py + j);
                }
            }
            d[sxi] = sy;
        }
    }

    end_time = millis();
    printf("\r\n");
    printf("time = %ld sec\r\n", (end_time - start_time) / 1000UL);
}



// Teraterm TEK emulation init
void init_tek_emu() {
	putchar(0x1b);
	putchar(0x0c);
	putchar(0x0d);
	putchar(0x0a);
}

// Teraterm TEK emulation set color
void tek_set_color(int color) {
    unsigned char cc;
    cc = 0x30 + (color & 0x0f);

	putchar(0x1b);
	putchar(0x5b);
	putchar(0x33);
	putchar(cc);
	putchar(0x6d);
}

// Teraterm TEK emulation draw pixel
void tek_draw_pixel(int px, int py) {
    unsigned char c1, c2, c3, c4, c5, c6, c7, c8;
	int py1;
	py1 = 780 - py;
    
    c1 = 0x20 + ((py1 >> 5) & 0x1f);
    c2 = 0x60 + (py1 & 0x1f);
    c3 = 0x20 + ((px >> 5) & 0x1f);
    c4 = 0x40 + (px & 0x1f);
    c5 = 0x20 + ((py1 >> 5) & 0x1f);
    c6 = 0x60 + (py1 & 0x1f);
    c7 = 0x20 + ((px >> 5) & 0x1f);
    c8 = 0x40 + (px & 0x1f);

    putchar(0x1d);
    putchar(c1);
    putchar(c2);
    putchar(c3);
    putchar(c4);
    putchar(c5);
    putchar(c6);
    putchar(c7);
    putchar(c8);
}

// Teraterm TEK emulation draw line
void tek_draw_line(int px1, int py1, int px2, int py2) {
    unsigned char c1, c2, c3, c4, c5, c6, c7, c8;
	int py1a, py2a;
	py1a = 780 - py1;
	py2a = 780 - py2;

    c1 = 0x20 + ((py1a >> 5) & 0x1f);
    c2 = 0x60 + (py1a & 0x1f);
    c3 = 0x20 + ((px1 >> 5) & 0x1f);
    c4 = 0x40 + (px1 & 0x1f);
    c5 = 0x20 + ((py2a >> 5) & 0x1f);
    c6 = 0x60 + (py2a & 0x1f);
    c7 = 0x20 + ((px2 >> 5) & 0x1f);
    c8 = 0x40 + (px2 & 0x1f);

    putchar(0x1d);
    putchar(c1);
    putchar(c2);
    putchar(c3);
    putchar(c4);
    putchar(c5);
    putchar(c6);
    putchar(c7);
    putchar(c8);
}

// SDCCでは1文字出力関数のputcharを作っておくとprintfで文字出力ができるようにります
int putchar(int ch) {
    __asm
        push    ix
        push    hl

        ld      e, l
        ld      d, #0

putchar_loop0001:
        in      a, (SIOAPORT_CMD)
        and     #0x04
        jp      z, putchar_loop0001
        ld      a, e
        out     (SIOAPORT_DATA), a

        pop     hl
        pop     ix
    __endasm;

    return 0;
}

void outp(uint8_t port, uint8_t data) {
   __asm
       ld      c, a
       out     (c), l
   __endasm;;
}

uint8_t inp(uint8_t port) {
   __asm
       ld      c, a
       in      a, (c)
   __endasm;;
}

void z80_ei(void) {
    __asm
        ei
    __endasm;;
}

void z80_di(void) {
    __asm
        di
    __endasm;;
}

void super_aki80_init(void) {
    __asm
        ld                bc, #SIOVECT
        ld                a, b
        ld                i, a
        im                2
    __endasm;;
}

void intcall01(void) {
    __asm

        jp              intcal01_jmp01

_ctc0_intcall::
        push            ix
        push            iy
        push            af
        push            hl
        push            bc
        push            de

        call            _ctc0_intcall_func

        pop             de
        pop             bc
        pop             hl
        pop             af
        pop             iy
        pop             ix
        ei
        reti

_sioa_intcall::
        push            ix
        push            iy
        push            af
        push            hl
        push            bc
        push            de

        call            _sioa_intcall_func

        pop             de
        pop             bc
        pop             hl
        pop             af
        pop             iy
        pop             ix
        ei
        reti

_iretmon::
        ei
        reti

intcal01_jmp01::

    __endasm;;
}

void init_ctc0(void) {
    volatile unsigned int *ctcvect01;
    ctcvect01 = CTC0VECT_PTR;
    *ctcvect01 = (volatile unsigned int)ctc0_intcall;
    __asm
        di
        ld              bc, #CTC0VECT       ;set interrupt vect
        ld                a, c
        out             (CTC0PORT), a
        ld              a, #0x87     ;割り込みあり、タイマーモード、1/16、
        out             (CTC0PORT), a
        ld              a, #154      ; 1/4000秒, 9830400/4000/16 = 153.6
        out             (CTC0PORT), a
        ei
    __endasm;;
}

void stop_ctc0(void) {
    __asm
        di
        ld              a, #0x03
        out             (CTC0PORT), a
        ei
    __endasm;;
}

volatile unsigned long counter01 = 0;
void ctc0_intcall_func(void) {
    counter01++;
}

unsigned long millis(void) {
    return (unsigned long)counter01 / 4;
}

void delay(unsigned long millisec) {
    unsigned long t1;
    t1 = millis();
    while ((millis() - t1) < millisec);
}


extern volatile int sioa_intcall_buf_rd_point;
extern volatile int sioa_intcall_buf_wr_point;
extern volatile int sioa_intcall_ch_count;
void init_sio(void) {

    *(SIOVECT_PTR + 0) = (uint16_t)sioa_intcall;
    *(SIOVECT_PTR + 1) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 2) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 3) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 4) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 5) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 6) = (uint16_t)iretmon;
    *(SIOVECT_PTR + 7) = (uint16_t)iretmon;

    outp(SIOAPORT_CMD, 0x00);
    outp(SIOAPORT_CMD, 0x18);    // reset
    outp(SIOAPORT_CMD, 0x14);
    //outp(SIOAPORT_CMD, 0x04);  // x1,  parity non
    outp(SIOAPORT_CMD, 0x44);    // x16, parity non
    outp(SIOAPORT_CMD, 0x03);
    outp(SIOAPORT_CMD, 0xc1);    // recive character length = 8bit, receiver enable
    outp(SIOAPORT_CMD, 0x05);
    outp(SIOAPORT_CMD, 0x68);    // send character length = 8bit, send enable
    outp(SIOAPORT_CMD, 0x01);
    outp(SIOAPORT_CMD, 0x10);    // rx interrupt enable

    outp(SIOBPORT_CMD, 0x00);
    outp(SIOBPORT_CMD, 0x18);    // reset
    outp(SIOBPORT_CMD, 0x02);
    outp(SIOBPORT_CMD, (SIOVECT & 0xff));    // set sio interrupt vector


    outp(CTC3PORT, 0x07);        // CTC3 x16
    outp(CTC3PORT, 0x04);        // CTC3 count = 4

    sioa_intcall_buf_rd_point = 0;
    sioa_intcall_buf_wr_point = 0;
    sioa_intcall_ch_count = 0;
}

#define CH_BUFFER_SIZE 1024
volatile int sioa_intcall_buf_rd_point = 0;
volatile int sioa_intcall_buf_wr_point = 0;
volatile int sioa_intcall_ch_count = 0;
unsigned char sioa_intcall_buf[CH_BUFFER_SIZE];
void sioa_intcall_func(void) {
    uint8_t a, ch;
    a = inp(SIOAPORT_CMD);
    if (((int)a & 0x01) == 1) {
        ch = inp(SIOAPORT_DATA);
        if (sioa_intcall_ch_count < CH_BUFFER_SIZE) {
            sioa_intcall_ch_count++;
            sioa_intcall_buf[sioa_intcall_buf_wr_point] = ch;
            sioa_intcall_buf_wr_point++;
            if (sioa_intcall_buf_wr_point >= CH_BUFFER_SIZE) {
                 sioa_intcall_buf_wr_point -= CH_BUFFER_SIZE;
            }
        }
    }
}

int sioa_read(void) {
    int ch;
    if (sioa_intcall_ch_count) {
        z80_di();
        sioa_intcall_ch_count--;
        ch = sioa_intcall_buf[sioa_intcall_buf_rd_point];
        sioa_intcall_buf_rd_point++;
        if (sioa_intcall_buf_rd_point >= CH_BUFFER_SIZE) {
            sioa_intcall_buf_rd_point -= CH_BUFFER_SIZE;
        }
        z80_ei();
        return ch;
    } else {
        return -1;
    }
}

int sioa_read_available(void) {
    return (sioa_intcall_ch_count > 0);
}

int sioaout_ok(void) {
    uint8_t stat01 = inp(SIOAPORT_CMD);
    int ret = 0;
    if((int)stat01 & 0x4) {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}
