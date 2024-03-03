#include <stdio.h>
#include "msxalib.h"
#include "msxclib.h"

void print_line(int x1, int y1, int x2, int y2, int color, int lg);
void print_str(char *);

extern char RDPORT01, WRPORT01;

char buff[256];
int main() {
    unsigned x1, y1, x2, y2, c, i, j, k;
	unsigned char     cc;

	ginit();
	screen(0);
	color(15, 1, 1);


	printf("RDPORT = 0x%02x, WRPORT=0x%02x\r\n", (int)RDPORT01, (int)WRPORT01);
	sprintf(buff,"RDPORT = 0x%02x, WRPORT=0x%02x\r\n$", (int)RDPORT01, (int)WRPORT01);
	print_str(buff);
    print_line(10, 20, 250, 210, 7, 3);
    print_line(10, 20, 250, 210, 7, 3);
    print_line(10, 20, 250, 210, 7, 3);
    print_line(10, 20, 250, 210, 7, 3);

    cc = getch();

	screen(5);

    color(15, 1, 1);
    x1 = 128;
    y1 = 106;
    c = 0;
    i = j = k = 1;

//	while(!kbhit()) cc = getch();

	long cnt = 0;
    while (cnt < 10000L) {
        x2 = 256 - x1;
        y2 = 212 - y1;
		if (x1 > 0)
		   if (x1 < 255)
		       if(y1 > 0)
				   if(y1 < 211) {
            line(x1, y1, x2, y1, c, 3);
            line(x1, y2, x2, y2, c, 3);
            line(x1, y1, x1, y2, c, 3);
            line(x2, y1, x2, y2, c, 3);
		}
		if (x1 < 10) i = k;
		if (x1 > 240) i = -k;
		if (y1 < 10) j = k;
        if (y1 > 200) j = -k;
		x1 = x1 + i;
		y1 = y1 + j;
        if (++c == 16) c = 0;
		cnt++;
    }

	screen(0);

	return 0;
}

