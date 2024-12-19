/******************************************************************************
 *																			  *
 *  EGB LIBRARY SAMPLE PROGRAM												  *
 *																			  *
 *        TITLE    : ｱｲｺﾝ表示												  *
 *																			  *
 *        使用方法 : 起動 : TownsMENU(中解像度)上からﾀﾞﾌﾞﾙｸﾘｯｸ 				  *
 *                   終了 : ﾏｳｽﾎﾞﾀﾝをｸﾘｯｸ           	   					  *
 *																			  *
 *****************************************************************************/

#include <stdlib.h>
#include "../include/egb.h"
#include "../include/mos.h"

#define NUM 128								/*	アイコン数					*/

struct	FISH{								/*	アイコン定義 構造体			*/
				short	x ;
				short	y ;
				char	icon ;
				signed char	dir ;
				char	color ;
			} ;

char mwork[MosWorkSize] ;
char work[EgbWorkSize] ;

struct	FISH fish[NUM] ;

void main()
{
		extern	void setFish( struct FISH * ) ;
		extern	void moveFish() ;

		int		i ;

		EGB_resolution( work, 0, 0x43 ) ;	/*	crtc操作なしの640×480画面	*/
		EGB_resolution( work, 1, 0x43 ) ;
		EGB_writePage( work, 0x40 ) ;

		for( i = 0 ; i < NUM ; i++ )		/*	アイコン定義				*/
			setFish( &fish[i] ) ;

		EGB_writeMode( work, 4 ) ;
		moveFish() ;						/*	アイコン移動				*/
}

void moveFish()
{
		extern	void setFish( struct FISH * ) ;

		int		i = 0 ;
		int		ch, x, y ;
		char	para[64] ;

		MOS_start( mwork, 4096 ) ;

		do
		{
			EGB_color( work, 0, fish[i].color) ;

			DWORD(para + 0) = 0x28000 + fish[i].icon * 256 ;
			WORD(para + 4) = 0x108 ;
			WORD(para + 6) = fish[i].x + fish[i].dir ;
			WORD(para + 8) = fish[i].y ;
			WORD(para + 10) = fish[i].x + 31 + fish[i].dir ;
			WORD(para + 12) = fish[i].y + 31 ;
			EGB_putBlockColor( work, 0, para ) ;
											/*	ドットデータの書き込み		*/

			WORD(para + 6) = fish[i].x ;
			WORD(para + 10) = fish[i].x + 31 ;
			EGB_putBlockColor( work, 0, para ) ;

			fish[i].x += fish[i].dir ;

			if (((fish[i].dir > 0)&&(fish[i].x > 640))||
				((fish[i].dir < 0)&&(fish[i].x < -32)))
				setFish( &fish[i] ) ;

			i++ ;
			if ( i == NUM ) i = 0 ;
			MOS_rdpos( &ch, &x, &y ) ;
		} while ( !ch ) ;
		MOS_end() ;
}

void setFish( data )
struct FISH *data ;
{
		int		dir ;
		int		spd ;

		dir = rand() & 1 ;
		spd = (rand() & 0xf)+1 ;

		data->x = dir ? -32 : 640 ;
		data->y = rand() & 0x1ff ;
		data->icon = rand() & 0x7f ;
		data->dir = dir ? spd : -spd ;
		data->color = rand() & 0xf ;
}
