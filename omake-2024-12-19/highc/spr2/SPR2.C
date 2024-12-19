/******************************************************************************
 *																			  *
 *  SPRITE LIBRARY SAMPLE PROGRAM 											  *
 *																			  *
 *        TITLE    : ﾋﾟﾝﾎﾟﾝﾎﾞｰﾙ												  *
 *																			  *
 *        使用方法 : [起動] TownsMENUまたはｺﾏﾝﾄﾞﾓｰﾄﾞ上    					  *
 *                   [終了] ﾏｳｽ左右ﾎﾞﾀﾝをｸﾘｯｸ			   					  *
 *																			  *
 *****************************************************************************/

//#include	<msdos.cf>
#include	<stdlib.h>
#include	"..\include\egb.h"
#include	"..\include\mos.h"
#include	"..\include\spr.h"

#define	TAMA  16
#define	sizeX 2
#define	sizeY 2
#define enALL 5

void	sprset() ;

int		color[enALL]       = { 0x4e73 , 0x5ad6 , 0x6318 , 0x6f7b , 0x7fff } ;
int		circle_x[enALL]    = {     15 ,     14 ,     13 ,     12 ,     11 } ;
int		circle_y[enALL]    = {     15 ,     13 ,     12 ,     11 ,     10 } ;
int		circle_size[enALL] = {     15 ,     12 ,      9 ,      8 ,      5 } ;

char	mwork[4096] ;
char	work[1536] ;
char	para[64] ;
char	get_buf[512] ;


int getds();

void main()
{
	int	vector_x[TAMA] ;
	int	vector_y[TAMA] ;
	int	x[TAMA],y[TAMA] ;
	int	ch,dx,bx ;
	int	i ;

	EGB_init(work,1536) ;				/*  グラフィック初期化            */
	EGB_resolution(work,0,11) ;			/*  グラフィック仮想画面の設定    */
	EGB_resolution(work,1,8) ;			/*  グラフィック仮想画面の設定    */
	EGB_displayPage(work,1,3) ;			/*  グラフィック表示ページの指定  */
	SPR_init() ;						/*  スプライト初期化              */
	sprset(work) ;						/*  スプライト定義                */

	EGB_color(work,1,0x043f) ;			/*  グラフィック背景色の設定      */
	EGB_clearScreen(work) ;				/*  背景を青く塗りつぶす          */
	EGB_writePage(work,1) ;				/*  グラフィック書き込みページ    */
	EGB_displayStart(work,2,5,1) ;		/*  グラフィック画面の拡大        */
	EGB_displayStart(work,3,256,240) ;	/*  グラフィック表示画面の大きさ  */
	MOS_start(mwork,4096) ;				/*  マウス動作開始の設定          */

	/*  乱数による動作開始位置と移動量の設定  */
	for (i = 0 ; i < TAMA ; i++){
		x[i] = (rand() & 0x7f) ;
		y[i] = (rand() & 0x7f) ;
		ch = (rand() & 3) + 1 ;
		vector_x[i] = (rand() & 1) ? ch : -ch ;
		vector_y[i] = (rand() & 1) ? ch : -ch ;
		}

	/*  スプライト位置の設定  スプライトアトリビュートの設定  */
	for (i = 0 ; i < TAMA ; i++){
		SPR_setPosition(0,1024-(sizeX*sizeY)*(i+1),sizeX,sizeY,x[i],y[i]) ;
		SPR_setAttribute(1024-(sizeX*sizeY)*(i+1),sizeX,sizeY,128,0) ;
		}

	/*  スプライト動作開始  */
	SPR_display(1,sizeX*sizeY*TAMA) ;

	do {
		for (i = 0 ; i < TAMA ; i++){
			/*  スプライト座標計算  */
			x[i] += vector_x[i] ;
			y[i] += vector_y[i] ;
			/*  スプライトＸ座標範囲チェック  */
			if ((x[i] < 2) || ((x[i] + sizeX*16) > 256)){
				vector_x[i] *= -1 ;
				x[i] += vector_x[i] ;
				}
			/*  スプライトＹ座標範囲チェック  */
			if ((y[i] < 0) || ((y[i] + sizeY*16) > 239)){
				vector_y[i] *= -1 ;
				y[i] += vector_y[i] ;
				}
			}
		/*  スプライトＲＥＡＤＹを待つ  */
		SPR_display(2,sizeX*sizeY*TAMA) ;
		/*  スプライト位置の設定  */
		for (i = 0 ; i < TAMA ; i++)
			SPR_setPosition(0,1024-(sizeX*sizeY)*(i+1),sizeX,sizeY,x[i],y[i]);
		MOS_rdpos(&ch,&dx,&bx) ;		/*  マウス位置とボタンの読み取り  */
		} while (ch != 3) ;				/*  左右ボタンが押されている？    */
	SPR_display(0,sizeX*sizeY*TAMA) ;	/*  スプライト動作停止            */
	MOS_end() ;							/*  マウス動作終了の設定          */
}

void sprset(work)
char *work;
{
	int i,x,y ;

	EGB_writeMode(work,0) ;				/*  グラフィック描画モードの設定    */
	EGB_paintMode(work,0x22) ;			/*  グラフィック面塗りモードの設定  */
	EGB_color(work,1,0x8000) ;			/*  グラフィック背景色の設定        */
	EGB_clearScreen(work) ;				/*  スーパーインポーズビットを設定  */

	for (i=0 ; i<enALL ; i++){
		EGB_color(work,0,color[i]) ;	/*  グラフィック前景色の設定    */
		EGB_color(work,2,color[i]) ;	/*  グラフィック面塗り色の設定  */
		WORD(para+0) = circle_x[i] ;
		WORD(para+2) = circle_y[i] ;
		WORD(para+4) = circle_size[i] ;
		EGB_circle(work,para) ;			/*  円の設定                    */
		}

	i = 0 ;
	DWORD(para+0) = (unsigned int)get_buf ;
	WORD(para+4)  = getds() ;
	for (y=0 ; y<sizeY ; y++){
		for (x=0 ; x<sizeX ; x++){
			WORD(para+6)  = 16*x ;
			WORD(para+8)  = 16*y ;
			WORD(para+10) = 16*x + 15 ;
			WORD(para+12) = 16*y + 15 ;
			/*  グラフィックドットデータの読み込み１  */
			EGB_getBlock(work,para) ;
			/*  スプライトの定義  */
			SPR_define(1,128+i,1,1,get_buf) ;
			i += 4 ;
			}
		}
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
