/******************************************************************************
 *    EGB LIBRARY SAMPLE PROGRAM                                              *
 *                                                                            *
 *        機能 : 画面描画を行い、矩形の移動を行います。                       *
 *                                                                            *
 *    使用方法 : TownsMENU上からダブルクリック                                *
 *               マウスボタンクリックで終了                                   *
 *                                                                            *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
//#include <msdos.cf>
#include "..\include\egb.h"
#include "..\include\mos.h"

#define PIXEL  8                                    //  ピクセル数
#define PAGE   1                                    //  総ページ数（１or２）
#define WPAGE  0                                    //  書き込みページ
#define XDOT   128                                  //  移動矩形サイズＸ
#define YDOT   128                                  //  移動矩形サイズＹ

char    egb_work[EgbWorkSize];                      //  ＥＧＢ作業領域
char    mos_work[MosWorkSizeFullColor];             //  ＭＯＳ作業領域

int getds();

void main()
{
    RESOLUTION  buf[PAGE];
    char        para[16];
    char        pal[PAGE*8];
    char        *putbuf;
    int         i,b,r,g,tr,x,y,xx,yy,addx,addy;
    int         col,handle,length;

    if ( PIXEL == 4 )
        length = (XDOT+7)/8*PIXEL*YDOT;             //  16色モード専用処理
    else
        length = XDOT*PIXEL/8*YDOT;                 //  その他の色モード
    if ( ( putbuf = malloc( length ) ) == 0 )       //  作業領域が確保できない
        exit( -1 );

    DWORD( para+0 ) = PAGE;                         //  総ページ数
    DWORD( para+4 ) = PIXEL;                        //  ページ０のピクセル数
    if ( PAGE == 2 )
        DWORD( para+8 ) = PIXEL;                    //  ページ１のピクセル数
    else
        DWORD( para+8 ) = 0;
    handle = EGB_getResolutionMax( 0,para,buf );    //  最大解像度の取得
    if ( handle == -1 )                             //  最大解像度が存在しない
        exit( -1 );
    MOS_start( mos_work,MosWorkSizeFullColor );     //  マウスＢＩＯＳ初期化
    EGB_resolutionHandle( egb_work,0,handle );      //  仮想画面の設定
    MOS_resolutionHandle( handle );                 //  マウス仮想画面の設定
    EGB_displayPage( egb_work,WPAGE,WPAGE+1 );      //  表示ページの設定
    EGB_writePage( egb_work,WPAGE );                //  書き込みページの設定

    EGB_paintMode( egb_work,0x20 );                 //  面塗をベタ塗りに指定
    EGB_colorIGRB( egb_work,1,0 );                  //  背景色設定
    EGB_clearScreen( egb_work );                    //  画面クリア

    memset( pal,0,PAGE*8 );                         //  取得用配列をクリア
    EGB_getActivePalette( handle,pal );             //  パレット有効ビット取得
    if ( pal[0] != 0 ){                             //  パレットが有効なら
        b = r = g = 0;                              //  階調初期化
        for ( i=0 ; i<=(int)pal[0] ; i++ ){
            DWORD( para+0  ) = 1;                   //  パレット設定個数
            DWORD( para+4  ) = i;                   //  色識別番号
             BYTE( para+8  ) = b;                   //  青の階調
             BYTE( para+9  ) = r;                   //  赤の階調
             BYTE( para+10 ) = g;                   //  緑の階調
             BYTE( para+11 ) = 0;
            EGB_palette( egb_work,0,para );         //  グレイ・スケールに
            b += (int)pal[4];                       //  青の最小値加算
            r += (int)pal[5];                       //  赤の最小値加算
            g += (int)pal[6];                       //  緑の最小値加算
            }
        }
    EGB_paintMode( egb_work,0x02 );                 //  境界をベタ塗りに指定
    i = 0;                                          //  描画色初期化
    for ( y=0 ; y<YDOT ; y++ ){
        col = i;                                    //  描画色設定
        for ( x=0 ; x<XDOT ; x++ ){
            if ( pal[0] != 0 )                      //  パレットが有効なら
                EGB_color( egb_work,0,col );        //  色識別番号で前景色設定
            else                                    //  パレットが無効なら
                EGB_colorIGRB( egb_work,0,col );    //  ＩＧＲＢで前景色設定
            WORD( para+0 ) = 1;                     //  座標点数
            WORD( para+2 ) = x;                     //  Ｘ座標
            WORD( para+4 ) = y;                     //  Ｙ座標
            EGB_pset( egb_work,para );              //  ポイント
            col = ( col + 0x010101 ) & 0xffffff;    //  描画色加算
            }
        i = ( i + 0x010101 ) & 0xffffff;            //  １ライン毎に色をずらす
        }

    xx = yy = 0;                                    //  始点初期化
    addx = addy = 1;                                //  座標加算値初期化
    DWORD( para+0  ) = (unsigned int)putbuf;        //  データ格納領域アドレス
     WORD( para+4  ) = getds();                     //  データ格納領域セレクタ
     WORD( para+6  ) = xx;
     WORD( para+8  ) = yy;
     WORD( para+10 ) = xx+XDOT-1;
     WORD( para+12 ) = yy+YDOT-1;
    EGB_getBlock( egb_work,para );                  //  ドットデータの読み込み
    do {
        xx += addx;                                 //  始点Ｘ加算
        yy += addy;                                 //  始点Ｙ加算
        if ( xx >= buf[WPAGE].dx-XDOT+1 )           //  始点Ｘ>=表示画面Ｘ
            addx = -1;                              //  加算値を負の値に
        if ( yy >= buf[WPAGE].dy-YDOT+1 )           //  始点Ｙ>=表示画面Ｙ
            addy = -1;                              //  加算値を負の値に
        if ( xx <= 0 )                              //  始点Ｘ<=０
            addx = 1;                               //  加算値を正の値に
        if ( yy <= 0 )                              //  始点Ｙ<=０
            addy = 1;                               //  加算値を正の値に
        DWORD( para+0  ) = (unsigned int)putbuf;    //  データ格納領域アドレス
         WORD( para+4  ) = getds();                 //  データ格納領域セレクタ
         WORD( para+6  ) = xx;
         WORD( para+8  ) = yy;
         WORD( para+10 ) = xx+XDOT-1;
         WORD( para+12 ) = yy+YDOT-1;
        EGB_putBlock( egb_work,0,para );            //  ドットデータの書き込み
        MOS_rdpos( &tr,&x,&y );                     //  位置とボタンの読み取り
        } while ( tr == 0x00 );                     //  左右ボタンをチェック

    free( putbuf );                                 //  作業領域の開放
    MOS_end();                                      //  マウス動作終了
}

int getds() {
    int ret01;
    __asm {
        mov	ax, ds
	movzx	eax, ax
	mov	ret01, eax
    };
    return ret01;
}
