MSXALIB_EXPTBL01	equ	0fcc1h		;slot address of BIOS ROM
MSXALIB_CALSLT01	equ	0001ch
MSXALIB_ACPAGE01	equ	0faf6h
MSXALIB_EXBRSA01	equ	0faf8h
MSXALIB_SCRMOD01	equ	0fcafh
MSXALIB_LINWRK01	equ	0fc18h

	psect	data
	global	vdpio_,vdpo_

	psect	text
        global  _calbio, _ldirvm, _ldirmv, _filvrm, _boxfil, _line, _cpym2v, _cpyv2v ,_palett


;--- CALBIO ---
_calbio:
	push	ix
	push	iy

	ld		ix,0
	add		ix,sp

	ld		c,(ix+2+2)
	ld		b,(ix+3+2)
	push	bc
	ld		e,(ix+6+2)
	ld		d,(ix+7+2)
	ld		l,(ix+4+2)
	ld		h,(ix+5+2)

	push	hl
	pop	ix
	push	de
	ex	de,hl
	ld	iy,0
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
	ld	bc,8
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

	pop	bc

	pop	iy
	pop	ix
	ret



;--- LDIRVM ---
;moves block from memory to the VRAM

_ldirvm:
	push	ix
	ld		ix,0
	add		ix,sp

	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		b,(ix+9)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)


	push	bc		;save length
	call	MSXALIB_SETWRT01
	ex	de,hl		;C..(vdpo_)
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

_ldirmv:
	push	ix
	ld		ix,0
	add		ix,sp
	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		b,(ix+9)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)


	ex	de,hl		;HL..pointer to destination  DE..length
	push	bc		;save length
	call	MSXALIB_SETRD01
	ex	de,hl		;C..(vdpio_)
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

_filvrm:
	push	ix
	ld		ix,0
	add		ix,sp

	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)


	push	bc		;save data
	call	MSXALIB_SETWRT01
	pop	hl		;L..data  DE..length  C..(vdpo_)
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
GETSTAT01:
GETSTAT:
	push	bc
	ld	bc,(vdpo_)	;Read Status Tegister Specified by A
	inc	c
	out	(c),a
	ld	a,8fh
	out	(c),a
	ld	bc,(vdpio_)
	inc	c
	in	a,(c)
	pop	bc

	ret

;--- WAITVDP ---

WAITVDP:

	ld	a,2
	call	GETSTAT
	and	1
	jr	nz,WAITVDP
	xor	a
	call	GETSTAT

	ret

;--- BOXFIL ---

_boxfil:
	push		ix
	ld		ix,0
	add		ix,sp

	ld		c,(ix+14)
	ld		b,(ix+15)
	push	bc
	ld		c,(ix+12)
	ld		b,(ix+13)
	push	bc
	ld		c,(ix+10)
	ld		b,(ix+11)
	push	bc
	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		b,(ix+9)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)


	pop	iy
	ld	(MSXALIB_LINWRK01+4),hl	;return address
	ld	a,(MSXALIB_SCRMOD01)
	cp	7
	ld	a,(MSXALIB_ACPAGE01)
	jr	c,BOXFI1	;active page
	add	a,a
BOXFI1:
	add	a,d
	ld	(MSXALIB_LINWRK01+7),a	;save y start
	ld	a,e
	ld	(MSXALIB_LINWRK01+6),a
	ld	a,4
	or	a
	sbc	hl,bc		;hl..x start - x end
	jr	nc,BOXFI2
	ld	c,l
	ld	b,h
	ld	hl,0
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
	ld	hl,0
	or	a
	sbc	hl,de
	ld	a,(MSXALIB_LINWRK01+13)
	set	3,a
	ld	(MSXALIB_LINWRK01+13),a
BOXFI3:
	inc	hl
	ld	(MSXALIB_LINWRK01+10),hl	;save number of dots for y
	pop	hl
	ld	a,l
	ld	(MSXALIB_LINWRK01+12),a	;color code
	pop	hl		;logical operation
	ld	a,l
	or	80h	;LMMV command   0b100000000
	ld	(MSXALIB_LINWRK01+14),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	iy		;return address
	ld	hl,MSXALIB_LINWRK01+4
	di
	call	WAITVDP
	ld	a,(vdpo_)
	ld	c,a
	inc	c
	ld	a,36
	out	(c),a
	ld	a,17+80h
	out	(c),a
	inc	c
	inc	c
	ld	b,11
	otir
	ei

	pop		hl
	pop		hl
	pop		hl
	pop		hl

	pop		ix
	ret

;--- LINE ---

_line:
	push	ix
	ld		ix,0
	add		ix,sp

	ld		c,(ix+14)
	ld		b,(ix+15)
	push	bc
	ld		c,(ix+12)
	ld		b,(ix+13)
	push	bc
	ld		c,(ix+10)
	ld		b,(ix+11)
	push	bc
	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		b,(ix+9)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)



	pop	iy		;return address
	ld	(MSXALIB_LINWRK01+4),hl	;save x start
	ld	a,(MSXALIB_SCRMOD01)
	cp	7
	ld	a,(MSXALIB_ACPAGE01)	;activ page
	jr	c,LINE1
	add	a,a
LINE1:
	add	a,d
	ld	(MSXALIB_LINWRK01+7),a	;save y start
	ld	a,e
	ld	(MSXALIB_LINWRK01+6),a
	ld	a,4
	or	a
	sbc	hl,bc		;hl..x start - x end
	jr	nc,LINE2
	ld	c,l
	ld	b,h
	ld	hl,0
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
	ld	hl,0
	or	a
	sbc	hl,de
	ld	a,(MSXALIB_LINWRK01+13)
	set	3,a
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
	set	0,a
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
	or	70h	;LINE commmand   0b01110000
	ld	(MSXALIB_LINWRK01+14),a
	push	hl
	push	hl
	push	hl
	push	iy
	ld	hl,MSXALIB_LINWRK01+4
	di
	call	WAITVDP
	ld	a,(vdpo_)
	ld	c,a
	inc	c
	ld	a,36
	out	(c),a
	ld	a,17+80h
	out	(c),a
	inc	c
	inc	c
	ld	b,11
	otir
	ei




	pop		af
	pop		af
	pop		af
	pop		af

	pop		ix
	ret


;--- CPYM2V ---

_cpym2v:
	push	ix
	ld		ix,0
	add		ix,sp

	ld		c,(ix+14)
	ld		b,(ix+15)
	push	bc
	ld		c,(ix+12)
	ld		b,(ix+13)
	push	bc
	ld		c,(ix+10)
	ld		b,(ix+11)
	push	bc
	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		b,(ix+9)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)


	pop	iy	;return	address
	ld	a,e
	and	3
	rlca
	rlca
	ld	(MSXALIB_LINWRK01+13),a	;arg
	ld	(MSXALIB_LINWRK01+4),bc	;save destination x start
	pop	bc		;destination y start
	pop	de		;destination page
	ld	a,(MSXALIB_SCRMOD01)
	cp	7
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
	or	0b0h	;LMMC command  0b10110000
	ld	(MSXALIB_LINWRK01+14),a
	ld	de,MSXALIB_LINWRK01+8
	ld	bc,4
	ldir
	ld	a,(hl)
	ld	(MSXALIB_LINWRK01+12),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	iy		;retun address
;
	push	hl
	ld	hl,MSXALIB_LINWRK01+4
	di
	call	WAITVDP
	ld	a,(vdpo_)
	ld	c,a
	inc	c
	ld	a,36
	out	(c),a
	ld	a,17+80h
	out	(c),a
	inc	c
	inc	c
	ld	b,11
	otir
	pop	hl
;
	dec	c
	dec	c		;C:PORT#1's address
CPYMV2:
	ld	a,2
	call	GETSTAT
	bit	0,a
	jr	z,CPYMV3	;check CE bit
	bit	7,a
	jr	z,CPYMV2	;check TR bit
	inc	hl
	ld	a,(hl)
	out	(c),a
	ld	a,44+80h
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

_cpyv2v:
	push	ix
	ld		ix,0
	add		ix,sp
	ld		c,(ix+20)
	ld		b,(ix+21)
	push	bc
	ld		c,(ix+18)
	ld		b,(ix+19)
	push	bc
	ld		c,(ix+16)
	ld		b,(ix+17)
	push	bc
	ld		c,(ix+14)
	ld		b,(ix+15)
	push	bc
	ld		c,(ix+12)
	ld		b,(ix+13)
	push	bc
	ld		c,(ix+10)
	ld		b,(ix+11)
	push	bc
	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		b,(ix+9)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)



	pop	iy		;return address
	ld	(MSXALIB_LINWRK01),hl	;save source x start
	ld	a,4
	or	a
	sbc	hl,bc		;hl..sx start - sx end
	jr	nc,CPYVV1
	ld	c,l
	ld	b,h
	ld	hl,0
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
	cp	7
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
	ld	hl,0
	or	a
	sbc	hl,de
	ld	a,(MSXALIB_LINWRK01+13)
	set	3,a
	ld	(MSXALIB_LINWRK01+13),a
CPYVV3:
	inc	hl
	ld	(MSXALIB_LINWRK01+10),hl	;save number of dots for y
	pop	hl		;destination x start
	pop	de		;destination y start
	pop	bc		;destination page
	ld	(MSXALIB_LINWRK01+4),hl
	ld	a,(MSXALIB_SCRMOD01)
	cp	7
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
	or	90h	;LMMV command  0b10010000
	ld	(MSXALIB_LINWRK01+14),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	hl
	push	hl
	push	hl
	push	iy		;return address
	ld	hl,MSXALIB_LINWRK01
	di
	call	WAITVDP
	ld	a,(vdpo_)
	ld	c,a
	inc	c
	ld	a,32
	out	(c),a
	ld	a,17+80h
	out	(c),a
	inc	c
	inc	c
	ld	b,15
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

_palett:
;set palette
;entry b..palette number @...red*8+blue d...green
	push	ix
	ld		ix,0
	add		ix,sp
	ld		c,(ix+10)
	ld		b,(ix+11)
	push	bc
	ld		c,(ix+2)
	ld		b,(ix+3)
	push	bc
	ld		c,(ix+8)
	ld		b,(ix+9)
	ld		e,(ix+6)
	ld		d,(ix+7)
	ld		l,(ix+4)
	ld		h,(ix+5)



	pop	ix
	ld	b,l
	pop	hl
	ld	a,c
	and	7
	ld	d,a
	ld	a,e
	and	7
	rlca
	rlca
	rlca	
	rlca
	ld	e,a
	ld	a,l
	and	7
	or	e
	ld	e,a
	push	hl
	push	ix
	ld	a,(vdpo_)
	ld	c,a	
	inc	c
	di
	out	(c),b		;palette number
	ld	a,16+80h
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
	psect	data
	global	_RNDSPD
_RNDSPD:
	defw	1991h, 0c204h

	psect	text
	global	_rnd
_rnd:
    push	ix
	ld	ix,0
	add	ix,sp
	ld	l,(ix+4)
	ld	h,(ix+5)

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
	ld	hl,0
	ld	b,16
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





;void setwrt(unsigned bit15_0)
;setwt In: HL-VRAM Address
;Use AF,BC,HL

	global	_setwrt

;extrn	vdpio_	;in ginit()
;extrn	vdpo_	;in ginit()

MSXALIB_SETWRT01:
	push	hl
	call	_setwrt
	dec	c
	pop	hl
	ret


;void setrd(unsigned bit15_0)
;setwt In: HL-VRAM Address
;Use AF,BC,HL

	global	_setrd
;extrn	vdpio_	;in ginit()
;extrn	vdpo_	;in ginit()

MSXALIB_SETRD01:
	push	hl
	call	_setrd
	dec	c
	pop	hl
	ret

