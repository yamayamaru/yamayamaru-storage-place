/******************************************************************************
 *																			  *
 *	MOSLIBRARY SAMPLE PROGRAM												  *
 *																			  *
 *		  TITLE	   : マウスオペレーションＣＤプレーヤー						  *
 *																			  *
 *		  使用方法 : [起動] TownsMENU 上からﾀﾞﾌﾞﾙｸﾘｯｸ						  *
 *					 [終了] 左上ボタンをクリック							  *
 *																			  *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
//#include <msdos.cf>
#include "..\..\include\egb.h"
#include "..\..\include\mos.h"
#include "..\..\include\snd.h"
#include "..\..\include\cdrfrb.h"

int getds();

#define	 maxsec		 60
#define	 maxframe	 75
#define	 max_x		639
#define	 max_y		479

int  x, y, bt, ms = 0, b_push , select, unselect ;
int  cd_err = 1, play, pause ;
int  qmin = 0, qsec = 0, qsong = -1 ;

int  panel_pos[4] ;
char para[75] ;
char ptn[258] ;

char pal[9][4] = { { 2, 11, 11, 11 } , {  3, 13, 13, 13 } , { 4, 15, 15, 15 }
				 , {  5, 3,  3,  4 } , { 10,  3, 10,  3 } , { 11, 3, 15,  3 }
				 , { 12, 4,  4, 10 } , { 13,  4,  4, 14 } , { 14, 0, 15, 11 }};

int  box_pos[11][4] = { { 160, 180, 479, 307 } , { 160, 164, 173, 177 }
					  , { 352, 261, 383, 303 } , { 400, 261, 463, 303 }
					  , { 304, 271, 335, 303 } , { 240, 271, 287, 303 }
					  , { 176, 271, 223, 303 } , { 176, 164, 479, 177 }
					  , { 232, 224, 295, 257 } , { 336, 224, 415, 241 }
					  , { 384, 224, 415, 241 } } ;

int  char_pos[7][7] = { { 184, 220, 0, 16, 16,  5, 1 }
					  , { 304, 220, 0, 16, 16,  5, 1 }
					  , { 361, 290, 0, 14, 14,  5, 0 }
					  , { 416, 291, 3, 16, 32, 12, 0 }
					  , { 304, 294, 1, 16, 32, 10, 0 }
					  , { 248, 296, 3, 16, 16,  5, 0 }
					  , { 183, 294, 1, 16, 16,  5, 0 } } ;

char  char_ptn[7][11] = { "Ｔｒａｃｋ" , "Ｔｉｍｅ　" , "■　　　　"
						, "▲　　　　" , "〓　　　　" , "▲▲　　　"
						, "▲▲　　　" } ;

char  num[21]="０１２３４５６７８９" ;

int  cdtype , status , btrk[2] , song ;

struct	TIMEADRS	trkstm[99] ;
struct	TIMEADRS	dsketm ;
struct	TIMEADRS	trktm ;
struct	TIMEADRS	dsktm ;
struct	TIMEADRS	starttm ;
struct	TIMEADRS	endtm ;

char *all_buf, *egb_work, *mos_work, *panel_buf, *save_buf ;

int xy_check( int st, int end, int offset )
{
	for( ; st < end ; st++ )
		if(( x + offset >= box_pos[st][0] ) && ( x <= box_pos[st][2] )
			&& ( y + offset >= box_pos[st][1] ) && ( y <= box_pos[st][3] ))
			return( st ) ;
	return( 0 ) ;
}

void set_num( char *str, char n ){
	strcpy( str + 0, num + n / 10 * 2 ) ;
	strcpy( str + 2, num + n % 10 * 2 ) ;
}

void set_para( char *buf, int *pos, int func, int wmode )
{
	int i ;

	EGB_writeMode( egb_work, wmode ) ;			/*	描画モードを設定		*/
	if( func ){
		DWORD( para + 0 ) = (unsigned int) buf ;
		WORD ( para + 4 ) = getds() ;
		}

	for( i = 0 ; i < 4 ; i++ )
		WORD ( para + !(func < 1 ) * 6 + i * 2 ) = *( pos + i ) ;

	MOS_disp( ms ) ;
	switch( func ){
		case 0:	EGB_rectangle( egb_work, para ) ;
												/*	矩形描画を行う			*/
				break ;
		case 1: EGB_getBlock( egb_work, para ) ;
												/*	ﾄﾞｯﾄﾃﾞｰﾀの読み込み		*/
				break ;
		case 2:	EGB_putBlock( egb_work, 0, para ) ;
												/*	ﾄﾞｯﾄﾃﾞｰﾀの書き込み		*/
				break ;
		}
	MOS_disp( 1 ) ;
	EGB_writeMode( egb_work, 0 ) ;				/*	描画モードをPSETに		*/
}

void tile( int n, int ln_col, int bk_col, int pmode, int expand )
{
	int i , bbuf[4] ;

	EGB_paintMode( egb_work, pmode ) ;			/*	面塗りﾓｰﾄﾞを面塗りに	*/
	EGB_color( egb_work, 0, ln_col ) ;			/*	前景色を設定			*/
	EGB_color( egb_work, 2, bk_col ) ;			/*	面塗り色を設定			*/
	for( i = 0 ; i < 4 ; i++ )
		WORD( bbuf + i ) = box_pos[n][i] + ( 1 - !( i > 1 ) * 2 ) * expand ;
	set_para( panel_buf, bbuf, 0, 0 ) ;
}

void border_line( int n, int flag )
{
	char con[6] = { 2, 1, 2, 3, 0, 3 } ;
	int i ;

	tile( n, 3 + flag, 3, 0x02, 1 ) ;

	EGB_color( egb_work, 0, 3 - flag ) ;
	WORD( para +  0 ) = 3 ;
	for( i = 1 ; i < 7 ; i++ )
		WORD( para +  i * 2 ) = box_pos[n][con[i-1]] + 1 ;
	MOS_disp( 0 ) ;
	EGB_connect( egb_work, para ) ;				/*	連続線分を描画			*/
	MOS_disp( 1 ) ;
	b_push = ( flag == 1 ) ? 1 : 0 ;
}

void gprintf( char *str, int px, int py, int dir, int xe, int ye 
			, int col, int style, int ch_len )
{
	EGB_paintMode( egb_work, 0x02 ) ;			/*	面塗りﾓｰﾄﾞを境界のみに	*/
	EGB_textDirection( egb_work, dir ) ;		/*	文字方向を設定			*/
	EGB_textZoom( egb_work, 1, xe, ye ) ;		/*	文字拡大率を設定		*/
	EGB_color( egb_work, 0, col ) ;				/*	前景色を設定			*/
	EGB_fontStyle( egb_work, style ) ;			/*	字体を設定				*/
	WORD( para + 0 ) = px + !(!( dir % 3 )) * (( dir < 2 ) ? ye : xe ) ;
	WORD( para + 2 ) = py - !(!( dir & 2 )) * (( dir < 3 ) ? ye : xe ) ;
	WORD( para + 4 ) = ch_len ;
	strcpy( para + 6, str ) ;
	MOS_disp( ms ) ;
	EGB_sjisString( egb_work, para ) ;			/*	文字列を表示			*/
	MOS_disp( 1 ) ;
}

void teikei( int n, int col, int ch_len )
{
	gprintf( char_ptn[n], char_pos[n][0], char_pos[n][1], char_pos[n][2]
		, char_pos[n][3], char_pos[n][4], col, char_pos[n][6], ch_len ) ; 
}

void teikei2( char *buf, int n, int size, int style, int ch_len )   
{
	tile( n, 5, 5, 0x22, 0 ) ;
	gprintf( buf, box_pos[n][0], box_pos[n][3], 0, size, size
		, 14, style, ch_len ) ;
}

void mouse()
{
	MOS_rdpos( &bt, &x, &y ) ;					/*	マウス状態の読み込み	*/
	bt &= 1 ;
	if( bt ){
		if( unselect ){
			if( select ){
				if( select != xy_check( select, 7, 0 ) ){
					if( !b_push )
						border_line( select, 1 ) ;
					}
				else if( b_push )
					border_line( select, -1 ) ;
				}
			else{
				if( ( select = xy_check( 1, 7, 0 ) ) > 0 
					&& ( !cd_err || select == 1 ) && ( play || select < 4 ) )
					border_line( select, -1 ) ;
				else if( xy_check( 7, 8, 0 ) != 7 )
					select = unselect = 0 ;
				}
			}
		}
	else{
		unselect = 1 ;
		if( select ){
			if( !b_push )
				border_line( select, 1 ) ;
			if(select != xy_check( select, 7, 0 ))
				select = 0 ;
			}
		}
}

void display()
{
	char cbuf[29] , flag ;

	flag = ( song != qsong ) ;

	if( flag && cd_err	){
		teikei2( "－－", 8, 32, 1, 4 ) ;
		teikei2( "－－：－－", 9, 16, 0, 10 ) ;
		}
	else{
		if( flag ){
			set_num( cbuf, song ) ;
			teikei2( cbuf, 8, 32, 1, 4 ) ;
			}
		if( flag || trktm.sec != qsec ){
			if( flag || trktm.min != qmin ){
				set_num( cbuf, trktm.min ) ;
				WORD( cbuf+4 ) = 0x4681 ;		/*	ｷｬﾗｸﾀｺｰﾄﾞ0x8146 = "："	*/
				teikei2( cbuf, 9, 16, 0, 6 ) ;
				}
			set_num( cbuf, trktm.sec ) ;
			ms = !( xy_check( 10, 11, 31 ) == 10 ) ;
			teikei2( cbuf, 10, 16, 0, 4 ) ;
			ms = 0 ;
			}
		}
	qsong = song ;
	qmin = trktm.min ;
	qsec = trktm.sec ;
}

void cd_stop( int stp )
{
	pause = play = 0 ;
	teikei( 3, 12, 2 ) ;
	teikei( 4, 10, 2 ) ;
	if( !cd_err ){
		song = 0 ;
		if( stp ){
			MOS_typeRom( 82, 16, 16, ptn ) ;	/*	時計カ－ソルに			*/
			cdr_mstop( 0 ) ;					/*	CDを停止				*/
			MOS_typeRom( 87, 0, 0, ptn ) ;		/*	指先カーソルに			*/
			}
		}
}

void fr( int dire, int check )
{
	int i, msong ;

	if( !pause ){
		cdr_pause( 0 ) ;						/*	曲移動のためPAUSE		*/
		msong = song ;
		for( i = check ; i < 2 ; i++ ){
			while( !( trkstm[msong - 1].min & 0x80 ) == i ){ 
				if( msong == btrk[ dire > 0 ? 1 : 0 ] ){
					msong = 0 ;
					break;
					}
				msong += dire ;
				}
			if ( !i ){
				if( msong ){
					starttm = trkstm[msong - 1] ;
					song = msong ;
					}
				else{
					if( !trkstm[song - 1].min & 0x80 )
						starttm = trkstm[song -1] ;
					else
						cd_stop( 1 ) ;
					}
				}
			dire = 1 ;
			}

	if( !msong )
		endtm = dsketm ;
	else{
		endtm = trkstm[msong - 1] ;
		endtm.min &= 0x7f ;
		}
	if( !endtm.frame ){
		if( !endtm.sec ){
			endtm.min-- ;
			endtm.sec = maxsec - 1 ;
			}
		else
			endtm.sec-- ;
		endtm.frame = maxframe - 1 ;
		}
	else
		endtm.frame-- ;

		if( play ){
			MOS_typeRom( 82, 16, 16, ptn ) ;	/*	時計カーソルに			*/
			cdr_mtplay( 0, &starttm, &endtm ) ;	/*	CD演奏を開始			*/
			MOS_typeRom( 87, 0, 0, ptn ) ;		/*	指先カーソルに			*/
			}
		}
}

void move_panel()
{
	int i, j, mpos[4], rx, ry ;

	MOS_horizon( x - panel_pos[0], max_x - ( panel_pos[2] - x ) ) ;
												/*	ﾏｳｽ水平移動範囲の設定	*/
	MOS_vertical( y - panel_pos[1], max_y - ( panel_pos[3] - y ) ) ;
												/*	ﾏｳｽ垂直移動範囲の設定	*/
	MOS_typeRom( 84, 9, 9, ptn ) ;				/*	手のひらカーソルに		*/
	EGB_color( egb_work, 0, 3 ) ;

	set_para( panel_buf, panel_pos, 1, 0 ) ;	/*	パネル画面の読み込み	*/

	for( i = 0 ; i < 4 ; i++)
		mpos[i] = panel_pos[i] ;

	set_para( panel_buf, mpos, 0, 4 ) ;

	do{
		rx = x ;
		ry = y ;
		MOS_rdpos( &bt, &x, &y );				/*	マウスの状態の読み込み	*/
		bt &= 1 ;
		if(bt){
			if( x != rx || y != ry ){
				MOS_disp( 0 ) ;
				set_para( panel_buf, mpos, 0, 4 ) ;
				mpos[0] += x - rx ;
				mpos[1] += y - ry ;
				mpos[2] += x - rx ;
				mpos[3] += y - ry ;
				set_para( panel_buf, mpos, 0, 4 ) ;
				MOS_disp( 1 ) ;
				}
			}
		}while( bt ) ;

	MOS_disp( 0 ) ;
	MOS_typeRom( 87, 0, 0, ptn ) ;				/*	指先カーソルに			*/
	set_para( panel_buf, mpos, 0, 4 ) ;

	if( mpos[0] != panel_pos[0] || mpos[1] != panel_pos[1] ){
		set_para( save_buf, panel_pos, 2, 0 ) ;	/*	パネル位置の画面復元	*/
		set_para( save_buf, mpos, 1, 0 ) ;		/*	移動先の画面退避		*/
		set_para( panel_buf, mpos, 2, 0 ) ;		/*	移動先にパネル表示		*/

		for( i = 0 ; i < 4 ; i++ ){
			mpos[i] -= panel_pos[i] ;
			panel_pos[i] += mpos[i] ;
			}
		for( i = 0 ; i < 11 ; i++ )
			for( j = 0 ; j < 4 ; j++ )
				box_pos[i][j] += mpos[j & 1] ;
		for( i = 0 ; i < 14 ; i++ )
			char_pos[i / 2][i & 1] += mpos[i & 1] ;
		}
	MOS_horizon( 0, max_x ) ;					/*	ﾏｳｽの水平移動範囲を全画面*/
	MOS_vertical( 0, max_y ) ;					/*	ﾏｳｽの垂直移動範囲を全画面*/
	MOS_disp( 1 ) ;
}

void main()
{
	int i, j, sbuf_size, all_buf_size, mute_data ;

	play = pause = select = unselect = 0 ;

	for( i = 0 ; i < 4 ; i++ )
		panel_pos[i] = box_pos[!( i > 1 )][i] - 1 + 2 * ( ! ( i < 2 ) ) ;

	sbuf_size = ( panel_pos[2] - panel_pos[0] + 1 + 7 ) / 8 * 4 
			  * ( panel_pos[3] - panel_pos[1] + 1 ) ;
	all_buf_size = EgbWorkSize + MosWorkSize + sbuf_size * 2 ;
	if(( all_buf = malloc( all_buf_size )) == 0 )
		exit( -1 ) ;
	
	egb_work = all_buf ;
	mos_work = all_buf + EgbWorkSize ;
	panel_buf = mos_work + MosWorkSize ;
	save_buf = panel_buf + sbuf_size ;
	
	mute_data = SND_get_elevol_mute( 0 ) | 0x30 ;
	SND_elevol_set( 0x01, 127, 127 ) ;			/*	CDのボリュームを最大に	*/
	SND_elevol_mute( 0x30 ) ;					/*	CDのミュートの解除		*/

	EGB_resolution( egb_work, 0, 0x43 ) ;
	EGB_resolution( egb_work, 1, 0x43 ) ;
	EGB_writePage( egb_work, 0x40 ) ;
	EGB_displayPage( egb_work, 0, 1 ) ;

	DWORD( para ) = 9 ;
	for( j = 0 ; j < 9 ; j++ ){
		DWORD( para +  4 + j * 8 ) = (int) pal[j][0] ;
		for( i = 1 ; i < 5 ; i++ )
			BYTE(  para +  7 + i + j * 8 ) = ( i < 4 ) ? pal[j][i] * 16 : 0 ;
		}
	EGB_palette( egb_work, 1, para ) ;			/*	パレットの設定			*/

	set_para( save_buf, panel_pos, 1, 0 ) ;		/*	画面の退避				*/

	MOS_start( mos_work, MosWorkSize ) ;
	MOS_typeRom( 87, 0, 0, ptn ) ;

	for( i = 0 ; i < 7 ; i++ ){
		tile( i, 3, 3, 0x22, 0 ) ;
		border_line( i, 1 ) ;
		}
		tile( 7, 8, 15, 0x22, 1 ) ;
	for( i = 8 ; i < 10 ; i++ ){
		tile( i, 5, 5, 0x22, 0 ) ;
		border_line( i, -1 ) ;
		}
	for( i = 0 ; i < 7 ; i++ )
		teikei( i, char_pos[i][5], 10 ) ;

	b_push = 1 ;
	do{
		if( cd_err ){
			cd_err = cdr_mphase( 0, &status, &song, &trktm, &dsktm ) ;
												/*	演奏状態の読み込み		*/
			if( !cd_err ){
				tile( 7, 9, 9, 0x22, 0 ) ;
				MOS_typeRom( 82, 16, 16, ptn ) ;/*	時計カーソルに			*/
				cdr_pause( 0 ) ;				/*	CD情報を読むためにPAUSE	*/
				cdr_cdinfo( 0, &cdtype, &btrk[0], &btrk[1], trkstm, &dsketm ) ;
												/*	CD情報の読み込み		*/
				MOS_typeRom( 87, 0, 0, ptn ) ;	/*	指先カーソルに			*/
				if ( status ){
					play = 1 ;
					starttm = dsktm ;
					fr( 1, 1 );
					teikei( 3, 13, 2 ) ;
					}
				}
			}
		else{
			cd_err = cdr_mphase( 0, &status, &song, &trktm, &dsktm ) ;
												/*	演奏状態の読み込み		*/
			if( status ){
				if( !play )
					teikei( 3, 13, 2 ) ;
				if( play && song != qsong && dsktm.min * maxsec + dsktm.sec
					< trkstm[song-1].min * maxsec + trkstm[song-1].sec )
					fr( 1, 0 ) ;
				play = 1 ;
			}
			else if( play && !pause )
				cd_stop( 0 ) ;
			if( !play )
				song = trktm.min = trktm.sec = 0 ;
			if( cd_err ){
				if( select > 1 && !b_push){
					border_line( select, 1 ) ;
					select = unselect = 0 ;
					}
				tile( 7, 15, 15, 0x22, 0 ) ;
				cd_stop( 0 ) ;
				song = -1 ;
				}
			else if( qsong != song && trkstm[song-1].min & 0x80 )
					fr( 1, 0 ) ;
			}

		display() ;								/*	TRACK,TIME表示			*/

		mouse() ;								/*	マウス処理				*/

		if( !select && bt && unselect && xy_check( 7, 8, 0 ) == 7 )
			move_panel() ;

		if( bt && select > 4 && !b_push ){
			i = select - 5 ;
			j = i ? -1 : 1 ;
			if( song * j < btrk[1 - i] * j )
				song += j ;
			else
				song = btrk[i] ;
			fr( j, 0 ) ;
			}

		if( !bt ){
			switch( select ){
				case 1 :MOS_end() ;
						free( all_buf ) ;
						SND_elevol_mute( mute_data ) ;
						exit( 0 ) ;
						break ;
				case 2 :cd_stop( 1 ) ;
						break ;
				case 3 :if( !play ){
							play = song = 1 ;
							teikei( 3, 13, 2 ) ;
							}
						if( !pause ){
							fr( 1, 0 ) ;
							break ;
							}
				case 4 :pause ^= 1 ;
						teikei( 4, 10 + pause, 2 ) ;
						if( pause ){
							cdr_mphase( 0, &status, &song, &trktm, &dsktm ) ;
												/*	演奏状態の読み込み		*/
							cdr_pause( 0 ) ;	/*	CDをPAUSE				*/
							starttm = dsktm ;
							}
						else
							fr( 1, 1 ) ;
						break ;
				}
				select = 0 ;
			}
		}while( 1 );
}

int getds() {
    int ret01;
    __asm {
        mov	ax, ds
	movzx	eax, ax
	mov	ret01, eax
    };
    return ret01;
}
