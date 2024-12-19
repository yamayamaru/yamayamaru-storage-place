/******************************************************************************
 *																			  *
 *  EGB LIBRARY SAMPLE PROGRAM												  *
 *																			  *
 *        TITLE    : GETPUT 												  *
 *																			  *
 *        使用方法 : 起動      : TownsMENU(中解像度)上からﾀﾞﾌﾞﾙｸﾘｯｸ			  *
 *                   ｺﾋﾟｰ      : ﾏｳｽ左ﾎﾞﾀﾝを押したまま移動  				  *
 *                   ｺﾋﾟｰ(連続): ﾏｳｽ右ﾎﾞﾀﾝを押したまま移動 					  *
 *                   終了      : ﾏｳｽ左右ﾎﾞﾀﾝをｸﾘｯｸ      					  *
 *																			  *
 *****************************************************************************/

//#include <msdos.cf>
#include "..\include\egb.h"
#include "..\include\mos.h"

char egb_work[EgbWorkSize];				/*  ＥＧＢ作業領域                  */
char mos_work[MosWorkSize];				/*  ＭＯＳ作業領域                  */
char para[64];							/*  パラメータ受け渡し作業領域      */
char get_buf[64*4/8*64];				/*  ドットデータの読み書き作業領域  */
char save_buf[64*4/8*64];				/*  画面退避領域                    */

int  x0,y0,x1,y1 = -1;

void get_data();
void put_data();
void xorbox();
void setpos();

int getds();

void main()
{
	int page,pri;
	int ch,x,y;
	int bgn_flag,get_flag,mv_flag;

	EGB_resolution(egb_work,0,0x43);	/*  グラフィック仮想画面の設定      */
	EGB_resolution(egb_work,1,0x43);	/*  グラフィック仮想画面の設定      */
	page = EGB_getWritePage(0,0);		/*  書き込みページの読み取り        */
	EGB_writePage(egb_work,page | 0x40);/*  グラフィック書き込みページ      */
	EGB_getDisplayPage(&page,&pri);		/*  表示ページの読み取り            */
	EGB_displayPage(egb_work,page,pri);	/*  グラフィック表示ページの指定    */
	MOS_start(mos_work,MosWorkSize);	/*  マウス動作開始の設定            */
	MOS_horizon(0,640-64);				/*  マウス水平移動範囲指定          */
	MOS_vertical(0,480-64);				/*  マウス垂直移動範囲指定          */

	bgn_flag = get_flag = mv_flag = 0;

	do {
		MOS_rdpos(&ch,&x,&y);			/*  マウス位置とボタンの読み取り    */

		if (!bgn_flag) {				/*  動作開始直後の初期設定          */
			setpos(x,y);
			xorbox(x0,y0,x1,y1);
			bgn_flag = 1;
			}

		if ((x != x0) || (y != y0)) {	/*  マウスの座標が変化した？        */
			ch &= 0x03;
			mv_flag = 1 ;
			switch(ch){
				case	0x00:			/*  ボタンが押されていない時の処理  */
					xorbox(x0,y0,x1,y1);
					setpos(x,y);
					xorbox(x0,y0,x1,y1);
					get_flag = 0;
					break;
				case	0x01:			/*  左ボタンが押された時の処理      */
					if (!get_flag){
						xorbox(x0,y0,x1,y1);
						setpos(x,y);
						get_data(get_buf,x0,y0,x1,y1);
						get_data(save_buf,x0,y0,x1,y1);
						xorbox(x0,y0,x1,y1);
						get_flag = 1;
						}
					else{
						xorbox(x0,y0,x1,y1);
						put_data(save_buf,x0,y0,x1,y1);
						setpos(x,y);
						get_data(save_buf,x0,y0,x1,y1);
						put_data(get_buf,x0,y0,x1,y1);
						xorbox(x0,y0,x1,y1);
						}
					break;
				case	0x02:			/*  右ボタンが押された時の処理      */
					if (!get_flag){
						xorbox(x0,y0,x1,y1);
						setpos(x,y);
						get_data(get_buf,x0,y0,x1,y1);
						xorbox(x0,y0,x1,y1);
						get_flag = 1;
						}
					else{
						xorbox(x0,y0,x1,y1);
						setpos(x,y);
						put_data(get_buf,x0,y0,x1,y1);
						xorbox(x0,y0,x1,y1);
						}
					break;
				}
			}
			else if(ch == 0x00 && get_flag && mv_flag){
					put_data(get_buf,x0,y0,x1,y1) ;
					get_data(save_buf,x0,y0,x1,y1);
					xorbox(x0,y0,x1,y1) ;
					mv_flag = 0 ;
					}

		} while (ch != 0x03);			/*  左右ボタンが押された？          */

	xorbox(x0,y0,x1,y1);
	MOS_end();							/*  マウス動作終了の設定            */
}

void setpos(int x,int y)
{
	x0 = (short)x;						/*  座標更新処理                    */
	y0 = (short)y;
	x1 = x0 + 63;
	y1 = y0 + 63;
}

void xorbox(int x0,int y0,int x1,int y1)
{
	EGB_color(egb_work,0,15);			/*  前景色を色識別番号１５に設定    */
	EGB_writeMode(egb_work,4);			/*  描画モードをＸＯＲに設定        */
	EGB_paintMode(egb_work,0x02);		/*  グラフィック面塗りモードの設定  */
	WORD(para + 0) = (short)x0;
	WORD(para + 2) = (short)y0;
	WORD(para + 4) = (short)x1;
	WORD(para + 6) = (short)y1;
	EGB_rectangle(egb_work,para);		/*  矩形描画                        */
}

void get_data(char *buf,int x0,int y0,int x1,int y1)
{
	EGB_writeMode(egb_work,0);			/*  描画モードをＰＳＥＴに設定      */
	DWORD(para + 0 ) = (unsigned int)buf;
	WORD (para + 4 ) = getds();
	WORD (para + 6 ) = (short)x0;
	WORD (para + 8 ) = (short)y0;
	WORD (para + 10) = (short)x1;
	WORD (para + 12) = (short)y1;
	EGB_getBlock(egb_work,para);		/*  ドットデータの読み込み１        */
}

void put_data(char *buf,int x0,int y0,int x1,int y1)
{
	EGB_writeMode(egb_work,0);			/*  描画モードをＰＳＥＴに設定      */
	DWORD(para + 0 ) = (unsigned int)buf;
	WORD (para + 4 ) = getds();
	WORD (para + 6 ) = (short)x0;
	WORD (para + 8 ) = (short)y0;
	WORD (para + 10) = (short)x1;
	WORD (para + 12) = (short)y1;
	EGB_putBlock(egb_work,0,para);		/*  ドットデータの書き込み１        */
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
