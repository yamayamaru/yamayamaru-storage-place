//*****************************************************************************
//
// ＷＡＶＥライブラリサンプルプログラム（録音編）
//
// 機  能   :マイクからの音を以下の条件でサンプリングします。
//           サンプリング周波数  ＝２２．０５０ｋＨｚ
//           サンプリングビット数＝  ８ビット（内蔵ＰＣＭ）
//                                 １６ビット（１６ビットＰＣＭ）
//           データ種別          ＝モノラル
//           サンプリング時間    ＝１０秒間
//
//           出力ファイル名      ＝MICREC.WAV
//
// 処理説明 :メモリ上にサンプリングしてファイルを作成するか
//           直接ファイルへ１６ＫＢ毎にサンプリングデータを書き込む
//
// 制限事項 :内蔵ＰＣＭでのサンプリングの場合は、直接ファイルに書き込むことは
//           できない
//
// 使用方法 :ｺﾏﾝﾄﾞﾓｰﾄﾞ上から run386 recwav
//
// 注　意   :run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、
//           ｶｰｿﾙが表示されません
//
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\egb.h"
#include "..\include\mos.h"
#include "..\include\snd.h"
#include "..\include\wav.h"

//	変数定義
#define	freq	22050						//	サンプリング周波数
#define	kind	1							//	データ種別（＝モノラル）
#define	bit8	8							//	サンプリングビット数
#define	pcmsz8	freq * ( 8/8) * 10			//	サンプリングデータ長
#define	bit16	16							//	サンプリングビット数
#define	pcmsz16	freq * (16/8) * 10			//	サンプリングデータ長

//	メッセージ文の定義
typedef struct	tagMSG_DATA
{
	short			x ;
	short			y ;
	unsigned short	len ;
	char			msg[80] ;
} MSG_DATA ;

MSG_DATA	m[13] =
{
	160,  80, 36, "マイクからの音を１０秒間録音します．",
	160, 112, 44, "マウスのボタンで録音時の処理を選択できます．",
	176, 144, 48, "左ボタン：一旦メモリに録音してファイルに保存する",
	176, 176, 32, "右ボタン：直接ファイルに保存する",
	160, 208, 32, "録音時の処理を選択してください．",
	160, 240, 20, "録音時の処理：メモリ",
	160, 240, 22, "録音時の処理：ファイル",
	160, 272, 36, "マウスの左ボタンを押すと開始します．",
	160, 304, 28, "サンプリングを開始しました．",
	160, 336, 28, "サンプリングが終了しました．",
	160, 368, 40, "現在８ビットＰＣＭです．処理できません．",
	160, 368, 30, "ファイルの作成に失敗しました．",
	160, 368, 30, "作業領域の確保に失敗しました．"
} ;

//	グローバル変数の定義
int		n_count ;							//	同期関数の呼び出し回数
char	egb_wk[EgbWorkSize] ;				//	EGB関数用作業領域
char	snd_wk[SndWorkSize] ;				//	SND関数用作業領域
char	mos_wk[MosWorkSize] ;				//	MOS関数用作業領域
char	*filenm = "MICREC.WAV" ;			//	作成するファイル名

//	メモリ上へのサンプリング
void	recmem()
{
	FILE	*pFile ;				//	ファイルポインタ
	char	*pring ;				//	リングバッファポインタ
	struct	buf_ctrl
	{
		int		sum ;
		int		apply_loc ;
		int		system_loc ;
		struct
		{
			char	*ptr ;
			char	reserve[8] ;
		}	buftbl[2] ;
	}	*pctrl ;					//	リングバッファ管理テーブルポインタ
	char	*pdata ;				//	サンプリングデータ格納領域ポインタ
	int		cap ;					//	性能情報
	int		status ;				//	ステータス
	int		ch, x, y ;				//	マウス用変数
	int		bitno ;					//	録音するサンプリングビット数
	int		pcmsz ;					//	録音するPCMデータサイズ
	int		pcmstart ;				//	PCMデータ相対開始位置
	int		wrtsize ;				//	書き込まれたデータサイズ

	//	リングバッファとリングバッファ管理テーブルの領域の確保を行う
	//	リングバッファ数は２個
	if ( (pring = malloc( (2+1)*4096 )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
		return ;
	}
	if ( (pctrl = (struct buf_ctrl *)malloc( (2+1)*12 )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
		goto MEMFIN_1 ;
	}

	//	リングバッファ管理テーブルの作成
	pctrl->sum = 2 ;						//	バッファ総数の設定
	pctrl->apply_loc = 0 ;					//	アプリケーション位置のクリア
	WAV_makeTable( pring, (char *)pctrl ) ;

	WAV_getCapability( &cap, freq ) ; 
	if ( cap & WAV_CAP_16PCM_EXIST )
	{										//	16ビットPCMによるサンプリング
		bitno = bit16 ;
		pcmsz = pcmsz16 ;
	}
	else
	{										//	内蔵PCMによるサンプリング
		bitno = bit8 ;
		pcmsz = pcmsz8 ;
	}

	//	サンプリングデータを格納する領域の確保
	if ( (pdata = malloc( pcmsz )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
		goto MEMFIN_2 ;
	}

	//	録音前準備
	WAV_recPrepare( freq, bitno, kind, pdata, (void (*)())NULL ) ;

	//	録音のメッセージを表示し、マウスのクリックを待つ
	do
	{
		MOS_rdpos(&ch, &x, &y) ;
	} while( ch != 0 ) ;						//	ボタンが解放されるのを待つ
	EGB_sjisString( egb_wk, (char *)&m[5] ) ;
	EGB_sjisString( egb_wk, (char *)&m[7] ) ;
	while( ch != 1 ) MOS_rdpos(&ch, &x, &y) ;	//	マウスの左ボタンが押される
												//	まで待つ

	//	録音開始
	EGB_sjisString( egb_wk, (char *)&m[8] ) ;	//	録音開始のメッセージ
	if ( cap & WAV_CAP_16PCM_EXIST )
	{											//	16ビットPCMによるサンプリング
		WAV_rec( WAV_REC_IMMEDIATE, 0, pcmsz ) ;
		do										//	録音が開始するまで待つ
		{
			WAV_getStatus( &status ) ;
		}	while( !( status & WAV_ST_REC_RECORDING ) ) ;
		do										//	録音が終了するまで待つ
		{
			WAV_getStatus( &status ) ;
		}	while( status & WAV_ST_REC_PROCESS ) ;
	}
	else
	{											//	内蔵PCMによるサンプリング
		SND_fm_timer_b_set( 0, 0 ) ;			//	音質向上の為タイマＢを停止
		WAV_rec( WAV_REC_COMPLETE, 0, pcmsz ) ;
		SND_fm_timer_b_start() ;				//	タイマＢを再起動
	}

	EGB_sjisString( egb_wk, (char *)&m[9] ) ;	//	録音終了のメッセージ

	//	サンプリングデータをファイルに保存する
	if ( (pFile = fopen( filenm, "wb" )) == NULL )	//	ファイル作成
	{
		EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
		goto MEMFIN_3 ;
	}
	//	WAVEファイルの情報作成しファイルに書きだす
	//	（リングバッファ領域を作業領域として使用する）
	WAV_setWaveInfo( pring, freq, bitno, kind, pcmsz, &pcmstart ) ;
	if ( (wrtsize = fwrite( pring, 1, 44, pFile)) != 44 )
	{
		EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
		goto MEMFIN_4 ;
	}
	//	サンプリングしたデータをファイルに書きだす
	if ( (wrtsize = fwrite( pdata, 1, pcmsz, pFile)) != pcmsz )
	{
		EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
	}

	//	後処理
MEMFIN_4:;
	fclose( pFile ) ;
MEMFIN_3:;
	free( pdata ) ;
MEMFIN_2:;
	free( pctrl ) ;
MEMFIN_1:;
	free( pring ) ;

	return ;
}

//	同期関数
//	注意：この同期関数のアドレスが０の場合、NULL指定と誤判断されるので、
//	      この関数の位置には注意が必要
void	syncfunc()
{
	n_count ++ ;
	return ;
}

//	直接ファイルへ１６ＫＢ毎にサンプリングデータを書き込む
void	recfile()
{
	FILE	*pFile ;				//	ファイルポインタ
	char	*pring ;				//	リングバッファポインタ
	struct	buf_ctrl
	{
		int		sum ;
		int		apply_loc ;
		int		system_loc ;
		struct
		{
			char	*ptr ;
			char	reserve[8] ;
		}	buftbl[2] ;
	}	*pctrl ;					//	リングバッファ管理テーブルポインタ
	int		cap ;					//	性能情報
	int		ch, x, y ;				//	マウス用変数
	int		pcmstart ;				//	PCMデータ相対開始位置
	int		wrtsz ;					//	書き込むデータサイズ
	int		wrtsize ;				//	書き込まれたデータサイズ
	int		remain ;				//	残りサンプリングデータサイズ
	int		fl = 0 ;				//	無限ループフラグ
	int		status ;				//	ステータス
	int		nextloc ;				//	次回のアプリケーション位置

	EGB_sjisString( egb_wk, (char *)&m[6] ) ;		//	処理内容表示
	WAV_getCapability( &cap, freq ) ; 
	if ( !( cap & WAV_CAP_16PCM_EXIST ) )
	{
		EGB_sjisString( egb_wk, (char *)&m[10] ) ;	//	エラーメッセージ通知
		return ;
	}

	//	リングバッファとリングバッファ管理テーブルの領域の確保を行う
	//	リングバッファ数は１６個
	if ( (pring = malloc( (16+1)*4096 )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
		return ;
	}
	if ( (pctrl = (struct buf_ctrl *)malloc( (16+1)*12 )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
		goto FILEFIN_1 ;
	}

	//	リングバッファ管理テーブルの作成
	pctrl->sum = 16 ;						//	バッファ総数の設定
	pctrl->apply_loc = 0 ;					//	アプリケーション位置のクリア
	WAV_makeTable( pring, (char *)pctrl ) ;

	//	サンプリングデータを保存するファイルを作成する
	if ( (pFile = fopen( filenm, "wb" )) == NULL )	//	ファイル作成
	{
		EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
		goto FILEFIN_2 ;
	}
	//	WAVEファイルの情報作成しファイルに書きだす
	//	（リングバッファ領域を作業領域として使用する）
	WAV_setWaveInfo( pring, freq, bit16, kind, pcmsz16, &pcmstart ) ;
	if ( (wrtsize = fwrite( pring, 1, 44, pFile)) != 44 )
	{
		EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
		goto FILEFIN_3 ;
	}

	//	録音前準備
	remain = pcmsz16 ;				//	残りサンプリングデータサイズ設定
	n_count = 0 ;					//	カウンタ値のクリア
	nextloc = 4 ;					//	次回のアプリケーション位置の設定
	wrtsz = 4*4096 ;				//	一回当たりのファイル書き込みサイズ
	WAV_recPrepare( freq, bit16, kind, NULL, syncfunc ) ;

	//	録音のメッセージを表示し、マウスのクリックを待つ
	do
	{
		MOS_rdpos(&ch, &x, &y) ;
	} while( ch != 0 ) ;						//	ボタンが解放されるのを待つ
	EGB_sjisString( egb_wk, (char *)&m[7] ) ;
	while( ch != 1 ) MOS_rdpos(&ch, &x, &y) ;	//	マウスの左ボタンが押される
												//	まで待つ
	//	録音開始
	EGB_sjisString( egb_wk, (char *)&m[8] ) ;	//	録音開始のメッセージ
	WAV_rec( WAV_REC_IMMEDIATE, 0, pcmsz16 ) ;	//	録音を行う

	//	指定サイズ録音されるまで繰り返す
	while( fl == 0 )
	{
		WAV_getStatus( &status ) ;
		if ( !( status & WAV_ST_REC_PROCESS ) )	//	録音処理が終了していれば
			fl++ ;								//	flに1設定

		//	リングバッファ４個分サンプリングされていればファイルにデータを書きだす
		if ( n_count >= 4 )
		{
			n_count -= 4 ;						//	カウンタ値の再設定
												//	書き込むサイズの設定
			if ( remain < wrtsz ) wrtsz = remain ;
												//	ファイルに書きだす
			if ( (wrtsize = fwrite( pctrl->buftbl[pctrl->apply_loc].ptr,
									1, wrtsz, pFile)) != wrtsz )
			{
				EGB_sjisString( egb_wk, (char *)&m[11] ) ;
				goto FILEFIN_3 ;
			}

			//	次回のアプリケーション位置を設定し、次々回のアプリケーション位置を算出
			pctrl->apply_loc = nextloc ;
			if ( (nextloc += 4) == pctrl->sum )	nextloc = 0 ;

			//	サンプリングデータの残りサイズの算出
			remain -= wrtsize ;
		}
		//	録音処理が終了していれば、残りのデータを全てファイルに書きだす
		else if ( ( fl != 0 ) && ( remain > 0 ) )
		{
			if ( (wrtsize = fwrite( pctrl->buftbl[pctrl->apply_loc].ptr,
									1, remain, pFile)) != remain )
			{
				EGB_sjisString( egb_wk, (char *)&m[11] ) ;
				goto FILEFIN_3 ;
			}
		}

	} ;							//	while文の終了

	EGB_sjisString( egb_wk, (char *)&m[9] ) ;	//	録音終了のメッセージ

	//	後処理
FILEFIN_3 :;
	fclose( pFile ) ;
FILEFIN_2 :;
	free( pctrl ) ;
FILEFIN_1 :;
	free( pring ) ;

	return ;
}

//	メインルーチン
void	main()
{
	int	mute ;								//	ミュート状態
	int	ch, x, y ;							//	マウス用変数

	SND_init( snd_wk ) ;					//	サウンドドライバの初期化
	WAV_init() ;							//	WAVEライブラリの初期化
	EGB_init( egb_wk, EgbWorkSize ) ;		//	グラフィック初期化
	MOS_start( mos_wk, MosWorkSize ) ;		//	マウス動作開始の設定

	SND_get_elevol_mute( &mute ) ;			//	現在のミュート状態の退避
	SND_elevol_mute( 0 ) ;					//	電子ボリュームを全てミュート
	SND_elevol_set( 2, 127, 127 ) ;			//	マイク入力を最大値設定

	//	録音のメッセージを表示し、マウスのクリックを待つ
	do
	{
		MOS_rdpos(&ch, &x, &y) ;
	} while( ch != 0 ) ;						//	ボタンが解放されるのを待つ
	EGB_sjisString( egb_wk, (char *)&m[0] ) ;
	EGB_sjisString( egb_wk, (char *)&m[1] ) ;
	EGB_sjisString( egb_wk, (char *)&m[2] ) ;
	EGB_sjisString( egb_wk, (char *)&m[3] ) ;
	EGB_sjisString( egb_wk, (char *)&m[4] ) ;
	while( ch == 0 ) MOS_rdpos(&ch, &x, &y) ;	//	マウスのボタンが押される
												//	まで待つ
	switch( ch )
	{
		case 1:									//	左ボタンが押された
			recmem() ;							//	メモリ
			break ;

		case 2:									//	右ボタンが押された
			recfile() ;							//	ファイル
			break ;

		default:
			break ;
	}

	SND_elevol_mute( mute ) ;				//	ミュート状態を復元

	MOS_end() ;								//	マウス動作終了の設定
	WAV_end() ;								//	WAVEライブラリの終了
	SND_end() ;								//	サウンド動作終了の設定
}
