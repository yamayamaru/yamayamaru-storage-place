/******************************************************************************
 *                                                                            *
 *  EUP DRIVER SAMPLE PROGRAM                                                 *
 *                                                                            *
 *        TITLE     : EUP形式ファイルの演奏                                   *
 *                                                                            *
 *        使用方法  : [起動] ｺﾏﾝﾄﾞﾓｰﾄﾞ上から run386 eupplay <eup_file.eup>    *
 *                    [終了] ﾏｳｽ左右ボタンを同時ｸﾘｯｸまたは演奏の終了          *
 *                                                                            *
 *        注意      : run386 実行時 -nocrt ｵﾌﾟｼｮﾝを指定すると表示画面が残り、 *
 *                    ｶｰｿﾙが表示されません                                    *
 *                                                                            *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\snd.h"
#include "..\include\mos.h"

char swork[16384];							/*　サウンドドライバ作業領域	*/
char mwork[MosWorkSize];					/*　マウスドライバ作業領域		*/
char *eup_buf;								/*  演奏データ用バッファ		*/
int flen,signature;							/*  ファイル長,拍子値			*/
FILE *fp;									/*	ストリーム用変数			*/

int eup_load(path)
char *path;
{
	int i,j,size,tempo;
	char buffer[128];
	char bankpath[128];
	char fm_bankname[8];
	char pcm_bankname[8];

	fseek(fp,0,SEEK_END);					/*	ﾌｧｲﾙの最後にﾎﾟｲﾝﾀを移動		*/
	flen = (unsigned int)ftell(fp);			/*  EUPﾌｧｲﾙ全体のﾊﾞｲﾄ数を取得	*/
	fseek(fp,0,SEEK_SET);					/*	ﾌｧｲﾙの先頭にﾎﾟｲﾝﾀを戻す		*/
	if (flen <= 2048+4+1+1)					/*  全体の大きさはﾍｯﾀﾞ長以下か?	*/
		return(1);
											/*  以下全32ﾄﾗｯｸについて設定	*/
	fseek(fp,852,SEEK_SET);
	fread(buffer,1,32,fp);
	for(i=0 ; i<32 ;i++)
		SND_eup_mute_set(i,(int)buffer[i]);	/*  ミュートの設定				*/
	fread(buffer,1,32,fp);
	for(i=0 ; i<32 ;i++)
		SND_eup_port_set(i,(int)buffer[i]);	/*  出力ポートの設定			*/
	fread(buffer,1,32,fp);
	for(i=0 ; i<32 ;i++)
		SND_eup_midi_ch_set(i,(int)buffer[i]);
											/*  出力MIDIチャンネルの設定	*/
	fread(buffer,1,32,fp);
	for(i=0 ; i<32 ;i++)
		SND_eup_bias_set(i,(int)buffer[i]);	/*  キーバイアスの設定			*/
	fread(buffer,1,32,fp);
	for(i=0 ; i<32 ;i++)
		SND_eup_transpose_set(i,(int)buffer[i]);
											/*  トランスポーズの設定		*/
	fseek(fp,1748,SEEK_SET);
	fread(buffer,1,6,fp);
	for(i = 0;i < 6;i++)
		SND_midi_ch_assign(i,(int)buffer[i]);
											/*  FM音源のMIDIチャンネル設定	*/
	fread(buffer,1,8,fp);
	for(i = 0;i < 8;i++)
		SND_midi_ch_assign(i+64,(int)buffer[i]);
											/*  PCM音源のMIDIチャンネル設定 */

	SND_pcm_mode_set(0);					/*	PCM音声ﾓｰﾄﾞﾁｬﾝﾈﾙ数 = 0		*/
	
	for(i = 0;path[i] != '\0';i++)			/*	EUPﾌｧｲﾙのﾊﾟｽ名を得る		*/
		bankpath[i] = path[i];
	while((path[i] != '\\') && (path[i] != ':') && (i >= 0))i--;
	i++;

	fread(&bankpath[i],1,8,fp);				/*	FM音源のﾃﾞｰﾀﾌｧｲﾙ名を読み込む*/
	if(bankpath[i] != '\0'){
		for(j= 0;bankpath[i+j] != '\0';j++);
		bankpath[i+j  ] = '.';
		bankpath[i+j+1] = 'f';
		bankpath[i+j+2] = 'm';
		bankpath[i+j+3] = 'b';
		bankpath[i+j+4] = '\0';
		SND_fm_bank_load(bankpath,fm_bankname);
							 				/*	FM音源ﾊﾞﾝｸﾃﾞｰﾀを読み込む	*/
		}

	fread(&bankpath[i],1,8,fp);				/*	PCMﾌｧｲﾙ名をﾊﾞｯﾌｧに読み込む	*/
	if(bankpath[i] != '\0'){
		for(j= 0;bankpath[i+j] != '\0';j++);
		bankpath[i+j  ] = '.';
		bankpath[i+j+1] = 'p';
		bankpath[i+j+2] = 'm';
		bankpath[i+j+3] = 'b';
		bankpath[i+j+4] = '\0';
		SND_pcm_bank_load(bankpath,pcm_bankname);
											/*	PCM音源ﾊﾞﾝｸﾃﾞｰﾀを読み込む	*/
		}

	fseek(fp,2048,SEEK_SET);				/*  演奏ﾃﾞ-ﾀの先頭情報部に移動	*/
	fread(&size,4,1,fp);					/*	演奏ﾃﾞｰﾀの大きさを読み込む	*/
	fread(buffer,1,2,fp);					/*	2つ分のﾃﾞｰﾀを読み込む		*/
	signature = (int)buffer[0];
	tempo = (int)buffer[1];
	SND_eup_tempo_set(tempo);				/*  曲始めのテンポをセット		*/

	flen -= 2048+4+1+1;                 	/*  ファイル長からヘッダ長を除く*/
	if ((eup_buf = malloc(flen)) == 0)  	/*  演奏ﾃﾞｰﾀの読み込み領域を確保*/
		return(1);
	fread(eup_buf,1,flen,fp);           	/*  演奏データを読み込む		*/
	return(0);
}

int main(int argc, char *argv[])
{
	int ret,tr,x,y;

	if (argc < 2){
		printf("Sample EUP-Player  (C)FUJITSU LIMITED\n");
		printf("usage:RUN386 EUPPLAY <EUP filename>\n");
		return(1);
		}

	SND_init(swork);						/*  サウンドドライバの初期化	*/
	MOS_start(mwork,MosWorkSize);			/*  マウス処理を開始			*/
	SND_elevol_set(0,127,127);				/*  LINE-INのボリューム設定		*/
	SND_elevol_mute(0x0f);					/*  CD,MIC,MODEMをミュート		*/

	SND_eup_init();							/*  EUPHONYドライバの初期化		*/
	SND_rs_midi_init();						/*	RS-MIDIドライバの初期化		*/
	if((fp = fopen(argv[1],"rb")) == NULL){	/*  EUPファイルをオープン		*/
		ret = 1;
		goto err_end;
		}
	ret = eup_load((char *)argv[1]);		/*  EUPﾌｧｲﾙ読み込み&ﾍｯﾀﾞ処理	*/
	fclose(fp);								/*  EUPファイルをクローズ		*/
	if (!ret){
		SND_eup_play_start(eup_buf,flen,signature);
											/*  EUPファイル演奏				*/
		do {
			MOS_rdpos(&tr,&x,&y);			/*  マウス状態を読み込む		*/
			} while(SND_eup_stat_flag() && (tr != 0x03));
											/*	演奏状態、マウスをチェック  */

		SND_eup_play_stop();				/*  演奏の停止					*/
		free(eup_buf);						/*  演奏バッファの領域の開放	*/
		}
err_end:
	SND_rs_midi_end();						/*	RS-MIDIドライバの終了		*/
	SND_eup_end();							/*  EUPHONYドライバの終了		*/
	MOS_end();								/*  マウスドライバの終了		*/
	SND_end();								/*  サウンドドライバの終了		*/
	return(ret);
}
