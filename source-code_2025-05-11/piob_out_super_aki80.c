/* CTC0は時間計測カウンタ用、CTC3はRS232C用、SIOAはRS232C用に使ってます */




#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
#define SIOVECT         0xf800
#define PIO1VECT        0xf810
#define CTC0VECT        0xf818
#define SIOVECT_PTR  ((volatile uint16_t *) SIOVECT)
#define PIO1VECT_PTR ((volatile uint16_t *) PIO1VECT)
#define CTC0VECT_PTR ((volatile uint16_t *) CTC0VECT)

void outp(uint8_t port, uint8_t data);
uint8_t inp(uint8_t port);
void z80_ei();
void z80_di();
void super_aki80_init(void);
void init_sio(void);
int getch(void); 
int kbhit(void); 
int sioaout_ok(void); 
void init_ctc0(void); 
void stop_ctc0(void); 
unsigned long millis(void); 
void delay(unsigned long millisec); 
extern void ctc0_intcall();
extern void sioa_intcall();
extern void iretmon();
int sioa_read_available(); 
int sioa_read(); 

void wait01(int number01);

void main(void) {
    int i;

    z80_di();
    
    super_aki80_init();
    init_sio();
    init_ctc0();

    z80_ei();

    outp(PIOBPORT_CMD, 0x0f);                             // Z80 PIO mode 0(ポートは出力、割り込みなし)に設定

    for (;;) {
        for (i = 0; i < 256; i++) {
            outp((uint8_t)PIOBPORT_DATA, (uint8_t)i);     // Z80 PIOのPORT Bにデータを出力
            printf("PORT B = %d\r\n", i);                 // Serialに文字出力

//            wait01(1);                                  // 一定時間待機
            delay(500);
        }
    }
}

volatile int dummy = 0;
void wait01(int number01) {
    int i, j;
    for (j = 0; j < number01; j++) {
        for (i = 0; i < 0x0fff; i++) {
            dummy++;
        }
    }
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
   __endasm;
}

uint8_t inp(uint8_t port) {
   __asm
   ld      c, a
   in      a, (c)
   __endasm;
}

void z80_ei() {
    __asm
        ei
    __endasm;
}

void z80_di() {
    __asm
        di    
    __endasm;
}

void super_aki80_init(void) {
    __asm
        ld                bc, #SIOVECT
        ld                a, b
        ld                i, a
        im                2
    __endasm;
}

void intcall01() {
    __asm

_ctc0_intcall::
    push                ix
        push            iy
        push            af
        push            hl
        push            bc
        push            de

        call            _ctc0_intcall_func

        pop                     de
        pop                     bc
        pop                     hl
        pop                     af
        pop                     iy
        pop                     ix
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

    __endasm;
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
    __endasm;
}

void stop_ctc0(void) {
    __asm
        di
        ld              a, #0x03
        out             (CTC0PORT), a
        ei
    __endasm;
}

volatile unsigned long counter01 = 0;
void ctc0_intcall_func(void) {
    counter01++;
}

unsigned long millis(void) {
    return (long)counter01 / 4;
}

void delay(unsigned long millisec) {
    unsigned long t1;
    t1 = millis();
    while ((millis() - t1) < millisec);
}

extern volatile int sioa_intcall_buf_rd_point = 0;
extern volatile int sioa_intcall_buf_wr_point = 0;
extern volatile int sioa_intcall_ch_count = 0;
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
    outp(SIOAPORT_CMD, 0x18);
    outp(SIOAPORT_CMD, 0x02);
    outp(SIOAPORT_CMD, (SIOVECT & 0xff));
    outp(SIOAPORT_CMD, 0x14);
    //outp(SIOAPORT_CMD, 0x04);  // x1,  parity non
    outp(SIOAPORT_CMD, 0x44);    // x16, parity non
    outp(SIOAPORT_CMD, 0x03);
    outp(SIOAPORT_CMD, 0xc1);
    outp(SIOAPORT_CMD, 0x05);
    outp(SIOAPORT_CMD, 0x68);
    outp(SIOAPORT_CMD, 0x01);
    outp(SIOAPORT_CMD, 0x10);    // rx interrupt enable
 

    outp(CTC3PORT, 0x07);       // CTC3 x16
    outp(CTC3PORT, 0x04);       // CTC3 count = 1

    sioa_intcall_buf_rd_point = 0;
    sioa_intcall_buf_wr_point = 0; 
    sioa_intcall_ch_count = 0; 
}

#define CH_BUFFER_SIZE 1024
volatile int sioa_intcall_buf_rd_point;
volatile int sioa_intcall_buf_wr_point;
volatile int sioa_intcall_ch_count;
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

int sioa_read() {
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

int sioa_read_available() {
    return (sioa_intcall_ch_count > 0);
}


int kbhit(void) {
    uint8_t stat01 = inp(SIOAPORT_CMD);
    int ret = -1;
    if ((int)stat01 & 0x1) {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
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

