#include <stdint.h>
#include <string.h>

#define CTC0PORT        0x10
#define CTC1PORT        0x11
#define CTC2PORT        0x12
#define CTC3PORT        0x13
#define PIOAPORT_DATA   0x1c
#define PIOAPORT_CMD    0x1d
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
void ctc0_intcall(void);
void ctc0_intcall_func(void);
void init_ctc0(void);
void stop_ctc0(void);
void init_sio(void);
int getch(void); 
int kbhit(void); 
int sioaout_ok(void); 
long millis(void);
void super_aki80_init(void);
extern volatile long counter01;
void print_uint_decimal_number(unsigned int a); 
void print_int_decimal_number(int a); 
void print_float_fix_point_number(float a); 
void print_str(const char *str);
void putchar01(int c);
void iretmon();

void set_m_param01();

struct m_param {
    float    xcorner;
    float    ycorner;
    float    length;
    int       depth;
} m_param01[7];

int mandel(double a1, double b1, int depth1); 

volatile int dummy = 0;
void main(void) {


    super_aki80_init();
    init_sio();

    int endflag01 = 0;

    //        xcorner = -2.4;
    //        ycorner = -1.7;
    //        length = 3.2;
    //        depth = 1000;
    int      i, j;
    double   xcorner, ycorner, length, xgap, ygap;
    int      depth;
    double   x01, y01;
    int      xdot, ydot;
    int      col01, colr, colg, colb;
    int      number01;


    set_m_param01();


    for (;;) {
        int endflag01 = 0;

        init_ctc0();

        unsigned long start_time_all = millis();

        for (number01 = 0; number01 < 7; number01++) {



            unsigned long start_time = millis();
            int count001 = 0;

            xcorner = m_param01[number01].xcorner;
            ycorner = m_param01[number01].ycorner;
            length  = m_param01[number01].length;
            depth   = m_param01[number01].depth;

            depth   = 100;

            xdot = 60, ydot = xdot * 0.4;

            xgap = length / xdot;
            ygap = length / ydot;


            print_str("\033[0m");
            print_str("\r\n");
            print_str("xcorner = ");
            print_float_fix_point_number(xcorner);
            print_str("\r\n");
            print_str("ycorner = ");
            print_float_fix_point_number(ycorner);
            print_str("\r\n");
            print_str("length = ");
            print_float_fix_point_number(length);
            print_str("\r\n");
            print_str("depth = ");
            print_int_decimal_number(depth);
            print_str("\r\n");
            print_str("\r\n");
            print_str("xdot = ");
            print_int_decimal_number(xdot);
            print_str("\r\n");
            print_str("ydot = ");
            print_int_decimal_number(ydot);
            print_str("\r\n");

            for (j = 0; j < ydot; j++) {
                y01 = ycorner + j * ygap;
                for (i = 0; i < xdot; i++) {
                    x01 = xcorner + i * xgap;
                    col01 = mandel(x01, y01, depth);
                    colr = (col01 % 32) * 8 + 7;
                    colg = (col01 % 32) * 8 + 7;
                    colb = (col01 % 32) * 4 + 128;

                    if (col01 == 0) {
                        colr = 0;
                        colg = 0;
                        colb = 0;
                    }

                    if (col01 != 0) {
                        switch (col01 % 15 + 1) {
                            case 1:
                                print_str("\033[0;31m");
                                break;
                            case 2:
                                print_str("\033[0;32m");
                                break;
                            case 3:
                                print_str("\033[0;33m");
                                break;
                            case 4:
                                print_str("\033[0;34m");
                                break;
                            case 5:
                                print_str("\033[0;35m");
                                break;
                            case 6:
                                print_str("\033[0;36m");
                                break;
                            case 7:
                                print_str("\033[0;37m");
                                break;
                            case 8:
                                print_str("\033[1;30m");
                                break;
                            case 9:
                                print_str("\033[1;31m");
                                break;
                            case 10:
                                print_str("\033[1;32m");
                                break;
                            case 11:
                                print_str("\033[1;33m");
                                break;
                            case 12:
                                print_str("\033[1;34m");
                                break;
                            case 13:
                                print_str("\033[1;35m");
                                break;
                            case 14:
                                print_str("\033[1;36m");
                                break;
                            case 15:
                                print_str("\033[1;37m");
                                break;
                        }
                        print_str("*");
                    } else {
                        print_str(" ");
                    }

                    dummy += col01;
                    count001++;
                }
                if (endflag01 == 1) break;
                print_str("\r\n");
            }

            unsigned long end_time = millis();

            unsigned long time01 = (end_time - start_time) % 1000 / 10;
            unsigned long time02 = (end_time - start_time) % 10000000;

            print_str("\033[0m");
            print_str("time = ");
            float time000001 = (end_time - start_time) / 1000.0;
            print_float_fix_point_number(time000001);
            print_str(" sec\r\n");

            print_str("\r\n");
        }    


        unsigned long time03 = millis();
        for (;;) {
            if ((millis() - time03) > 20000L) break;
        }

        stop_ctc0();
    }
}

int mandel(double a1, double b1, int depth1) {
    int     i1;
    double  x1, y1, tmp1, x2, y2;

    i1 = 0;
    x1 = 0; y1 = 0;
    x2 = x1 * x1;   y2 = y1 * y1;
    while ((i1 < depth1) && (x2+y2 <=4)) {
        tmp1 = x2 - y2 + a1;
        y1 = 2 * x1 * y1 + b1;
        x1 = tmp1;
        x2 = x1 * x1;  y2 = y1 * y1;
        i1++;
    }

    if (i1 == depth1) i1 = 0;
    else            i1++;
    return i1;

}

void set_m_param01(){

    m_param01[0].xcorner =  -2.4;
    m_param01[0].ycorner =  -1.7;
    m_param01[0].length  =   3.2;
    m_param01[0].depth   =  1000;

    m_param01[1].xcorner = -6.1099999999999965e-001;
    m_param01[1].ycorner =  6e-001;
    m_param01[1].length  =  1.0000000000000001e-001;
    m_param01[1].depth   =  1000;

    m_param01[2].xcorner = -7.451562499999977e-002;
    m_param01[2].ycorner = -6.500117187500000e-001;
    m_param01[2].length = 7.812500000000000e-004;
    m_param01[2].depth   =  1000;

    m_param01[3].xcorner = -1.023473307291662e-001;
    m_param01[3].ycorner = 9.571370442708340e-001;
    m_param01[3].length = 4.882812500000000e-005;
    m_param01[3].depth   =  1000;

    m_param01[4].xcorner = -7.451562499999977e-002;
    m_param01[4].ycorner = -6.500117187500000e-001;
    m_param01[4].length = 7.812500000000000e-004;
    m_param01[4].depth   =  1000;

    m_param01[5].xcorner = -1.023473307291662e-001;
    m_param01[5].ycorner = 9.571370442708340e-001;
    m_param01[5].length = 4.882812500000000e-005;
    m_param01[5].depth   =  1000;

    m_param01[6].xcorner = -6.703997395833329e-001;
    m_param01[6].ycorner = -4.582591145833326e-001;
    m_param01[6].length = 3.906250000000000e-004;
    m_param01[6].depth   =  1000;
}


void print_uint_decimal_number(unsigned int a) {
    unsigned char c01[5];
    unsigned int aa = a;
    for (int i = 0; i < 5; i++){
        c01[i] = aa % 10; 
        aa = aa / 10;
    }
    int zeroflag = 0;
    for (int i = 0; i < 5; i++){
        if (!(c01[4-i] == 0 && zeroflag == 0))    {
            putchar01((c01[4-i] + 0x30));
            zeroflag = 1;
        } else {
            if ((4-i) == 0) {
                putchar01(c01[4-i] + 0x30);
            }
        }

    }
}

void print_int_decimal_number(int a) {
    if (a < 0) {
        putchar01('-');
        print_uint_decimal_number(0 - a);
    } else {
        print_uint_decimal_number(a);
    }
}

void print_float_fix_point_number(float a) {
    int minusflag = 0;
    float aa = a;
    unsigned int i01;
    if (a < 0) {
        minusflag = 1;
        aa = 0 - a;
        putchar01('-');
    }
    i01 = aa / 1.0;
    aa = aa - i01;
    print_uint_decimal_number(i01);
    putchar01('.');
    for (int i = 0; i < 5; i++) {
        aa = aa * 10.0;
        i01 = aa / 1.0;
        print_uint_decimal_number(i01);
        aa = aa - i01;
    }
}

void print_str(const char *str) {
    char *str1 = (char *)str;
    while (*(str1) != (char)0) {
        putchar01(*(str1++));
    }
}

/*
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
 */

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

long millis(void) {
    return (long)counter01 / 4;
}

volatile long counter01 = 0;
void ctc0_intcall_func(void) {
    counter01++;
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

    outp(CTC3PORT, 0x07);      //  x16
    outp(CTC3PORT, 0x04);      // count = 1
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

