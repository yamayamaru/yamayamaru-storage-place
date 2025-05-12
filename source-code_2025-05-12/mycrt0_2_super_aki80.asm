; モジュール名とグローバル宣言は一緒です。
	.module	crt0
	.globl	_main, l__DATA, s__DATA, l__INITIALIZER, s__INITIALIZED, s__INITIALIZER

	; リンカへのエリア指定ですが、使うものだけでも構いません。
	; コード(_CODE)、初期化ルーチン(_GSINIT)、データ(_DATA)はsdccで出力するので必須です。
	; _GSFINALは初期化ルーチンから戻るために必須です。_DATAFINALは別に必須ではないのですが、
	; 大域変数の初期化時にデータ領域の終了アドレスが必要なので入れています。
    .area   _HOME
    .area   _CODE
    .area   _INITIALIZER
    .area   _GSINIT
    .area   _GSFINAL

    .area   _DATA
    .area   _INITIALIZED
    .area   _BSEG
    .area   _BSS
    .area   _HEAP

	; コードセグメントの最初の部分です。
	; 0番地から飛んできませんが、リンカの--code-loc指定でここから開始するので問題ありません。
	.area	_CODE
init:
		ld		sp, #0xf7fe
	; グローバル変数初期化ルーチンを呼び出します。
	call	gsinit
	; メインに飛びます。
	; メインがretするとプログラム自体の呼び出し元に返ります。
	jp	_main
	
    .area   _GSINIT
gsinit::

    ; Default-initialized global variables.
        ld      bc, #l__DATA
        ld      a, b
        or      a, c
        jr      Z, zeroed_data
        ld      hl, #s__DATA
        ld      (hl), #0x00
        dec     bc
        ld      a, b
        or      a, c
        jr      Z, zeroed_data
        ld      e, l
        ld      d, h
        inc     de
        ldir
zeroed_data:

    ; Explicitly initialized global variables.
    ld  bc, #l__INITIALIZER
    ld  a, b
    or  a, c
    jr  Z, gsinit_next
    ld  de, #s__INITIALIZED
    ld  hl, #s__INITIALIZER
    ldir

gsinit_next:

    .area   _GSFINAL
    ret

