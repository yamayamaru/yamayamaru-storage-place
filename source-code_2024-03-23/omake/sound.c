#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


int main()
{
	static int datapsg[14] = {
		55, 0, 0, 0, 0, 0, 29, 54,
		16, 0, 0, 116, 1, 12};
	int i;

	gicini();

	for (i = 0; i < 14; ++i) {
		sound(i, datapsg[i]);
	}
	getch();
	gicini();

	return 0;
}
