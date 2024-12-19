/***************************************************************/
/*                                                             */
/* CDRFRB.H                                                    */
/*            COPYRIGHT (C) 1988-1991, FUJITSU LIMITED         */
/*                                                             */
/***************************************************************/

struct TIMEADRS {
	char min;
	char sec;
	char frame;
};

/* ドライブモードの設定 */
int cdr_sdrvmd( int deviceno, int sectorsize );

/* ドライブモードの読み取り */
int cdr_rdrvmd( int deviceno, int *sectorsize );

/* ドライブステータス情報の読み取り */
int cdr_status( int deviceno, int *sectorsize, long *lsectorsize );

/* シリンダ0へのシーク */
int cdr_restore( int deviceno );

/* 指定位置へのシーク (論理セクタ指定) */
int cdr_seek( int deviceno, int lsector );

/* データの読み取り (論理セクタ指定) */
int cdr_read( int deviceno, int lsector, char *buffer, unsigned int count );

/* 指定位置へのシーク (時間指定) */
int cdr_tseek( int deviceno, struct TIMEADRS *timeadrs );

/* データの読み取り (時間指定) */
int cdr_tread( int deviceno, struct TIMEADRS *timeadrs,
				char *buffer, unsigned int count );

/* 音楽演奏スタート (時間指定) */
int cdr_mtplay( int deviceno, struct TIMEADRS *starttime,
				struct TIMEADRS *endtime );

/* 音楽演奏情報の読み取り (時間指定) */
int cdr_rmtplay( int deviceno, struct TIMEADRS *starttimr,
				struct TIMEADRS *endtime );

/* 音楽演奏ストップ */
int cdr_mstop( int deviceno );

/* 音楽演奏状態の読み取り */
int cdr_mphase( int deviceno, int *status, int *nowmusicno,
				struct TIMEADRS *tracktime, struct TIMEADRS *disktime );

/* コンパクトディスク情報の読み取り */
int cdr_cdinfo( int deviceno, int *cdtype, int *starttrack, int *endtrack,
				struct TIMEADRS *tracktime, struct TIMEADRS *disktime );

/* 音楽演奏一時停止 */
int cdr_pause( int deviceno );

/* 音楽演奏一時停止解除 */
int cdr_continue( int deviceno );

/* データの読み取り２ (論理セクタ指定) */
int cdr_read2( int deviceno, int lsector, char *buffer, unsigned int count );

/* データの読み取り２ (時間指定) */
int cdr_tread2( int deviceno, struct TIMEADRS *timeadrs,
				char *buffer, unsigned int count );

/* 音楽演奏スタート (時間･回数指定) */
int cdr_mtrplay( int deviceno, struct TIMEADRS *starttime,
				struct TIMEADRS *endtime, unsigned char count );

/* ＣＤ－ＲＯＭドライブの回転停止時間の設定 */
int cdr_stptime( int deviceno, unsigned char stoptime );

/* トレイオープン */
int cdr_dopen( int deviceno );

/* トレイクローズ */
int cdr_dclose( int deviceno );

/* オープン/クローズの有効設定 */
int cdr_dunlock( int deviceno );

/* オープン/クローズの無効設定 */
int cdr_dlock( int deviceno );

/* エラー値の定義 */
#define	CDERR0		0x001		/* ノットレディ */
#define	CDERR1		0x002		/* パラメータエラー */
#define	CDERR2		0x004		/* ドライブ非接続 */
#define	CDERR3		0x008		/* コマンド異常終了 */
#define	CDERR4		0x010		/* メディアエラー */
#define	CDERR5		0x020		/* ハードエラー */
#define	CDERR6		0x040		/* パリティエラー */
#define	CDERR7		0x080		/* メディア交換あり */
#define	DEVERR		0x100		/* 指定デバイス番号誤り */
#define	DEVSEC		0x101		/* セクタ長が違う */
#define	DEVPLY		0x102		/* 音楽演奏中 */
#define	DEVPAUSE	0x104		/* 既に演奏が一時停止中 */
#define	DEVNPAUSE	0x108		/* 演奏が一時停止状態でない */
#define UNKNOWNERR	0xffffffff	/* 未定義エラー */
