/***************************************************************/
/*                                                             */
/* SPR.H                                                       */
/*            COPYRIGHT (C) 1994 FUJITSU LIMITED               */
/*                                                             */
/***************************************************************/

/* 初期化 */
int SPR_init();
/* 画面の表示 */
int SPR_display(int n,int num) ;
/* スプライトの定義 */
int SPR_define(int mode,int num,int x,int y,char *pat);
/* パレットブロックの設定 */
int SPR_setPaletteBlock(int block,int num,char *pal);
/* 位置指定 */
int SPR_setPosition(int size,int num,int x,int y,int xdot,int ydot);
/* アトリビュート設定 */
int SPR_setAttribute(int num,int x,int y,int attr,int col);
/* 移動指定 */
int SPR_setMotion(int num,int x,int y,int xm,int ym);
/* オフセット指定 */
int SPR_setOffset(int xm,int ym);
/*  アトリビュートの読み込み */
int SPR_readAttribute(int num,int x,int y, char *attr);  
