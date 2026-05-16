;		name	GX
		.386p
;**** G-Extender ver 0.0.1 ****

;=== SYSTEM SELECTION ===
_SYSTEM_PC9801 =	0	; NEC PC9801 seris
_SYSTEM_PCAT   =	1	; IBM-PC/AT compatible arch.

;=== SEGMENT DEFINITIONS ===
_TEXT		segment para public use16 'CODE'
_TEXT		ends
;** GX data part **
_DATA		segment para public use16 'DATA'
_DATA		ends
STACK		segment para stack use16
STACK		ends
;** data is shared by both mode **
DGROUP		group	_DATA, STACK
;** 32-bit protect mode **
_TEXT_GX	segment para public use32
TextGxOffset0	label	byte
_TEXT_GX	ends
;** users address space (use32) **
_TEXT_USER	segment para public use32
_TEXT_USER	ends
_DATA_USER	segment para public use32
_DATA_USER	ends

;** 80386/80486 segment descriptor **
_DESC		struc		; descriptor skelton
_LIM_0_15	dw	0	; segment limit
_BAS_0_15	dw	0	; base (0 - 15)
_BAS_16_23	db	0	; base (16 - 23)
_ACCESS		db	0	; access right byte
_GRAN		db	0	; granurity
_BAS_24_31	db	0	; base (24 - 31)
_DESC		ends

;=== segment selector definition ===
SEG_ALIAS	=	08h		; alias selector offset
;** GDT section **
SEG_NULL	=	00h		; null selector
SEG_GDT		=	08h		; GDT
SEG_LDT		=	10h		; LDT
SEG_LDT_AL	=	SEG_LDT+SEG_ALIAS
SEG_IDT		=	20h		; IDT
SEG_FLAT	=	28h		; physical memory (4GB flat)
SEG_64K_CODE	=	30h		; my use16 code (_TEXT)
SEG_64K_DATA	=	38h		; my use16 data (DGROUP)
SEG_GX_CODE	=	40h		; my use32 code (_TEXT_GX)
SEG_GX_DATA	=	48h		; my use32 data (DGROUP)
;** LDT section **
SEG_USER_CODE	=	04h		; user code space
SEG_USER_TEXT	=	0ch		; user text space
SEG_SYSCALL	=	14h		; call gate to system-call

_SystemCallGate	macro			; call-gate to SEG_SYSCALL
		db	0eah, 0, 0, 0, 0; jmp far SEG_SYSCALL:0
		dw	SEG_SYSCALL
		endm

_DATA		segment
;** GDT **
GdtBase		label	byte
GdtDescNull	_DESC	<>		; null segment
GdtDescGdt	_DESC	<_GDT_SIZE-1,?,?,92h,40h,?>	; GDT itself
GdtDescLdt	_DESC	<_LDT_SIZE-1,?,?,82h,000,?>	; LDT
GdtDescLdtAl	_DESC	<_LDT_SIZE-1,?,?,92h,40h,?>	; LDT alias
GdtDescIdt	_DESC	<_IDT_SIZE-1,?,?,92h,40h,?>	; IDT alias
GdtDescFlat	_DESC	<0ffffh,?,?,92h,0cfh,?>		; 4GB flat data segment
GdtDesc1MCode	_DESC	<0ffffh,?,?,9ah,000h,?>		; 64KB flat code segment
GdtDesc1MData	_DESC	<0ffffh,?,?,92h,000h,?>		; 64KB flat data segment
GdtDescGxCode	_DESC	<0ffffh,?,?,9ah,0cfh,?>		; kernel code segment
GdtDescGxData	_DESC	<0ffffh,?,?,92h,0cfh,?>		; kernel data segment
_GDT_SIZE	=	($ - GdtBase)
;** LDT **
		align	16
LdtBase		label	byte
LdtDescUserCode _DESC	<0ffffh,?,?,9ah,0cfh,?>		; user's code space
LdtDescUserData _DESC	<0ffffh,?,?,92h,0cfh,?>		; user's data space
LdtDescSysCall	_DESC	<SystemCall-TextGxOffset0,SEG_GX_CODE,0,8ch,0,0>

_LDT_SIZE	=	($ - LdtBase)
;** IDT **
		align	16
_IdtEntryMacro	macro	n
		_DESC	<IdtEntry&n&-TextGxOffset0,SEG_GX_CODE,0,8eh,0,0>
		endm
IdtBase		label	byte
_IdtEntryNumber	=	0
	rept	256
		_IdtEntryMacro	%_IdtEntryNumber
_IdtEntryNumber	=	_IdtEntryNumber + 1
	endm
_IDT_SIZE	=	($ - IdtBase)
		align	4
GdtPtr		label	fword		; GDT base & limit
GdtPtrLimit	dw	_GDT_SIZE-1
GdtPtrBase	dd	GdtBase
IdtPtr		label	fword		; protect mode IDT
IdtPtrLimit	dw	_IDT_SIZE-1
IdtPtrBase	dd	0h
IdtRealPtr	label	fword		; real mode IDT
IdtRealPtrLimit dw	4*256-1
IdtRealPtrBase	dd	0h
;** segment register save area, used by kernel **
RealModeSegDS	dw	?		; initial value are set by init routine
RealModeSegES	dw	?
RealModeSegSS	dw	?
; protect mode save area
ProtModeSegDS	dw	SEG_GX_DATA	; default segment selector
ProtModeSegES	dw	SEG_GX_DATA
ProtModeSegSS	dw	SEG_GX_DATA
;
EndStr		db	'GX End....', 0dh, 0ah, '$'

_DATA		ends
							; 386 call-gate for system call

STACK		segment
StackBottom	label	byte
		db	4096 dup(?)
StackTop	dd	?
STACK		ends

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
;** Main routine **
begin:		mov	ax, DGROUP
		mov	ds, ax
		mov	ss, ax
		lea	sp, StackTop
		cli
		call	ProtectSetup		; setup misc data structure
		call	EnableA20		; enable A20 line
		_GotoProtMode	ProtSampCode
GxTerminate:	sti
		mov	dx, offset EndStr	; ending message
		mov	ah, 9h
		int	21h

		mov	ax, 4c00h		; program terminate
		int	21h
;
KeyInput	proc	near			; read char without echo back
		mov	ah, 08h
		int	21h			; result char --> AL
		ret				; return to protect mode
KeyInput	endp

;== ProtectSetup : Setup misc data for protect mode management
ProtectSetup	proc	near
;** setup for LGDT, LIDT instruction
		xor	ebx, ebx		; compute linear address of GDT base
		mov	bx, ds
		shl	ebx, 4
		lea	eax, GdtBase[ebx]
		mov	[GdtPtrBase], eax
		lea	eax, IdtBase[ebx]	; linear address of IDT base
		mov	[IdtPtrBase], eax
;** setup each segment base
_CompLinearAddr	macro	seg,off			;; compute lineaar address, seg:off ==> eax
	ifb	<off>
		xor	eax, eax
	  ifnb	<seg>
		mov	ax, seg
		shl	eax, 4
	  endif
	else
		xor	eax, eax
		mov	ax, seg
		shl	eax, 4
		lea	eax, off[eax]
	endif
		endm
_SetDescBase	macro	seg,off,dst		;; setup each field
		_CompLinearAddr	seg,off		;; linear addr ==> eax
		mov	[dst]._BAS_0_15, ax
		shr	eax, 16
		mov	[dst]._BAS_16_23, al
		mov	[dst]._BAS_24_31, ah
		endm
;** setup each descriptor in GDT **
		_SetDescBase	DGROUP, GdtBase, GdtDescGdt
		_SetDescBase	DGROUP, LdtBase, GdtDescLdt
		_SetDescBase	DGROUP, LdtBase, GdtDescLdtAl
		_SetDescBase	DGROUP, IdtBase, GdtDescIdt
		_SetDescBase	,,GdtDescFlat
		_SetDescBase	_TEXT, , GdtDesc1MCode
		_SetDescBase	DGROUP, , GdtDesc1MData
		_SetDescBase	_TEXT_GX, , GdtDescGxCode
		_SetDescBase	DGROUP, , GdtDescGxData
		_SetDescBase	_TEXT_USER, ,LdtDescUserCode
		_SetDescBase	_DATA_USER, ,LdtDescUserData
		ret
ProtectSetup    endp
;
EnableA20	proc	near
	if _SYSTEM_PC9801
PORT_A20	=	0f2h
		out	PORT_A20, al
	endif
	if _SYSTEM_PCAT
		mov	al, 0d1h		; this is buggy method, sorry
		out	064h, al
		mov	cx, -1
		loop	$
		mov	al, 0ffh
		out	064h, al
		mov	cx, -1
		loop	$
	endif
		ret
EnableA20	endp

;** Real mode interface routine, call by 'ProtIntHandler' **
;   get interrupt number, issue INT ??h, return to protect mode
RealIntHandler	proc	near
		push	ax
		mov	al, 2+6+8[esp]		; get interrupt vector number
				; 2 := 'push ax'
				; 6 := '_CallRealMode' return information
				; 8 := 'pushed 2 segment registers
		mov	byte ptr cs:RIHIntVector, al
		jmp	short RealIntHandler1	; flush instruction queue
		db	16 dup (0)
		align	4
RealIntHandler1:
		pop	ax
		db	0cdh			; INT ??h
RIHIntVector	db	?
		ret				; near return
RealIntHandler	endp

_TEXT		ends

		assume	cs:_TEXT_GX
_TEXT_GX	segment
;** Protect modo Interrupt handler **
_ProtIntHandler	macro	n
IdtEntry&n&:	push	dword ptr n		; push interrupt number
		jmp	near ptr ProtIntHandler	; re-issue it in real mode
		endm
_IdtEntryNumber =	0
	rept	256			;; prepare for INT 00h -- INT 0FFh
		_ProtIntHandler	%_IdtEntryNumber
_IdtEntryNumber =	_IdtEntryNumber + 1
	endm
ProtIntHandler	proc	near			; protect mode interrupt handler
		push	ds
		push	es
		_CallRealMode	RealIntHandler	; re-issue it in real mode
		pop	es
		pop	ds
		add	esp, 4
		iretd
ProtIntHandler	endp

;** Protect mode System call handler **
SystemCall	proc	far
		_GotoRealMode	GxTerminate	; terminate immediately
SystemCall	endp

;** Protect mode sample **
ProtSampCode:	sti
		mov	bl, 10
loop1:		_CallRealMode KeyInput		; read char without echo back
		mov	cl, 10
		mov	dl, al
		mov	ah, 02h
loop2:		int	21h			; write char, direct call to MS-DOS
		inc	dl
		dec	cl
		jne	loop2
		mov	dl, 0dh			; out CR/LF
		int	21h
		mov	dl, 0ah
		int	21h
		dec	bl			; outer loop done?
		jne	loop1
;
		_SystemCallGate			; program end
;
_TEXT_GX	ends
		end	begin
