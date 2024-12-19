//*****************************************************************************
//
// ＷＡＶＥライブラリサンプルプログラム（再生編）
//
// 機  能   :WAVEファイルを再生します
//           再生ファイル名：MICREC.WAV
//
// 処理説明 :一旦メモリ上に再生データを読み込んでから再生するか
//           直接ファイルから１６ＫＢ毎に再生データを読み込み再生する
//
// 使用方法 :ｺﾏﾝﾄﾞﾓｰﾄﾞ上から run386 playwav
//
// 注　意   :(1)MICREC.WAV ﾌｧｲﾙは RECWAV.C をｺﾝﾊﾟｲﾙ／実行して作成します
//           (2)run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、
//              ｶｰｿﾙが表示されません
//
//*****************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\egb.h"
#include "..\include\mos.h"
#include "..\include\snd.h"
#include "..\include\wav.h"

//	メッセージ文の定義
typedef struct	tagMSG_DATA
{
	short			x ;
	short			y ;
	unsigned short	len ;
	char			msg[80] ;
} MSG_DATA ;

MSG_DATA	m[14] =
{
	160,  80, 32, "ファイルMICREC.WAVを再生します．",
	160, 112, 44, "マウスのボタンで再生時の処理を選択できます．",
	176, 144, 38, "左ボタン：一旦メモリに読み込み再生する",
	176, 176, 34, "右ボタン：直接ファイルから再生する",
	160, 208, 32, "再生時の処理を選択してください．",
	160, 240, 20, "再生時の処理：メモリ",
	160, 240, 22, "再生時の処理：ファイル",
	160, 272, 36, "マウスの左ボタンを押すと開始します．",
	160, 304, 20, "再生を開始しました．",
	160, 336, 20, "再生が終了しました．",
	160, 368, 34, "ファイルのオープンに失敗しました．",
	160, 368, 32, "ファイルのシークに失敗しました．",
	160, 368, 34, "ファイルの読み込みに失敗しました．",
	160, 368, 30, "作業領域の確保に失敗しました．"
} ;

//	グローバル変数の定義
int		n_count ;							//	同期関数の呼び出し回数
char	egb_wk[EgbWorkSize] ;				//	EGB関数用作業領域
char	snd_wk[SndWorkSize] ;				//	SND関数用作業領域
char	mos_wk[MosWorkSize] ;				//	MOS関数用作業領域
char	*filenm = "MICREC.WAV" ;			//	作成するファイル名

//	メモリ上からの再生
void	playmem()
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
	int		ret ;					//	戻り値
	int		status ;				//	ステータス
	int		ch, x, y ;				//	マウス用変数
	int		freq ;					//	サンプリング周波数
	int		bitno ;					//	サンプリングビット数
	int		kind ;					//	データ種別
	int		pcmsz ;					//	PCMデータサイズ
	int		pcmstart ;				//	PCMデータ相対開始位置
	int		rdsz ;					//	読み込むデータサイズ
	int		rdsize ;				//	読み込まれたデータサイズ

	//	再生ファイルをオープンする
	if ( (pFile = fopen( filenm, "rb" )) == NULL )	//	ファイルオープン
	{
		EGB_sjisString( egb_wk, (char *)&m[10] ) ;	//	エラーメッセージ通知
		return ;
	}

	//	リングバッファとリングバッファ管理テーブルの領域の確保を行う
	//	リングバッファ数は２個
	if ( (pring = malloc( (2+1)*4096 )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[13] ) ;	//	エラーメッセージ通知
		goto MEMFIN_1 ;
	}
	if ( (pctrl = (struct buf_ctrl *)malloc( (2+1)*12 )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[13] ) ;	//	エラーメッセージ通知
		goto MEMFIN_2 ;
	}

	//	リングバッファ管理テーブルの作成
	pctrl->sum = 2 ;						//	バッファ総数の設定
	pctrl->apply_loc = 0 ;					//	アプリケーション位置のクリア
	WAV_makeTable( pring, (char *)pctrl ) ;

	//	リングバッファを使用してファイルの情報を取得する
	rdsz = 0 ;
	do
	{
		rdsz += 50 ;
		//	ファイルポインタをWAVEデータの先頭に位置づける
		ret = fseek( pFile, 0, SEEK_SET ) ;
		if ( ret != 0 )
		{
			EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
			goto MEMFIN_3 ;
		}
		rdsize = fread( pring, 1, rdsz, pFile ) ;
		if ( rdsize != rdsz )
		{
			EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
			goto MEMFIN_3 ;
		}
		ret = WAV_getWaveInfo( pring, rdsz, &freq, &bitno, &kind,
													 &pcmsz, &pcmstart) ;
	} while( ret == 23 ) ;

	//	PCMデータを格納する領域の確保
	if ( (pdata = malloc( pcmsz )) == NULL )
	{
		EGB_sjisString( egb_wk, (char *)&m[13] ) ;	//	エラーメッセージ通知
		goto MEMFIN_3 ;
	}

	//	PCMデータを格納する
	//	ファイルポインタをPCMデータの先頭に位置づける
	ret = fseek( pFile, pcmstart, SEEK_SET ) ;
	if ( ret != 0 )
	{
		EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
		goto MEMFIN_4 ;
	}
	//	PCMデータを読み込み、格納する
	rdsize = fread( pdata, 1, pcmsz, pFile ) ;
	if ( rdsize != pcmsz )
	{
		EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
		goto MEMFIN_4 ;
	}

	//	再生前準備
	WAV_playPrepare( freq, bitno, kind, pdata, (void (*)())NULL ) ;

	//	再生のメッセージを表示し、マウスのクリックを待つ
	do
	{
		MOS_rdpos(&ch, &x, &y) ;
	} while( ch != 0 ) ;						//	ボタンが解放されるのを待つ
	EGB_sjisString( egb_wk, (char *)&m[5] ) ;
	EGB_sjisString( egb_wk, (char *)&m[7] ) ;
	while( ch != 1 ) MOS_rdpos(&ch, &x, &y) ;	//	マウスの左ボタンが押される
												//	まで待つ

	//	再生開始
	EGB_sjisString( egb_wk, (char *)&m[8] ) ;	//	再生開始のメッセージ
	WAV_play( pcmsz ) ;							//	再生開始
	do											//	再生が開始するまで待つ
	{
		WAV_getStatus( &status ) ;
	}	while( !( status & WAV_ST_PLAY_PLAYING ) ) ;
	do											//	再生が終了するまで待つ
	{
		WAV_getStatus( &status ) ;
	}	while( status & WAV_ST_PLAY_PROCESS ) ;

	EGB_sjisString( egb_wk, (char *)&m[9] ) ;	//	再生終了のメッセージ

	//	後処理
MEMFIN_4:;
	free( pdata ) ;
MEMFIN_3:;
	free( pctrl ) ;
MEMFIN_2:;
	free( pring ) ;
MEMFIN_1:;
	fclose( pFile ) ;

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

//	直接ファイルから再生
void	playfile()
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
	int		ret ;					//	戻り値
	int		ch, x, y ;				//	マウス用変数
	int		remain ;				//	残りサンプリングデータサイズ
	int		fl = 0 ;				//	無限ループフラグ
	int		status ;				//	ステータス
	int		nextloc ;				//	次回のアプリケーション位置
	int		freq ;					//	サンプリング周波数
	int		bitno ;					//	サンプリングビット数
	int		kind ;					//	データ種別
	int		pcmsz ;					//	PCMデータサイズ
	int		pcmstart ;				//	PCMデータ相対開始位置
	int		rdsz ;					//	読み込むデータサイズ
	int		rdsize ;				//	読み込まれたデータサイズ

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

	//	再生ファイルをオープンする
	if ( (pFile = fopen( filenm, "rb" )) == NULL )	//	ファイルオープン
	{
		EGB_sjisString( egb_wk, (char *)&m[10] ) ;	//	エラーメッセージ通知
		goto FILEFIN_2 ;
	}

	//	リングバッファを使用してファイルの情報を取得する
	rdsz = 0 ;
	do
	{
		rdsz += 50 ;
		//	ファイルポインタをWAVEデータの先頭に位置づける
		ret = fseek( pFile, 0, SEEK_SET ) ;
		if ( ret != 0 )
		{
			EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
			goto FILEFIN_3 ;
		}
		rdsize = fread( pring, 1, rdsz, pFile ) ;
		if ( rdsize != rdsz )
		{
			EGB_sjisString( egb_wk, (char *)&m[12] ) ;	//	エラーメッセージ通知
			goto FILEFIN_3 ;
		}
		ret = WAV_getWaveInfo( pring, rdsz, &freq, &bitno, &kind,
													 &pcmsz, &pcmstart) ;
	} while( ret == 23 ) ;

	//	ファイルポインタをPCMデータの先頭に位置づける
	ret = fseek( pFile, pcmstart, SEEK_SET ) ;
	if ( ret != 0 )
	{
		EGB_sjisString( egb_wk, (char *)&m[11] ) ;	//	エラーメッセージ通知
		goto FILEFIN_3 ;
	}

	//	再生データをリングバッファ分先読みしておく
	rdsz = 16*4096 ;
	if ( rdsz > pcmsz )	rdsz = pcmsz ;
	if ( (rdsize = fread( pctrl->buftbl[pctrl->apply_loc].ptr,
								1, rdsz, pFile )) != rdsz )
	{
		EGB_sjisString( egb_wk, (char *)&m[12] ) ;
		goto FILEFIN_3 ;
	}

	//	再生前準備
	remain = pcmsz - rdsize ;		//	残りPCMデータサイズ設定
	n_count = 0 ;					//	カウンタ値のクリア
	nextloc = 4 ;					//	次回のアプリケーション位置の設定
	rdsz = 4*4096 ;					//	一回当たりのファイル読み込みサイズ
	WAV_playPrepare( freq, bitno, kind, NULL, syncfunc ) ;

	//	再生のメッセージを表示し、マウスのクリックを待つ
	do
	{
		MOS_rdpos(&ch, &x, &y) ;
	} while( ch != 0 ) ;						//	ボタンが解放されるのを待つ
	EGB_sjisString( egb_wk, (char *)&m[6] ) ;	//	処理内容表示
	EGB_sjisString( egb_wk, (char *)&m[7] ) ;
	while( ch != 1 ) MOS_rdpos(&ch, &x, &y) ;	//	マウスの左ボタンが押される
												//	まで待つ
	//	再生開始
	EGB_sjisString( egb_wk, (char *)&m[8] ) ;	//	再生開始のメッセージ
	WAV_play( pcmsz ) ;	//	再生を行う

	//	指定サイズ再生されるまで繰り返す
	while( fl == 0 )
	{
		WAV_getStatus( &status ) ;
		if ( !( status & WAV_ST_PLAY_PROCESS ) )	//	再生処理が終了していれば
			fl++ ;									//	flに1設定

		//	リングバッファ４個分再生されていればファイルからデータを読みだす
		if ( ( remain > 0 ) && ( n_count >= 4 ) )
		{
			n_count -= 4 ;						//	カウンタ値の再設定
												//	読みだすサイズの設定
			if ( remain < rdsz ) rdsz = remain ;
												//	ファイルから読みだす
			if ( (rdsize = fread( pctrl->buftbl[pctrl->apply_loc].ptr,
									1, rdsz, pFile )) != rdsz )
			{
				EGB_sjisString( egb_wk, (char *)&m[12] ) ;
				goto FILEFIN_3 ;
			}

			//	次回のアプリケーション位置を設定し、次々回のアプリケーション位置を算出
			pctrl->apply_loc = nextloc ;
			if ( (nextloc += 4) == pctrl->sum )	nextloc = 0 ;

			//	サンプリングデータの残りサイズの算出
			remain -= rdsize ;
		}

	} ;							//	while文の終了

	EGB_sjisString( egb_wk, (char *)&m[9] ) ;	//	再生終了のメッセージ

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
	int	ch, x, y ;							//	マウス用変数

	SND_init( snd_wk ) ;					//	サウンドドライバの初期化
	WAV_init() ;							//	WAVEライブラリの初期化
	EGB_init( egb_wk, EgbWorkSize ) ;		//	グラフィック初期化
	MOS_start( mos_wk, MosWorkSize ) ;		//	マウス動作開始の設定

	WAV_setVolume( 127, 127 ) ;				//	再生音量の設定

	//	再生のメッセージを表示し、マウスのクリックを待つ
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
			playmem() ;							//	メモリ
			break ;

		case 2:									//	右ボタンが押された
			playfile() ;						//	ファイル
			break ;

		default:
			break ;
	}

	MOS_end() ;								//	マウス動作終了の設定
	WAV_end() ;								//	WAVEライブラリの終了
	SND_end() ;								//	サウンド動作終了の設定
}
