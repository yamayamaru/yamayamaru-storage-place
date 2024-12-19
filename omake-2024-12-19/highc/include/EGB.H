/***************************************************************/
/*                                                             */
/* EGB.H                                                       */
/*            COPYRIGHT (C) 1994 FUJITSU LIMITED               */
/*                                                             */
/***************************************************************/

#define EgbWorkSize 1536

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
	short	reserve;	/*  リザーブ                */
} RESOLUTION;			/*  画面モード無依存構造体  */

/*	セッティングオペレーション	*/
/*	初期化	*/
int	EGB_init( void *work,int size );
/*	解像度の設定	*/
int	EGB_resolution( void *work,int page,int mode );
int	EGB_resolutionRam( void *work,int page,int pixel,int x,int y,void *ram );
/*	画面の表示位置の設定	*/
int	EGB_displayStart( void *work,int mode,int x,int y );
/*	ビューポートの設定	*/
int	EGB_viewport( void *work,void *para );
/*	パレットレジスタの設定	*/
int	EGB_palette( void *work,int vsync,void *para );
/*	書き込みページの指定	*/
int	EGB_writePage( void *work,int page );
/*	表示ページの指定	*/
int	EGB_displayPage( void *work,int priority,int display );
/*	描画色の設定	*/
int	EGB_color( void *work,int color,int number );
/*	描画色の設定１	*/
int	EGB_colorIGRB( void *work,int color,int IGRB );
/*	混色比率の設定	*/
int	EGB_pastel( void *work,int pastel );
/*	描画モードの設定	*/
int	EGB_writeMode( void *work,int mode );
/*	線分パターンの設定	*/
int	EGB_linePattern( void *work,int mode,int pattern );
/*	面塗りモードの設定	*/
int	EGB_paintMode( void *work,int mode );
/*	ハッチングパターンの設定	*/
int	EGB_hatchingPattern( void *work,int mode,int x,int y,void *pattern );
/*	タイルパターンの設定	*/
int	EGB_tilePattern( void *work,int mode,int x,int y,void *pattern );
/*	画面のマスク	*/
int	EGB_maskRegion( void *work,void *maskArea );
/*	画面マスクの設定	*/
int	EGB_mask( void *work,int mode );
/*	ペンの設定	*/
int	EGB_pen( void *work,int mode );
/*	ペンの太さの設定	*/
int	EGB_penSize( void *work,int size );
/*	ペンの形状の設定	*/
int	EGB_penStyle( void *work,void *penPattern );
/*	マスクビットの設定	*/
int	EGB_maskBit( void *work,int maskBit );
/*	文字方向	*/
int	EGB_textDirection( void *work,int direction );
/*	文字表示方向	*/
int	EGB_textDisplayDirection( void *work,int direction );
/*	文字間空白	*/
int	EGB_textSpace( void *work,int dot );
/*	文字拡大率	*/
int	EGB_textZoom( void *work,int type,int x,int y );
/*	字体の設定	*/
int	EGB_fontStyle( void *work,int style );
/*	スーパーインポーズの設定	*/
int	EGB_superImpose( void *work,int mode );
/*	ディジタイズ	*/
int	EGB_digitize( void *work,int sw );
/*	ディジタイズ画面取り込み位置の補正	*/
int	EGB_digitizeAdjust( void *work,int xPlus,int yPlus );

/*	ブロックオペレーション	*/
/*	全画面の消去	*/
int	EGB_clearScreen( void *work );
/*	画面の消去	*/
int	EGB_partClearScreen( void *work );
/*	ドットデータの読み込み	*/
int	EGB_getBlockColor( void *work,void *para );
/*	ドットデータの書き込み	*/
int	EGB_putBlockColor( void *work,int mode,void *para );
/*	ドットデータの読み込み１	*/
int	EGB_getBlock( void *work,void *para );
/*	ドットデータの書き込み１	*/
int	EGB_putBlock( void *work,int mode,void *para );
/*	ドットデータの読み込み２	*/
int	EGB_getBlockZoom( void *work,void *para );
/*	ドットデータの書き込み２	*/
int	EGB_putBlockZoom( void *work,int mode,void *para );
/*	グラフィックカーソル	*/
int	EGB_graphicCursor( void *work,int mode,void *para );
/*	マスクデータの書き込み	*/
int	EGB_maskData( void *work,int mode,void *para );
/*	全画面スクロール	*/
int	EGB_scroll( void *work,int mode,int x,int y );
/*	部分画面スクロール	*/
int	EGB_partScroll( void *work,int mode,int x,int y,void *para );
/*	領域の設定	*/
int	EGB_region( void *work,
		int *check,int *workSize,int *x,int *y,int *x1,int *y1,void *para );
/*	画面の複写	*/
int	EGB_copy( void *work,int mode,void *para,void *workArea );
/*	画面の回転	*/
int	EGB_rotate( void *work,int mode,void *para,void *workArea );
/*	画面のぼかし	*/
int	EGB_resolve( void *work,void *workArea );
/*	矩形の複写	*/
int	EGB_copyBlock( void *work,void *para );

/*	グラフィックオペレーション	*/
/*	ポイント	*/
int	EGB_pset( void *work,void *para );
/*	連続線分	*/
int	EGB_connect( void *work,void *para );
/*	不連続線分	*/
int	EGB_unConnect( void *work,void *para );
/*	ポリゴン	*/
int	EGB_polygon( void *work,void *para );
/*	回転体	*/
int	EGB_rotatePolygon( void *work,void *para );
/*	三角形	*/
int	EGB_triangle( void *work,void *para );
/*	矩形	*/
int	EGB_rectangle( void *work,void *para );
/*	円	*/
int	EGB_circle( void *work,void *para );
/*	円弧	*/
int	EGB_arc( void *work,void *para );
/*	扇形	*/
int	EGB_fan( void *work,void *para );
/*	楕円	*/
int	EGB_ellipse( void *work,void *para );
/*	楕円弧	*/
int	EGB_ellipticArc( void *work,void *para );
/*	楕扇形	*/
int	EGB_ellipticFan( void *work,void *para );
/*	ペイント１	*/
int	EGB_paint( void *work,void *para );
/*	ペイント２	*/
int	EGB_closePaint( void *work,void *para );
/*	ポイント識別	*/
int	EGB_point( void *work,int mode,int x,int y,int *number );
/*	弓形1	*/
int	EGB_bow( void *work,void *para );
/*	弓形2	*/
int	EGB_semiBow( void *work,void *para );

/*	フォントオペレーション	*/
/*	文字列	*/
int	EGB_sjisString( void *work,void *para );
/*	追加文字列	*/
int	EGB_connectSjisString( void *work,void *para );
/*	文字列１	*/
int	EGB_asciiString( void *work,int fontType,void *para );
/*	文字列１	*/
int	EGB_connectAsciiString( void *work,int fontType,void *para );
/*	文字列２	*/
int	EGB_jisString( void *work,void *para );
/*	文字列２	*/
int	EGB_connectJisString( void *work,void *para );
/*	任意文字	*/
int	EGB_anyChar( void *work,int x,int y,void *pattern );

/*	システム情報関数	*/
/*	画面モードの取得	*/
int	EGB_getResolution( int *page0,int *page1 );
/*	書き込みページの取得	*/
int	EGB_getWritePage( void *work,unsigned short egb_seg );
/*	表示状態の取得	*/
int	EGB_getDisplayPage( int *priority,int *display );
/*	表示開始位置の取得	*/
int	EGB_getDisplayStart( int page,int mode,int *disp_x,int *disp_y );
/*	パレットレジスタの読み取り	*/
int	EGB_getPalette( int page,void *pal_ );
/*	画面モードに関する情報の取得	*/
int	EGB_getModeInfo( int mode,int *vx,int *vy,int *dx,int *dy,int *col );
/*  現在の表示画面サイズの取得	*/
int	EGB_getDisplayInfo( int page , void *buf );

/*	TownsMENU-Version2.1  メニュー色設定関数	*/
/*	メニュー色の設定１	*/
void	EGB_tmenuPalette( void *work );
/*	メニュー色の設定２	*/
void	EGB_tmenuPalette2( void *work );
/*	システム情報領域からのメニュー色の読み取り	*/
int	EGB_getTmenuPalette( void *data );
/*	システム情報領域へのメニュー色の書き込み	*/
void	EGB_setTmenuPalette( void *data );
/*	致命的エラーハンドラの登録	*/
void	TOS_setINT24( void );
/*	致命的エラーハンドラの登録解除	*/
void	TOS_resetINT24( void );
/*	環境変数の取得	*/
int	TOS_getPath( void *buf,void *cmpData,int cmpData_length );

/*	画面モード無依存システム情報関数	*/
/*	パラメータでの解像度の取得	*/
int	EGB_getResolutionIndex( int video,int totalPage,void *para );
/*	指定したページの解像度の取得	*/
int	EGB_getResolutionPage( int page,void *para );
/*	指定したピクセル数（色数）での解像度の取得	*/
int	EGB_getResolutionMax( int video,void *color,void *buf );
/*	画面モード番号での解像度の取得	*/
int	EGB_getResolutionMode( void *mode,void *buf );
/*	現在表示可能なページの取得	*/
int	EGB_getActivePage( int handle , void *display );
/*	パレット有効ビットの取得	*/
int	EGB_getActivePalette( int handle , void *palette );
/*	ＶＲＡＭ有効ビットの取得	*/
int	EGB_getActiveBit( int handle , void *buf );

/*	画面モード無依存セッティングオペレーション	*/
/*	ハンドルによる仮想画面の設定	*/
int	EGB_resolutionHandle( void *work,int crtc,int number );

/*	画面描画用スタック領域設定関数	*/
/*	スタック領域サイズの取得	*/
int	EGB_getStackSize( void *work,void *buf );
/*	スタック領域アドレスの設定	*/
int	EGB_setStackAddress( void *work,void *buf );
/*	スタック領域取得開放イベントの設定	*/
int	EGB_setStackEvent( void *work,void(*allocEvent)(),void(*freeEvent)() );
