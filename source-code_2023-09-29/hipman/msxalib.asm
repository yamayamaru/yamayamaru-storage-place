;--- GINIT ---
MSXALIB_EXPTBL01	.equ	0xfcc1		;slot address of BIOS ROM
MSXALIB_CALSLT01	.equ	0x001c
MSXALIB_ACPAGE01	.equ	0xfaf6
MSXALIB_EXBRSA01	.equ	0xfaf8
MSXALIB_SCRMOD01	.equ	0xfcaf
MSXALIB_LINWRK01	.equ	0xfc18

;	.area	_HEADER(ABS)
;	.org	0x001c
;MSXALIB_CALSLT01:
;	.org	0xfcc1
;MSXALIB_EXPTBL01:
;	.org	0xfaf6
;MSXALIB_ACPAGE01:
;	.org	0xfaf8
;MSXALIB_EXBRSA01:
;	.org	0xfcaf
;MSXALIB_SCRMOD01:
;	.org	0xfc18
;MSXALIB_LINWRK01:


	.area	_CODE
_ginit::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,2(ix)
	ld		b,3(ix)
	push	bc

	
	ld	hl,#6
	ld	a,(MSXALIB_EXPTBL01)
	call	0x000c
	ld	(_RDPORT01),a
	ld	hl,#7
	ld	a,(MSXALIB_EXPTBL01)
	call	0x000c
	ei
	ld	(_WRPORT01),a
	

	pop		bc	

	pop		ix
	ret
	.area	_DATA
	.globl	_RDPORT01,_WRPORT01
_RDPORT01:
	.db		#0x98	;This initial value is for ill-behaviored
_WRPORT01:
	.db		#0x98	;programs

	.area	_CODE

;--- CALBIO ---
_calbio::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)

	push	hl
	pop	ix
	push	de
	ex	de,hl
	ld	iy,#0
	add	iy,sp
	di
	ld	sp,hl
	pop	af
	pop	bc
	pop	de
	pop	hl
	ei
	ld	sp,iy
	push	iy
	ld	iy,(MSXALIB_EXPTBL01-1)
	call	MSXALIB_CALSLT01
	pop	iy
	exx
	pop	hl
	ld	bc,#8
	add	hl,bc
	di
	ld	sp,hl
	exx
	push	hl
	push	de
	push	bc
	push	af
	ei
	ld	sp,iy
	pop	af

	pop		bc

	pop		ix
	ret

;--- MSXALIB_SETADR01 ---
;low level routine for MSXALIB_SETRD01 and MSXALIB_SETWRT01
;entry  HL..read/write address
MSXALIB_SETADR01::
	ld	c,a
	inc	c
	ld	a,(MSXALIB_EXBRSA01)
	or	a		;MSX1?
	jr	z,MSXALIB_SETAD3	;Yes
	ld	a,(MSXALIB_SCRMOD01)
	cp	#5
	jr	c,MSXALIB_SETAD3
	ld	b,h
	cp	#7		;extract the high address
	ld	a,(MSXALIB_ACPAGE01)
	jr	c,MSXALIB_SETAD1
	sla	b
	rla
	jr	MSXALIB_SETAD2
MSXALIB_SETAD1:
	sla	b
MSXALIB_SETAD2:
	sla	b
	rla
	di
	out	(c),a		;set data to be written
	ld	a,#(14+0x80)	;set it to register #14
	out	(c),a
MSXALIB_SETAD3:
	di
	out	(c),l		;set the low address
	ld	a,h		;prepare to set the middle address
	and	#0x3f

	ret

;--- MSXALIB_SETWRT01 ---
;sets up the VDP for write

MSXALIB_SETWRT01::

	ld	a,(_WRPORT01)
	call	MSXALIB_SETADR01
	or	#0x40	;set the low address with wirte bit   0b01000000
	out	(c),a
	dec	c
	ei

	ret

;--- MSXALIB_SETRD01 ---
;sets up the VDP for read

MSXALIB_SETRD01::

	ld	a,(_RDPORT01)
	call	MSXALIB_SETADR01
	out	(c),a
	dec	c
	ei

	ret

;--- VPOKE ---
;writes to the VRAM

_vpoke::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		e,6(ix)
	ld		l,4(ix)
	ld		h,5(ix)


	call	MSXALIB_SETWRT01
	out		(c),e


	pop		bc

	pop		ix
	ret

;--- VPEEK ---
;reads the VRAM

_vpeek::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		l,4(ix)
	ld		h,5(ix)


	call	MSXALIB_SETRD01
	in	a,(c)


	ld		l,a

	pop		bc

	pop		ix
	ret

;--- LDIRVM ---
;moves block from memory to the VRAM

_ldirvm::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		b,9(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)


	push	bc		;save length
	call	MSXALIB_SETWRT01
	ex	de,hl		;C..(_WRPORT01)
	pop	de		;HL..pointer to souce DE..length
	ld	b,e		;set higher byte of length
	ld	a,e
	or	a
	ld	a,d		;set higher byte of length
	jr	z,LDIRV1
	inc	a
LDIRV1:
	outi
	jp	nz,LDIRV1
	dec	a
	jp	nz,LDIRV1


	pop		bc	

	pop		ix
	RET

;--- LDIRMV ---
;moves block from VRAM to memory

_ldirmv::
	push	ix
	ld		ix,#0
	add		ix,sp
	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		b,9(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)


	ex	de,hl		;HL..pointer to destination  DE..length
	push	bc		;save length
	call	MSXALIB_SETRD01
	ex	de,hl		;C..(_RDPORT01)
	pop	de		;HL.pointer to dest ination  DE..length
	ld	b,e		;set lower byte of length
	ld	a,e
	or	a
	ld	a,d		;set higher byte of length
	jr	z,LDIRM1
	inc	a
LDIRM1:
	ini
	jp	nz,LDIRM1
	dec	a
	jp	nz,LDIRM1

	pop		hl
	pop		ix
	ret

;--- FILVRM ---
;files the VRAM

_filvrm::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)


	push	bc		;save data
	call	MSXALIB_SETWRT01
	pop	hl		;L..data  DE..length  C..(_WRPORT01)
	ld	b,e		;set lower byte of length
	ld	a,e
	or	a
	ld	a,d		;set higher byte of length
	jr	z,FILVR1
	inc	a
FILVR1:
	out	(c),l
	djnz	FILVR1
	dec	a
	jr	nz,FILVR1


	pop		hl
	pop		ix
	ret

;--- GETSTAT ---
; Wait VDP Ready
;modify aaf,bc
GETSTAT01::
GETSTAT:
	push	bc
	ld	bc,(_WRPORT01)	;Read Status Tegister Specified by A
	inc	c
	out	(c),a
	ld	a,#0x8f
	out	(c),a
	ld	bc,(_RDPORT01)
	inc	c
	in	a,(c)
	pop	bc

	ret

;--- WAITVDP ---

WAITVDP:

	ld	a,#2
	call	GETSTAT
	and	#1
	jr	nz,WAITVDP
	xor	a
	call	GETSTAT

	ret

;--- BOXFIL ---

_boxfil::
	push		ix
	ld		ix,#0
	add		ix,sp

	ld		c,14(ix)
	ld		b,15(ix)
	push	bc
	ld		c,12(ix)
	ld		b,13(ix)
	push	bc
	ld		c,10(ix)
	ld		b,11(ix)
	push	bc
	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		b,9(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)


	pop	iy
	ld	(MSXALIB_LINWRK01+4),hl	;return address
	ld	a,(MSXALIB_SCRMOD01)
	cp	#7
	ld	a,(MSXALIB_ACPAGE01)
	jr	c,BOXFI1	;active page
	add	a,a
BOXFI1:
	add	a,d
	ld	(MSXALIB_LINWRK01+7),a	;save y start
	ld	a,e
	ld	(MSXALIB_LINWRK01+6),a
	ld	a,#4
	or	a
	sbc	hl,bc		;hl..x start - x end
	jr	nc,BOXFI2
	ld	c,l
	ld	b,h
	ld	hl,#0
	or	a
	sbc	hl,bc
	xor	a
BOXFI2:
	inc	hl
	ld	(MSXALIB_LINWRK01+8),hl	;save number of dots for x
	ld	(MSXALIB_LINWRK01+13),a	;arg
	pop	hl		;hl..y end
	or	a
	sbc	hl,de		;hl..end - y start
	jr	nc,BOXFI3
	ex	de,hl
	ld	hl,#0
	or	a
	sbc	hl,de
	ld	a,(MSXALIB_LINWRK01+13)
	set	#3,a
	ld	(MSXALIB_LINWRK01+13),a
BOXFI3:
	inc	hl
	ld	(MSXALIB_LINWRK01+10),hl	;save number of dots for y
	pop	hl
	ld	a,l
	ld	(MSXALIB_LINWRK01+12),a	;color code
	pop	hl		;logical operation
	ld	a,l
	or	#0x80	;LMMV command   0b100000000
	ld	(MSXALIB_LINWRK01+14),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	iy		;return address
	ld	hl,#(MSXALIB_LINWRK01+4)
	di
	call	WAITVDP
	ld	a,(_WRPORT01)
	ld	c,a
	inc	c
	ld	a,#36
	out	(c),a
	ld	a,#(17+0x80)
	out	(c),a
	inc	c
	inc	c
	ld	b,#11
	otir
	ei

	pop		hl
	pop		hl
	pop		hl
	pop		hl

	pop		ix
	ret

;--- LINE ---

_line::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,14(ix)
	ld		b,15(ix)
	push	bc
	ld		c,12(ix)
	ld		b,13(ix)
	push	bc
	ld		c,10(ix)
	ld		b,11(ix)
	push	bc
	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		b,9(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)



	pop	iy		;return address
	ld	(MSXALIB_LINWRK01+4),hl	;save x start
	ld	a,(MSXALIB_SCRMOD01)
	cp	#7
	ld	a,(MSXALIB_ACPAGE01)	;activ page
	jr	c,LINE1
	add	a,a
LINE1:
	add	a,d
	ld	(MSXALIB_LINWRK01+7),a	;save y start
	ld	a,e
	ld	(MSXALIB_LINWRK01+6),a
	ld	a,#4
	or	a
	sbc	hl,bc		;hl..x start - x end
	jr	nc,LINE2
	ld	c,l
	ld	b,h
	ld	hl,#0
	or	a
	sbc	hl,bc
	xor	a
LINE2:
	ld	(MSXALIB_LINWRK01+8),hl	;save nuber of data for x
	ld	(MSXALIB_LINWRK01+13),a	;arg
	pop	hl		;hl..y end
	or	a
	sbc	hl,de		;hl..y end - y start
	jr	nc,LINE3
	ex	de,hl
	ld	hl,#0
	or	a
	sbc	hl,de
	ld	a,(MSXALIB_LINWRK01+13)
	set	#3,a
	ld	(MSXALIB_LINWRK01+13),a
LINE3:
	ld	de,(MSXALIB_LINWRK01+8)	;number of dots for x
	or	a
	sbc	hl,de
	jr	nc,LINE4
	add	hl,de
	ld	(MSXALIB_LINWRK01+10),hl	;save number of dots for x
	jr	LINE5
LINE4:
	ld	a,(MSXALIB_LINWRK01+13)
	set	#0,a
	ld	(MSXALIB_LINWRK01+13),a
	ld	(MSXALIB_LINWRK01+10),de	;save number of dots for x
	add	hl,de
	ld	(MSXALIB_LINWRK01+8),hl	;number of dots for y
LINE5:
	pop	hl
	ld	a,l
	ld	(MSXALIB_LINWRK01+12),a	;color code
	pop	hl		;logical operation
	ld	a,l
	or	#0x70	;LINE commmand   0b01110000
	ld	(MSXALIB_LINWRK01+14),a
	push	hl
	push	hl
	push	hl
	push	iy
	ld	hl,#(MSXALIB_LINWRK01+4)
	di
	call	WAITVDP
	ld	a,(_WRPORT01)
	ld	c,a
	inc	c
	ld	a,#36
	out	(c),a
	ld	a,#(17+0x80)
	out	(c),a
	inc	c
	inc	c
	ld	b,#11
	otir
	ei




	pop		af
	pop		af
	pop		af
	pop		af

	pop		ix
	ret


;--- CPYM2V ---

_cpym2v::
	push	ix
	ld		ix,#0
	add		ix,sp

	ld		c,14(ix)
	ld		b,15(ix)
	push	bc
	ld		c,12(ix)
	ld		b,13(ix)
	push	bc
	ld		c,10(ix)
	ld		b,11(ix)
	push	bc
	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		b,9(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)


	pop	iy	;return	address
	ld	a,e
	and	#3
	rlca
	rlca
	ld	(MSXALIB_LINWRK01+13),a	;arg
	ld	(MSXALIB_LINWRK01+4),bc	;save destination x start
	pop	bc		;destination y start
	pop	de		;destination page
	ld	a,(MSXALIB_SCRMOD01)
	cp	#7
	ld	a,e
	jr	c,CPYMV1
	add	a,a
CPYMV1:
	add	a,b
	ld	(MSXALIB_LINWRK01+7),a	;save destination y start
	ld	a,c
	ld	(MSXALIB_LINWRK01+6),a
	pop	bc		;logop
	ld	a,c
	or	#0xb0	;LMMC command  0b10110000
	ld	(MSXALIB_LINWRK01+14),a
	ld	de,#MSXALIB_LINWRK01+8
	ld	bc,#4
	ldir
	ld	a,(hl)
	ld	(MSXALIB_LINWRK01+12),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	iy		;retun address
;
	push	hl
	ld	hl,#(MSXALIB_LINWRK01+4)
	di
	call	WAITVDP
	ld	a,(_WRPORT01)
	ld	c,a
	inc	c
	ld	a,#36
	out	(c),a
	ld	a,#(17+0x80)
	out	(c),a
	inc	c
	inc	c
	ld	b,#11
	otir
	pop	hl
;
	dec	c
	dec	c		;C:PORT#1's address
CPYMV2:
	ld	a,#2
	call	GETSTAT
	bit	#0,a
	jr	z,CPYMV3	;check CE bit
	bit	#7,a
	jr	z,CPYMV2	;check TR bit
	inc	hl
	ld	a,(hl)
	out	(c),a
	ld	a,#(44+0x80)
	out	(c),a
	jr	CPYMV2	
CPYMV3:
	xor	a
	call	GETSTAT
	ei


	pop		af
	pop		af
	pop		af
	pop		af	

	pop		ix
	ret

;--- CPYV2V --

_cpyv2v::
	push	ix
	ld		ix,#0
	add		ix,sp
	ld		c,20(ix)
	ld		b,21(ix)
	push	bc
	ld		c,18(ix)
	ld		b,19(ix)
	push	bc
	ld		c,16(ix)
	ld		b,17(ix)
	push	bc
	ld		c,14(ix)
	ld		b,15(ix)
	push	bc
	ld		c,12(ix)
	ld		b,13(ix)
	push	bc
	ld		c,10(ix)
	ld		b,11(ix)
	push	bc
	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		b,9(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)



	pop	iy		;return address
	ld	(MSXALIB_LINWRK01),hl	;save source x start
	ld	a,#4
	or	a
	sbc	hl,bc		;hl..sx start - sx end
	jr	nc,CPYVV1
	ld	c,l
	ld	b,h
	ld	hl,#0
	or	a
	sbc	hl,bc
	xor	a
CPYVV1:
	inc	hl
	ld	(MSXALIB_LINWRK01+8),hl	;save number of dots for x
	ld	(MSXALIB_LINWRK01+13),a	;arg
	pop	hl		;hl..sorce y end
	pop	bc		;c..sorce page
	ld	a,(MSXALIB_SCRMOD01)
	cp	#7
	ld	a,c
	jr	c,CPYVV2
	add	a,a
CPYVV2:
	add	a,d
	ld	(MSXALIB_LINWRK01+3),a	;save sorce y start
	ld	a,e
	ld	(MSXALIB_LINWRK01+2),a
	or	a
	sbc	hl,de		;hl.sy end - sy start
	jr	nc,CPYVV3
	ex	de,hl
	ld	hl,#0
	or	a
	sbc	hl,de
	ld	a,(MSXALIB_LINWRK01+13)
	set	#3,a
	ld	(MSXALIB_LINWRK01+13),a
CPYVV3:
	inc	hl
	ld	(MSXALIB_LINWRK01+10),hl	;save number of dots for y
	pop	hl		;destination x start
	pop	de		;destination y start
	pop	bc		;destination page
	ld	(MSXALIB_LINWRK01+4),hl
	ld	a,(MSXALIB_SCRMOD01)
	cp	#7
	ld	a,c
	jr	c,CPYVV4
	add	a,a
CPYVV4:
	add	a,d
	ld	(MSXALIB_LINWRK01+7),a	;save destination y start
	ld	a,e
	ld	(MSXALIB_LINWRK01+6),a
	pop	hl		;logical operation
	ld	a,l
	or	#0x90	;LMMV command  0b10010000
	ld	(MSXALIB_LINWRK01+14),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	hl
	push	hl
	push	hl
	push	iy		;return address
	ld	hl,#MSXALIB_LINWRK01
	di
	call	WAITVDP
	ld	a,(_WRPORT01)
	ld	c,a
	inc	c
	ld	a,#32
	out	(c),a
	ld	a,#(17+0x80)
	out	(c),a
	inc	c
	inc	c
	ld	b,#15
	otir
	ei


	pop		hl
	pop		hl
	pop		hl
	pop		hl
	pop		hl
	pop		hl
	pop		hl

	pop		ix
	ret

;--- PALETT ---
;set palette datas

_palett::
;set palette
;entry b..palette number @...red*8+blue d...green
	push	ix
	ld		ix,#0
	add		ix,sp
	ld		c,10(ix)
	ld		b,11(ix)
	push	bc
	ld		c,2(ix)
	ld		b,3(ix)
	push	bc
	ld		c,8(ix)
	ld		b,9(ix)
	ld		e,6(ix)
	ld		d,7(ix)
	ld		l,4(ix)
	ld		h,5(ix)



	pop	ix
	ld	b,l
	pop	hl
	ld	a,c
	and	#7
	ld	d,a
	ld	a,e
	and	#7
	rlca
	rlca
	rlca	
	rlca
	ld	e,a
	ld	a,l
	and	#7
	or	e
	ld	e,a
	push	hl
	push	ix
	ld	a,(_WRPORT01)
	ld	c,a	
	inc	c
	di
	out	(c),b		;palette number
	ld	a,#(16+0x80)
	out	(c),a
	inc	c
	out	(c),e
	push	bc
	pop	bc
	push	bc
	pop	bc
	out	(c),d
	ei


	pop		hl
	pop		hl
	pop		ix
	ret

;--- RND ---

_rnd::
    push	ix
	ld		ix,#0
	add		ix,sp
	ld		l,4(ix)
	ld		h,5(ix)

	ld	a,h
	or	l
	jr	z,rnd_jmp01
	ex	de,hl		;save the range
	ld	hl,(_RNDSPD)
	ld	b,h
	ld	c,l
	add	hl,hl
	add	hl,hl
	add	hl,bc
	ld	(_RNDSPD),hl
	ld	hl,(_RNDSPD+2)
	adc	hl,hl
	adc	hl,bc
	ld	(_RNDSPD+2),hl
	ld	a,h
	ld	c,l
	ld	hl,#0
	ld	b,#16
	srl	a
	rr	c
RND1:	jr	nc,RND2
	add	hl,de
RND2:	rr	h
	rr	l
	rra
	rr	c
	djnz	RND1


rnd_jmp01:
	pop		ix
	ret

	.area	_DATA
	.globl	_RNDSPD
_RNDSPD:	.dw	#0x1991,#0xc204
	.area	_CODE

_getch::
        push    ix

		push	bc
		push	de
		push	iy

        ld      c,#0x07
        call    0x0005
        ld      l,a
        ld      h,#0

		pop		iy
		pop		de
		pop		bc

        pop     ix
        ret

_getch_non_wait::
        push    ix

		push	bc
		push	de	
		push	iy

		ld		e,#0xff
        ld      c,#0x06
        call    0x0005
        ld      l,a
        ld      h,#0

		pop		iy
		pop		de
		pop		bc

        pop     ix
        ret

_kbhit::
        push    ix

		push	bc
		push	de
		push	iy

        ld      c,#0x0b
        call    0x0005
        ld      l,a
        ld      h,a

		pop		iy
		pop		de
		pop		bc

        pop     ix
        ret
