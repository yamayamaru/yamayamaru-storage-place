/******************************************************************************
 *																			  *
 *  SOUND LIBRARY SAMPLE PROGRAM											  *
 *																			  *
 *        TITLE    : 音声録音･再生                							  *
 *																			  *
 *        使用方法 : ｺﾏﾝﾄﾞﾓｰﾄﾞ上から run386 voice                 			  *
 *					 ﾏｳｽ左ﾎﾞﾀﾝをｸﾘｯｸし,音声入力を開始				  		  *
 *				     [再生] ﾏｳｽ左ﾎﾞﾀﾝをｸﾘｯｸ        							  *
 *				     [終了] ﾏｳｽ右ﾎﾞﾀﾝをｸﾘｯｸ        							  *
 *																			  *
 *        注意     : run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、  *
 *                   ｶｰｿﾙが表示されません                                     *
 *																			  *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "..\..\include\egb.h"
#include "..\..\include\mos.h"
#include "..\..\include\snd.h"

#define ch   71						/*  音声出力チャンネル                  */
#define note 60						/*  音声出力の音階                      */
#define vol  127					/*  音声出力の音量                      */
#define freq 19200					/*  サンプリングレート                  */
#define size freq*10				/*  サンプリングデータ長                */
#define evol 2						/*  電子ボリュームチャンネル（マイク）  */

struct str_data{
				short	x ;
				short	y ;
				unsigned short len ;
				char	string_buf[60] ;
		} ;

struct str_data m[5] = {
		184,159, 34,"マイクからの音をサンプリングします",
		184,191, 34,"マウスの左ボタンを押すと開始します",
		232,223, 22,"サンプリングしています",
		200,255, 30,"サンプリングした音を再生します",
		 96,287, 56,"マウスの左ボタンを押すと再生．右ボタンを押すと終了します"
		} ;

void main()
{
	char *snd_work ;
	char *egb_work ;
	char *mos_work ;
	char *buff ;

	int btn,x,y ;

	if ((snd_work = malloc(16384)) == 0)	/*  サウンド作業領域の取得      */
		exit(1) ;
	if ((egb_work = malloc(1536)) == 0)		/*  グラフィック作業領域の取得  */
		exit(1) ;
	if ((mos_work = malloc(4096)) == 0)		/*  マウス作業領域の取得        */
		exit(1) ;
	if ((buff = malloc(size+32)) == 0)		/*  サンプリング作業領域の確保  */
		exit(1) ;

	SND_init(snd_work) ;					/*  サウンドドライバの初期化    */
	EGB_init(egb_work,1536) ;				/*  グラフィック初期化          */
	MOS_start(mos_work,4096) ;				/*  マウス動作開始の設定        */
	SND_elevol_all_mute(0) ;				/*  サウンド全ミュートの設定    */
	SND_elevol_mute(0) ;					/*  サウンドミュートの設定      */
	SND_elevol_set(evol,127,127) ;			/*  サウンドマイク入力の設定    */
	EGB_sjisString(egb_work,(char *)&m[0]) ;/*  グラフィック文字列          */
	EGB_sjisString(egb_work,(char *)&m[1]) ;/*  グラフィック文字列          */
	do {
		MOS_rdpos(&btn,&x,&y) ;				/*  マウス位置とボタンの読み取り*/
		} while(btn != 0x01) ;				/*  左ボタンが押されている？    */

	EGB_sjisString(egb_work,(char *)&m[2]) ;/*  グラフィック文字列          */
	SND_pcm_rec(freq,&buff[32],size,0) ;	/*  サウンドＰＣＭ録音開始      */

	/*  サウンドネームの設定      */
	*(int *)&buff[0]        = 0 ;
	*(int *)&buff[4]        = 0 ;
	/*  サウンドＩＤの設定        */
	srand(((int)buff[32]+(int)buff[34]*256)*(int)buff[38]*256+size) ;
	/*  データ長の設定            */
	*(int *)&buff[8]        = rand()+(buff[33]+buff[35]*256+1) ;
	*(int *)&buff[12]       = size ;
	/*  ループポイントの設定      */
	*(int *)&buff[16]       = size ;
	/*  ループレングスの設定      */
	*(int *)&buff[20]       = 0 ;
	/*  サンプリング周波数の設定  */
	*(short int *)&buff[24] = freq*0x62/1000 ;
	/*  原音の補正値の設定        */
	*(short int *)&buff[26] = 0 ;
	/*  原音の音階の設定          */
	*(char *)&buff[28]      = 60 ;
	/*  リザーブ領域の設定        */
	*(char *)&buff[29]      = 0 ;
	*(short int*)&buff[30]  = 0 ;

	SND_elevol_all_mute(-1) ;	/*  サウンド全ミュートの解除                */
	SND_elevol_mute(0x01) ;		/*  サウンドミュートの設定                  */
	SND_pcm_mode_set(1) ;		/*  サウンド音声モードチャンネル数の設定    */

	EGB_sjisString(egb_work,(char *)&m[3]) ;/*  グラフィック文字列          */
	EGB_sjisString(egb_work,(char *)&m[4]) ;/*  グラフィック文字列          */

	do {
		MOS_rdpos(&btn,&x,&y) ;				/*  マウス位置とボタンの読み取り*/
		if (btn == 0x01){					/*  左ボタンが押されている？    */
			SND_pcm_play(ch,note,vol,buff) ;/*  サウンド音声モード再生開始  */
			do{}while(SND_pcm_status(ch)) ;	/*  音声モード再生終了待ち      */
			SND_pcm_play_stop(ch) ;			/*  サウンド音声モード再生停止  */
			}
		}while(btn != 0x02) ;				/*  右ボタンが押されている？    */

	MOS_end() ;								/*  マウス動作終了の設定        */
	SND_end() ;								/*  サウンド動作終了の設定      */
	free(buff) ;
	free(mos_work) ;
	free(egb_work) ;
	free(snd_work) ;
}
