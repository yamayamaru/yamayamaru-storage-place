;**********************************************************
;*
;*                  垂直同期割り込みサンプル
;*
;*        1995.1.17                         by ちょもらんま
;*
;**********************************************************

;******************* セグメント関係 ***********************
VRAM2		equ	0104h
TBIOS		equ	0110h

;******************* 割り込みハードウェア関係 *************
VSYNCclear	equ	05cah
IMR_S		equ	0012h
OCW1_S		equ	0012h
OCW2_M		equ	0000h
OCW2_S		equ	0010h
EOIcommand	equ	00100000b    ;20h
WaitRegister	equ	006ch

;******************        BIOS関係        ****************
EGB_workSize	equ	1536
EGB		equ	20h
EGB_init	equ	00h
EndProcess	equ	4ch
GetNativeVector	equ	2502h
GetRealVector	equ	2503h
SetNativeAlways	equ	2506h
SetBothVector	equ	2507h
VsyncNumber	equ	4bh

;************************ その他 **************************
Ylines		equ	480
PaintColor	equ	0ffffffffh
HByte		equ	1024/2
MaxBar		equ	640/8
VsyncNotOccured	equ	0
VsyncOccured	equ	-1

		.386p
		assume	cs:flat,ds:flat,ss:stack1

;******************* ワークエリア *************************

flat		segment

EGB_work	db	EGB_workSize dup(?)

realVector	dd	?
nativeVectorOff	dd	?
nativeVectorSeg	dw	?
saveIMR_S	db	?
saveIMR_M	db	?

originalDS	dw	?
vsyncflag	dd	?

;********************** 初期化 ****************************

main		proc	near
		push	ds
		pop	gs
		push	TBIOS
		pop	fs
; EGB初期化
		mov	ah, EGB_init
		mov	edi, offset EGB_work
		mov	ecx, EGB_workSize
		call	pword ptr fs:EGB
; フラグ初期化
		mov	vsyncflag, VsyncNotOccured

;------------------ PICアクセス待ちウエイト ---------------
PicWait		macro
		out	WaitRegister, al ; 1μ秒ウエイト
		cmc
		cmc
		endm

;****************** VSYNC割り込み設定 *********************

;-------------------- 割り込み禁止 ------------------------
		cli

;--------------- 旧割り込みべクタの保存 -------------------
		mov	cl,VsyncNumber
; リアル側
		mov	ax, GetRealVector
		int	21h
		mov	realVector, ebx
; ネイティブ側
		mov	ax, GetNativeVector
		int	21h
		mov	nativeVectorOff, ebx
		mov	ax, es
		mov	nativeVectorSeg, ax

;-------------- 割り込みベクタの設定 ----------------------
		push	ds
		mov	edx, offset VsyncHandler
		mov	ax, cs
		mov	ds, ax
		mov	ax, SetNativeAlways
		int	21h
		pop	ds

;---------------- DSレジスタの保存 ------------------------
		mov	originalDS, ds

;----------- 割り込みマスタレジスタの設定 -----------------
		in	al, IMR_S
		PicWait
		mov	saveIMR_S, al
		and	al, 11110111b   ; VSYNC割り込み許可
		out	OCW1_S, al

;------------------- 割り込み許可 -------------------------
		sti

;**************** デモンストレーション ********************

; レジスタ表
;   EAX:色    EBX:描画回数     ECX:縦ライン数     EDX:VRAMアドレス
;   ESI, ESI, EBP:未使用

; 描画準備
		mov	eax, PaintColor
		push	VRAM2
		pop	es
		xor	ebx, ebx
waiting_loop:
		cmp	vsyncflag, VsyncOccured
		jne	short waiting_loop
; VRAMアドレス計算
		lea	edx, [ebx * 4] ; edx <= ebx * 4;
		mov	ecx, Ylines
; 描画
drawing_loop:
		mov	es:[edx], eax
		add	edx, HByte
		loop	drawing_loop

		mov	vsyncflag, VsyncNotOccured
		inc	ebx
		cmp	ebx, MaxBar
		jb	short waiting_loop

;******************* VSYNC割り込み終了 ********************

;--------------------- 割り込み禁止 -----------------------
		cli

;---------------- 割り込みマスクレジスタ復元 --------------
		mov	al, saveIMR_S
		out	OCW1_S, al

;------------------ 割り込みベクタ復元 --------------------
; リアル側
		mov	ebx, realVector
; ネイティブ側
		mov	edx, nativeVectorOff
		mov	ax, nativeVectorSeg
		mov	ds, ax

		mov	cl, VsyncNumber
		mov	ax, SetBothVector
		int	21h

;--------------------- 割り込み許可 -----------------------
		sti

;******************** プログラム終了 **********************
		mov	ah, EndProcess
		xor	al, al
		int	21h
main		endp

;******************* 割り込みハンドラ *********************

VsyncHandler	proc	near
		cli
		push	ds
		push	eax
		push	edx

;------ VSYNC割り込み原因クリアレジスタへの書き込み -------
		mov	dx, VSYNCclear
		out	dx, al

;------------------ DSレジスタの復元 ----------------------
		mov	ds, cs:originalDS

;--------------- 割り込みハンドラの処理 -------------------
; フラグを立てる
		mov	vsyncflag, VsyncOccured;

;--------------------- EOI発行 ----------------------------
; スレーブ側
		mov	al, EOIcommand
		out	OCW2_S, al
		PicWait
; マスタ側
		out	OCW2_M, al

		pop	edx
		pop	eax
		pop	ds
		iretd
VsyncHandler	endp

flat		ends

;********************** スタック設定 **********************

stack1		segment	stack
		db	2000 dup(?)
stack1		ends

		end
