SuperASCII 1990年11月号に掲載の連載記事「386 de MS-DOS」のサンプルプログラムです

この号は簡易的なDOSエクステンダーのサンプルプログラムが掲載されてました

gx_ext01.asmが記事掲載のサンプルプログラムのソースコードです

gx_ext01.batがMASM Ver5.1以上でのアセンブルを実行するバッチファイル

gx_ext01.lstがMASM Ver5.1でアセンブルしてできたリスティングファイル


TASMではアセンブルがうまくいきません
必ずMASM Ver5.1以降でアセンブルしてください

公開されたMSDOS Ver4.0のソースコードの中のTOOLというディレクトリに
MASM Ver5.1が含まれていますのでそれを利用してもいいです

実行する際は、EMM386などのプロテクトモードを使うプログラムを起動しない状態で実行してください
プログラム自体はキー入力を受け付けて入力された文字を表示するだけの
簡単なプログラムのようなので、実行しなくてもいいかもしれないです

プログラムはPC/AT互換機とPC9801用です
PC9801の方はソースコードの
_SYSTEM_PC9801 =	0	; NEC PC9801 seris
_SYSTEM_PCAT   =	1	; IBM-PC/AT compatible arch.
の部分で、_SYSTEM_PC9801を1、_SYSTEM_PCATを0にしてからアセンブルしてください



画像は記事を撮影したものです
解説が掲載されているのでソースコードと一緒に読んでみてください




GDT、IDT、LDTの設定が面倒なだけで実際にプロテクトモードへ移行する部分はこんなに簡単です

1.リアルモードのセグメントレジスタを保存
2.32bitのlgdt命令を実行
3.32bitのlidt命令を実行
4.cr0レジスタのビット0を1にする
5.ジャンプ命令を実行することで命令キューをクリア
6.プロテクトモードのセグメントセレクタ値をセグメントレジスタにロード

これだけです。


		assume	cs:_TEXT, ds:DGROUP, ss:DGROUP
_TEXT		segment
;** MACRO _GotoProtMode <dst>
;** desc : Enter protect mode and goto <dst>
;** 	CS := _TEXT_GX,   DS, ES, SS := DGROUP
;** 	SS:(ESP) points to same addr before and after mode switching
_GotoProtMode	macro	dst
		push	word ptr SEG_GX_CODE	;; simulate 16-bit mode far call
		push	word ptr (offset dst - offset TextGxOffset0)
		jmp	near ptr GotoProtMode
		endm
;** actual mode switching code **
		align	4
GotoProtMode	proc	far
		mov	RealModeSegDS, ds	; save real mode segment
		mov	RealModeSegES, es
		mov	RealModeSegSS, ss
		push	eax			; save eax
		db	66h			; 32-bit form LGDT inst
		lgdt	GdtPtr
		db	66h			; 32-bit form LGDT inst
		lidt	IdtPtr
		mov	eax, cr0		; protection enable
		or	al, 1			; set bit-0 (PE)
		mov	cr0, eax
		jmp	short GotoProtMode1	; flush instruction queue
		align	4
GotoProtMode1:	mov	ax, SEG_LDT		; setup LDT
		lldt	ax
		mov	ds, ProtModeSegDS	; setup protect mode segment
		mov	es, ProtModeSegES
		mov	ss, ProtModeSegSS
		pop	eax
		ret				; 16bit-mode far return
GotoProtMode	endp






プロテクトモードからリアルモードへの移行はも同様です

1.プロテクトモードのセグメントレジスタを保存
2.16bitのlidtを実行
3.CR0レジスタのビット0を0にする
4.ジャンプ命令を実行して命令キューをクリア
5.リアルモードのセグメントレジスタ値を各セグメントレジスタにロード

これだけです


;** MACRO _GotoRealMode <dst>
;** desc : back to real mode and goto <dst>
;** 	CS := _TEXT,   DS, ES, SS := DGROUP
;** 	SS:(E)SP points to same addr before and after mode switching
_GotoRealMode	macro	dst
		push	word ptr (offset dst)
		db	66h, 0eah		; use16 jmp far ptr GotoRealMode
		dw	offset GotoRealMode, SEG_64K_CODE
		endm
;** MACRO _CallRealMode <dst>
;** desc : call real mode routine at <_TEXT>:<dst>
;** 	<dst> routine must be return with 16bit-near-ret instruction
_CallRealMode	macro	dst
		local	ProtReturnAddr
		push	word ptr SEG_GX_CODE	;; simulate 32-bit mode far call
		push	word ptr (offset ProtReturnAddr - offset TextGxOffset0)
		push	word ptr (offset GotoProtMode)	;; return addr
		push	word ptr (offset dst)
		db	66h, 0eah		;; use16 jmp far ptr GotoRealMode
		dw	offset GotoRealMode, SEG_64K_CODE
		align	4
ProtReturnAddr	label	near
		endm
;** actual mode switching code **
		align	4
GotoRealMode	proc	near
		pushf
		cli
		mov	ProtModeSegDS, ds	; save protect mode segment
		mov	ProtModeSegES, es
		mov	ProtModeSegSS, ss
		push	eax			; save eax
		mov	ax, SEG_64K_DATA	; setup segment descriptor cache
		mov	ds, ax
		mov	es, ax
		mov	ss, ax
		db	66h			; 32-bit form LIDT inst
		lidt	IdtRealPtr		; load real mode IDT
		mov	eax, cr0		; protection disable
		and	al, 0feh		; clear bit-0 (PE)
		mov	cr0, eax
		db	0eah			; jmp far ptr GotoRealMode1
		dw	offset GotoRealMode1, _TEXT	; flush queue & setup CS
		align	4
GotoRealMode1:	mov	ds, RealModeSegDS	; setup real mode segment
		mov	es, RealModeSegES
		mov	ss, RealModeSegSS
		pop	eax
		popf
		ret				; near return
GotoRealMode	endp

