#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <dos.h>

#define __PC98__
#if defined( __PC98__ )
  #include "graph98.h"
#else
  #include <graph.h>
#endif


struct videoconfig  VC;
char far old_mask[ 8 ];
char far new_mask[ 8 ] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
extern int xmax, ymax, maxcolor;
int InitScreen( void );

void hat();

int main(int argc, char *argv[]) {
    long      i, j;
    int       c, count01, x1, y1, x2, y2, r;


    if( !InitScreen() ) {
	puts( "No graphics adapter present" );
	return( 1 );
    }
    _getfillmask( old_mask );
    _setfillmask( new_mask );

    _clearscreen(0);
    count01 = 0;
    for (j = 50; j < 400; j+=100) {
        for (i = 0; i < 640; i+=100) {
            if (j < 200)
                r = 50;
                _setcolor((count01 + 1) % 15 + 1);
                _arc(i-r, j-r, i+r, j+r, i, j-r, i+1, j-r);
                _moveto(i - 50, j - 50);
                _lineto( i + 50 - 1, j + 50 - 1);
                _moveto(i + 50, j - 50);
                _lineto(i - 50 - 1, j + 50 - 1);
            if (j >= 200)
                _setcolor((count01 + 1) % 15 + 1);
                _rectangle(_GBORDER, i - 50, j - 50, i + 50 - 1, j + 50 - 1);
                _moveto(i - 50, j - 50);
                _lineto(i + 50 - 1, j + 50 - 1);
                _moveto(i + 50, j - 50);
                _lineto(i - 50 - 1, j + 50 - 1);
            count01++;
        }
    }
    delay(3000);


    _clearscreen(0);
    for (i = 0; i < 60000*4; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        c = (rand() % 15) + 1;
        _setcolor(c);
        _rectangle(_GFILLINTERIOR, x1, y1, x1, y1 );
        //_setpixel(x1, y1);        // _setpixel()にはバグがある模様
    }
    delay(3000);

    _clearscreen(0);
    for (i = 0; i < 4000*4; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        x2 = rand() % 640;
        y2 = rand() % 400;
        c = (rand() % 15) + 1;
        _setcolor(c);
        _moveto(x1, y1);
        _lineto(x2, y2);
    }
    delay(3000);

    _clearscreen(0);
    for (i = 0; i < 1500*2; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        x2 = rand() % 640;
        y2 = rand() % 400;
        c = (rand() % 15) + 1;
        _setcolor(c);
        _rectangle(_GFILLINTERIOR, x1, y1, x2, y2);
    }
    delay(3000);

    _clearscreen(0);
    for (i = 0; i < 2000*2; i++) {
        x1 = rand() % 640;
        y1 = rand() % 400;
        r  = rand() % 50;
        c = (rand() % 15) + 1;
        _setcolor(c);
        _pie(_GFILLINTERIOR, x1-r, y1-r, x1+r, y1+r, x1, y1-r, x1+1, y1-r);
    }
    delay(3000);


    _clearscreen(0);
    hat();


    getch();
    _setfillmask( old_mask );
    _setvideomode( _DEFAULTMODE );

    return 0;
}

int xmax, ymax, maxcolor;
int InitScreen( void )
/*===========================

    This routine selects the best video mode for a given adapter. */

{
    int                 mode;

#if defined( __PC98__ )
    mode = _MAXCOLORMODE;
#else
    _getvideoconfig( &VC );
    switch( VC.adapter ) {
    case _VGA :
    case _SVGA :
	mode = _VRES16COLOR;
	break;
    case _MCGA :
	mode = _MRES256COLOR;
	break;
    case _EGA :
	if( VC.monitor == _MONO ) {
	    mode = _ERESNOCOLOR;
	} else {
	    mode = _ERESCOLOR;
	}
	break;
    case _CGA :
	mode = _MRES4COLOR;
	break;
    case _HERCULES :
	mode = _HERCMONO;
	break;
    default :
	return( 0 );          /* report insufficient hardware */
    }
#endif

    if( _setvideomode( mode ) == 0 ) {
	return( 0 );
    }
    _getvideoconfig( &VC );

    maxcolor = VC.numcolors;
    xmax = VC.numxpixels;
    ymax = VC.numypixels;

    return( 1 );
}


double d[160];

void hat() {
    int       i, x, y, zz, px, py, f, col, sxi;
    double    dr, r, z, sx, sy;

    for (i = 0; i < 160; ++i) {
        d[i] = 100.0;
    }
    dr = 3.141592653589793 / 180.0;
    for (y = -180; y <= 180; y += 6) {
        for (x = -180; x < 180; x += 4) {
            r = dr * sqrt((double)x * (double)x + (double)y * (double)y);
            z = 100 * cos(r) - 30 * cos(3*r);
            sx = 80 + x / 3 - y / 6;
            sy = 40 - y / 6 - z / 4;
            sxi = (int)sx;
            if ((sx < 0) || (sx >= 160)) continue;
            if (d[sxi] <= sy) continue;
            zz = (int)((z + 100) * 0.035) + 1;
            col = 0;
            f = 0;
            if ((zz == 1) || (zz == 3) || (zz == 5) || (zz == 7)) {
                col = col | 1;
                f = 1;
            }
            if ((zz == 2) || (zz == 3) || (zz >= 6)) {
                col = col | 4;
                f = 1;
            }
            if ((zz >= 4)) {
                col = col | 2;
                f = 1;
            }
            px = (int)(sx * 2.0 * xmax / 320.0);
            py = (int)(sy * 2.0 * ymax / 200.0);
            if (f == 1) {
                _setcolor(col);
		/*_setpixel(px, py);*/                 // _setpixel()にはバグがある模様
                _rectangle(_GFILLINTERIOR, px, py, px+3, py+3 );
            }
            d[sxi] = sy;
        }
    }
}
