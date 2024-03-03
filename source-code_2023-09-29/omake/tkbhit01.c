#include <stdio.h>
#include "msxalib.h"
#include "msxclib.h"

int main(int argc, char *argv[]) {
	char c;

    printf("agec = %d\r\n", argc);
	for (int i = 0; i < argc; i++) {
	    printf("%s", argv[i]);
	}

	while (!kbhit());

	while (1) {
		printf("0x%04x\r\n",(int)kbhit());
	    if ((char)(c = getch_non_wait()) == 'q') break;
	}

	return 0;
}
