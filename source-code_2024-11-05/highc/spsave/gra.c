/*
 *    グラフィック系共通関数
 *
 */
#include <dos.h>
#include <egb.h>

void wait(int c)
{
    int i;
    for (i = 0; i < c; i++);
}

void EGB_singleLine(char *egbwork, int x1, int y1, int x2, int y2)
{
    struct {
        short int num, x1, y1, x2, y2;
    } par = {2, 0, 0, 0, 0};

    par.x1 = x1;
    par.y1 = y1;
    par.x2 = x2;
    par.y2 = y2;

    EGB_connect(egbwork, (char *)&par);
    return;
}

void pict(char *egbwork, int x, int y, int e)
{
    struct {
        short num, x, y;
    } par = {1, 0, 0};

    par.x = x;
    par.y = y;

    EGB_color(egbwork, 0, e);
    EGB_pset(egbwork, (char *)&par);
    return;
}


void geta(char *egbwork, char *pat, int x1, int y1, int x2, int y2)
{
    struct SREGS sreg;
    struct {
        char    *adr;
        short   ds, x1, y1, x2, y2;
    } par;

    segread(&sreg);

    par.adr = pat;
    par.ds = sreg.ds;
    par.x1 = x1;
    par.y1 = y1;
    par.x2 = x2;
    par.y2 = y2;
    EGB_getBlock(egbwork, (char *)&par);
    return;

}

int point(char *egbwork, int x, int y)
{
    int    c;

    geta(egbwork, (char *)&c, x, y, x, y);
    return c;

}

void palette(char *egbwork, int p, int r, int g, int b)
{
    struct {
        int    num;
        int    p;
        char   b, r, g;
    } par = {1, 0, 0, 0, 0};

    par.p = p;

    par.g = (char)g;
    par.r = (char)r;
    par.b = (char)b;

    EGB_palette(egbwork, 1, (char *)&par);
}
