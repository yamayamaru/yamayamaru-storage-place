#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


char Ptspr0[32] = {
	0x0f, 0x3f, 0x63, 0x41, 0x41, 0x41, 0x63, 0x7f,
	0x4f, 0x1e, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00,
	0xc0, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 0x78, 0x78,
	0x78, 0xf0, 0xf4, 0xf4, 0xf6, 0x6e, 0x3e, 0x1c };

char Ptspr1[32] = {
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x03, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0 };

char Ptspr2[32] = {
	0x00, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0x7f, 0xff,
	0xfc, 0xff, 0xf7, 0xfb, 0xfb, 0x7b, 0x77, 0x3f,
	0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xf8, 0xfc,
	0x7c, 0xbe, 0xde, 0xde, 0xde, 0x5e, 0x5e, 0xbc };

char stop() {
    static char old_s = 0;
	char s, r;

	s = ((snsmat(7) & 0x10) == 0);
	r = (s && !old_s);
	old_s = s;
	return r;
}

int main() {
    char x = 0;
	int i;

	color(15, 1, 1);
	screen(1);
	inispr(2);
	sprite(0, Ptspr0);
	sprite(4, Ptspr1);
	sprite(8, Ptspr2);

	while ((snsmat(7) & 0x80) != 0) {
	    if (stop()) {
		    putspr(0, x, 150, 4, 8);
			putspr(1, x, 216, 0, 0);
			while (!stop());
		}
		x -= 2;
		putspr(0, x, 150, 4, 0);
		putspr(1, x, 150, 15, 4);
		for (i = 0; i < 20000; ++i);
	}

	screen(0);

	return 0;
}

