/******************************************************************************
 *    EGB LIBRARY SAMPLE PROGRAM                                              *
 *                                                                            *
 *        機能 : カラーパターンを表示し、ドラッグ移動可能にする               *
 *                解像度無依存インタフェースを使用し、現在の画面の解像度に    *
 *                合わせた画面表示とマウス移動範囲制御を行う。                *
 *                                                                            *
 *    使用方法 : TownsMENU上からダブルクリック                                *
 *                 [マウス左ボタン]ドラッグにより移動                         *
 *                 [マウス右ボタン]終了                                       *
 *                                                                            *
 *****************************************************************************/

#include <stdlib.h>
#include "..\include\egb.h"
#include "..\include\mos.h"
//#include <msdos.cf>

#define	TILEX		32								//  横タイル数
#define	TILEY		8								//  縦タイル数
#define	TILE		16								//  サイズ
#define	SIZEX		TILEX*TILE						//  横ドット数
#define	SIZEY		TILEY*TILE						//  縦ドット数
#define	TILESIZE	SIZEX*SIZEY						//  カラーパターンサイズ
#define	TENOHIRA	84								//  掌カーソル番号
#define	YUBI		87								//  指差しカーソル番号
#define UPAGE		0x80							//  ユーザページ
#define WPAGE		0								//  書き込みページ
#define	INSIDE		5								//  移動範囲補正値

char *egb_work;										//  ＥＧＢワークエリア
char *mos_work;										//  ＭＯＳワークエリア
char *sav_area;										//  画面退避領域
char *usr_vram;										//  ユーザＶＲＡＭ

int getds();

void get( void *buff,int x,int y )
{
	char para[ 16 ];

	DWORD( para+0  ) = ( unsigned int )buff;
	 WORD( para+4  ) = getds();
	 WORD( para+6  ) = x;
	 WORD( para+8  ) = y;
	 WORD( para+10 ) = x+SIZEX-1;
	 WORD( para+12 ) = y+SIZEY-1;
	EGB_writeMode( egb_work,0 );					//  ＰＳＥＴ描画
	MOS_disp( 0 );
	EGB_getBlock( egb_work,para );					//  ドットデータの読み込み
	MOS_disp( 1 );
}

void put( void *buff,int x,int y )
{
	char para[ 16 ];

	DWORD( para+0  ) = ( unsigned int )buff;
	 WORD( para+4  ) = getds();
	 WORD( para+6  ) = x;
	 WORD( para+8  ) = y;
	 WORD( para+10 ) = x+SIZEX-1;
	 WORD( para+12 ) = y+SIZEY-1;
	EGB_writeMode( egb_work,0 );					//  ＰＳＥＴ描画
	MOS_disp( 0 );
	EGB_putBlock( egb_work,0,para );				//  ドットデータの書き込み
	MOS_disp( 1 );
}

void rectangle( int x0,int y0,int x1,int y1 )
{
	char para[ 8 ];

	WORD( para+0 ) = x0;
	WORD( para+2 ) = y0;
	WORD( para+4 ) = x1;
	WORD( para+6 ) = y1;
	EGB_writeMode( egb_work,4 );					//  ＸＯＲ描画
	MOS_disp( 0 );
	EGB_rectangle( egb_work,para );					//  矩形
	MOS_disp( 1 );
}

void main( void )
{
	RESOLUTION  buff[ 2 ];							//  情報取得バッファ
	char        mos_pat[ 260 ];						//  マウスパターン展開領域
	char        para[ 12 ];							//  パラメータ領域

	int         handle,btn,soto,tr,x,y;
	int         msx,msy,ox,oy,bx,by,cx,cy;

	DWORD( para+0 ) = 1;							//  １画面モード
	DWORD( para+4 ) = 8;							//  ８ビット（２５６色）
	DWORD( para+8 ) = 0;
	handle = EGB_getResolutionMax( 0,para,buff );	//  最大解像度の取得
	if ( handle == -1 )								//  解像度が存在しない
		exit( -1 );

	if ( ( egb_work = malloc( EgbWorkSize ) ) == 0 )//  ＥＧＢワークエリア確保
		exit( -1 );
	if ( ( mos_work = malloc( MosWorkSize ) ) == 0 )//  ＭＯＳワークエリア確保
		exit( -1 );
	if ( ( sav_area = malloc( TILESIZE ) ) == 0 )	//  画面退避領域確保
		exit( -1 );
	if ( ( usr_vram = malloc( TILESIZE ) ) == 0 )	//  ユーザＶＲＡＭ確保
		exit( -1 );

	EGB_resolutionRam( egb_work,UPAGE,8,SIZEX,SIZEY,usr_vram );
	EGB_writePage( egb_work,UPAGE );
	EGB_paintMode( egb_work,0x20 );
	for ( y=0 ; y<TILEY ; y++ ){
		for ( x=0 ; x<TILEX ; x++ ){
			EGB_color( egb_work,2,y*TILEX+x );
			WORD( para+0 ) = x*TILE;
			WORD( para+2 ) = y*TILE;
			WORD( para+4 ) = x*TILE+TILE-1;
			WORD( para+6 ) = y*TILE+TILE-1;
			EGB_rectangle( egb_work,para );			//  カラーパターン描画
			}
		}

	MOS_start( mos_work,MosWorkSize );				//  マウスＢＩＯＳ初期化
	EGB_resolutionHandle( egb_work,0,handle );		//  仮想画面の設定
	MOS_resolutionHandle( handle );					//  マウス仮想画面の設定

	switch( EGB_getActivePage( 0,0 ) ){				//  画面クリア
		case 0x03:
			EGB_writePage( egb_work,1 );
			EGB_clearScreen( egb_work );
		case 0x01:
			EGB_writePage( egb_work,0 );
			EGB_clearScreen( egb_work );
		default:
			break;
		}

	EGB_displayPage( egb_work,WPAGE,WPAGE+1 );		//  表示ページ設定
	EGB_writePage( egb_work,WPAGE );				//  ページ０に表示
	EGB_paintMode( egb_work,0x02 );					//  境界ベタ塗り
	EGB_colorIGRB( egb_work,0,-1 );					//  描画色の設定

	MOS_writePage( WPAGE );							//  ページ０に表示
	MOS_horizon( INSIDE,buff[WPAGE].dx-1-INSIDE );	//  移動範囲を表示範囲内に
	MOS_vertical( INSIDE,buff[WPAGE].dy-1-INSIDE );	//  移動範囲を表示範囲内に
	MOS_typeRom( YUBI,0,0,mos_pat );				//  カーソル形状設定
	MOS_disp( 1 );									//  マウス表示

	bx = ( buff[WPAGE].dx - SIZEX ) / 2;			//  Ｘ座標センタリング
	by = ( buff[WPAGE].dy - SIZEY ) / 2;			//  Ｙ座標センタリング
	get( sav_area,bx,by );							//  画面退避
	put( usr_vram,bx,by );							//  カラーパターン描画
	cx = bx;
	cy = by;

	btn = soto = 0;									//  フラグ初期化

	do {
		MOS_rdpos( &tr,&x,&y );
		switch( tr & 0x01 ){
			case 0x01:								//  左ボタン処理
				if ( !soto ){
					if ( !btn ){					//  今ボタンが押された
						if ( ( x >= bx )&&( x <= bx+SIZEX )&&
							 ( y >= by )&&( y <= by+SIZEY ) ){
							btn = 1;
							MOS_typeRom( TENOHIRA,10,10,mos_pat );
							msx = x - bx;
							msy = y - by;
							MOS_horizon(  msx,
										  buff[WPAGE].dx - ( SIZEX - msx ) );
							MOS_vertical( msy,
										  buff[WPAGE].dy - ( SIZEY - msy ) );
							bx  = x - msx;
							by  = y - msy;
							rectangle( bx,by,bx+SIZEX-1,by+SIZEY-1 );
							ox  = x;
							oy  = y;
							}
						else{
							soto = 1;
							}
						}
					else{							//  既に押されている
						if ( ( x != ox )||( y != oy ) ){
							rectangle( bx,by,bx+SIZEX-1,by+SIZEY-1 );
							bx  = x - msx;
							by  = y - msy;
							rectangle( bx,by,bx+SIZEX-1,by+SIZEY-1 );
							ox  = x;
							oy  = y;
							}
						}
					}
				break;
			case 0x00:								//  ボタン開放処理
				if ( ( soto == 0 )&&( btn == 1 ) ){
					rectangle( bx,by,bx+SIZEX-1,by+SIZEY-1 );
					if ( ( cx != bx )||( cy != by ) ){
						put( sav_area,cx,cy );
						get( sav_area,bx,by );
						put( usr_vram,bx,by );
						cx = bx;
						cy = by;
						}
					MOS_horizon( INSIDE,buff[WPAGE].dx-1-INSIDE );
					MOS_vertical( INSIDE,buff[WPAGE].dy-1-INSIDE );
					MOS_typeRom( YUBI,0,0,mos_pat );
					}
				btn = soto = 0;
				break;
			default:
				break;
			}
		} while ( tr != 0x02 );						//  右ボタン押されたら終了

	MOS_end();										//  マウスＢＩＯＳ終了
	free( usr_vram );								//  メモリ開放
	free( sav_area );
	free( mos_work );
	free( egb_work );
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
