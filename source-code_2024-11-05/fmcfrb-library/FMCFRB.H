/************************************************/
/*                                              */
/* FMC.H      COPYRIGHT FUJITSU LIMITED         */
/*                  1988-1989                   */
/*                                              */
/************************************************/

/************************************************/
/* VDB_ATR(1)									*/
/************************************************/
typedef struct _VDB_ATR {
	unsigned char		charatr;	/* 文字属性			  */
	unsigned char		dispatr;	/* 表示属性			　*/
	unsigned short int	color;		/* 最大色識別番号、又は色識別番号 */
} VDB_ATR;

/************************************************/
/* DKB_SEC(2)									*/
/************************************************/
typedef struct _DKB_SEC {
	unsigned char	trakno;			/*　トラック番号 */
	unsigned char	hedno;			/*  ヘッド番号	 */
	unsigned char	secno;			/*  セクタ番号	 */
	unsigned char	seccnt;			/*　セクタ長     */
	unsigned char	crc1;
	unsigned char	crc2;
} DKB_SEC;

/************************************************/
/* DKB_PARA(3)									*/
/************************************************/
typedef struct _DKB_PARA {
	unsigned char	comcode;		/*　コマンドコード				*/
	unsigned char	compara[9];		/*  コマンドパラメータ			*/
	unsigned long	transadr;		/*  データ番号アドレス			*/
	unsigned short	transadr_seg;	/*  データ番号セグメント		*/
	unsigned long	statusadr;		/*　リザルトステータスアドレス 	*/
	unsigned short	statusadr_seg;	/*　リザルトステータスセグメント*/
} DKB_PARA;

/************************************************/
/* TIM_CALEN(4)									*/
/************************************************/
typedef struct _TIM_CALEN {
	short int	year;				/*  年　(1980-2079) */
	char		month;				/*　月　(1-12)	    */
	char		day;				/*　日　(1-31)	    */
	char		what_day;			/*  曜日(0- 6)	    */
	char		hour;				/*　時　(0-23)	    */
	char		minute;				/*　分　(0-59)	    */
	char		sec;				/*　秒　(0-59)	    */
	char		msec;				/*　1/100秒(0-99)   */
	char		ext;				/*　0       	    */
} TIM_CALEN;

/************************************************/
/* TIM_TIME(5)									*/
/************************************************/
typedef struct _TIM_TIME {
	unsigned char		mode;		/*　タイマー割り込みモード		*/
	unsigned char		inf;		/*　0							*/
	unsigned long		adr;		/*　タイマー割り込み処理アドレス*/
	unsigned short		adr_seg;	/*　タイマー割り込み処理ｾｸﾞﾒﾝﾄ	*/
	unsigned short int	hcycle;		/*　タイマー周期(HIGH)			*/
	unsigned short int	lcycle;		/*　タイマー周期(LOW)			*/
} TIM_TIME;

/************************************************/
/* TIM_CLOCK(6)									*/
/************************************************/
typedef struct _TIM_CLOCK {
	unsigned char	flg;		/*  設定フラグ						*/
	unsigned char	ext1;		/*  0								*/
	unsigned long	adr;		/*  時計管理割り込み処理アドレス	*/
	unsigned short	adr_seg;	/*　時計管理割り込み処理セグメント	*/
	short int		cycle;		/*  割り込み周期					*/
	short int		ext2;		/*									*/
	short int		year;		/*  年　(1980-2079) 				*/
	char			month;		/*　月　(1-12)						*/
	char			day;		/*　日　(1-31)						*/
	char			what_day;	/*  曜日(0- 6)						*/
	char			hour;		/*　時　(0-23)						*/
	char			minute;		/*　分　(0-59)						*/
	char			sec;		/*　秒　(0-59)						*/
} TIM_CLOCK;

/************************************************/
/* RSB_PARA(8)									*/
/************************************************/
typedef struct _RSB_PARA {
	unsigned char		mode;		/*  通信モード				*/
	unsigned char		baud;		/*  ボーレート				*/
	unsigned long		rbuf;		/*  受信バッファアドレス	*/
	unsigned short		rbuf_seg;	/*  受信バッファセグメント	*/
	unsigned short int	stime;		/*　送信タイムアウト		*/
	unsigned short int	rtime;		/*　受信タイムアウト		*/
	unsigned long		rinfbuf;	/*  受信通知アドレス		*/
	unsigned short		rinfbuf_seg;/*  受信通知セグメント		*/
	unsigned char		extmode;	/*  拡張モード				*/
	unsigned char		xon;		/*  XONコード				*/
	unsigned char		xoff;		/*  XOFFコード				*/
	unsigned long		obuf;		/*  送信バッファアドレス	*/
	unsigned short		obuf_seg;	/*  送信バッファセグメント	*/
} RSB_PARA;

/************************************************/
/* RSB_INT(9)									*/
/************************************************/
typedef struct _RSB_INT {
	unsigned char	flg;		/*  割り込みフラグ			*/
	unsigned long	cs_adr;		/*  CS信号割り込みアドレス	*/
	unsigned short	cs_adr_seg;	/*  CS信号割り込みｾｸﾞﾒﾝﾄ	*/
	unsigned long	ci_adr;		/*  CI信号割り込みアドレス	*/
	unsigned short	ci_adr_seg;	/*  CI信号割り込みｾｸﾞﾒﾝﾄ	*/
} RSB_INT;

/************************************************/

void BSETDATAADR(char *,char *) ;
void BSETCODEADR(char *,int (*)()) ;

/************************************************/

/*****************************************************************************/
/*                        ＦＲＢインターフェース                             */
/*****************************************************************************/

/* forRBIOSインストゥール状態の取得 */
int FRB_getstatus(void);

/* forRBIOSのクライアント宣言(登録) */
int FRB_setclient(void);

/* forRBIOSのクライアント宣言(解除) */
int FRB_resetclient(void);

/* forRBIOSのクライアント宣言全解除 */
int FRB_clrclient(void);

/* forRBIOSのクライアントメンバ取得 */
int FRB_getclient(unsigned short *clients);

/* forRBIOSの全クライアントメンバ一括登録 */
int FRB_popclient(unsigned short *clients);

/* ユーザ指定stack領域にstackをchangeさせ､指定関数を起動 */
int FRB_change_stack(unsigned long u_stack_off,unsigned int u_stack_sel,
											int(*(u_func))(),int p_cnt,...);
/*****************************************************************************/
/*                        キーボードインターフェース                         */
/*****************************************************************************/

/* 初期化 */
void KYB_init(void);

/* バッファリング機能の設定 */
void KYB_setbuf(int sw);

/* コード系の設定 */
void KYB_setcode(unsigned int mode);

/* コード系の読み取り */
unsigned int KYB_rdcode(void);

/* キーボードロックの制御 */
void KYB_lock(int sw);

/* クリック音の制御 */
void KYB_clic(int sw);

/* バッファのクリア */
void KYB_clrbuf(void);

/* 入力のチェック */
unsigned int KYB_inpchk(int *chrcnt,unsigned int *encode);

/* シフトキー状態の読み取り */
unsigned int KYB_shift(void);

/* 文字の読み込み */
unsigned int KYB_read(int sw,unsigned int *encode);

/* マトリクス入力 */
void KYB_matrix(char *matrix);

/* 入力文字列の追加 */
int KYB_addstr(int sw,int chrcnt,unsigned short int *string);

/* ＰＦキー割り込み処理ルーチンの登録 */
void KYB_pfint(unsigned int pfflg,unsigned int extflg,unsigned int markflg,
				char *intentry);

/*リアルＰＦキー割り込み処理ルーチンの登録 */
void KYB_rpfint(unsigned int pfflg,unsigned int extflg,unsigned int markflg,
				char *intentry);

/* リアルＰＦキー割り込み処理ルーチンの読み取り */
void KYB_rdpfint(unsigned int *pfflg,unsigned int *extflg,
				unsigned int *markflg,char *intentry);

/* キーの割り当て */
void KYB_asign(int sw,unsigned int moji,int chrcnt,char *string);

/* キーの割り当て状態の読み取り */
void KYB_rdasign(int *sw,unsigned int moji,int *chrcnt,char *string);

/*****************************************************************************/
/*                          文字表示インターフェース                         */
/*****************************************************************************/

/* 初期化 */
void VDB_init(void);

/* 画面の表示制御 */
void VDB_kind(unsigned int mode);

/* 全画面消去 */
void VDB_cls(void);

/* 表示画面サイズの設定 */
int VDB_setsize(int x,int y);

/* 表示画面サイズの読み取り */
void VDB_rdsize(int *x,int *y);

/* 表示サイズレパートリの読み取り */
void VDB_screensize(char *screen);

/* アトリビュート機能範囲の読み取り */
void VDB_rdatrarea(VDB_ATR *atr);

/* フォントパターンの取り出し */
int VDB_rdfont(int x,int y,unsigned int moji,char *pattern);

/* 外字パターンの登録 */
int VDB_setfont(int x,int y,unsigned int moji,char *pattern);

/* カーソル形状の設定 */
void VDB_setcustype(unsigned int type,int startraster,int endraster);

/* カーソル形状の読み取り */
unsigned int VDB_rdcustype(int *startraster,int *endraster);

/* カーソル表示状態の設定 */
void VDB_setcusmd(int sw);

/* カーソル表示状態の読み取り */
int VDB_rdcusmd(void);

/* カーソル位置の指定 */
int VDB_setposcus(int x,int y);

/* カーソル位置の読み取り */
void VDB_rdposcus(int *x,int *y);

/* アトリビュートの設定 */
int VDB_setatr(int x,int y,VDB_ATR *atr);

/* アトリビュートの読み取り */
int VDB_rdatr(int x,int y,VDB_ATR *atr);

/* デフォルトアトリビュートの設定 */
void VDB_setdefatr(VDB_ATR *atr);

/* デフォルトアトリビュートの読み取り */
void VDB_rddefatr(VDB_ATR *atr);

/* 文字設定 */
int VDB_setchr(int sw,int chratr,int moji,int x,int y);

/* 文字読み取り */
int VDB_rdchr(int x,int y,int *chratr,int *moji);

/* 矩形域設定 */
int VDB_setarea(int sw,int x1,int y1,int x2,int y2,char *chrbuf,char *atrbuf);

/* 矩形域読み取り */
int VDB_rdarea(int sw,int x1,int y1,int x2,int y2,char *chrbuf,char *atrbuf);

/* 矩形域複写 */
int VDB_cpyarea(int sw,int x1,int y1,int x2,int y2,int x3,int y3);

/* 矩形域消去 */
int VDB_clrarea(int x1,int y1,int x2,int y2);

/* 全画面スクロール */
void VDB_scroll(int direction,int count);

/* 部分スクロール */
int VDB_areascroll(int direction,int count,int x1,int y1,int x2,int y2);

/* コンソール機能の設定 */
void VDB_setcon(unsigned int flg);

/* コンソール機能設定値の読み取り */
unsigned int VDB_rdcon(void);

/* 文字の出力 */
void VDB_outchr(char moji);

/* 文字列の出力 */
void VDB_outstr(int chrcnt,char *string);

/* システム行書き込み */
void VDB_wtsysline(int sw,int chrcnt,int sta_y,char *chrbuf,char *atrbuf); 

/* 背景色の読み取り */
unsigned int VDB_rdbackground(void); 

/*****************************************************************************/
/*                     フロッピィディスクインターフェース                    */
/*****************************************************************************/

/* ドライブモードの設定 */
int DKB_setmode(int devno,unsigned int mode1,unsigned int mode2);

/* ドライブモードの取り出し */
int DKB_rdmode(int devno,unsigned int *mode1,unsigned int *mode2);

/* ドライブステータス情報の取り出し */
int DKB_rdstatus(int devno,unsigned int *status);
int DKB_rdstatus2(int devno,unsigned int *mode,long *blockcnt);

/* シリンダ0へのシーク */
int DKB_restore(int devno);

/* シーク */
int DKB_seek(int devno,int cylno);

/* データの読み出し */
int DKB_read(int devno,int cylno,int headno,int secno,int seccnt,
			char *buf,int *secnum);
int DKB_read2(int devno,long blockno,int blockcnt,char *buf,int *blocknum);

/* データの書き込み */
int DKB_write(int devno,int cylno,int headno,int secno,int seccnt,
			char *buf,int *secnum);
int DKB_write2(int devno,long blockno,int blockcnt,char *buf,int *blocknum);

/* セクタの検査 */
int DKB_chksec(int devno,int cylno,int headno,int secno,int seccnt,
			int *secnum);
int DKB_chksec2(int devno,long blockno,int blockcnt,int *blocknum);

/* ハードディスクコントローラのリセット */
int DKB_reset(void);

/* セクタIDの取り出し */
int DKB_rdsecid(int devno,int cylno,int headno,DKB_SEC *secid);

/* トラックのフォーマット */
int DKB_format(int devno,int cylno,int headno,int datasize,char *buf);

/* 詳細エラー情報の読み出し */
int DKB_rddetailerr(int devno,char *err);

/* 特殊SCSIインターフェース */
int DKB_scsiintf(int devno,int unitno,int cmdsize,int sendsize,int *memsize,
						char *status, char *message, char *paraaddr);

/*****************************************************************************/
/*                          プリンタインターフェース                         */
/*****************************************************************************/

/* プリンタ状態の読み取り */
int PRB_status(unsigned int *status);

/* 1文字出力 */
int PRB_chr(char chr);

/* 文字列出力 */
int PRB_string(int count,char *string);

/* 未印字バイト数の読み取り */
int PRB_remainbyte(void);

/*****************************************************************************/
/*                            時計インターフェース                           */
/*****************************************************************************/

/* 日付/時刻の設定 */
int TIM_setcalendar(TIM_CALEN *calendar);

/* 日付/時刻の読み取り */
void TIM_rdcalendar(TIM_CALEN *calendar);

/* タイマの登録 */
int TIM_settime(TIM_TIME *timer,int *timeno);

/* タイマの取消 */
int TIM_clrtime(int timeno);

/* タイマのカウント値の読み取り */
int TIM_rdtime(TIM_TIME *timer,int timeno);

/* 指定時刻の割り込み処理の登録 */
int TIM_setclock(TIM_CLOCK *clock,int *clockno);

/* 指定時刻割り込み処理の取り消し */
int TIM_clrclock(int clockno);

/*****************************************************************************/
/*                         ＲＳ－２３２Ｃインターフェース                    */
/*****************************************************************************/

/* シリアルポートの検出 */
unsigned int RSB_chk(void);

/* 回線オープン */
int RSB_open(int port);

/* 回線クローズ */
int RSB_close(int port);

/* 通信パラメータの設定 */
int RSB_setpara(int port,RSB_PARA *paraadr);

/* 通信パラメータの読み取り */
int RSB_rdpara(int port,RSB_PARA *paraadr);

/* リアル通信パラメータの設定 */
int RSB_rsetpara(int port,RSB_PARA *paraadr);

/* リアル通信パラメータの読み取り */
int RSB_rdrpara(int port,RSB_PARA *paraadr);

/* 受信バッファ内の有効データ数の読み取り */
int RSB_read(int port,int *leng);

/* 送信バッファ内の有効データ数の読み取り */
int RSB_gettxlen(int port,int *leng);

/* XOFF受信のクリア */
int RSB_clsxoff(int port);

/* データの受信 */
int RSB_receive(int port,int *data,unsigned int *status);

/* データの送信 */
int RSB_send(int port,int data,unsigned int *status);

/* シリアルポートの制御 */
int RSB_ctrl(int port,unsigned int ctrl);

/* ステータス情報の読み取り */
int RSB_status(int port,unsigned int *status,unsigned int *serial);

/* 受信バッファの初期化 */
int RSB_initbuf(int port);

/* ブレーク信号の送信 */
int RSB_break(int port,unsigned int breaktime);

/* 拡張割り込みの設定 */
int RSB_setint(int port,RSB_INT *intbuf);

/* 拡張割り込みの読み取り */
int RSB_rdint(int port,RSB_INT *inbuf);

/* 拡張ＤＴＲ信号の保持設定 */
int RSB_dtr(int port,int sw);

/* 回線テストモードの設定 */
int RSB_testmode(int port,unsigned int ctrl);

/* FIFOポートの検出 */
unsigned short RSB_hdchk(void);

/*****************************************************************************/
/*                             ブザーインターフェース                        */
/*****************************************************************************/

/* ブザー  ＯＮ */
void BPB_on(void);

/* ブザー  ＯＦＦ */
void BPB_off(void);

/* ブザー (一定時間) ＯＮ */
void BPB_fixon(void);

/* ブザー (カウンタ数,指定時間) ＯＮ */
void BPB_freeon(int time,int count);

/* ブザー情報の読み取り(1) */
int BPB_rdinf(int *time,int *count);

/* ブザー (周波数,指定時間) ＯＮ */
void BPB_freeon2(int time,int freq);

/* ブザー情報の読み取り(2) */
int BPB_rdinf2(int *time,int *freq);

/*****************************************************************************/
/*                         即時復帰インターフェース                          */
/*****************************************************************************/

/*	即時復帰ファイルアクセス開始	*/
int	BGRW_start(void);

/*	即時復帰ファイルアクセス終了	*/
int	BGRW_end(void);

/*	即時復帰ファイルオープン	*/
int	BGRW_open(char *path,unsigned mode);

/*	即時復帰ファイル作成	*/
int	BGRW_create(char *path,unsigned atr);

/*	即時復帰ファイルシーク	*/
int	BGRW_seek(unsigned handle,long size,int how);

/*	即時復帰ファイルクローズ	*/
int	BGRW_close(unsigned handle);

/*	即時復帰ファイルリード開始	*/
int	BGRW_read(unsigned handle,char *buffer,int size,int *rsize);

/*	即時復帰ファイルライト開始	*/
int	BGRW_write(unsigned handle,char *buffer,int size,int *rsize);

/*	転送状態の取得	*/
int	BGRW_check(void);
