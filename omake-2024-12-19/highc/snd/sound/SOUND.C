/******************************************************************************
 *																			  *
 *  SOUND LIBRARY SAMPLE PROGRAM											  *
 *																			  *
 *        TITLE    : SOUND FILE 演奏（音声モード）							  *
 *																			  *
 *        使用方法 : ｺﾏﾝﾄﾞﾓｰﾄﾞ上から	 				   					  *
 *					   ＞run386 sound <sound_filename.snd>					  *
 *				または ＞run386 sound 1～10									  *
 *																			  *
 *					 例＞run386 sound stop_f.snd  							  *
 *																			  *
 *        注意     : run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、  *
 *                   ｶｰｿﾙが表示されません                                     *
 *																			  *
 *****************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\include\snd.h"

#define	ch		71


void key_in() ;
void snd_set() ;
void close_routine() ;

char *buffer ;
char *snd_work ;

int  mute_data ;

FILE *fp ;

void main( argc , argv )
int argc ;
char *argv[] ;
{
	int	flen , note , num ;

	if( argc != 2 ){
		printf( "使い方 : run386 sound <SNDファイル>\n" ) ;
		printf( "  または run386 sound 1～10\n" ) ;
		key_in() ;
		exit( -1 ) ;
		}

	if( ( snd_work = malloc(16384) ) == 0 ){
										/*	サウンドドライバ用の取得		*/
		printf( "サウンドドライバ用ワークが取得できません\n" ) ;
		key_in() ;
		exit( -1 ) ;
		}

		num = atoi( argv[ 1 ] ) ;

		if ( num > 0 && strlen( argv[ 1 ] ) < 3 ){
			if ( num > 10 ){
				printf( "数字パラメータは1～10の範囲で指定して下さい\n" );
				free( snd_work ) ;
				key_in() ;
				exit( -1 ) ;
				}
			snd_set() ;
			SND_pcm_play_rom( ch , 62 , 127 , num ) ;
										/*	システムROM内音声データ演奏開始	*/
			while( SND_pcm_status( ch ) ) ;
										/*	演奏終了を待つ					*/
			close_routine() ;
			exit( 0 ) ;
			}

	if( ( fp = fopen( argv[1],"rb" ) ) == 0 ){
		printf( "指定したファイルが見つかりません\n" ) ;
		free( snd_work );
		key_in() ;
		exit( -1 ) ;
		}

	fseek(fp,0,SEEK_END) ;
	flen = (unsigned int)ftell( fp ) ;
	fseek(fp,0,SEEK_SET) ;

	if(( buffer = malloc(flen) ) == 0 ){
		fclose( fp );
		free( snd_work ) ;
		printf( "演奏用バッファが取得できません\n" ) ;
		key_in() ;
		exit( -1 ) ;
		}

	memset( buffer , 0 , flen ) ;
	fread( buffer , 1 , flen , fp ) ;
	fclose( fp );

	note = ( char )buffer[ 28 ] ;			/*  ｻｳﾝﾄﾞﾃﾞｰﾀの基本音階の取得	*/
	snd_set() ;
	SND_pcm_play( ch , note , 127 , buffer ) ;
	while( SND_pcm_status( ch ) ) ;

	free( buffer ) ;
	close_routine() ;
}

void key_in()
{
	printf( "\nRETURNキーを押してください\n" ) ;
	getchar() ;
}

void snd_set()
{
	SND_init( snd_work ) ;					/*  サウンドドライバの初期化	*/
	mute_data = SND_get_elevol_mute( 0 ) ;	/*  ミュート情報の取得			*/
	SND_elevol_mute( 0x01 ) ;				/*  PCM以外をミュート			*/
	SND_pcm_mode_set( 1 ) ;					/*  PCMの1ﾁｬﾝﾈﾙを音声モードに	*/
}

void close_routine()
{
	SND_pcm_play_stop( ch ) ;				/*	演奏を止める				*/
	SND_end() ;								/*	サウンドドライバの終了		*/
	SND_elevol_mute( mute_data ) ;			/*	ミュート状態を元に戻す		*/
	free( snd_work ) ;
}
