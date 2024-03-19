
#include <stdio.h>
#include <stdlib.h>

struct regs {
    char     f;
    char     a;
    unsigned bc;
    unsigned de;
    unsigned hl;
};


void ginit() __sdcccall(0);
void line(int x1, int y1, int x2, int y2, int color, int logi) __sdcccall(0);
void boxfil(int x1, int y1, int x2, int y2, int color, int logi) __sdcccall(0);
void calbio(unsigned ad, struct regs *rg) __sdcccall(0);

void vpoke(unsigned int ad, char dt) __sdcccall(0);
char vpeek(unsigned int ad) __sdcccall(0);
void ldirvm(unsigned int ds, char *sr, unsigned int ln) __sdcccall(0);
void ldirmv(char *ds, unsigned int sr, unsigned int ln) __sdcccall(0);
void filvrm(unsigned int ad, unsigned int ln, char dt) __sdcccall(0);
void cpym2v(char *sr, int dr, int dx, int dy, int dp, int lg) __sdcccall(0);
void cpyv2v(int x1, int y1, int x2, int y2, int sp, int dx, int dy, int dp, int lg) __sdcccall(0);
void palett(int pl, int r, int g, int b) __sdcccall(0);
unsigned int rnd(unsigned int r) __sdcccall(0);
int getch() __sdcccall(0);
int getch_non_wait() __sdcccall(0);
int kbhit() __sdcccall(0);
