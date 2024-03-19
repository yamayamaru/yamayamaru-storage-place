#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msxalib.h"
#include "msxclib.h"
#include "hipman.h"

#define T32NAM *((int *)0xf3bd)
#define ALIVE 0
#define DEAD  1

unsigned char Maze[23][25];
unsigned Stage;
unsigned Dot, Rest;
int  Hx, Hy;
int  Htx, Hty;
unsigned Score, Addsco;
struct enemy E[3];
int Ecl[5] = { 8, 13, 3, 7, 4 };

extern void init(), drawmaze();
extern void highscore();
extern void putenemy(struct enemy *ep), mvenemy();

void putnum(int x, int y, unsigned n);
void putstr(int x, int y, char *s);
void mvplayer();
static int distance(struct enemy *ep);
unsigned char judge();
void starting();
static unsigned adjustspeed();
void game();
unsigned char replay();


void putnum(int x, int y, unsigned n)
{
    char digit[5];
    char *p;

    strcpy(digit, "     ");
    p = &(digit[5]);

    do {
        *--p = (n % 10) + '0';
    } while ((n /= 10) != 0);

    ldirvm(T32NAM + x + y * 32, digit, 5);
}

void putstr(int x, int y, char *s)
{
    ldirvm(T32NAM + x + y * 32, s, strlen(s));
}

void mvplayer()
{
    static int movex[9] = { 0,  0, 0, 1, 0, 0, 0, -1, 0 };
    static int movey[9] = { 0, -1, 0, 0, 0, 1, 0,  0, 0 };
    int i;

    i = (gtstck(0) | gtstck(1));

    if (i != 0 && Maze[Hy + movey[i]][Hx + movex[i]] != WALL) {
        Htx = movex[i];
        Hty = movey[i];
    }

    switch (Maze[Hy + Hty][Hx + Htx]) {
     case SPACE:
        Hx += Htx;
        Hy += Hty;
        break;
     case CHIP:
        for (i = 0; i < 3; ++i) {
            if (E[i].st != FAINT) {
                E[i].st = WEAK;
                E[i].wt = 85;
            }
        }
        Addsco = 10;
        /*FALLTHROUGH*/
     case DOT:
        --Dot;
        ++Score;
        Hx += Htx;
        Hy += Hty;
        Maze[Hy][Hx] = SPACE;
        vpoke(T32NAM + (Hy+1) * 32 + (Hx+1), ' ');
        break;
    }
    putspr(0, Hx * 8, Hy * 8, 10, 0);
}

static int distance(struct enemy *ep)
{
    return abs(Hx - ep->x) + abs(Hy - ep->y);
}

unsigned char judge()
{
    struct enemy *ep;

    for (ep = &E[0]; ep <= &E[2]; ++ep) {
        switch (ep->st) {
         case RED:
         case PINK:
         case GREEN:
            if (distance(ep) < 2)
                return DEAD;
            break;
         case WEAK:
            if (distance(ep) < 2) {
                ep->st = FAINT;
                ep->wt = 60;  ep->tx = ep->ty = 0;
                putenemy(ep);
                Score += Addsco;
                Addsco *= 2;
            }
            break;
        }
    }
    return ALIVE;
}

void starting()
{
    static int rndx[8] = { 1, 4, 7, 10, 14, 17, 20, 23 };
    static int rndy[4] = { 4, 7, 10, 21 };
    struct enemy *ep;

    srnd();
    Hx = 12;
    Hy = 15;
    Htx = 0;
    Hty = 0;
    putspr(0, Hx * 8, Hy * 8, 10, 0);

    for (ep = &E[0]; ep <= &E[2]; ++ep) {
        ep->st = ep - &E[0];
        ep->x  = rndx[rnd(8)];
        ep->y  = rndy[rnd(4)];
        ep->tx = 0;
        ep->ty = 0;
        ep->wt = 0;
        putenemy(ep);
    }

    putnum(26, 8, Score);
    putnum(26, 14, Rest);
    putnum(26, 20, Stage);
    putstr(6, 0, "HIT SPACE KEY!!");
    while (!gttrig(0) && !gttrig(1));
    putstr(6, 0, "               ");
}

static unsigned adjustspeed()
{
    return ((Stage > 81) ? 100 : (6580 - Stage * 80));
}

void game()
{
    unsigned wait, i;

    Score = 0;
    Stage = 1;
    wait = adjustspeed();
    for (Rest = 3; Rest > 0; --Rest) {
        starting();
        for (;;) {
            if (Dot == 0) {
                ++Stage;
                drawmaze();
                starting();
                wait = adjustspeed();
            }
            for (i = 0; i < wait; ++i);
            mvplayer();
            if (judge() == DEAD)
                break;
            putnum(26, 8, Score);
            mvenemy();
            if (judge() == DEAD)
                break;
        }
        for (i = 0; i < 5000; ++i);
    }
}

unsigned char replay()
{
    char c;

    printf("\33Y%c%cREPLAY ? (Y/N)", 32+20, 32+7); kilbuf();
 /* fflush(stdout); */
    return ((c = getch()) == 'y' || c == 'Y');
}

void main()
{
    ginit();

    do {
        init();
        game();
        highscore();
    } while (replay());

    screen(0);
}
