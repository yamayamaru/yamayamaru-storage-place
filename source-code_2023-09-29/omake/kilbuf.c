#include <stdio.h>
#include "msxclib.h"

int main() {
    puts("Type some words!\n");
	puts(" and hit [RETURN] key to exit.");
	while ((snsmat(7) & 0x80) != 0);
	kilbuf();

	return 0;
}

