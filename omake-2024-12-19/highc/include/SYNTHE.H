/******************************************************************************

名前:	SYNTHE.H

機能:	音声合成ライブラリプロトタイプ宣言ファイル

説明:	

関連:	

日付:
	[新規] 1994年 8月26日(金) V--L-- FPL)第二開発部音声プロジェクト 土屋 靖
	[更新] 

						COPYRIGHT(C) FUJITSU LIMITED 1994.

******************************************************************************/

/** 復帰情報 **/
#define SYRET_END (0)
#define SYRET_CONTINUE (1)
#define SYRET_NOTPLAY (0)
#define SYRET_PLAY (1)
#define SYRET_PAUSE (2)
#define SYRET_GENERATE (3)

#define SYERR_NOERROR (0)
#define SYERR_INIT (-1)
#define SYERR_BUSY (-2)
#define SYERR_INVALPARAM (-3)
#define SYERR_NOTSTARTED (-4)
#define SYERR_NOMEM (-5)
#define SYERR_INVALWDICNAME (-6)
#define SYERR_WDICFMTERROR (-7)
#define SYERR_INVALWDICKIND (-8)
#define SYERR_NOQUEUE (-9)
#define SYERR_WAVLIBERR (-10)

/** 波形辞書の種類 **/
#define SYT_WAVE_KIND_MALE (1)
#define SYT_WAVE_KIND_FEMALE (2)
#define SYT_WAVE_KIND_BOTH (3)

/** プロトタイプ宣言 **/
extern int SYT_syntheInit( char *, int, int );
extern int SYT_syntheEnd();
extern int SYT_synthesize( char * );
extern int SYT_synthePause();
extern int SYT_syntheRestart();
extern int SYT_syntheStop();
extern int SYT_syntheID();
extern int SYT_syntheStat();

extern int SYT_generatePcmStart( char * );
extern int SYT_generatePcm( char *, int, int * );
extern int SYT_generatePcmStop();

extern int SYT_setDicType( int );
extern int SYT_getDicType();
extern int SYT_setSpeed( int );
extern int SYT_getSpeed();
extern int SYT_setPitch( int );
extern int SYT_getPitch();
extern int SYT_setVolume( int );
extern int SYT_getVolume();
extern int SYT_setTone( int );
extern int SYT_getTone();
extern int SYT_setIntonation( int );
extern int SYT_getIntonation();

extern void SYT_syntheMfunc( void *((*)(size_t)), void (*)(void *) );


