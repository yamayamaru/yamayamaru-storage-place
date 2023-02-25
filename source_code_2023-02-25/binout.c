/*
    gcc -o binout binout.c
*/


#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	FILE *fp;
	int  c;

    if (argc != 2) {
	    printf("usage:%s filename\n", argv[0]);
		return -1;
	}
	if((fp = fopen(argv[1],"ro")) == NULL) {
	    printf("%s ファイルが開けません\n", argv[1]);
		return -1;
	}
	for (int i = 0; i < 0x100; i++){
		if(feof(fp) != 0) break;
	    c = fgetc(fp);
	}
	while(feof(fp) == 0) {
		c = fgetc(fp);
		if(feof(fp) !=0) break;
		putchar(c);
	}

	fclose(fp);

	return 0;
}
