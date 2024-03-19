#include <stdio.h>
#include "msxalib.h"
#include "msxclib.h"
#include "hipman.h"

extern unsigned char Maze[23][25];
extern int  Hx;
extern int  Hy;
extern int  Ecl[5];
extern struct enemy E[3];

static void rndmvenemy(struct enemy *ep);
static unsigned char chasex(struct enemy *ep);
static unsigned char chasey(struct enemy *ep);
static void refresh(struct enemy *ep);
static void red(struct enemy *ep);
static void pink(struct enemy *ep);
static void green(struct enemy *ep);
static void weakling(struct enemy *ep);
static void fainting(struct enemy *ep);
void putenemy(struct enemy *ep);
void mvenemy();

static void rndmvenemy(struct enemy *ep)
{
    static int  movex[7] = {  0, 1, 0, -1,  0, 1, 0 };
    static int  movey[7] = { -1, 0, 1,  0, -1, 0, 1 };
    int  tx, ty, i, count;

    for (count = 0, i = rnd(4); count < 4; ++count, ++i) {
        tx = movex[i];
        ty = movey[i];
        if (Maze[ep->y + ty][ep->x + tx] != WALL
                    && (ep->tx != -tx || ep->ty != -ty)) {
            ep->tx = tx;
            ep->ty = ty;
            return;
        }
    }
    ep->tx = 0;
    ep->ty = 0;
}

static unsigned char chasex(struct enemy *ep)
{
    int tx;

    tx = sgn(Hx - ep->x);

    if (tx == 0)
        return 0;

    if (tx == -(ep->tx))
        return 0;

    if (Maze[ep->y][ep->x + tx] == WALL)
        return 0;

    ep->tx = tx;
    ep->ty = 0;
    return 1;
}

static unsigned char chasey(struct enemy *ep)
{
    int ty;

    ty = sgn(Hy - ep->y);

    if (ty == 0)
        return 0;

    if (ty == -(ep->ty))
        return 0;

    if (Maze[ep->y + ty][ep->x] == WALL)
        return 0;

    ep->tx = 0;
    ep->ty = ty;
    return 1;
}

static void refresh(struct enemy *ep)
{
    ep->tx = 0;
    ep->ty = 0;
    ep->st = (ep - &E[0]);
}

static void red(struct enemy *ep)
{
    if (!chasex(ep) && !chasey(ep))
        rndmvenemy(ep);
}

static void pink(struct enemy *ep)
{
    if (!chasey(ep) && !chasex(ep))
        rndmvenemy(ep);
}

static void green(struct enemy *ep)
{
    rndmvenemy(ep);
}

static void weakling(struct enemy *ep)
{
    if (--(ep->wt) == 0)
        refresh(ep);
    else
        rndmvenemy(ep);
}

static void fainting(struct enemy *ep)
{
     if (--(ep->wt) == 0)
         refresh(ep);
}

void putenemy(struct enemy *ep)
{
    int col, i;

    i = ep - &E[0];

    if ((ep->wt & 1) != 0 || ep->wt >= 10)
        col = Ecl[ep->st];
    else
        col = Ecl[i];
    putspr(i+1, (ep->x) * 8, (ep->y) * 8, col, 0);
}

void mvenemy()
{
    static void (*adr[5])(struct enemy *ep) = {red, pink, green, weakling, fainting};
    struct enemy *ep;

    for (ep = &E[0]; ep <= &E[2]; ++ep) {
        (*adr[ep->st])(ep);
        ep->x += ep->tx;
        ep->y += ep->ty;
        putenemy(ep);
    }
}

