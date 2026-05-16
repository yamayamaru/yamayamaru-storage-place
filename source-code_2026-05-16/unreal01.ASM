		.386p
;** Real mode enhancer : 4GB segment in real mode !!

_SYSTEM_PC9801 = 	0	; PC9801 series
_SYSTEM_PCAT =		1	; IBM-PC/AT
				; compatible arch.


_DESC		struc		; descriptor skellton
_LIM_0_15	dw	0	; segment limit
_BAS_0_15	dw	0	; base (0 ~ 15)
_BAS_16_23	db	0	; base (16 ~ 23)
_ACCESS		db	0	; access right byte
_GRAN		db	0	; granurity
_BAS_24_31	db	0	; base (24 ~ 31)
_DESC		ends
;

		assume	cs:prog,ds:prog
prog		segment	use16
		org	100h
start:		jmp	begin
		org	110h
;
; GDT[0] = NULL
; GDT[1] = 4GB data area
;
desc_base	label	byte
desc_null	_DESC	<>	; null segment
desc_flat	_DESC	<0ffffh,0,0,92h,0cfh,0>
				; 4GB flat data segment
SEG_NULL	=	00h	; null selector
SEG_FLAT	=	08h	; 4GB selector
;
gdt_ptr		label	fword
gdt_ptr_limit	dw	8*2-1
gdt_ptr_base	dd	0h
;
begin:		push	cs
		push	ds
		xor	eax,eax
		mov	ax,ds	; compute linear address
		shl	eax,4	;  of desc_base
		xor	ebx,ebx
		lea	bx,desc_base
		add	eax,ebx
		mov	[gdt_ptr_base],eax
		cli
		call	ProtectEnable
		sti
;
		mov	ax,4c00h
		int	21h
;
ProtectEnable	proc
		pushf
		cli
		push	ds	; save old segment registers
		push	es
		push	fs
		push	gs
		mov	bx,ss	; save ss
		db	66h	; 32-bit from LGDT
		lgdt	cs:gdt_ptr
		mov	eax,cr0	; protection enable
		or	al,1	; set bit-0 (PE)
		mov	cr0,eax
		jmp	short $+2
		mov	ax,SEG_FLAT
				; setup segment descriptor cashe
		mov	ds,ax
		mov	es,ax
		mov	fs,ax
		mov	gs,ax
		mov	ss,ax
		mov	sp,sp
		mov	eax,cr0	; protection disable
		and	al,0feh	; clear bit-0 (PE)
		mov	cr0,eax
		jmp	$+2	; flush queue
		mov	ss,bx	; first, restore SS:SP
		mov	sp,sp
		pop	gs	; restore other seg regs
		pop	fs
		pop	es
		pop	ds
		call	EnableA20	; enable A20 line
		popf
		ret
ProtectEnable	endp
;
EnableA20	proc	near
	if _SYSTEM_PC9801
PORT_A20	=	0f2h
		out	PORT_A20,al
	endif
	if _SYSTEM_PCAT
		mov	al,0d1h	; this is very buggy method
		out	064h,al
		mov	cx,-1
		loop	$
		mov	al,0ffh
		out	064h,al
		mov	cx,-1
		loop	$
	endif
		ret
EnableA20	endp
		db	128 dup (?)
progend		label	byte
prog		ends
		end	start
