/************************************************/
/* FGS.H      COPYRIGHT Fujitsu LIMITED         */
/*                 1992-1994                    */
/************************************************/

#define FGSWorkSize 1536

#define	DWORD(x)	*((unsigned int			*)(x))
#define	WORD(x)		*((unsigned short int	*)(x))
#define	BYTE(x)		*((unsigned char		*)(x))

typedef struct {
	int		vx;			/*  仮想画面Ｘドット数      */
	int		vy;			/*  仮想画面Ｙドット数      */
	int		dx;			/*  表示画面Ｘドット数      */
	int		dy;			/*  表示画面Ｙドット数      */
	int		pixel;		/*  ＶＲＡＭピクセル数      */
	int		byte;		/*  １ラインのバイト数      */
	void	*offset;	/*  ＶＲＡＭ先頭オフセット  */
	short	segment;	/*  ＶＲＡＭセレクタ        */
	short	reserve;	/*	リザーブ				*/
} F_RESOLUTION;			/*  画面モード無依存構造体  */

/*	セッティングオペレーション	*/

//	初期化
int	FGS_init( void *work,int worksize );
//	初期化
int	FGS_init2( void *work,int worksize );
//	終了処理
int	FGS_close( void );
//	解像度の設定
int	FGS_resolution( void *work,int page,int mode );
//	画面の表示位置の設定
int	FGS_displayStart( void *work,int mode,int x,int y );
//	パレットレジスタの設定
int	FGS_palette( void *work,int vsync,void *data );
//	ビューポートの設定
int	FGS_viewport( void *work,void *para );
//	書き込みページの指定
int	FGS_writePage( void *work,int page );
//	表示ページの指定
int	FGS_displayPage( void *work,int pri,int display );
//	ハードウェア･NSDD常駐チェック
int	FGS_openCheck( void );
//	ハードウェア･NSDD常駐チェック
int	FGS_openCheck2( void );
//	重ね合わせ信号の設定
int	FGS_setys( void );
//	本体16m色時のパックモードの設定
int	FGS_setPack( int sw );

/*	ブロックオペレーション	*/

//	全画面の消去
int	FGS_clearScreen( void *work );
//	ドットデータの読み込み
int	FGS_getBlock( void *work,int mode,void *para );
//	ドットデータの書き込み
int	FGS_putBlock( void *work,int mode,void *para );
//	ドットデータの読み込み2
int	FGS_partGetBlock( void *work,int mode,void *para );
//	ドットデータの書き込み2
int	FGS_partPutBlock( void *work,int mode,void *para );
//	画面の複写
int	FGS_copy( void *work,void *para );
//	ドットデータの書き込み３
int	FGS_fillRaster( void *work,int x,int y,int size,int data );
//	ドットデータの書き込み４
int	FGS_putRaster( void *work,int x,int y,int size,void *data );

/*	リーディングオペレーション	*/

//	画面モードの取得
int	FGS_getResolution( int *page0,int *page1 );
//	書き込みページの取得
int	FGS_getWritePage( void *work,unsigned short seg );
//	表示状態の取得
int	FGS_getDisplayPage( int *priority,int *display );
//	表示開始位置の取得
int	FGS_getDisplayStart( int page,int mode,int *x,int *y );
//	パレットレジスタの読み取り
int	FGS_getPalette( void *buff );
//	ハードウェア情報の取得
int	FGS_getInformation( void *buff,int seg );
//	指定したページの解像度の取得
int	FGS_getResolutionPage( int page,void *para );
//	本体16m色時のパックモードの取得
int	FGS_getPack( void );
//	表示可能ページの取得
int	FGS_getActivePage( void );

/*	ディジタイズオペレーション	*/

//  ディジタイズの開始終了の設定
int	FGS_digitize( void *work,int mode );
//	取り込みモードの設定
int	FGS_digitizeMode( void *work,int mode );
//	取り込み範囲の設定
int	FGS_digitizeArea( void *work,void *para );
//	画質の調整
int	FGS_digitizeQuality( void *work,int mode,int *value,int writemode );
//	反転の設定
int	FGS_digitizeFlip( void *work,int mode );
//	仮想画面上の転送開始位置の設定
int	FGS_digitizeStart( void *work,int x,int y );
//	転送時の拡大率の設定
int	FGS_digitizeZoom( void *work,int x,int y );
