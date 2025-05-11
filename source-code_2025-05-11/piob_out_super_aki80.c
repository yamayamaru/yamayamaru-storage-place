#include <stdio.h>
#include <stdint.h>

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

void wait01(int number01);

void main(void) {
	int i;

    super_aki80_init();
    init_sio();

	outp(PIOBPORT_CMD, 0x0f);                 // Z80 PIO mode 0(ポートは出力、割り込みなし)に設定

    for (;;) {
        for (i = 0; i < 256; i++) {
			outp((uint8_t)PIOBPORT_DATA, (uint8_t)i);     // Z80 PIOのPORT Bにデータを出力
			printf("PORT B = %d\r\n", i);       // Serialに文字出力

		    wait01(1);                       // 一定時間待機
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

void init_sio(void) {

    outp(SIOAPORT_CMD, 0x00);
    outp(SIOAPORT_CMD, 0x18);
    outp(SIOAPORT_CMD, 0x14);
    //outp(SIOAPORT_CMD, 0x04);  // x1,  parity non
    outp(SIOAPORT_CMD, 0x44);    // x16, parity non
    outp(SIOAPORT_CMD, 0x03);
    outp(SIOAPORT_CMD, 0xc1);
    outp(SIOAPORT_CMD, 0x05);
    outp(SIOAPORT_CMD, 0x68);

    outp(CTC2PORT, 0x07);      //  x16
    outp(CTC2PORT, 0x01);      // count = 1
}

int getch(void) {
    uint8_t stat01 = inp(SIOAPORT_CMD);
    int ret = -1;
    if ((int)stat01 & 0x1) {
        ret = (int)inp(SIOAPORT_DATA);
    } else {
        ret = -1;
    }

    return ret;
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

