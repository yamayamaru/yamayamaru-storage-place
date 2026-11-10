/*
	functions for GCC
		by Waku Factory
	90/10/12 V1.0
*/

/****************************************
	graphic reset for MS-DOS mode
 ****************************************/

#include "egb.h"

#define PORT_PRIO 0x448
#define PORT_PALE 0x0fd90

static unsigned char palette[16][3]= {
				  0,  0,  0,
				176,  0,  0,
				  0,176,  0,
				176,176,  0,
				  0,  0,176,
				176,  0,176,
				  0,176,176,
				176,176,176,
				 64, 64, 64,
				255,  0,  0,
				  0,255,  0,
				255,255,  0,
				  0,  0,255,
				255,  0,255,
				  0,255,255,
				255,255,255
		 } ;
unsigned char INPB(unsigned short addr);
void OUTPB(unsigned short addr, unsigned char data);
void CRTC(unsigned char addr, unsigned char data);
static void FM_write(int addr, int data)
{
	int		i ;

	while (INPB(0x04d8) & 0x80) ;
	OUTPB(0x04d8, addr) ;
	for (i = 0 ; i < 4 ; i += 1) ;
	while (INPB(0x04d8) & 0x80) ;
	OUTPB(0x04da, data) ;
}

void screenterm( char *w )
{
	int i ;
	EGB_resolution( w, 0, 1 ) ;
	EGB_resolution( w, 1, 1 ) ;
	EGB_writePage( w, 0 ) ;
	EGB_clearScreen( w ) ;
	EGB_writePage( w, 1 ) ;
	EGB_clearScreen( w ) ;
	EGB_displayPage( w, 1, 3) ;

/* init palette */
 	OUTPB( PORT_PRIO, 1 ) ;
 	OUTPB( PORT_PRIO+2, 0x29 ) ;
	for (i =0 ; i < 16 ; i++)
	{
		OUTPB(PORT_PALE,i) ;
		OUTPB(PORT_PALE+2,palette[i][0]) ;
		OUTPB(PORT_PALE+4,palette[i][1]) ;
		OUTPB(PORT_PALE+6,palette[i][2]) ;
	}
	OUTPB( PORT_PRIO, 1 ) ;
	OUTPB( PORT_PRIO+2, 9 ) ;

/*	FM音源のタイマBを始動	*/
	FM_write(0x21, 0x00) ;
	FM_write(0x2c, 0x80) ;
	FM_write(0x2b, 0x00) ;
	FM_write(0x27, 0x30) ;
	FM_write(0x26, 0xdd) ;
	FM_write(0x27, 0x2a) ;

	CRTC( 0x18, 0x80 ) ;
}


