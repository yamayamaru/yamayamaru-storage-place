
#include <stdio.h>
#include <stdlib.h>

struct regs {
    char     f;
    char     a;
    unsigned bc;
    unsigned de;
    unsigned hl;
};


void ginit();
void line_m(int x1, int y1, int x2, int y2, int color, int logi);
void boxfil(int x1, int y1, int x2, int y2, int color, int logi);
void calbio(unsigned ad, struct regs *rg);

void vpoke(unsigned int ad, char dt);
char vpeek(unsigned int ad);
void ldirvm(unsigned int ds, char *sr, unsigned int ln);
void ldirmv(char *ds, unsigned int sr, unsigned int ln);
void filvrm(unsigned int ad, unsigned int ln, char dt);
void cpym2v(char *sr, int dr, int dx, int dy, int dp, int lg);
void cpyv2v(int x1, int y1, int x2, int y2, int sp, int dx, int dy, int dp, int lg);
void palett(int pl, int r, int g, int b);
unsigned int rnd(unsigned int r);

