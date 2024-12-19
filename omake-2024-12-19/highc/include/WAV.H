/***************************************************************/
/*                                                             */
/* WAV.H                                                       */
/*            COPYRIGHT (C) 1994 FUJITSU LIMITED               */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* 録音／再生性能                                              */
/***************************************************************/
#define	WAV_CAP_8BIT_REC		0x00000001	/* 8ビット録音可   */
#define	WAV_CAP_8BIT_PLAY		0x00000002	/* 8ビット再生可   */
#define	WAV_CAP_16BIT_REC		0x00000004	/* 16ビット録音可  */
#define	WAV_CAP_16BIT_PLAY		0x00000008	/* 16ビット再生可  */
#define	WAV_CAP_STEREO_REC		0x00000010	/* ステレオ録音可  */
#define	WAV_CAP_STEREO_PLAY		0x00000020	/* ステレオ再生可  */
#define	WAV_CAP_FREQ_REC		0x00000040	/* 指定FREQ録音可  */
#define	WAV_CAP_FREQ_PLAY		0x00000080	/* 指定FREQ再生可  */
#define	WAV_CAP_16PCM_EXIST		0x80000000	/* 16ﾋﾞｯﾄPCM音源有 */

/***************************************************************/
/* 録音／再生状態                                              */
/***************************************************************/
#define	WAV_ST_REC_PROCESS		0x00000001	/* 録音開始受付中  */
#define	WAV_ST_REC_RECORDING	0x00000002	/* 録音中          */
#define	WAV_ST_REC_STOP			0x00000004	/* 録音を強制終了  */
#define	WAV_ST_REC_ABNORMAL		0x00000008	/* 録音中にエラー  */
#define	WAV_ST_PLAY_PROCESS		0x00000010	/* 再生開始受付中  */
#define	WAV_ST_PLAY_PLAYING		0x00000020	/* 再生中          */
#define	WAV_ST_PLAY_STOP		0x00000040	/* 再生を強制終了  */
#define	WAV_ST_PLAY_ABNORMAL	0x00000080	/* 再生中にエラー  */
#define	WAV_ST_REC_OVERRUN		0x00000100	/* 録音中にｵｰﾊﾞｰﾗﾝ */
#define	WAV_ST_PLAY_UNDERRUN	0x00000200	/* 再生中にｱﾝﾀﾞｰﾗﾝ */

/***************************************************************/
/* 録音モード                                                  */
/***************************************************************/
#define	WAV_REC_COMPLETE		1			/* 完了復帰モード  */
#define	WAV_REC_IMMEDIATE		2			/* 即時復帰モード  */

/***************************************************************/
/* WAVEライブラリ関数プロトタイプ                              */
/***************************************************************/
/* ライブラリの終了                                            */
int WAV_end( void ) ;
/* 録音／再生性能取得                                          */
int WAV_getCapability( int *capa, int freq ) ;
/* 再生音量のミュート取得                                      */
int WAV_getMute( int *mode ) ;
/* 録音／再生状態取得                                          */
int WAV_getStatus( int *status ) ;
/* 再生音量取得                                                */
int WAV_getVolume( int *l_vol, int *r_vol ) ;
/* WAVEデータ情報取得                                          */
int WAV_getWaveInfo( char *buf, int bufsz,int *freq, int *bitno, int *kind,
                     int *pcmsz, int *pcmst ) ;
/* ライブラリの初期化                                          */
int WAV_init( void ) ;
/* リングバッファ管理テーブル作成                              */
int WAV_makeTable( char *ringbuf, char *cntltbl ) ;
/* 再生開始                                                    */
int WAV_play( unsigned int bufsz ) ;
/* 再生データアドレスの取得                                    */
int WAV_playGetAddress( char *buf ) ;
/* 再生前準備                                                  */
int WAV_playPrepare( int freq, int bitno, int kind, char *playbuf,
                     void (*syncFunc)() ) ;
/* 再生強制終了                                                */
int WAV_playStop( void ) ;
/* リングバッファ管理テーブル及びリングバッファアドレスの復帰  */
int WAV_popTable( char *ptr ) ;
/* リングバッファ管理テーブル及びリングバッファアドレスの退避  */
int WAV_pushTable( char *ptr ) ;
/* 録音開始                                                    */
int WAV_rec( int mode, int trig, unsigned int bufsz ) ;
/* 録音データの格納アドレスの取得                              */
int WAV_recGetAddress( char *buf ) ;
/* 録音前準備                                                  */
int WAV_recPrepare( int freq, int bitno, int kind, char *recbuf,
                    void (*syncFunc)() ) ;
/* 録音強制終了                                                */
int WAV_recStop( void ) ;
/* 録音・再生状態の初期化                                      */
int WAV_reset( void ) ;
/* 再生音量のミュート設定                                      */
int WAV_setMute( int mode ) ;
/* 再生音量設定                                                */
int WAV_setVolume( int l_vol, int r_vol ) ;
/*  WAVEデータ情報設定                                         */
int WAV_setWaveInfo( char *buf, int freq, int bitno, int kind,
                     int pcmsz, int *pcmst ) ;
