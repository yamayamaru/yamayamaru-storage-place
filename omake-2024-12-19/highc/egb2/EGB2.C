/******************************************************************************
 *																			  *
 *  EGB LIBRARY SAMPLE PROGRAM												  *
 *																			  *
 *        TITLE    : TownsMENU 分割	    									  *
 *																			  *
 *        使用方法 : 起動 : TownsMENU(中解像度)上からﾀﾞﾌﾞﾙｸﾘｯｸ				  *
 *                   終了 : ﾏｳｽ左右ﾎﾞﾀﾝをｸﾘｯｸ			   					  *
 *																			  *
 *****************************************************************************/

#include <stdlib.h>
#include "..\include\egb.h"
#include "..\include\mos.h"

char egb_work[EgbWorkSize] ;			/*  ＥＧＢ作業領域                  */
char mos_work[MosWorkSize] ;			/*  ＭＯＳ作業領域                  */
char para[64] ;							/*  パラメータ受け渡し作業領域      */
char cpara[64] ;						/*  パラメータ受け渡し作業領域      */
char rpara[64] ;						/*  パラメータ受け渡し作業領域      */

void main()
{
	char *copy_buf ;

	int page, dpage, pri ;
	int n, x1, y1, x2, y2, wsize ;
	int ch, x, y ;

	EGB_resolution( egb_work, 0, 0x43 );/*  グラフィック仮想画面の設定      */
	EGB_resolution( egb_work, 1, 0x43 );/*  グラフィック仮想画面の設定      */
	page = EGB_getWritePage( 0, 0 ) ;	/*  書き込みページの読み取り        */
	EGB_writePage(egb_work,page | 0x40);/*  グラフィック書き込みページ      */
	EGB_getDisplayPage( &dpage , &pri );/*  表示ページの読み取り            */
	EGB_displayPage(egb_work,dpage,pri);/*  グラフィック表示ページの指定    */
	MOS_start( mos_work, MosWorkSize ) ;/*  マウス動作開始の設定            */

	WORD(para + 0 ) = 4 ;				/*  グラフィック領域の設定          */
	WORD(para + 2 ) = 0 ;
	WORD(para + 4 ) = 0 ;
	WORD(para + 6 ) = 639 ;
	WORD(para + 8 ) = 0 ;
	WORD(para + 10) = 639 ;
	WORD(para + 12) = 239 ;
	WORD(para + 14) = 0 ;
	WORD(para + 16) = 239 ;
	n  = 0x80 ;
	x1 = 0 ;
	y1 = 0 ;
	EGB_region( egb_work, &n, &wsize, &x1, &y1, &x2, &y2, para );

	if ((copy_buf = malloc(wsize)) == 0)/*  作業領域の確保                  */
		exit(-1) ;

	n = 0 ;								/*  グラフィック画面の複写          */
	BYTE(cpara + 0) = (char)page ;
	BYTE(cpara + 1) = 0 ;
	WORD(cpara + 2) = 0 ;
	WORD(cpara + 4) = 240 ;
	EGB_copy( egb_work, n, cpara, copy_buf ) ;

	free(copy_buf) ;					/*  作業領域の開放                  */

	WORD(para + 0 ) = 4 ;				/*  グラフィック領域の設定          */
	WORD(para + 2 ) = 0 ;
	WORD(para + 4 ) = 240 ;
	WORD(para + 6 ) = 640 ;
	WORD(para + 8 ) = 240 ;
	WORD(para + 10) = 640 ;
	WORD(para + 12) = 480 ;
	WORD(para + 14) = 0 ;
	WORD(para + 16) = 480 ;
	n  = 0x80 ;
	x1 = 0 ;
	y1 = 0 ;
	EGB_region( egb_work, &n, &wsize, &x1, &y1, &x2, &y2, para ) ;

	if ((copy_buf = malloc(wsize)) == 0)/*  作業領域の確保                  */
		exit( -1 ) ;

	n = 0;								/*  グラフィック画面の回転          */
	BYTE(rpara + 0) = (char)page ;
	BYTE(rpara + 1) = 0 ;
	WORD(rpara + 2) = 320 ;
	WORD(rpara + 4) = 360 ;
	WORD(rpara + 6) = 180 ;
	EGB_rotate( egb_work, n, rpara, copy_buf ) ;

	free( copy_buf ) ;					/*  作業領域の開放                  */

	do {
		MOS_rdpos( &ch, &x, &y ) ;
		} while( ch != 0x03 ) ;			/*  左右ボタンが押されるまでループ  */
	MOS_end() ;							/*  マウス動作終了の設定            */
}
