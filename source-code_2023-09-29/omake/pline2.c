#include <stdio.h>
#include "msxalib.h"
#include "msxclib.h"

void print_line(int x1, int y1, int x2, int y2, int color, int lg);
void print_line2(int x1, int y1, int x2, int y2, int color, int lg);
void print_str(char *);
void print_dec_2(int a);
void print_abs_value(void);

extern char RDPORT01, WRPORT01;

char buff[256];
int main() {
    int x1, y1, x2, y2, j, c1, cnt;
	unsigned char     cc;

	ginit();
	screen(0);
	color(15, 1, 1);


	printf("RDPORT = 0x%02x, WRPORT=0x%02x\r\n", (int)RDPORT01, (int)WRPORT01);
	sprintf(buff,"RDPORT = 0x%02x, WRPORT=0x%02x\r\n$", (int)RDPORT01, (int)WRPORT01);
	print_str(buff);
	printf("T32NAM = 0x%04x, T32CGP = 0x%04x\r\n", *((int *)0xf3bd), *((unsigned *)0xf3c1));
//    print_line(10, 20, 250, 210, 7, 3);
//	print_abs_value();

    while (!kbhit());

    screen(5);
//    screen(0);

    color(15, 1, 1);
    x1 = 128;
    y1 = 106;
    c1 = 0;
    j = 1;

//	while(!kbhit()) cc = getch();

    cnt = 0;
    while ((char)cc != 'q') {
		cc = getch_non_wait();
		line(10, y1, 250, y1, c1, 0);
		if (y1 == 10) {
			j = 1;
		}
		if (y1 == 200) {
			j = -1;
		}
		y1 = y1 + j;

/*
		if (y1 > 200) {
			y1 = 10;
		}
        y1++;
*/

		c1 = c1 + 1;
		cnt++;

		if (c1 >= 16) {
			c1 = 0;
		}
    }

	screen(0);

	return 0;
}

