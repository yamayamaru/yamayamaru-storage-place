
#include <stdio.h>
#include <stdlib.h>
#include <msxalib.h>
#include <msxclib.h>
#include <time.h>
#include <conio.h>

void chgcpu_subfunc(char mode);
void chgcpu_r800_dram();
void chgcpu_r800_rom();
void chgcpu_z80();

void mandel(double xcorner, double ycorner, double length, double xdot, double ydot, double depth);
int mand(double a1, double b1, int depth1);
void set_m_param01();

struct m_param {
    double    xcorner;
    double    ycorner;
    double    length;
    int       depth;
} m_param01[21];


double second();

int main(int argc, char *argv[])
{

    double    xcorner, ycorner, length;
    int       xdot, ydot, depth;
    double    start_time, end_time;
    double    ratio01;
    int       i;
    int      number01;




    chgcpu_r800_dram();




	ginit();
    color(0, 0, 0);
    screen(8);
    boxfil(0, 0, 255, 211, 0, 0);

    start_time = second();

    i = 0;
    set_m_param01();

	xdot = 100;
	ydot = 100;

/*
    xcorner = -2.4;
    ycorner = -1.7;
    length = 3.2;
    depth = 1000;
*/


//    for (number01 = 0; number01 < 21; number01++){
    for (number01 = 0; number01 < 2; number01++){
        xcorner = m_param01[number01].xcorner;
        ycorner = m_param01[number01].ycorner;
        length  = m_param01[number01].length;
        depth   = m_param01[number01].depth;

		depth = 1000;
		depth = 100;

        mandel(xcorner, ycorner, length, xdot, ydot, depth);
    }

    end_time = second();

    getch();
	color(15, 1, 1);
	screen(0);



    chgcpu_z80();



	printf("time = %.4f sec\n", end_time - start_time);

	return 0;
}

int col(int color) {
	int  c;

    if (color == 0) return 0;
	c = color % 255 + 1;

    if (c == 0) return 0;
    if (c >=128) return (c - 128 + 1);
	else         return (c + 128);
}

void mandel(double xcorner, double ycorner, double length, double xdot, double ydot, double depth)
{
    double xgap, ygap;
    double xx, yy;
    int    i, j;
	int    c, color;

    xgap = length / xdot;
    ygap = length / ydot;
    xx = xcorner;
    yy = ycorner;
    for(j = 0; j < ydot; j++) {
        for(i = 0; i < xdot; i++) {
          c = mand( xx, yy, depth);

          line_m(i, j, i, j, col(c), 0);

          xx = xx + xgap;
        }
        yy = yy + ygap;
        xx = xcorner;
    }
}




int mand(double a1, double b1, int depth1)
{
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

        m_param01[0].xcorner = -2.4;                        m_param01[00].ycorner = -1.7;
        m_param01[0].length  =  3.2;                        m_param01[00].depth   = 1000;
        m_param01[1].xcorner = -6.1099999999999965e-001;    m_param01[01].ycorner = 6e-001;
        m_param01[1].length  =  1.0000000000000001e-001;    m_param01[01].depth   = 1000;
        m_param01[2].xcorner = -1.7878664118448890e+000;    m_param01[02].ycorner = -4.6994043985999932e-003;
        m_param01[2].length  =  1.5258789062500001e-006;    m_param01[02].depth   = 1000;
        m_param01[3].xcorner =  3.6540625000000060e-001;    m_param01[03].ycorner =  1.1625000000000031e-001;
        m_param01[3].length  =  6.2500000000000003e-003;    m_param01[03].depth   = 1000;
        m_param01[4].xcorner = -1.6247249348958287e-001;    m_param01[04].ycorner = -1.0341339518229167e+000;
        m_param01[4].length  =  2.4414062500000001e-005;    m_param01[04].depth   = 1000;

        m_param01[5].xcorner = -7.496979166666662e-001;     m_param01[5].ycorner = 8.252083333333364e-002;
        m_param01[5].length  =  3.125000000000000e-003;     m_param01[5].depth   = 1000;
        m_param01[6].xcorner = -1.942551920572915e+000;     m_param01[6].ycorner = -2.457682291648995e-005;
        m_param01[6].length  =  4.882812500000000e-005;     m_param01[6].depth   = 1000;
        m_param01[7].xcorner = -1.7860575510660803e+000;    m_param01[7].ycorner = 1.7055765787787343e-004;
        m_param01[7].length  =  6.1035156250000003e-006;    m_param01[7].depth   = 1000;
        m_param01[8].xcorner = -1.7860434722900387e+000;    m_param01[8].ycorner = 1.6493225097683180e-004;
        m_param01[8].length  =  1.5258789062500001e-006;    m_param01[8].depth   = 1000;
        m_param01[9].xcorner =  3.0717668660481867e-001;    m_param01[9].ycorner = 3.1935620625813711e-002;
        m_param01[9].length  =  9.7656250000000005e-005;    m_param01[9].depth   = 1000;

        m_param01[10].xcorner =  -7.513718439737953e-001;    m_param01[10].ycorner = 2.933079783121612e-002;
        m_param01[10].length  =   6.103515625000000e-006;    m_param01[10].depth   = 1000;
        m_param01[11].xcorner =  -7.513724441528318e-001;    m_param01[11].ycorner = 2.932934315999216e-002;
        m_param01[11].length  =   1.220703125000000e-005;    m_param01[11].depth   = 1000;
        m_param01[12].xcorner =  -7.512120844523107e-001;    m_param01[12].ycorner = 2.939359283447132e-002;
        m_param01[12].length  =   2.441406250000000e-005;    m_param01[12].depth   = 1000;
        m_param01[13].xcorner =  -1.255153877258301e+000;    m_param01[13].ycorner = 3.834366226196277e-001;
        m_param01[13].length  =   2.441406250000000e-005;    m_param01[13].depth   = 1000;
        m_param01[14].xcorner =  -1.255149571736653e+000;    m_param01[14].ycorner = 3.834463500976551e-001;
        m_param01[14].length  =   3.814697265625000e-007;    m_param01[14].depth   = 1000;

        m_param01[15].xcorner =  -1.255150037765502e+000;    m_param01[15].ycorner = 3.834445966084784e-001;
        m_param01[15].length  =   3.051757812500000e-006;    m_param01[15].depth   = 1000;
        m_param01[16].xcorner =  -1.255161532719929e+000;    m_param01[16].ycorner = 3.834325675964341e-001;
        m_param01[16].length  =   2.441406250000000e-005;    m_param01[16].depth   = 1000;
        m_param01[17].xcorner =  -1.255219516118367e+000;    m_param01[17].ycorner = 3.833973706563299e-001;
        m_param01[17].length  =   9.765625000000001e-005;    m_param01[17].depth   = 1000;
        m_param01[18].xcorner =  -1.258991814295450e+000;    m_param01[18].ycorner = 3.781393953959134e-001;
        m_param01[18].length  =   6.250000000000000e-003;    m_param01[18].depth   = 1000;
        m_param01[19].xcorner =  -1.255149231592813e+000;    m_param01[19].ycorner = 3.834458554585760e-001;
        m_param01[19].length  =   7.629394531250000e-007;    m_param01[19].depth   = 1000;

        m_param01[20].xcorner =  -1.255058807618605e+000;    m_param01[20].ycorner = 3.834129844820908e-001;
        m_param01[20].length  =   5.960464477539063e-009;    m_param01[20].depth   = 1000;


}

char second_10mili, second_sec, second_min, second_hour;
double second_get_time() {
	double time01;
		#asm
		second_bdos	equ	0005h
		push	af
		push	bc
		push	de
		push	hl

		ld		c,2ch
		call	second_bdos
		ld		a,e
		ld		(_second_10mili),a
		ld		a,d
		ld		(_second_sec),a
		ld		a,l
		ld		(_second_min),a
		ld		a,h
		ld		(_second_hour),a

		pop		hl
		pop		de
		pop		bc
		pop		af
        #endasm
	time01 = time(NULL) + second_10mili * 0.01;
	return time01;
}

double second() {

    static double count01 = 0.0;
	double count02;
	if (count01 < 0.00001) {
		count01 = second_get_time();
	} 
    count02 = second_get_time() - count01;

	return count02;
}
	
void chgcpu_subfunc(char mode) {
#asm

    ld      hl, 2
    add     hl, sp
    ld      a, (hl)

    push    af
    ld      a,(0fcc1h)      // a:slot EXPTBL:0xfcc1
    ld      hl,0180h      // hl:RDSLT read address
    call    000ch          // RDSLT
    ei
    pop     hl

    cp      0c3h
    jr      nz,chgcpu_subfunc_jmp01

    ld      iy,(0fcc1h-1)   // EXPTBL:0xfcc1
    ld      a,h         // A register:0x00:z80 mode 0x81:R800 ROM mode   0x82:R800 DRAM mode
    ld      ix,0180h      // ix:CALSLT call address
    call    001ch          // CALSLT

chgcpu_subfunc_jmp01:

#endasm
}

void chgcpu_r800_dram() {
    chgcpu_subfunc(0x82);
}

void chgcpu_r800_rom() {
    chgcpu_subfunc(0x81);
}

void chgcpu_z80() {
    chgcpu_subfunc(0x00);
}

