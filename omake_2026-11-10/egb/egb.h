#ifndef EGB_H
#define EGB_H

__attribute__((noinline)) int EGB_init( char *w, int size);    //         グラフィックBIOSの初期化
__attribute__((noinline)) int EGB_resolution( char *w, int page, int mode );    //                画面モードの設定
__attribute__((noinline)) int EGB_resolutionRAM( char *w, int page, int pixel, int width, int height, char *ram );  //        仮想RAM画面モードの設定
__attribute__((noinline)) int EGB_displayStart( char *w, int mode, int x, int y );    //          表示開始位置の設定
__attribute__((noinline)) int EGB_displayOffset( char *w, int sx, int sy );    //         表示画面の画面枠内の移動
__attribute__((noinline)) int EGB_screenOffset( char *w, int sx, int sy ); //             VRAM中の表示領域の開始座標
__attribute__((noinline)) int EGB_screenZoom( char *w, int wx, int wy ); //               画面の拡大
__attribute__((noinline)) int EGB_screenWidth( char *w, int wx, int wy ); //              画面の大きさ設定
__attribute__((noinline)) int EGB_viewport( char *w, char *param ); //            ビューポート設定
__attribute__((noinline)) int EGB_viewportRect( char *w, int sx, int sy, int ex, int ey ); //             ビューポート設定
__attribute__((noinline)) int EGB_palette( char *w, int syncf, char *param ); //          パレットの設定
__attribute__((noinline)) int EGB_setPalette( char *w, int num, int g, int r, int b ); //         1 つのパレット設定
__attribute__((noinline)) int EGB_writePage( char *w, int page ); //              書き込みページの設定
__attribute__((noinline)) int EGB_displayPage( char *w, int prior, int page ); //         表示ページの設定

__attribute__((noinline)) int EGB_color( char *w, int mode, int color ); //               描画色の設定(パレットコード)
__attribute__((noinline)) int EGB_foreColor( char *w, int col ); //               前景色の設定(パレ ットコード)
__attribute__((noinline)) int EGB_backColor( char *w, int col );    //            背景色の設定(パレ ットコード)
__attribute__((noinline)) int EGB_paintColor( char *w, int col );  //             面塗り色の設定(パ レットコード)
__attribute__((noinline)) int EGB_transColor( char *w, int col );  //             透過色の設定(パレ ットコード)
__attribute__((noinline)) int EGB_colorIGRB( char *w, int mode, int col ); //             描画色の設定(IGRBコード)
__attribute__((noinline)) int EGB_foreColorIGRB( char *w, int col );   //         前景色の設定(IGRB コード)
__attribute__((noinline)) int EGB_backColorIGRB( char *w, int col );   //         背景色の設定(IGRB コード)
__attribute__((noinline)) int EGB_paintColorIGRB( char *w, int col );   //                面塗り色の設定(IGRBコード)
__attribute__((noinline)) int EGB_transColorIGRB( char *w, int col );   //                透過色の設定(IGRBコード)
__attribute__((noinline)) int EGB_pastel( char *w, int level );      //           Pastelモード描画の混色比
__attribute__((noinline)) int EGB_writeMode( int mode );          //              描画モードの設定

__attribute__((noinline)) int EGB_linePattern( char *w, int mode, int pattern );     //[0B]               線分パターンの設定
__attribute__((noinline)) int EGB_paintMode( char *w, int mode );     // [0C]             ペイントモードの設定
__attribute__((noinline)) int EGB_hatchingPattern( char *w, int mode, int x, int y, char *pat );     // [0D]              ハッチングパターン設定
__attribute__((noinline)) int EGB_tilePattern( char *w, int mode, int x, int y, char *pat );     // [0E]          タイルパターン設定
__attribute__((noinline)) int EGB_maskRegion( char *w,char *pat );    // [0F]             画面マスク領域の設定
__attribute__((noinline)) int EGB_mask( char *w, int mode );    // [10]           マスクモードの設定
__attribute__((noinline)) int EGB_maskBit( char *w, int bit );     //[14]         マスクビットの設定

//egb_s4.o
__attribute__((noinline)) int EGB_pen( char *w, int penmode );    // [11]         描画ペンのモード設定
__attribute__((noinline)) int EGB_penSize( char *w, int pensize );    // [12]             描画ペンの太さ設定
__attribute__((noinline)) int EGB_penStyle( char *w, char *pattern );    // [13]          描画ペン形状定義

//egb_s5.o
__attribute__((noinline)) int EGB_textDirection( char *w, int dir );    // [15]           文字方向の設定
__attribute__((noinline)) int EGB_textDisplayDirection( char *w, int dir );     // [16]           文字表示方向の設定
__attribute__((noinline)) int EGB_textSpace( char *w, int space );     // [17]            文字間空白の設定
__attribute__((noinline)) int EGB_textZoom( char *w, int mode, int x, int y );     //[18]         文字拡大率の設定
__attribute__((noinline)) int EGB_fontStyle( char *w, int style);    // [19]              字体の設定
__attribute__((noinline)) int EGB_textAttrib( char *w, int mx, int my, int style, int dir, int ddir, int space );    // [**]          文字属性の一括設定
__attribute__((noinline)) int EGB_superImpose( char *w, int mode );    // [1A]            スーパーインポーズ
__attribute__((noinline)) int EGB_digitize( char *w, int mode);    // [1B]                ディジタイズ

//egb_b1.o
__attribute__((noinline)) int EGB_clearScreen( char *w );     //  [20]            VRAM領域の消去
__attribute__((noinline)) int EGB_partClearScreen( char *w );     //  [21]                ビューポート領域の消去
__attribute__((noinline)) int EGB_clearVRAM( int page );     // [**]              VRAMのダイレクトゼロクリア

// egb_b2.o
__attribute__((noinline)) int EGB_getBlockColor( char *w, char *param );     // [22]              矩形領域読みだし(色指定)
__attribute__((noinline)) int EGB_putBlockColor( char *w, int clip, char *param );     // [23]            矩形領域書き込み(色 指定)
__attribute__((noinline)) int EGB_getBlock( char *w, char *param );     // [24]           矩形領域読みだし(フルカラー)
__attribute__((noinline)) int EGB_putBlock( char *w, int clip, char *param );     // [25]         矩形領域書き込み(フルカラー)
__attribute__((noinline)) int EGB_getBlockZoom( char *w, char *param );    // [26]                矩形領域読みだし(ズーム付)
__attribute__((noinline)) int EGB_putBlockZoom( char *w, int clip, char *param );     // [27]             矩形領域書き込み(ズ ーム付)

//egb_b3.o
__attribute__((noinline)) int EGB_getRectColor( char *w, char *buf,int sx, int sy,int ex,int ey, int color);    // [22]       矩形領域読込(色指定)
__attribute__((noinline)) int EGB_putRectColor( char *w, int clip, char *buf, int sx, int sy, int ex, int ey); //  [23]       矩形領域書き込み(色指定)
__attribute__((noinline)) int EGB_getRect( char *w, char *buf, int sx, int sy, int ex, int ey );    // [24]               矩形領域読込(フルカラー)
__attribute__((noinline)) int EGB_putRect( char *w, int clip, char *buf, int sx, int sy, int ex, int ey );     // [25]        矩形領域書き込み

//egb_b4.o
__attribute__((noinline)) int EGB_graphicCursor(int clip, char *param);    // [28]                グラフィックカーソル
__attribute__((noinline)) int EGB_maskData( char *w, int clip, char *param );    // [29]          マスクデータの書き込み
__attribute__((noinline)) int EGB_scroll( char *w, int mode, int dx, int dy );      // [2A]               全画面スクロール
__attribute__((noinline)) int EGB_partScroll( char *w, int mode, int dx, int dy, char *param );   // [2B]         部分スクロール

//egb_b5.o
__attribute__((noinline)) int EGB_region( char *w, int *in, int *work, int *x, int *y, int *x2, int *y2, char *poly );    // [2C]             領域設定
__attribute__((noinline)) int EGB_copy( char *w, int mode, char *param, char *work );      //  [2D]               画面の複写
__attribute__((noinline)) int EGB_rotate( char *w,int mode, char *param,  char *work );    //   [2E]              画面の回転
__attribute__((noinline)) int EGB_resolve( char *w, char *work );    // [2F]              画面のぼかし

//egb_g1.o
__attribute__((noinline)) int EGB_pset( char *w, char *parm );    // [40]         ポイント
__attribute__((noinline)) int EGB_connect( char *w, char *param);     // [41]             連続線分
__attribute__((noinline)) int EGB_unConnect( char *w, char *param );    // [42]           不連続線分
__attribute__((noinline)) int EGB_polygon( char *w, char *param );      // [43]           ポリゴン
__attribute__((noinline)) int EGB_rectangle( char *w, char *param );    // [46]           矩形
__attribute__((noinline)) int EGB_circle( char *w, char *param );    //   [47]                    円

// egb_g2.o
__attribute__((noinline)) int EGB_rotatePolygon( char *w, char *param );    // [44]               回転多角形
__attribute__((noinline)) int EGB_triangle( char *w, char *param );     // [45]           三角形
__attribute__((noinline)) int EGB_arc( char *w, char *param );     // [48]                円弧
__attribute__((noinline)) int EGB_fan( char *w, char *param );     // [49]                扇形
__attribute__((noinline)) int EGB_ellipse( char *w, char *param ); // [4A]                楕円
__attribute__((noinline)) int EGB_ellipticArc( char *w, char *param );     //[4B]         楕円弧
__attribute__((noinline)) int EGB_ellipticFan( char *w, char *param );    // [4C]         楕扇形
__attribute__((noinline)) int EGB_bow( char *w, char *param );    // [50]         弓形(通過3点指定)
__attribute__((noinline)) int EGB_semiBow( char *w, char *param );   // [51]              弓形(半径 指定)

// egb_g3.o
__attribute__((noinline)) int EGB_paint( char *w, char *param );    // [4D]               ペイント( 境界色指定)
__attribute__((noinline)) int EGB_closePaint( char *w, char *param );    // [4E]          ペイント( 色変更)
__attribute__((noinline)) int EGB_point( char *w, int mode, int x, int y , int *result);   // [4F]                ポイント色を得る

__attribute__((noinline)) int EGB_singlePset( char *w, int x, int y ); // [40]            一点描画
__attribute__((noinline)) int EGB_singleLine( char *w, int sx, int sy, int ex, int ey );    // [41]               一本線描画
__attribute__((noinline)) int EGB_box( char *w, int sx, int sy, int ex, int ey );    // [46]              四角を描く
__attribute__((noinline)) int EGB_maru( char *w, int x, int y, int r );      //  [47]             まるを描く

__attribute__((noinline)) int EGB_sjisString( char *w, char *param ); // [60]             漢字文字列
__attribute__((noinline)) int EGB_connectSjisString( char *w, char *param );   // [61]            追加漢字文字列
__attribute__((noinline)) int EGB_asciiString( char *w, int font, char *param );   // [62]                JIS8ビット文字列
__attribute__((noinline)) int EGB_connectAsciiString( char *w, int font, char *param );    // [63]                追加JIS文字 列
__attribute__((noinline)) int EGB_jisString( char *w, char *param );    // [64]           JISコード 文字列
__attribute__((noinline)) int EGB_connectJisString( char *w, char *param );   // [65]     追加JISコ ード文字列
__attribute__((noinline)) int EGB_anyChar( char *w, int wx, int wy,  char *pattern );  // [66]            任意文字表示

__attribute__((noinline)) int EGB_putSjis( char *w, int x, int y,  char *str ); //                文字列の表示
__attribute__((noinline)) int EGB_putSjisAdd( char *w, char *str );        //             文字列の追加表示

__attribute__((noinline)) void screenterm( char *w );

/*
        header for registers
*/

/* double word registers */
#pragma pack(push, 1)
struct DWORDREGS {
        unsigned int eax;
        unsigned int ebx;
        unsigned int ecx;
        unsigned int edx;
        unsigned int esi;
        unsigned int edi;
        unsigned int eflag;
        };
/* word registers */
struct WORDREGS {
        unsigned short ax;
        unsigned short __ax;
        unsigned short bx;
        unsigned short __bx;
        unsigned short cx;
        unsigned short __cx;
        unsigned short dx;
        unsigned short __dx;
        unsigned short si;
        unsigned short __si;
        unsigned short di;
        unsigned short __di;
        unsigned short cflag;
        unsigned short __cflag;
        };
/* byte registers */
struct BYTEREGS {
        unsigned char al, ah;
        unsigned short __ax;
        unsigned char bl, bh;
        unsigned short __bx;
        unsigned char cl, ch;
        unsigned short __cx;
        unsigned char dl, dh;
        unsigned short __dx;
        };

union REGS {
        struct DWORDREGS        e;
        struct WORDREGS         x;
        struct BYTEREGS         h;
        };

/* segment registers */
struct SREGS {
        unsigned short es;
        unsigned short cs;
        unsigned short ss;
        unsigned short ds;
        unsigned short fs;
        unsigned short gs;
        };
#pragma pack(pop)
//    旧TOWNSユーザのみその恩恵にあずかれる、デバッグランプ(またの名をレベルメー
//  タとも言う)を点灯する関数｡ パラメータの下位4ビットが有効｡

__attribute__((noinline))   void LAMP( int data );


//    キーポートのデータを読み取る関数。これはキーのマトリクスアドレスを返すも
//  ので、アスキーコードではないことに注意｡

__attribute__((noinline))   int INKEY(void);


//    さらに893なCRTCのレジスタを設定する関数です。reg にレジスタ番号､ data に
//  16ビットのデータをセットします。

__attribute__((noinline))   void CRTC( int reg, int data );


__attribute__((noinline))   int INPB( int addr );
__attribute__((noinline))   int INPW( int addr );
__attribute__((noinline))   void OUTPB( int addr, int data );
__attribute__((noinline))   void OUTPW( int addr, int data );


//     native : プロテクトモードアドレス
//     len : アドレスの示す先のデータのサイズ
//     seg,ofs :  リアルモードのセグメントとオフセット

//   プロテクトモードのアドレスから，リアルモードのfarアドレスに変換する。
//   DOS-ExtenderのINT 21h AX=250fhを使用。

__attribute__((noinline))   int nativetoreal( char *native,int len, int *seg, int *ofs );


//   現在のセグメントレジスタの値を得る。
__attribute__((noinline))   int segread( struct SREGS *seg );


//        num : INT番号
//        sregs : リアル側に渡すセグメントレジスタ
//    INT XXHを発行して，リアルモードのBIOS等を呼び出す。
//    DOS-ExtenderのINT 21h AX=2511hを使用｡
__attribute__((noinline))   int intreal(int num, union REGS *cregs, union REGS *rregs, struct SREGS *sregs );


//      cregs : DOSに渡すレジスタ値
//      rregs : DOSから帰るレジスタ値

//    DOSの INT 21hシステムコールを発行する。

__attribute__((noinline))   int intdos( union REGS *cregs, union REGS *rregs );


//   ページ指定による，全VRAM領域のゼロクリア｡ 単ページの画面モードの場合は
//   page=0,1の2回呼べば全面クリアされる。

__attribute__((noinline))   void VRAM_clear( int page );                  // VRAMのゼロクリア

//    VRAMのアドレス指定による，アクセス関数。
//    2ページモードでのセレクタ0x104を使用｡
__attribute__((noinline))   void VRAM_putPixelW( int pos, int color );    // VRAMへのワード書き込み
__attribute__((noinline))   void VRAM_getPixelW( int pos );               // VRAMデータのワード読込
__attribute__((noinline))   void VRAM_putPixelB( int pos, int color );    // VRAMへのバイト書き込み
__attribute__((noinline))   void VRAM_getPixelB( int pos );               // VRAMデータのバイト読込

//   VRAMのアドレス指定による，アクセス関数。
//   単ページモードでのセレクタ0x10Cを使用｡
__attribute__((noinline))   void VRAM_putPixelW2( int pos, int color );   // VRAMへのワード書き込み
__attribute__((noinline))   void VRAM_getPixelW2( int pos );              // VRAMデータのワード読込
__attribute__((noinline))   void VRAM_putPixelB2( int pos, int color );   // VRAMへのバイト書き込み
__attribute__((noinline))   void VRAM_getPixelB2( int pos );              // VRAMデータのバイト読込


__attribute__((noinline)) int allocate_msdos_memory_block(unsigned int seg_size01, unsigned int *segment01);
//    ret: success = 0, failed != 0


__attribute__((noinline)) int free_msdos_memory_block(unsigned int segment01);
//     ret: success = 0, failed != 0


 __attribute__((noinline)) void pokeb_far(unsigned int segment, unsigned int offset, unsigned int data);
 __attribute__((noinline)) void pokew_far(unsigned int segment, unsigned int offset, unsigned int data);
 __attribute__((noinline)) void pokel_far(unsigned int segment, unsigned int offset, unsigned int data);

__attribute__((noinline)) unsigned int peekb_far(unsigned int segment, unsigned int offset);
__attribute__((noinline)) unsigned int peekw_far(unsigned int segment, unsigned int offset);
__attribute__((noinline)) unsigned int peekl_far(unsigned int segment, unsigned int offset);



#define   BYTE(x)      *((unsigned char  *)((unsigned char *)(x)))
#define   WORD(x)      *((unsigned short *)((unsigned char *)(x)))
#define   DWORD(x)     *((unsigned int   *)((unsigned char *)(x)))



#endif // EGB_H
