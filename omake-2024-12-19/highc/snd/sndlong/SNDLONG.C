/******************************************************************************
 *																			  *
 *  SOUND LIBRARY SAMPLE PROGRAM											  *
 *																			  *
 *		  TITLE    : 少ない作業領域で大きなＳＮＤデータを鳴らすプログラム	  *
 *					  (ダブルバッファ方式を用いています）					  *
 *																			  *
 *        使用方法 : ｺﾏﾝﾄﾞﾓｰﾄﾞ上から run386 sndlong <sound_file.snd>		  *
 *																			  *
 *        注意     : run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、  *
 *                   ｶｰｿﾙが表示されません                                     *
 *																			  *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\include\snd.h"

#define  head_length	32					/*  ｻｳﾝﾄﾞﾃﾞｰﾀのﾍｯﾀﾞ長			*/
#define  cont_length 	128 * 1024			/*  1つのｳｪｰﾌﾞﾃﾞｰﾀﾊﾞｯﾌｧの大きさ	*/
#define  buf_length  	head_length+cont_length*2
											/*  再生用作業領域の大きさ		*/
#define  ch          	71					/*  音声ﾓｰﾄﾞに使用するﾁｬﾝﾈﾙ番号	*/

extern int SND_get_elevol_mute() ;

void main(argc,argv)
int argc ;
char *argv[] ;
{
	char str_buf[8] ;

	char *snd_work ;
	char *voice_buffer ;

	FILE *fp ;

	int flen,rlen,vlen ;

	int note,bank,end_flag,mute_data ;

	if (argc < 2)
		exit(1) ;

	if((snd_work = malloc(16384)) == 0)		/*  サウンド用作業領域の取得	*/
		exit(1) ;
	if((voice_buffer = malloc(buf_length)) == 0){
											/*  再生用作業領域の取得		*/
		free(snd_work);
		exit(1) ;
		}

	SND_init(snd_work) ;					/*  サウンドドライバの初期化	*/
	mute_data = SND_get_elevol_mute(0) ;	/*  ミュート情報の取得			*/
	SND_elevol_mute(0x01) ;					/*  PCM以外をミュート			*/
	SND_pcm_mode_set(1) ;					/*  PCMの1ﾁｬﾝﾈﾙを音声モードに	*/

	if ((fp = fopen(argv[1],"rb")) == 0)
		goto abort ;
	if (fseek(fp,0,SEEK_END))
		goto abort ;
	flen = (unsigned int)ftell(fp) ;
	if (fseek(fp,0,SEEK_SET))
		goto abort ;

	memset(voice_buffer,0,buf_length) ;

	rlen = fread(voice_buffer,1,buf_length,fp) ;
											/*  全作業領域分ﾃﾞｰﾀを読み込む	*/
	note = (char)voice_buffer[28] ;			/*  ｻｳﾝﾄﾞﾃﾞｰﾀの基本音階の取得	*/
	if (flen <= rlen){						/*  ﾌｧｲﾙ長より多く読み込んだ時	*/
		SND_pcm_play(ch,note,127,voice_buffer) ;
											/*	 データを再生する			*/
		while(SND_pcm_status(ch)) ;			/* 	 再生終了まで待つ			*/
		SND_pcm_play_stop(ch) ;				/*	 再生を止める				*/
		goto abort ;
		}

	end_flag = 0 ;							/*	ウェーブデータ終了フラグ	*/
	bank = 1 ;								/*	演奏中のバッファのフラグ	*/
	vlen = *(unsigned int *)&voice_buffer[12] ;
											/*	ｳｪｰﾌﾞﾃﾞｰﾀ長の取得			*/
	vlen -= (buf_length-head_length) ;
	*(unsigned int *)&voice_buffer[12] = buf_length-head_length ;
											/*	ｳｪｰﾌﾞﾃﾞｰﾀ長をﾊﾞｯﾌｧ長に設定	*/
	*(unsigned int *)&voice_buffer[16] = 0 ;/*	ループポイントを先頭に設定	*/
	*(unsigned int *)&voice_buffer[20] = buf_length-head_length ;
											/*	ﾙ-ﾌﾟ長をｳｪｰﾌﾞﾃﾞｰﾀﾊﾞｯﾌｧ長に	*/
	SND_pcm_play(ch,note,127,voice_buffer) ;/*	ｳｪ-ﾌﾞﾃﾞｰﾀの再生を始める		*/
	do{
		SND_status(ch,0,str_buf) ;			/*	演奏中のアドレスを読み込む	*/
		if ((*(unsigned int *)&str_buf[0] >= (unsigned int)
			(voice_buffer+head_length+(cont_length*bank))) &&
			(*(unsigned int *)&str_buf[0] <= (unsigned int)
			(voice_buffer+head_length+(cont_length*bank)+cont_length))){													/*	演奏中のﾊﾞｯﾌｧが切り替わったか*/
			bank ^= 1 ;						/*	フラグの反転				*/
			if (vlen == 0){					/*	残りウェーブデータが無いとき*/
				memset(voice_buffer+(bank*cont_length)+head_length
				  ,0x80,cont_length) ;		/*	書き込みﾊﾞｯﾌｧを振幅0で埋める*/
				while(*(unsigned int *)&str_buf[0] <= (unsigned int)
				  (voice_buffer+head_length+(!bank*cont_length)+rlen))
					SND_status(ch,0,str_buf) ;
				end_flag = 1 ;
				}
			else{
				rlen = fread(voice_buffer+(bank*cont_length)+head_length
						,1,cont_length,fp) ;/*	ｳｪｰﾌﾞﾃﾞｰﾀを読み込む			*/
				if (vlen <= rlen){			/*	ﾃﾞｰﾀ長が残りｳｪｰﾌﾞﾃﾞｰﾀ長以上か*/
					memset(voice_buffer+(bank*cont_length)+head_length+rlen
					  ,0x80,cont_length-rlen) ;
											/*	ｳｪｰﾌﾞﾃﾞｰﾀ後を振幅0で埋める	*/
					vlen = 0 ;
					}
				else
					vlen -= rlen ;
				}
			}
		}while(!end_flag) ;

abort:
	fclose(fp) ;
	SND_pcm_play_stop(ch) ;					/*	再生を止める				*/
	SND_end() ;								/*	サウンドドライバの終了		*/
	SND_elevol_mute(mute_data) ;			/*	ミュート状態を元に戻す		*/
	free(voice_buffer) ;
	free(snd_work) ;
}
