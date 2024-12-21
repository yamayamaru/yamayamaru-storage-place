/*
 *  graph.h     Graphics functions
 *
 *  Copyright by WATCOM International Corp. 1988-1995.  All rights reserved.
 */
#ifndef _GRAPH_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
#if __WATCOMC__ > 1000
#pragma pack(push,1);
#else
#pragma pack(1);
#endif

#if defined ( __386__ )
    #define _FAR
    #define _HUGE
#else
    #define _FAR    __far
    #define _HUGE   __huge
#endif

#pragma library (graph98);

struct xycoord {                /* structure for pixel position */
	short   xcoord;
	short   ycoord;
};

struct _wxycoord {              /* structure for window position*/
	double  wx;
	double  wy;
};

struct rccoord {                /* structure for text position  */
	short   row;
	short   col;
};

struct videoconfig {            /* structure for _getvideoconfig */
	short   numxpixels;
	short   numypixels;
	short   numtextcols;
	short   numtextrows;
	short   numcolors;
	short   bitsperpixel;
	short   numvideopages;
	short   mode;
	short   adapter;
	short   monitor;
	short   memory;
};

struct textsettings {           /* structure for _gettextsettings */
	short   basevectorx;
	short   basevectory;
	short   txpath;
	short   height;
	short   width;
	short   spacing;
	short   horizalign;
	short   vertalign;
};

struct _fontinfo {              /* structure for _getfontinfo */
	short   type;
	short   ascent;
	short   pixwidth;
	short   pixheight;
	short   avgwidth;
	char    filename[ 81 ];
	char    facename[ 32 ];
};

/* Calling conventions for -fpi(87) are different than for -fpc */
#if defined( __FPI__ )
    #define _arc_w              _arc_w_87
    #define _ellipse_w          _ellipse_w_87
    #define _floodfill_w        _floodfill_w_87
    #define _getimage_w         _getimage_w_87
    #define _getpixel_w         _getpixel_w_87
    #define _getviewcoord_w     _getviewcoord_w_87
    #define _grtext_w           _grtext_w_87
    #define _imagesize_w        _imagesize_w_87
    #define _lineto_w           _lineto_w_87
    #define _moveto_w           _moveto_w_87
    #define _pie_w              _pie_w_87
    #define _putimage_w         _putimage_w_87
    #define _rectangle_w        _rectangle_w_87
    #define _setcharsize_w      _setcharsize_w_87
    #define _setcharspacing_w   _setcharspacing_w_87
    #define _setpixel_w         _setpixel_w_87
    #define _setwindow          _setwindow_87
#endif

/* Video Setup and Query Functions */

short _FAR              _setvideomode( short );
short _FAR              _setvideomoderows( short, short );
struct videoconfig _FAR * _FAR
			_getvideoconfig( struct videoconfig _FAR * );
short _FAR              _grstatus( void );
short _FAR              _setactivepage( short );
short _FAR              _getactivepage( void );
short _FAR              _setvisualpage( short );
short _FAR              _getvisualpage( void );

#define _MAXRESMODE     (-3)    /* graphics mode with highest res. */
#define _MAXCOLORMODE   (-2)    /* graphics mode with most colours */
#define _DEFAULTMODE    (-1)    /* restore screen to original mode */
#define _98TEXT80        3      /* 80 column text, 8 color         */
#define _98RESSCOLOR     81     /* 640 x 400, 8 color (superimpose)*/
#define _98RESS8COLOR    84     /* 640 x 400, 8 of 4096 (superimpose)*/
#define _98RESS16COLOR   85     /* 640 x 400, 16 of 4096 (superimpose)*/
#define _98HIRESS16COLOR 93     /* 1120 x 750, 16 of 4096 (superimpose)*/
#define _98RESCOLOR      97     /* 640 x 400, 8 color              */
#define _98RES8COLOR     100    /* 640 x 400, 8 of 4096 color      */
#define _98RES16COLOR    101    /* 640 x 400, 16 of 4096 color     */
#define _98HIRES16COLOR  109    /* 1120 x 750, 16 of 4096 color    */

#define _NODISPLAY      (-1)    /* no display device            */
#define _UNKNOWN        0       /* unknown adapter/monitor type */

#define _98CGA          0x2000  /* Color Graphics Adapter (digital)   */
#define _98EGA          0x4000  /* Enhanced Graphics Adapter (analog) */

#define _98ANALOG       0x0100  /* Analog color monitor               */
#define _98DIGITAL      0x0200  /* Digital color monitor              */

#define _GROK                   0      /* no error                     */
#define _GRERROR                (-1)   /* graphics error               */
#define _GRMODENOTSUPPORTED     (-2)   /* video mode not supported     */
#define _GRNOTINPROPERMODE      (-3)   /* function n/a in this mode    */
#define _GRINVALIDPARAMETER     (-4)   /* invalid parameter(s)         */
#define _GRINSUFFICIENTMEMORY   (-5)   /* out of memory                */
#define _GRFONTFILENOTFOUND     (-6)   /* can't open font file         */
#define _GRINVALIDFONTFILE      (-7)   /* font file has invalid format */
#define _GRNOOUTPUT             1      /* nothing was done             */
#define _GRCLIPPED              2      /* output clipped               */

/* Colour Setting and Query Functions */

short _FAR              _setcolor( short );
short _FAR              _getcolor( void );
long _FAR               _setbkcolor( long );
long _FAR               _getbkcolor( void );
long _FAR               _remappalette( short, long );
short _FAR              _remapallpalette( long _FAR * );
short _FAR              _selectpalette( short );

#define _98BLACK        0x000000L      /* colour values for analog display */
#define _98BLUE         0x0F0000L
#define _98GREEN        0x000F00L
#define _98CYAN         0x0F0F00L
#define _98RED          0x00000FL
#define _98MAGENTA      0x0F000FL
#define _98YELLOW       0x000F0FL
#define _98WHITE        0x0F0F0FL
#define _98GRAY         0x080808L
#define _98DARKBLUE     0x080000L
#define _98DARKGREEN    0x000800L
#define _98DARKCYAN     0x080800L
#define _98DARKRED      0x000008L
#define _98DARKMAGENTA  0x080008L
#define _98DARKYELLOW   0x000808L
#define _98DARKWHITE    0x0C0C0CL

#define _98BLACK_D      0x000000L      /* colour values for digital display */
#define _98BLUE_D       0x000001L
#define _98GREEN_D      0x000002L
#define _98CYAN_D       0x000003L
#define _98RED_D        0x000004L
#define _98MAGENTA_D    0x000005L
#define _98YELLOW_D     0x000006L
#define _98WHITE_D      0x000007L

/* Shape and Curve Drawing Functions */

short _FAR              _lineto( short, short );
short _FAR              _lineto_w( double, double );
short _FAR              _rectangle( short, short, short, short, short );
short _FAR              _rectangle_w( short, double, double, double, double );
short _FAR              _rectangle_wxy( short, struct _wxycoord _FAR *,
					       struct _wxycoord _FAR * );
short _FAR              _arc( short, short, short, short, short, short,
			      short, short );
short _FAR              _arc_w( double, double, double, double, double,
				double, double, double );
short _FAR              _arc_wxy( struct _wxycoord _FAR *,
				  struct _wxycoord _FAR *,
				  struct _wxycoord _FAR *,
				  struct _wxycoord _FAR * );
short _FAR              _ellipse( short, short, short, short, short );
short _FAR              _ellipse_w( short, double, double, double, double );
short _FAR              _ellipse_wxy( short, struct _wxycoord _FAR *,
				      struct _wxycoord _FAR * );
short _FAR              _pie( short, short, short, short, short, short,
			      short, short, short );
short _FAR              _pie_w( short, double, double, double, double,
				double, double, double, double );
short _FAR              _pie_wxy( short, struct _wxycoord _FAR *,
				  struct _wxycoord _FAR *,
				  struct _wxycoord _FAR *,
				  struct _wxycoord _FAR * );
short _FAR              _polygon( short, short, struct xycoord _FAR * );
short _FAR              _polygon_w( short, short, double _FAR * );
short _FAR              _polygon_wxy( short, short, struct _wxycoord _FAR * );
short _FAR              _floodfill( short, short, short );
short _FAR              _floodfill_w( double, double, short );
short _FAR              _setpixel( short, short );
short _FAR              _setpixel_w( double, double );
short _FAR              _getpixel( short, short );
short _FAR              _getpixel_w( double, double );
short _FAR              _getarcinfo( struct xycoord _FAR *,
				     struct xycoord _FAR *,
				     struct xycoord _FAR * );

/* Position Determination Functions */

struct xycoord _FAR     _getcurrentposition( void );
struct _wxycoord _FAR   _getcurrentposition_w( void );
struct xycoord _FAR     _getviewcoord( short, short );
struct xycoord _FAR     _getviewcoord_w( double, double );
struct xycoord _FAR     _getviewcoord_wxy( struct _wxycoord _FAR * );
struct xycoord _FAR     _getphyscoord( short, short );
struct _wxycoord _FAR   _getwindowcoord( short, short );
struct xycoord _FAR     _moveto( short, short );
struct _wxycoord _FAR   _moveto_w( double, double );
struct xycoord _FAR     _setvieworg( short, short );

#define _getlogcoord   _getviewcoord        /* for compatibility */
#define _setlogorg     _setvieworg

/* Output Determination Functions */

void _FAR               _setfillmask( unsigned char _FAR * );
unsigned char _FAR * _FAR
			_getfillmask( unsigned char _FAR * );
void _FAR               _setlinestyle( unsigned short );
unsigned short _FAR     _getlinestyle( void );
short _FAR              _setplotaction( short );
short _FAR              _getplotaction( void );

#define _setwritemode   _setplotaction      /* for compatibility */
#define _getwritemode   _getplotaction

enum {                          /* plotting action */
	_GOR, _GAND, _GPRESET, _GPSET, _GXOR
};

/* Screen Manipulation Functions */

void _FAR               _clearscreen( short );
void _FAR               _setviewport( short, short, short, short );
void _FAR               _setcliprgn( short, short, short, short );
void _FAR               _getcliprgn( short _FAR *, short _FAR *,
				     short _FAR *, short _FAR * );
short _FAR              _displaycursor( short );
short _FAR              _wrapon( short );
short _FAR              _setwindow( short, double, double, double, double );

#define _GCLEARSCREEN   0
#define _GVIEWPORT      1
#define _GWINDOW        2
#define _GCLEARGRAPH    3
#define _GCLEARTEXT     4

#define _GBORDER        2
#define _GFILLINTERIOR  3

enum {                          /* cursor display */
	_GCURSOROFF, _GCURSORON
};

enum {                          /* text wrapping */
	_GWRAPOFF, _GWRAPON
};

/* Graphics Text Manipulation Functions and Constants */

struct textsettings _FAR * _FAR
			_gettextsettings( struct textsettings _FAR * );
void _FAR               _gettextextent( short, short, char _FAR *,
				struct xycoord _FAR *, struct xycoord _FAR * );
void _FAR               _setcharsize( short, short );
void _FAR               _setcharsize_w( double, double );
void _FAR               _settextalign( short, short );
void _FAR               _settextpath( short );
void _FAR               _settextorient( short, short );
void _FAR               _setcharspacing( short );
void _FAR               _setcharspacing_w( double );
short _FAR              _grtext( short, short, char _FAR * );
short _FAR              _grtext_w( double, double, char _FAR * );

enum {                          /* horizontal alignment */
	_NORMAL, _LEFT, _CENTER, _RIGHT
};

enum {                          /* vertical alignment */
	_TOP=1, _CAP, _HALF, _BASE, _BOTTOM
};

enum {                          /* text path */
	_PATH_RIGHT, _PATH_LEFT, _PATH_UP, _PATH_DOWN
};

/* Text Manipulation Functions */

#define _GSCROLLUP      1
#define _GSCROLLDOWN    (-1)
#define _MAXTEXTROWS    (-1)

void _FAR               _settextwindow( short, short, short, short );
void _FAR               _outtext( char _FAR * );
short _FAR              _settextcolor( short );
short _FAR              _gettextcolor( void );
struct rccoord _FAR     _settextposition( short, short );
struct rccoord _FAR     _gettextposition( void );
void _FAR               _scrolltextwindow( short );
void _FAR               _gettextwindow( short _FAR *, short _FAR *,
					short _FAR *, short _FAR * );
short _FAR              _gettextcursor( void );
short _FAR              _settextcursor( short );
void _FAR               _outmem( unsigned char _FAR *, short );
short _FAR              _settextrows( short );

/* Image Manipulation Functions */

void _FAR               _getimage( short, short, short, short, char _HUGE * );
void _FAR               _getimage_w( double, double, double, double, char _HUGE * );
void _FAR               _getimage_wxy( struct _wxycoord _FAR *,
				       struct _wxycoord _FAR *, char _HUGE * );
void _FAR               _putimage( short, short, char _HUGE *, short );
void _FAR               _putimage_w( double, double, char _HUGE *, short );
long _FAR               _imagesize( short, short, short, short );
long _FAR               _imagesize_w( double, double, double, double );
long _FAR               _imagesize_wxy( struct _wxycoord _FAR *,
					struct _wxycoord _FAR * );

/* KANJI Font Functions */

short _FAR              _getkanji( unsigned short, unsigned char _FAR * );
short _FAR              _setkanji( unsigned short, unsigned char _FAR * );
short _FAR              _kanjisize( short );

/* Font Manipulation Functions */

short _FAR              _registerfonts( char _FAR * );
void _FAR               _unregisterfonts( void );
short _FAR              _setfont( char _FAR * );
short _FAR              _getfontinfo( struct _fontinfo _FAR * );
void _FAR               _outgtext( char _FAR * );
short _FAR              _getgtextextent( char _FAR * );
struct xycoord _FAR     _setgtextvector( short, short );
struct xycoord _FAR     _getgtextvector( void );

#undef _FAR
#undef _HUGE

#if __WATCOMC__ > 1000
#pragma pack(pop);
#else
#pragma pack();
#endif
#define _GRAPH_H_INCLUDED
#ifdef __cplusplus
};
#endif
#endif
