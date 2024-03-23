struct regs {
    char     f;
    char     a;
    unsigned bc;
    unsigned de;
    unsigned hl;
};

void calbio(unsigned ad, struct regs *rg);
void ldirvm(unsigned int ds, char *sr, unsigned int ln);
void ldirmv(char *ds, unsigned int sr, unsigned int ln);
void filvrm(unsigned int ad, unsigned int ln, char dt);
void cpym2v(char *sr, int dr, int dx, int dy, int dp, int lg);
void cpyv2v(int x1, int y1, int x2, int y2, int sp, int dx, int dy, int dp, int lg);
void palett(int pl, int r, int g, int b);
void line(int x1, int y1, int x2, int y2, int color, int logi);
void boxfil(int x1, int y1, int x2, int y2, int color, int logi);
unsigned int rnd(unsigned int r);
