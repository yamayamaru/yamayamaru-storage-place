/************************************************/
/* MOS.H      COPYRIGHT FPS LIMITED             */
/*                  1988-1994                   */
/************************************************/

#define MosWorkSize             4096
#define MosWorkSizeFullColor    6144
#define MosEventSave            36

/*	マウスインタフェースの動作開始	*/
int		MOS_start( void *work,int size );
/*	マウスインタフェースの動作終了	*/
int		MOS_end( void );
/*	マウスカーソルの表示／消去	*/
int		MOS_disp( int disp );
/*	マウスカーソル位置とボタンの読み取り	*/
int		MOS_rdpos( int *button,int *x,int *y );
/*	マウスカーソル位置の設定	*/
int		MOS_setpos( int x,int y );
/*	ボタンの押下情報の読み取り	*/
int		MOS_rdon( int number,int *status,int *count,int *x,int *y );
/*	ボタンの開放情報の読み取り	*/
int		MOS_rdopen( int number,int *status,int *count,int *x,int *y );
/*	マウスカーソルの水平移動範囲指定	*/
int		MOS_horizon( int minX,int maxX );
/*	マウスカーソルの垂直移動範囲指定	*/
int		MOS_vertical( int minY,int maxY );
/*	マウスカーソル形状の設定	*/
int		MOS_type( int mode,int x,int y,void *pattern );
/*	マウスの移動距離の読み取り	*/
int		MOS_motion( int *x,int *y );
/*	ユーザ定義サブルーチンの呼び出し条件の設定	*/
int		MOS_entsub( int condition,void(*edi)() );
/*	パルス数／画素比の設定	*/
int		MOS_pulse( int x,int y );
/*	マウス仮想画面の設定	*/
int		MOS_resolution( int page,int mode );
/*	マウス書き込み画面の設定	*/
int		MOS_writePage( int page );
/*	マウス表示色の設定	*/
int		MOS_color( int mode,int color );
/*	マウス表示タイルパターンの設定	*/
int		MOS_tilePattern( int x,int y,void *pattern );
/*	マウスインタフェースの動作開始	ポーリング用	*/
int		MOSP_start( void *work,int size );
/*	マウスインタフェースの動作終了	ポーリング用	*/
int		MOSP_end( void );
/*	マウスのポート読み込み	*/
int		MOS_int( void );
/*	マウスイベントルーチンの登録	*/
int		MOS_setEvent( void(*mouseEvent)() );
/*	マウスイベントの読みだし	*/
void	(*MOS_getEvent())();
/*	マウスタイマの読み込み		*/
unsigned int	MOS_getTime( void );
/*	マウスコントロールの登録	*/
int		MOS_setControl( int condition );
/*	マウスコントロールの取得	*/
int		MOS_getControl( void );
/*	マウスカーソルの水平消去範囲の指定	*/
int		MOS_viewHorizon( int mode,int minX,int maxX );
/*	マウスカーソルの垂直消去範囲の指定	*/
int		MOS_viewVertical( int mode,int minY,int maxY );
/*	マウスボタンの左右入れ換え	*/
int		MOS_btnXchg( int sw );
/*	マウスの加速度検出機能の有効／無効の設定	*/
int		MOS_acceleration( int sw );
/*	マウスカーソル形状の設定（システムＲＯＭパターン）	*/
int		MOS_typeRom( int number,int x,int y,void *work );
/*	ハンドルによるマウス仮想画面の設定	*/
int		MOS_resolutionHandle( int number );

/*	ハードウェア・カーソル関数	*/
/*	ハードウェア・カーソル有効／無効の設定	*/
int		MOS_setHwc( int sw );
/*	ハードウェア・カーソル有効／無効の取得	*/
int		MOS_getHwc( void );

/*	システム情報関数	*/
/*;	マウス表示／消去の状態読み取り	*/
int		MOS_getDisp( int *disp,int *level );
/*	サブルーチンの登録状態の読み取り	*/
int		MOS_getEntsub( int *term,void(**sub_offset)(),int *sub_seg );
/*	マウスの水平移動範囲の読み取り	*/
int		MOS_getHorizon( int *xmin,int *xmax );
/*	マウスの垂直移動範囲の読み取り	*/
int		MOS_getVertical( int *ymin,int *ymax );
/*	仮想画面の設定状態の読み取り	*/
int		MOS_getResolution( int *page0,int *page1 );
/*	書き込みページの読み取り	*/
int		MOS_getWritePage( void );
/*	パルス数／画素比の設定値の読み取り	*/
int		MOS_getPulse( int *x,int *y );
/*	マウスボタンの入れ換え状態の取得	*/
int		MOS_getBtnXchg( int *sw );
/*	マウスダブルクリック時間の書き込み	*/
int		MOS_checkTime( int time );
/*	マウスダブルクリック時間の読み取り	*/
int		MOS_getCheckTime( int *time );
/*	マウスの加速度検出機能の設定の取得	*/
int		MOS_getAcceleration( int *sw );
/*	マウスの動作状態の取得	*/
int		MOS_openCheck( int *flag );

/*	割り込み管理関数	*/
/*	マウス割り込みハンドラ登録状態の取得	*/
int		MOS_getIntStatus( void );
/*	マウスイベント・サブルーチンの退避	*/
void		MOS_saveEvent( void *buf );
/*	マウスイベント・サブルーチンの復元	*/
void		MOS_restoreEvent( void *buf );
