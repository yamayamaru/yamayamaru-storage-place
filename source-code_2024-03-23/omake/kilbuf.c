#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <glib.h>
#include <msxbios.h>
#include <msxalib.h>
#include <msxclib.h>
#include <msxc_def.h>


int main() {
    puts("Type some words!\n");
	puts(" and hit [RETURN] key to exit.");
	while ((snsmat(7) & 0x80) != 0);
	kilbuf();

	return 0;
}

