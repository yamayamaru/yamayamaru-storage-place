#include <stdio.h>

int main(int argc, char *argv[]) {

    FILE *fp, *fp2;
    int  c;

    if (argc != 2) {
	printf("usage : rmeof filename\n");
	return 1;
    }
    if ((fp = fopen(argv[1], "rb")) == NULL) {
	printf("file open error %s\n", argv[1]);
	return 1;
    }
    fprintf(stderr,"open file %s\n", argv[1]);
    remove("NONAME99.BAK");
    if ((fp2 = fopen("NONAME99.BAK", "wb")) == NULL) {
        printf("file open error noname99.bak\n");
        fclose(fp);
        return 1;
    }
    while(1) {
	c = fgetc(fp);
	if (feof(fp)) break;
	if (c == 0x1a) continue;
	fputc(c,fp2);
    }
    fclose(fp);
    fclose(fp2);

    remove(argv[1]);
    rename("NONAME99.BAK", argv[1]);

    return 0;
}
