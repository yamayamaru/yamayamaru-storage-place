
#pragma off(Floating_point);

/*****************************************************************************

                    Copyright(C) 1995 FUJITSU LIMITED

************************* MIDIﾏﾈｰｼﾞｬｰｻﾝﾌﾟﾙﾌﾟﾛｸﾞﾗﾑ ****************************

【概要】
  このﾌﾟﾛｸﾞﾗﾑはMIDIﾏﾈｰｼﾞｬをコンソール上でﾃﾞﾊﾞｯｸﾞするためにつくられたﾃｽﾄﾌﾟﾛｸﾞﾗﾑ
です。ただし、Towns OSに標準でｻﾎﾟｰﾄされている｢ｺﾏﾝﾄﾞﾓｰﾄﾞ｣では動作しません。
これはSND_init()を行うﾌﾟﾛｸﾞﾗﾑ全般に言える話ですが、｢ｺﾏﾝﾄﾞﾓｰﾄﾞ｣ではFM音源の割り
込みを使用しているため、SND_init()を発行した瞬間に画面表示ができなくなってしま
うためです。
　この様なﾌﾟﾛｸﾞﾗﾑは添付の｢CONSBIOS.EXE｣等のV-SYNC割り込みを使用したｺﾝｿｰﾙ上で動
かさなければなりません。
  このﾌﾟﾛｸﾞﾗﾑでは、標準MIDIﾌｧｲﾙ(Format0のみ)やEUPﾌｧｲﾙを読み込んで演奏を行うこと
ができます。その際にSMPTE同期やｽﾃｯﾌﾟ演奏などのMIDIﾏﾈｰｼﾞｬの色々な機能を使うこと
ができます。

【使用方法】
 CONSBIOS.EXEを起動し｢RUN386 MIDI {0,1}｣とﾀｲﾌﾟしてください。
引数はRS-MIDIのﾓｰﾄﾞを示します。
　0なら外部ｸﾛｯｸを持ったRS-MIDIｱﾀﾞﾌﾟﾀ、1ならSC-55MK2などのﾊﾟｿｺﾝ直結ﾀｲﾌﾟの音源の
38400bpsﾓｰﾄﾞに初期化します。指定がない場合はRS-MIDIの初期化は行われません。
　起動後は｢?｣ｷｰにてﾍﾙﾌﾟを表示します。

【制限事項】
  ｢CONSBIOS.EXE｣はMS-DOS V6.xベースのTownsOS上では正常動作しませんので、MS-DOS
V3.xベースのTownsOS上からご使用ください。

******************************************************************************/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"..\include\snd.h"
#include	"..\include\midiman.h"
#include	"..\include\rsmidi.h"

#define	MAXPLAYNOTE	256			/* 最大同時発音数			*/
#define	MAXTRACK	100			/* 最大ﾄﾗｯｸ数				*/
#define	PBSIZE		0x60000		/* 演奏ﾊﾞｯﾌｧｻｲｽﾞ			*/
#define	LOCALSTACKSIZE	4096	/* 割り込み用ﾛｰｶﾙｽﾀｯｸｻｲｽﾞ	*/

/* ｴﾗｰ番号 */
#define	OUTMEM		(0x01)
#define	ENDDETECT	(0x02)
#define	FORMATERR	(0x03)
#define	CANTOPEN	(0x04)
#define	CANTREAD	(0x05)

/*	clock resolusion	*/
#define RESO96	(0x60)
#define RESO480	(0x1e0)

extern	char key_check();

/* EUPﾌｧｲﾙﾍｯﾀﾞ構造体 */
typedef	struct	{
	char	dummy1[852];
	char	trk_mute[32];
	char	trk_port[32];
	char	trk_midi_ch[32];
	char	trk_key_bias[32];
	char	trk_transpose[32];
	char	trk_play_filter[7][32];
	char	instruments_name[128][4];
	char	fm_midi_ch[6];
	char	pcm_midi_ch[8];
	char	fm_file_name[8];
	char	pcm_file_name[8];
	char	reserve[260];
	char	appli_name[8];
	char	version[2];
}	EUPHEAD;

int	rec_coming;		/* 録音ｺｰﾙﾊﾞｯｸ確認ｶｳﾝﾀｰ		*/
int	idle_coming;	/* ｱｲﾄﾞﾙｺｰﾙﾊﾞｯｸ確認ｶｳﾝﾀｰ	*/
int	event_coming;	/* ｲﾍﾞﾝﾄｺｰﾙﾊﾞｯｸ確認ｶｳﾝﾀｰ	*/
unsigned char	*play_buffer;	/* 演奏ﾊﾞｯﾌｧ			*/
unsigned char	*work_buffer;	/* ﾌｧｲﾙ読み込みﾊﾞｯﾌｧ	*/
unsigned char	gs_init[] = {0,0,0,0,11,0,0,0,0xf0,0x41,0x10,0x42,0x12,0x40,
				0x00,0x7f,0x00,0x41,0xf7};	/* GS音源初期化ｺﾏﾝﾄﾞ	*/
unsigned char	stack[LOCALSTACKSIZE];		/* 割り込み用ﾛｰｶﾙｽﾀｯｸ	*/
unsigned char	event_buff[1024];			/* ｲﾍﾞﾝﾄﾊﾞｯﾌｧ			*/
unsigned char	fmt_out_buff[64*8];			/* MIDI出力ﾊﾞｯﾌｧ		*/
unsigned char	fmt_in_buff[128*4];			/* MIDI入力ﾊﾞｯﾌｧ		*/
unsigned char	smpu_buff[256];				/* S-MPUﾊﾞｯﾌｧ			*/
char	sound_work[16384];					/* ｻｳﾝﾄﾞBIOSﾜｰｸｴﾘｱ		*/
NOTEOFFTABLE	noteoff_buff[MAXPLAYNOTE+1];/* ﾉｰﾄｵﾌ管理ﾊﾞｯﾌｧ		*/
TRACKWORK		trackwork_buff[MAXTRACK];	/* ﾄﾗｯｸ管理ﾊﾞｯﾌｧ		*/
SIGN	initial_sign;						/* ﾃﾞﾌｫﾙﾄ拍子値			*/
int		initial_tempo;						/* ﾃﾞﾌｫﾙﾄﾃﾝﾎﾟ値			*/
char	format;								/* EUP/MIDIﾌｫｰﾏｯﾄﾌﾗｸﾞ	*/
ASSIGNFILTER	assign;		/* ｱｻｲﾝﾌｨﾙﾀ				*/
char	ctrl_map[128];		/* ｺﾝﾄﾛｰﾙチェンジﾏｯﾌﾟ	*/
char	prog_map[128];		/* ﾌﾟﾛｸﾞﾗﾑﾁｪﾝｼﾞﾏｯﾌﾟ		*/
char	step_mode_flag;		/* ｽﾃｯﾌﾟ演奏ﾓｰﾄﾞﾌﾗｸﾞ	*/
/*
----------------------------------------------------------------------
演奏位置等の表示及びｷｰ入力のﾁｪｯｸ
*/
char idle_loop(int mode)
{
	int	flags;
	unsigned char *ptr;
	PLACE	place;
	SMPTE	smpte;
	char	c;

	if(mode)
	{
		TMM_getPlayPlace(&flags,&ptr,&place);
		printf("%c%3d:%2d:%3d|",0x0d,
			place.p_meas+1,place.p_beat+1,place.p_tick);
		smpte.tc_type = SMPTE25;
		if(TMM_realtimeToSmpte(&smpte,&place.p_time) == 0)
			printf("%2d:%2d:%2d.%2d|",
				smpte.tc_hr,smpte.tc_min,smpte.tc_sec,smpte.tc_fr);
		printf("%c",(flags & PLAYFLAG) == 0 ? ' ':'p');
		printf("%c",(flags & PAUSEFLAG) == 0 ? ' ':'*');
		printf("%c",(flags & RECFLAG) == 0 ? ' ':'r');
		printf(" rc%x ic%x ec%x",
				rec_coming & 0x0f,idle_coming & 0x0f,event_coming & 0x0f);
	}
	c = key_check();
	if((c >= 'A' ) && (c <= 'Z'))
		c += 0x20;
	return(c);
}

/*
----------------------------------------------------------------------
	ｺﾏﾝﾄﾞの入力およびｴｺｰﾊﾞｯｸ(ESCｷｰによるｷｬﾝｾﾙﾁｪｯｸ)
*/
char command_check(char *cmds)
{
	char key;
	char *ptr;

	while(1)
	{
		key = idle_loop(0);
		if(key == 0)
			continue;
		if(key == 0x1b)
		{
			printf("\nCancel!!\n");
			return 0;
		}
		ptr = cmds;
		while(*ptr != 0)
		{
			if(key == *ptr++)
			{
				printf("%c\n",key);
				return key;
			}
		}
	}
}
/*
----------------------------------------------------------------------
	各種ｺｰﾙﾊﾞｯｸﾙｰﾁﾝ(確認ﾌﾗｸﾞをｲﾝｸﾘﾒﾝﾄするだけでなにもしていない)
*/
void idle_func()
{
	idle_coming++;
}
void rec_func(PORT input,FLAG assign_flag,TRACK track,unsigned char *event)
{
	rec_coming++;
}
void event_func(TRACK track,PORT port,unsigned char *event)
{
	event_coming++;
}
/*
----------------------------------------------------------------------
	SMPTE時間の表示ｻﾌﾞﾙｰﾁﾝ
*/
void print_smpte(SMPTE *smpte)
{
		printf("%2d:%2d:%2d.%2d\n",
			smpte->tc_hr,smpte->tc_min,smpte->tc_sec,smpte->tc_fr);
}
/*
----------------------------------------------------------------------
	ﾌﾟﾛｸﾞﾗﾑｴﾝﾄﾘ

	ｺﾏﾝﾄﾞの引数は
*/
void main(int argc,char **argv)
{
	int	ret;
	int	i;
	REALTIME	r_time;
	MIDIMANCTRL	midiman;
	CFUNCCTRL	c_func;
	SMPTE	smpte;
	int	int_time;
	int	func_handle;
	METRONOME	metro;
	int	rs_mode;

	/* ﾒﾓﾘの確保 */
	if((play_buffer = malloc(PBSIZE*2)) == NULL)
	{
		printf("Out of memory\n");
		exit(0);
	}
	/* ﾌｧｲﾙ読み込みﾊﾞｯﾌｧを設定 */
	work_buffer = play_buffer+PBSIZE;
	/* とりあえずﾀﾞﾐｰﾃﾞｰﾀを設定 */
	format = 0;
	play_buffer[0] = 0xff;
	play_buffer[1] = 0x7f;
	play_buffer[2] = 0x00;
	play_buffer[3] = 0xff;
	play_buffer[4] = 0x2f;
	play_buffer[5] = 0x00;

	/* RS-MIDIの設定 */
	if(argc > 1)
		rs_mode =  atoi(argv[1]);
	else
		rs_mode = -1;
	int_time = 25;				/* ﾒｲﾝｲﾝﾀｰﾊﾞﾙを2.5msに設定	*/
	SND_init(sound_work);		/* ｻｳﾝﾄﾞBIOSを初期化		*/
	SND_elevol_mute(0x0f);		/* 各種ﾐｭｰﾄの解除			*/
	SND_elevol_set(0,127,127);	/* 電子ﾎﾞﾘｭｰﾑ(LINE)の設定	*/

	midiman.mc_interval = 480000*int_time/10;
	midiman.mc_maxnote = MAXPLAYNOTE;
	midiman.mc_maxtrack = MAXTRACK;
	midiman.mc_sbios = (unsigned char *)sound_work;
	midiman.mc_trackwork = trackwork_buff;
	midiman.mc_noteoff = noteoff_buff;
	midiman.mc_filter[0] = ctrl_map;
	midiman.mc_filter[1] = prog_map;
	for(i = 2; i < 16 ; i++)
		midiman.mc_filter[i] = NULL;
	midiman.mc_event = event_buff;
	midiman.mc_eventsize = 1024;
	midiman.mc_fmtout = fmt_out_buff;
	midiman.mc_fmtoutsize = 64*8;
	midiman.mc_fmtin = fmt_in_buff;
	midiman.mc_fmtinsize = 128*4;
	midiman.mc_smpu = smpu_buff;
	midiman.mc_smpusize = 256;
	midiman.mc_stackadr = stack;
	midiman.mc_stacksize = LOCALSTACKSIZE;

	initial_sign = DEFSIGN;
	for(i = 0;i < 128;i++)
		prog_map[i] = ctrl_map[i] = i;
	for(i = 0;i < MAXTRACK;i++)
	{
		trackwork_buff[i].trk_number = i;
		trackwork_buff[i].trk_mute = 0xff;
		trackwork_buff[i].trk_filter = 0xffff;
		trackwork_buff[i].trk_port = 0;
		trackwork_buff[i].trk_midich = 0xff;
		trackwork_buff[i].trk_notemap = -1;
		trackwork_buff[i].trk_ctrlmap = 0;
		trackwork_buff[i].trk_progmap = 1;
		trackwork_buff[i].trk_bias = 0;
		trackwork_buff[i].trk_oct = 0;
		trackwork_buff[i].trk_vol = 0;
		trackwork_buff[i].trk_exp = 0;
	}
	/* MIDIﾏﾈｰｼﾞｬのｵｰﾌﾟﾝ */
	if((ret = TMM_openMidiMan(&midiman)) != NOERR)
	{
		printf("open error %d.\n",ret);
		exit(0);
	}
	/* 内蔵音源の初期化 */
	TMM_initInternalVoice();
	/* RS-MIDIの初期化 */
	if(rs_mode >= 0)
		RSM_Init(rs_mode,0);
	/* ﾒﾄﾛﾉｰﾑの初期化 */
	metro.metro_mode	= 1;
	metro.metro_port	= 0;
	metro.metro_midich	= 9;
	metro.metro_hinote	= 34;
	metro.metro_hivelo	= 120;
	metro.metro_lownote	= 33;
	metro.metro_lowvelo	= 40;
	metro.metro_duration= 240;
	TMM_setMetronome(&metro);
	/* ｺｰﾙﾊﾞｯｸﾙｰﾁﾝの登録 */
	c_func.cfc_rectask = rec_func;
	c_func.cfc_idletask = idle_func;
	c_func.cfc_eventtask = event_func;
	func_handle = TMM_setCFunc(&c_func);
	if(func_handle < 0)
		printf("setfunc error %d.\n",func_handle);
	/* ｱｻｲﾝﾌｨﾙﾀの設定 */
	for(i = 0;i < 4;i++)
	{
		assign.as_status = 0xffff;
		assign.as_chmap = 0xffff;
		assign.as_track = i;
		assign.as_velo = 0;
		assign.as_oct = 0;
		assign.as_ctrlmap = 0;
		TMM_setAssignFilter(i,&assign);
	}
	TMM_setAssignMap(0,0);
	TMM_setAssignMap(1,0);
	TMM_setAssignMap(2,0);
	TMM_setAssignMap(3,0);
	TMM_setAssignMap(32,0);

	/* SMPTEの同期開始時刻(01:00:00.00)の設定 */
	smpte.tc_hr = 1;
	smpte.tc_min = 0;
	smpte.tc_sec = 0;
	smpte.tc_fr = 0;
	smpte.tc_bit = 0;
	smpte.tc_type = SMPTE25;
	TMM_smpteToRealtime(&r_time,&smpte);
	TMM_setSmpteOffset(&r_time);
	/* 同期信号出力の設定 */
	TMM_setSyncOut(MIDISYNC,0);
	/* ﾘﾓｰﾄﾓｰﾄﾞの設定 */
	TMM_setRemoteMode(1);
	step_mode_flag = 0;

	/* メインﾙｰﾌﾟへ */
	command();

	/* ｺｰﾙﾊﾞｯｸﾙｰﾁﾝの解除 */
	TMM_resetUserFunc(func_handle);
	/* RS-MIDIの解除 */
	if(rs_mode >= 0)
		RSM_Term();
	/* MIDIﾏﾈｰｼﾞｬのｸﾛｰｽﾞ */
	TMM_closeMidiMan();
	/* ｻｳﾝﾄﾞBIOSの終了処理 */
	SND_end();
	/* ﾒﾓﾘの開放 */
	free(play_buffer);
	/* ﾌﾟﾛｸﾞﾗﾑ終了 */
	exit(0);
}
/*
----------------------------------------------------------------------
	ﾒｲﾝﾙｰﾌﾟ
*/
int command()
{
	int		i,flags;
	char	key;
	PORT	port,logical_port,physical_port;
	int		tempo,lag,ret;
	char	filename[128];
	PLACE	start_place,place;
	SMPTE	smpte;
	REALTIME	r_time;
	char	master,level;
	unsigned char *ptr;
	TRACK	track;
	char	bankname[16];
	MIDIMANCTRL	midiman;

	while(1)
	{
		/* ｺﾏﾝﾄﾞ待ち */
		key = idle_loop(1);
		switch(key){
		
			case '?':		/*	help */
				printf("\n");
				printf("[esc] = exit this program\n");
				printf("[spc] = play stop\n");
				printf("[tab] = pause\n");
				printf("'B'   = load internal voice bank data\n");
				printf("'C'   = port map\n");
				printf("'F'   = file\n");
				printf("'T'   = tempo\n");
				printf("'P'   = play start\n");
				printf("'R'   = rec start\n");
				printf("'Y'   = sync mode\n");
				printf("'X'   = play mode\n");
				printf("'Z'   = relative tempo\n");
				printf("'\\'   = set smpte lag & correction level\n");
				printf("'!'   = output port\n");
				printf("'#'   = output MIDI channel\n");
				printf("'$'   = sync out port\n");
				printf("'%%'   = smpte offset time\n");
				printf("'&'   = smpu internal time\n");
				printf("'_'   = main interval time\n");
				printf("'^'   = GS initalize \n");
				printf("'0'   = step mode tap\n");
				break;

			case 0x09:	/* tab */
				/* 演奏のﾎﾟｰｽﾞ/解除 */
				TMM_getPlayPlace(&flags,&ptr,&place);
				if((flags & PAUSEFLAG) == 0)
				{
					printf("\nPause\n");
					TMM_playPause();
				}
				else
				{
					printf("\nRestart\n");
					TMM_playRestart();
				}
				break;

			case 0x1b:	/* esc */
				/* ﾌﾟﾛｸﾞﾗﾑの終了 */
				printf("\nExit this program. Are you sure? 'Y'/'N'->");
				if((key = command_check("yn")) == 0)
					break;
				if(key == 'y')
				{
					TMM_playStop();
					return 0;
				}
				else
					printf("continue\n");
				break;

			case 'b':
				/* 内蔵音源ﾌｧｲﾙの読み込み */
				printf("\n'F'mb file or 'P'mb file?->");
				if((key = command_check("fp")) == 0)
					break;
				printf("Input file name ->");
				scanf("%s",filename);
				printf("Loading %s\n",filename);
				if(key == 'p')
					ret = SND_pcm_bank_load(filename,bankname);
				else
					ret = SND_fm_bank_load(filename,bankname);
				printf("finish %d\n",ret);
				break;

			case 'c':
				/* ﾎﾟｰﾄﾏｯﾌﾟの変更 */
				printf("\n'I'nput port or 'O'utput port?->");
				if((key = command_check("io")) == 0)
					break;
				printf("\nInput logical port number ->");
				scanf("%d",&i);
				logical_port = i;
				printf("\nInput physical port number ->");
				scanf("%d",&i);
				physical_port = i;
				if(key == 'i')
					ret = TMM_setInputPortMap(logical_port,physical_port);
				else
					ret = TMM_setOutputPortMap(logical_port,physical_port);
				if(ret != NOERR)
					printf("\nError %d\n",ret);
				break;

			case 'f':
				/* MID/EUP ﾌｧｲﾙの読み込み */
				TMM_playStop();
				printf("\nFile ... 'E'up or 'M'id (format 0 only)?->");
				if((key = command_check("em")) == 0)
					break;
				printf("Input file name ->");
				scanf("%s",filename);
				printf("Loading %s\n",filename);
				if(key == 'm')
					ret = readMidiFile(filename);
				else
					ret = readEupFile(filename);
				printf("finish %d\n",ret);
				break;

			case 't':
				/* ﾃﾝﾎﾟの設定 */
				printf("\nInput Tempo(qb/m)->");
				scanf("%d",&tempo);
				ret = TMM_setTempo(0,tempo*10);
				if(ret != NOERR)
					printf("\nError %d\n",ret);
				break;

			case 'p':
				/* 演奏開始 */
				start_place.p_meas = 0;
				start_place.p_beat = 0;
				start_place.p_tick = 0;
				start_place.p_clock = 0;
				start_place.p_sign = initial_sign;
				start_place.p_time.rt_lo = 0;
				start_place.p_time.rt_hi = 0;
				TMM_setTempo(0,initial_tempo * 10);
				TMM_playStart(format,0,play_buffer,PBSIZE,&start_place);
				printf("\nPlay Start\n");
				break;

			case 'r':
				/* Metronomeの確認用 録音開始 (実際は録音はしない) */
				start_place.p_meas = 0;
				start_place.p_beat = 0;
				start_place.p_tick = 0;
				start_place.p_clock = 0;
				start_place.p_sign = initial_sign;
				start_place.p_time.rt_lo = 0;
				start_place.p_time.rt_hi = 0;
				TMM_setTempo(0,initial_tempo * 10);
				TMM_playStart(format,1,play_buffer,PBSIZE,&start_place);
				printf("\nPlay Start\n");
				break;

			case 'x':
				/* 演奏ﾓｰﾄﾞの指定 */
				printf("\n'N'ormal ,'E'ndless,'L'oop ?->");
				if((key = command_check("nel")) == 0)
					break;
				if(key == 'n')
					TMM_setPlayMode(NORMALPLAY);
				else if(key == 'e')
					TMM_setPlayMode(ENDLESSPLAY);
				else
					TMM_setPlayMode(LOOPPLAY);
				break;

			case 'y':
				/* 同期ﾓｰﾄﾞの指定 */
				printf("\n'I'nternal, 'M'idi or 'S'mpte ?->");
				if((key = command_check("ims")) == 0)
					break;
				TMM_playStop();
				if(key == 'i')
				{
					printf("\nInput remote port number ->");
					scanf("%d",&i);
					port = i & 0xff;
					TMM_setSyncIn(INTERNALSYNC,port,0,SMPTE24,0);
				}
				else if(key == 'm')
				{
					printf("\nInput sync in port number ->");
					scanf("%d",&i);
					port = i & 0xff;
					TMM_setSyncIn(MIDISYNC,port,0,SMPTE24,0);
				}
				else
				{
					printf("\nWho's master? 'I'nternl,'E'xternal or 'M'idi t.c.->");
					if((key = command_check("iem")) == 0)
						break;
					if(key == 'i')
						master = 0;
					else if(key == 'e')
						master = 1;
					else
						master = 2;
					ret = TMM_setSyncIn(SMPTESYNC,0,master,SMPTE25,5);
					if(ret)
						printf("\nError at setSyncIn %d\n",ret);
				}
				break;

			case 'z':
				/* 相対ﾃﾝﾎﾟの指定(ｽﾃｯﾌﾟﾓｰﾄﾞも可) */
				printf("\nInput Relative tempo->(0 -> 1000%%)");
				scanf("%d",&tempo);
				ret = TMM_setRelativeTempo(tempo);
				if(tempo == 0)
					step_mode_flag = 1;
				else
					step_mode_flag = 0;
				if(ret != NOERR)
					printf("\nError %d\n",ret);
				break;

			case '%':
				/* SMPTE同期開始位置の設定 */
				printf("\nInput SMPTE offset time->(hr:mn:sc.fr/bit)");
				scanf("%d %d %d %d %d",&smpte.tc_hr,&smpte.tc_min
					,&smpte.tc_sec,&smpte.tc_fr,&smpte.tc_bit);
				smpte.tc_type = SMPTE25;
				TMM_smpteToRealtime(&r_time,&smpte);
				TMM_setSmpteOffset(&r_time);
				TMM_realtimeToSmpte(&smpte,&r_time);
				printf("start offset");
				print_smpte(&smpte);
				break;

			case '&':
				/* S-MPUの内部時計の設定 */
				printf("\nInput SMPU start Time->(hr:mn:sc.fr/bit)");
				scanf("%d %d %d %d %d",&smpte.tc_hr,&smpte.tc_min
					,&smpte.tc_sec,&smpte.tc_fr,&smpte.tc_bit);
				smpte.tc_type = SMPTE25;
				TMM_smpteToRealtime(&r_time,&smpte);
				TMM_setSmpuTime(&r_time);
				TMM_realtimeToSmpte(&smpte,&r_time);
				printf("start at ");
				print_smpte(&smpte);
				break;

			case ' ':
				/* 演奏の停止 */
				TMM_playStop();
				printf("\nPlay Stop!\n");
				break;

			case'\\':
				/* 同期精度の設定 */
				printf("\nInput lag time (msec)->");
				scanf("%d",&i);
				lag = i;
				printf("Input correction time(msec)->");
				scanf("%d",&i);
				level = i;
				TMM_setSmpteLag(lag*480000,level*480000);
				break;

			case '!':
				/* 全ﾄﾗｯｸの出力ﾎﾟｰﾄの設定 */
				printf("\nInput data output port number->");
				scanf("%d",&i);
				port = i & 0xff;
				for(i = 0;i < MAXTRACK;i++)
					trackwork_buff[i].trk_port = port;
				break;

			case '#':
				/* 各ﾄﾗｯｸの出力MIDIﾁｬﾝﾈﾙの設定 */
				printf("\nInput track(0-255)->");
				scanf("%d",&i);
				track = i & 0xff;
				printf("\ninput MIDI channel(1-16 or 256)->");
				scanf("%d",&i);
				if((i < 1) || (i > 16))
					i = 256;
				trackwork_buff[i].trk_midich = i-1;
				break;

			case '$':
				/* 同期信号(MIDI CLOCK)の出力ﾎﾟｰﾄの設定*/
				printf("\nInput sync out port number->");
				scanf("%d",&i);
				port = i & 0xff;
				TMM_setSyncOut(MIDISYNC,port);
				break;

			case '*':
				/* ｱｻｲﾝﾌｨﾙﾀの出力設定 */
				printf("\ninput output track(0-255)->");
				scanf("%d",&i);
				track = i & 0xff;
				assign.as_status = 0xffff;
				assign.as_chmap = 0xffff;
				assign.as_track = track;
				assign.as_velo = 0;
				assign.as_oct = 0;
				assign.as_ctrlmap = 0;
				TMM_setAssignFilter(0,&assign);
				break;

			case '^':
				/* GS音源初期化ｺﾏﾝﾄﾞの出力 */
				printf("\ninput output port for init GS->");
				scanf("%d",&i);
				i &= 0xff;
				TMM_outputMidiEvent(i,gs_init);
				break;

			case '_':
				/* ﾒｲﾝｲﾝﾀｰﾊﾞﾙの変更 */
				printf("\ninput interval time (*0.1msec)->");
				scanf("%d",&i);
				TMM_getMidiManCtrl(&midiman);
				midiman.mc_interval = 480000*i/10;
				TMM_closeMidiMan();
				TMM_openMidiMan(&midiman);
				break;

			case '0':
				/* ｽﾃｯﾌﾟﾓｰﾄﾞ時のﾀｯﾌﾟ */
				if(step_mode_flag)
					TMM_stepModeEntry(240);
				break;
				
		}
	}
}
/*
-------------------------------------------------------
	ﾌｧｲﾙのｵｰﾌﾟﾝｻﾌﾞﾙｰﾁﾝ
*/
int OpenFile(char *filename,char *mode,FILE **fp)
{
	if((*fp = fopen(filename,mode)) == NULL)
		return CANTOPEN;
	else
		return NOERR;
}

/*
-------------------------------------------------------
	ﾌｧｲﾙからの読み込みｻﾌﾞﾙｰﾁﾝ
*/
int	ReadFile(unsigned char *buffer,int size,FILE *fp)
{
	if(fread((char *)buffer,1,size,fp) < size)
		return CANTREAD;
	else
		return NOERR;
}

/*
-------------------------------------------------------
	EUP拍子記号からMID拍子記号への変換ｻﾌﾞﾙｰﾁﾝ
*/
SIGN EUPsignToMFsign(int eup_sign)
{
	SIGN	mf_sign;
	int	beat;
	int	base;

	beat = (eup_sign & 0x0f) + 1;
	base = (eup_sign >> 4) & 0x07;
	base = (base + 1) >> 1;
	mf_sign =  (beat << 24)+(base << 16)+((96/(base+1)) << 8)+8;
	return mf_sign;
}
/*
-------------------------------------------------------------------------
	LOCATIONより可変長数値を取り出し,ﾎﾟｲﾝﾀを進める
*/
int XE_readLocLen(LOCATION *loc)
{
	unsigned char *ptr;
	int	val;

	ptr = loc->ptr;
	val = TMM_readLength(&loc->ptr);
	loc->remain -= loc->ptr - ptr;
	return(val);
}

/*
-------------------------------------------------------
	EUPﾌｧｲﾙの読み込み
*/
int readEupFile(char *filename)
{
	FILE *fp;
	int	ret,size,i;
	unsigned char	buffer[256];
	char bankpath[128];
	char bankname[16];
	EUPHEAD	eup;

	if(OpenFile(filename,"rb",&fp))
	{
		printf("CANTOPEN!!\n");
		return 1;
	}

	format = 1;

	if(ReadFile((unsigned char *)&eup,2048,fp) != NOERR)
		goto error;
	if(ReadFile(buffer,6,fp) != NOERR)
		goto error;
	size = *(int *)&buffer[0];
	if(size > PBSIZE)
		goto error;
	initial_sign = EUPsignToMFsign(buffer[4]);
	initial_tempo = buffer[5]+30;
	if(ReadFile(play_buffer,size,fp) != NOERR)
		goto error;
	fclose(fp);

	for(i = 0;i < 32;i++)
	{
		trackwork_buff[i].trk_port = eup.trk_port[i];
		trackwork_buff[i].trk_midich = eup.trk_midi_ch[i];
		trackwork_buff[i].trk_mute = eup.trk_mute[i];
		trackwork_buff[i].trk_bias = eup.trk_key_bias[i];
		trackwork_buff[i].trk_oct = eup.trk_transpose[i];
		trackwork_buff[i].trk_vol = 0;
		trackwork_buff[i].trk_exp = 0;
	}
	for(i = 0;i < 6;i++)
		if((ret =TMM_setInternalChannel(i,(int)eup.fm_midi_ch[i])) != NOERR)
			printf("Error at setInternalChannel %d",ret);
	for(i = 0;i < 8;i++)
		if((ret =TMM_setInternalChannel(i+64,(int)eup.pcm_midi_ch[i])) != NOERR)
			printf("Error at setInternalChannel %d",ret);
	
	SND_pcm_mode_set(0);
	for(i = 0;filename[i] != '\0';i++)
		bankpath[i] = filename[i];
	while((filename[i] != '\\') && (filename[i] != ':') && (i >= 0))i--;
	i++;
	if(*eup.fm_file_name != 0)
	{
		strncpy(&bankpath[i],eup.fm_file_name,8);
		bankpath[i+8] = 0;
		strcat(bankpath,".fmb");
		printf("Loading FMB :%s \n",bankpath);
		SND_fm_bank_load(bankpath,bankname);
	}
	if(*eup.pcm_file_name != 0)
	{
		strncpy(&bankpath[i],eup.pcm_file_name,8);
		bankpath[i+8] = 0;
		strcat(bankpath,".pmb");
		printf("Loading PMB :%s \n",bankpath);
		SND_pcm_bank_load(bankpath,bankname);
	}
	return NOERR;

error:
	fclose(fp);
	return FORMATERR;
}

/*
-------------------------------------------------------
	MIDIﾌｧｲﾙの読み込み
*/
	
int readMidiFile(char *filename)
{
	extern	int readFormat0(FILE *fp,int division);

	FILE *fp;
	unsigned char	buffer[256];
	int 	size,size2;
	int		format;
	int		ntrks;
	int		division;
	int		ret,i;

	if(OpenFile(filename,"rb",&fp))
	{
		printf("CANTOPEN!!\n");
		return 1;
	}
	format = 0;
	for(i = 0;i < 16;i++)
	{
		trackwork_buff[i].trk_port = 0;
		trackwork_buff[i].trk_midich = i;
		trackwork_buff[i].trk_mute = 0xff;
		trackwork_buff[i].trk_bias = 0;
		trackwork_buff[i].trk_oct = 0;
		trackwork_buff[i].trk_vol = 0;
		trackwork_buff[i].trk_exp = 0;
	}

	initial_tempo = 120;
	initial_sign = DEFSIGN;

	if((ret = ReadFile(buffer,8,fp)) != NOERR)
		goto error;
	size = (buffer[4] << 24) + (buffer[5] << 16)
		 + (buffer[6] <<8) + buffer[7];
	buffer[4] = 0;
	if(strcmp((char *)buffer,"MThd") != 0)
	{
		fseek(fp,128,SEEK_SET);
		if((ret = ReadFile(buffer,8,fp)) != NOERR)
			goto error;
		else
		{
			size2 = (buffer[4] << 24) + (buffer[5] << 16)
			 + (buffer[6] <<8) + buffer[7];
			buffer[4] = 0;
			if(strcmp((char *)buffer,"MThd") != 0)
				fseek(fp,8,SEEK_SET);
			else
				size = size2;
		}
	}

	while(strcmp((char *)buffer,"MThd") != 0)
	{
		fseek(fp,size,SEEK_CUR);
		if((ret = ReadFile(buffer,8,fp)) != NOERR)
			goto error;
		size = (buffer[4] << 24) + (buffer[5] << 16)
			 + (buffer[6] <<8) + buffer[7];
		buffer[4] = '\0';
	}

	if((ret = ReadFile(buffer,size,fp)) != NOERR)
		goto error;

	format = (buffer[0] << 8) + buffer[1];
	ntrks = (buffer[2] << 8) + buffer[3];
	division = (buffer[4] << 8) + buffer[5];
	if(division < 0)
	{
		ret = FORMATERR;
		goto error;
	}

	if(format == 0)
	{
		if(ntrks != 1)
		{
			ret = FORMATERR;
			goto error;
		}
		ret = readFormat0(fp,division);
	}
	else
	{
		ret = FORMATERR;
		goto error;
	}
error:
	fclose(fp);
	return(ret);
}

/*
-------------------------------------------------------
	ﾌｫｰﾏﾄ0の読み込み
*/
int readFormat0(FILE *fp,int division)
{
	LOCATION	loc_t,loc_d;
	unsigned char	*ptr;
	TIME	time;
	int		ret,size;
	unsigned char 	buffer[256];
	unsigned char	status,run_stat;

	if((ret = ReadFile(buffer,8,fp)) != NOERR)
		goto error;
	size = (buffer[4] << 24) + (buffer[5] << 16)
		 + (buffer[6] <<8) + buffer[7];
	buffer[4] = '\0';
	while(strcmp((char *)buffer,"MTrk") != 0)
	{
		fseek(fp,size,SEEK_CUR);
		if((ret = ReadFile(buffer,8,fp)) != NOERR)
			goto error;
		size = (buffer[4] << 24) + (buffer[5] << 16)
			 + (buffer[6] <<8) + buffer[7];
		buffer[4] = '\0';
	}
	if(size > PBSIZE)
		goto error;
	if((ret = ReadFile(work_buffer,size,fp)) != NOERR)
		goto error;

	run_stat = 0;
	loc_t.ptr = work_buffer;
	loc_t.time = 0;
	loc_t.count = 0;
	loc_t.remain = size;
	loc_d = loc_t;
	loc_d.ptr = play_buffer;
	loc_d.remain = PBSIZE;
	loc_t.time += XE_readLocLen(&loc_t);
	while(loc_t.ptr != NULL)
	{
		if(XE_isEnd(&loc_t))
			loc_t.ptr = NULL;
		else
		{
			ptr = loc_t.ptr;
			if((*ptr < 0x80) || (*ptr == 0xf1))
				status = run_stat;
			else
				status = *ptr++;
			if(((status & 0xf0) == 0x80) ||
				(((status & 0xf0)== 0x90)&&(*(ptr+1)) == 0))
			{
				ret = XE_fSkipEvent(&loc_t,&run_stat);
				if(ret)
					goto error;
			}
			else
			{
				if(division != RESO480)
					time = loc_t.time * RESO480/division;
				else
					time = loc_t.time;
				ret = XE_writeLocLen(&loc_d,time - loc_d.time);
				if(status >= 0xf0)
					*loc_d.ptr++ = 0;
				else
					*loc_d.ptr++ = status & 0x0f;
				ptr = loc_d.ptr;
				loc_d.remain -= 1;
				if(ret)
					goto error;
				ret = XE_fCopyEvent(&loc_d,&loc_t,&run_stat,division);
				if(*ptr < 0xf0)
					*ptr &= 0xf0;
				if(ret)
					goto error;
				loc_d.time = time;
			}
			loc_t.time += XE_readLocLen(&loc_t);
		}
	}
	time = loc_t.time * RESO480/division;
	if((ret = XE_writeLocLen(&loc_d,time - loc_d.time)) != NOERR)
		goto error;
	*loc_d.ptr++ = 0;
	loc_d.remain -= 1;
	if((ret = XE_copyEof(&loc_d)) != NOERR)
			goto error;

	return(ret);

read_error:
	ret = FORMATERR;
	goto error;
mem_error:
	ret = OUTMEM;
error:
	return(ret);

}

/*
-------------------------------------------------------
	MIDIﾌｧｲﾙ上のｲﾍﾞﾝﾄｽｷｯﾌﾟ
*/
XE_fSkipEvent(LOCATION *loc,unsigned char *run_stat)
{
	unsigned char	*ptr;
	int		ret;
	unsigned char	c;
	
	ptr = loc->ptr;
	if((*ptr < 0x80) || (*ptr == 0xf1))
		c = *run_stat;
	else
		c = *ptr++;
	if((c < 0xf0) && (c >= 0x80))
	{
		*run_stat = c;
		ptr += 2;
		if((c < 0xe0) && (c >= 0xc0))
			ptr -= 1;
	}
	else if((c == 0xf0) || (c == 0xf7))
	{
		*run_stat = 0;
		ret = TMM_readLength(&ptr);
		ptr += ret;
	}
	else if(c == 0xff)
	{
		if(*ptr++ == 0x2f)
			return ENDDETECT;
		else
		{
			ret = TMM_readLength(&ptr);
			ptr += ret;
		}
	}
	else if(c == 0xf3)
	{
		*run_stat = 0;
		ptr += 1;
	}
	else if(c == 0xf2)
	{
		*run_stat = 0;
		ptr += 2;
	}
	else if(c == 0)
		return FORMATERR;
	if((loc->remain -= (ptr - loc->ptr)) < 0)
		return FORMATERR;
	loc->ptr = ptr;
	loc->count++;
	return NOERR;
}
/*
--------------------------------------------------------------
	あるｲﾍﾞﾝﾄがEnd of trackかどうかをﾁｪｯｸする
	ﾎﾟｲﾝﾀは更新されない
*/
int	XE_isEnd(LOCATION *loc)
{
	if(*loc->ptr != 0xff)
		return 0;
	if(*(loc->ptr+1) == 0x2f)
		return 1;
	else
		return 0;
}
/*
--------------------------------------------------------------------------

	EOFイベントをコピーしてポインタを進める
	(実行後のポインタを参照するとページフォルトを起こす可能性大)

*/
int XE_copyEof(LOCATION *dest)
{
	
	if(dest->remain < 3)
		return(OUTMEM);
	dest->remain -= 3;
	*dest->ptr++ = 0xff;
	*dest->ptr++ = 0x2f;
	*dest->ptr++ = 0x00;
	dest->count++;
	return NOERR;
}
/*
-------------------------------------------------------
	MIDIﾌｧｲﾙからのｲﾍﾞﾝﾄのｺﾋﾟｰ
*/
int XE_fCopyEvent(LOCATION *loc_d,LOCATION *loc_s,unsigned char *run_stat,int division)
{
	extern int XE_fSearchNoteOff(LOCATION *loc,unsigned char run_stat,MIDICH *ch,char *note,char *velo,TIME *gate);

	unsigned char	*ptr,*nptr;
	int		size;
	int		ret;
	LOCATION	loc;
	MIDICH ch;
	char	note,velo;
	TIME	gate;
	
	loc = *loc_s;
	ptr = loc_s->ptr;
	nptr = loc_d->ptr;
	if((ret = XE_fSkipEvent(loc_s,run_stat)) != NOERR)
		return (ret);
	size = loc_s->ptr - ptr;
	if(*ptr < 0x80)
	{
		if(loc_d->remain < size+1)
			return(OUTMEM);
		*loc_d->ptr++ = *run_stat;
		loc_d->remain -= size+1;
	}
	else
	{
		if(loc_d->remain < size)
			return(OUTMEM);
		loc_d->remain -= size;
	}
	while(size--)
		*loc_d->ptr++ = *ptr++;
	loc_d->count++;
	if((*nptr & 0xf0) == 0x90)
		if(*(nptr+2) == 0)
			*nptr &= 0x8f;
	if((*nptr & 0xf0) == 0x90)
	{
		if(loc_d->remain < 5)
			return OUTMEM;
		ret = XE_fSearchNoteOff(&loc,*run_stat,&ch,&note,&velo,&gate);
		if((ret == NOERR) && (loc.ptr != NULL))
		{
			if(*loc.ptr >= 0x80)
				*(loc.ptr+1) = 0xf1;
			else
				*loc.ptr = 0xf1;
		}
		if(division != RESO480)
			gate = gate * RESO480/division;
		ret = XE_writeLocLen(loc_d,gate);
		if(ret)
			return ret;
		*loc_d->ptr++ = velo;
		loc_d->remain--;
	}
	return NOERR;
}
/*
--------------------------------------------------------------------------
	LOCATIONに可変長数値を書き込み,ﾎﾟｲﾝﾀを進める
*/
int XE_writeLocLen(LOCATION *loc,int val)
{
	int	tmp;

	if(loc->remain < 5)
		return OUTMEM;
	tmp = val & 0x7f;
	val &= 0x0fffffff;
	while((val >>= 7) > 0)
	{
		tmp <<= 8;
		tmp += (val & 0x7f) | 0x80;
	}
	while(1)
	{
		*loc->ptr++ = (unsigned char)(tmp & 0xff);
		loc->remain--;
		if(tmp & 0x80)
			tmp >>= 8;
		else
			break;
	}
	return NOERR;
}

/*
---------------------------------------------------------------------------
	MIDIﾌｧｲﾙ上の対応するNOTE OFFのｻｰﾁ
*/
int XE_fSearchNoteOff(LOCATION *loc,unsigned char run_stat,MIDICH *ch,char *note,char *velo,TIME *gate)
{
	unsigned char	*ptr;
	unsigned char	status;

	ptr = loc->ptr;
	if(*ptr < 0x80)
		status = run_stat;
	else
		status = *ptr++;
	*ch = status & 0x0f;
	*note = *ptr;
	*gate = loc->time;
	*velo = 0;
	XE_fSkipEvent(loc,&run_stat);

	while(1)
	{
		loc->time += XE_readLocLen(loc);
		ptr = loc->ptr;
		if(*ptr < 0x80)
			status = run_stat;
		else
			status = *ptr++;
		if(((status & 0xf0) == 0x80)
			||(((status & 0xf0) == 0x90) && (*(ptr+1) == 0)))
		{
			if((*ch == (status & 0x0f)) && (*note == *ptr))
				break;
		}
		if(XE_fSkipEvent(loc,&run_stat) == ENDDETECT)
		{
			*gate = loc->time - *gate;
			return ENDDETECT;
		}
	}
	*velo = *(ptr+1);
	*gate = loc->time - *gate;
	return NOERR;
}

#include <dos.h>
void _segread(struct SREGS * __segregs) {
    unsigned short ds01;
    unsigned short es01;
    unsigned short cs01;
    unsigned short ss01;
    __asm {
        mov	ax, ds
	mov	ds01, ax
	mov	ax, es
	mov	es01, ax
	mov	ax, cs
	mov	cs01, ax
	mov	ax, ss
	mov	ss01, ax
    }
    __segregs->ds = ds01;
    __segregs->es = es01;
    __segregs->cs = cs01;
    __segregs->ss = ss01;
}
