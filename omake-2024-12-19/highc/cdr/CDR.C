/****************************************************************************
 *																			*
 *  CD-ROM LIBRARY SAMPLE PROGRAM											*
 *																			*
 *        TITLE    : ＣＤ演奏   											*
 *																			*
 *        使用方法 : ｺﾏﾝﾄﾞﾓｰﾄﾞ上から run386 -nocrt cdr 	   					*
 *  							                 							*
 *			    	 CD-ROMの内容から音楽演奏データを選んで演奏します  		*
 *					 [→ｷｰ] １曲進む										*
 *					 [←ｷｰ] １曲戻る										*
 *  							                 							*
 *                   [終了] ESCｷｰを押す             	   					*
 *																			*
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "..\include\snd.h"
#include "..\include\cdrfrb.h"
#include "..\include\fmcfrb.h"

extern	void SubOne(char, char, char, char *, char *, char *);
extern	void IfKey_Next(void);

struct	TIMEADRS	dsktim;
struct	TIMEADRS	trktim[99];
struct	TIMEADRS	starttime;
struct	TIMEADRS	endtime;

int		i, ret, driveno = 0x00;
int		dsktype = 0, sttrk = 0, endtrk = 0;
unsigned int	song_num;

void main()
{
	SND_elevol_init();			/*	サウンドライブラリを用いて */
	SND_elevol_set(1,127,127);	/*	ボリュームを調節します */
	SND_elevol_mute(0x0030);

	printf("ディスク情報を読み取ります\n");
retry:;
	ret = cdr_cdinfo( driveno, &dsktype, &sttrk, &endtrk,
									trktim,&dsktim );
	if((ret & 0x20)!= 0) {
		printf("ハードエラーです!!ディスク情報が読めませんでした｡\n");
		exit(1);
	}
	else if((ret & 0x80) != 0) {
		printf("メディア交換がされたので､リトライします。\n");
		goto retry;
	}

	printf("先頭トラック(曲)番号 = %d\n",sttrk);
	printf("最終トラック(曲)番号 = %d\n",endtrk);
	printf("ディスク内演奏時間(分) = %d\n",dsktim.min);
	printf("ディスク内演奏時間(秒) = %d\n",dsktim.sec);
	printf("ディスク内演奏時間(フレーム) = %d\n",dsktim.frame);

	for(i = sttrk-1 ; i < endtrk ; i++){	/* トラック情報を表示します */

		if(((trktim[i].min) & 0x80) != 0){
			printf("☆%2d曲目 データ     ",i);
		}else{
			printf("★%2d曲目 オーディオ ",i);
		}
		printf("%3d:%3d:%3d ★\n",trktim[i].min, trktim[i].sec, trktim[i].frame);

	}

	printf("\n先頭トラックから最終トラックまで演奏します｡\n\n");
	printf("途中で\x22→\x22キーが押されると一曲進み､\x22←\x22キーが押されると一曲戻ります｡\n");
	printf("また、ESCキーを押すとプログラムを終了します。\n");
	for(song_num = sttrk-1 ; song_num < endtrk ; song_num++){

		if((trktim[song_num].min & 0x80) != 0){
			printf("%d曲目はデータです!飛ばします\n",song_num+1);
		}else{
			printf("%d曲目は音楽データです!演奏します\n",song_num+1);

			starttime.min = trktim[song_num].min;
			starttime.sec = trktim[song_num].sec;
			starttime.frame = trktim[song_num].frame;

			if(song_num != endtrk - 1){		/* 最終トラック以外の場合 */

				SubOne(trktim[song_num+1].min, trktim[song_num+1].sec,
						trktim[song_num+1].frame, &endtime.min, &endtime.sec,
						&endtime.frame);

			}else {							/* 最終トラックの場合 */

				SubOne(dsktim.min, dsktim.sec, dsktim.frame,
						&endtime.min, &endtime.sec, &endtime.frame);

			}

			ret = cdr_mtplay( driveno, &starttime, &endtime);

			IfKey_Next();

		}
	}
}

void SubOne(char a, char b, char c, char *d, char *e, char *f)
								/*	演奏終了時間を計算します。 */
{
	int		i;
	int		maxtruck = 75;
	int		maxsec = 60;

	i = a*maxsec*maxtruck+b*maxtruck+c-1;
	*d = i/(maxsec*maxtruck);
	*e = (i%(maxsec*maxtruck))/maxtruck;
	*f = (i%(maxsec*maxtruck))%maxtruck;
}

void IfKey_Next(void)	/* 押されたキーを読み取り,キーに応じた処理を行う */
{
	int		sw = 1;
	int		musno;
	int		status = 1;
	unsigned int	moji, encode;
	struct	TIMEADRS	rdsktim;
	struct	TIMEADRS	rdtim;

	while(status != 0){

		ret = cdr_mphase(driveno, &status, &musno,&rdtim,
													&rdsktim );

		if(((moji = KYB_read(sw,&encode)) & 0xff00) != 0xff00) {
		
			switch(moji){
			case 0x1c	: printf("→\n");
						  cdr_mstop( driveno );
						  break;
			case 0x1d	: printf("←\n");
						  if(song_num != sttrk-1){
								song_num -= 2;
							}else{
								song_num -= 1;
							}
						  cdr_mstop( driveno );
						  break;
			case 0x1b	: printf("終了\n");
						  cdr_mstop( driveno );
						  exit(0);
			}
		}

	}

}
