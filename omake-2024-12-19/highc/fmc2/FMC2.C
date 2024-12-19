/******************************************************************************
 *																			  *
 *  FMCFRB LIBRARY SAMPLE PROGRAM 2											  *
 *																			  *
 *        TITLE    : BEEP音を鳴らす											  *
 *																			  *
 *        使用方法 : ｺﾏﾝﾄﾞﾓｰﾄﾞ上から run386 -nocrt fmc2    					  *
 *																			  *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\fmcfrb.h"

int		endflg = 0;
extern	void	Timer_Hdr(void);	/* タイマ割り込み処理ルーチン */

void main()
{
	int			timeno;
	TIM_TIME	timer;

	printf("タイマー割り込みを設定しました．5秒後にBEEP音を一回鳴らします．\n");
	timer.mode = 1;					/*　タイマー割り込みモード      */
	timer.inf = 0;					/*　0							*/
	timer.hcycle = 0;				/*　タイマー周期(HIGH)			*/
	timer.lcycle = 500;				/*　タイマー周期(LOW) 5秒後		*/
	BSETCODEADR((char *)&timer.adr,(int (*)())Timer_Hdr);
									/*　タイマー割り込み処理アドレス*/
	TIM_settime( &timer,&timeno );

	while (endflg == 0);

}

void Timer_Hdr(void)
{
	BPB_fixon();					/*  ブザーを一定時間鳴らす		*/
	puts("Timer_Hdrに制御が渡りました．\n");
	endflg = 1;
}
