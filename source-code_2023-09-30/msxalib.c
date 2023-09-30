
#include <stdio.h>
#include <stdlib.h>
#include <msxalib.h>


char rdport = 98;
char wrport = 99;

void ginit() {
#asm
GINIT_EXPTBL	equ	0fcc1h		;slot address of BIOS ROM
	ld	hl,6
	ld	a,(GINIT_EXPTBL)
	call	000ch
	ld	(_rdport),a
	ld	hl,7
	ld	a,(GINIT_EXPTBL)
	call	000ch
	ei
	ld	(_wrport),a
#endasm
}

char getstat_m(char r) {

#asm
		ld		hl, 2
		add		hl, sp
		ld		a, (hl)
		call	GETSTAT
		jp	endgetstat1
GETSTAT:
		push	bc
		ld		bc, (_wrport)
		inc		c
		out		(c), a
		ld		a, 8fh
		out		(c), a
		ld		bc, (_rdport)
		inc		c
		in		a, (c)
		pop		bc
		ret
endgetstat1:

#endasm

}

char waitvdp01() {
#asm
		call 	WAITVDP
		jp		endwaitvdp1
WAITVDP:
		ld		a, 2
		call	GETSTAT
		and		1
		jr		nz, WAITVDP
		xor		a
		call 	GETSTAT
		ret
endwaitvdp1:
#endasm
}

void etc_func01() {

#asm
	jp	jmp_etc_func01
;--- SETADR ---
;low level routine for SETRD and SETWRT
;entry  HL..read/write address

ETC_FUNC01_ACPAGE	equ	0faf6h
ETC_FUNC01_EXBRSA	equ	0faf8h
ETC_FUNC01_SCRMOD	equ	0fcafh
SETADR:
	ld	c,a
	inc	c
	ld	a,(ETC_FUNC01_EXBRSA)
	or	a		;MSX1?
	jr	z,SETAD3	;Yes
	ld	a,(ETC_FUNC01_SCRMOD)
	cp	5
	jr	c,SETAD3
	ld	b,h
	cp	7		;extract the high address
	ld	a,(ETC_FUNC01_ACPAGE)
	jr	c,SETAD1
	sla	b
	rla
	jr	SETAD2
SETAD1:
	sla	b
SETAD2:
	sla	b
	rla
	di
	out	(c),a		;set data to be written
	ld	a,14+80h	;set it to register #14
	out	(c),a
SETAD3:
	di
	out	(c),l		;set the low address
	ld	a,h		;prepare to set the middle address
	and	3fh
	ret

;--- SETWRT ---
;sets up the VDP for write

SETWRT:
	ld	a,(_wrport)
	call	SETADR
	or	01000000b	;set the low address with wirte bit
	out	(c),a
	dec	c
	ei
	ret

;--- SETRD ---
;sets up the VDP for read

SETRD:
	ld	a,(_rdport)
	call	SETADR
	out	(c),a
	dec	c
	ei
	ret
jmp_etc_func01:

#endasmm
}

void vpoke(unsigned int ad, char dt) {
#asm
	ld	hl,2
	add	hl,sp
	ld	e,(hl)
	inc	hl
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ld	l,c
	ld	h,b

vpoke01:
	call	SETWRT
	out	(c),e
#endasm
}

char vpeek(unsigned int ad) {
#asm
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ld	l,c
	ld	h,b

vpeed01:
	call	SETRD
	in	a,(c)


	ld	l, a
	ld	h, 0
#endasm
}

unsigned int ldirvm_param3;
void ldirvm(unsigned int ds, char *sr, unsigned int ln) {
#asm
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_ldirvm_param3),bc
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ld	l,c
	ld	h,b
	ld	bc,(_ldirvm_param3)

ldirvm01:
	push	bc		;save length
	call	SETWRT
	ex	de,hl		;C..(_wrport)
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
#endasm
}

unsigned int ldirmv_param3;
void ldirmv(char *ds, unsigned int sr, unsigned int ln) {
#asm
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_ldirmv_param3),bc
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ld	l,c
	ld	h,b
	ld	bc,(_ldirmv_param3)

ldirmv01:
	ex	de,hl		;HL..pointer to destination  DE..length
	push	bc		;save length
	call	SETRD
	ex	de,hl		;C..(_rdport)
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
#endasm
}


unsigned int filvrm_param3;
void filvrm(unsigned int ad, unsigned int ln, char dt) {
#asm
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_filvrm_param3),bc
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ld	bc,(_filvrm_param3)
	ld	l,c
	ld	h,b


filvrm01:
	push	bc		;save data
	call	SETWRT
	pop	hl		;L..data  DE..length  C..(_wrport)
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
#endasm
}

//パラメータは左から積まれる模様 char型も16bitで積まれる
//msx-cとはパラメータの積まれ方が違うので変換している

int line_m_work01, line_m_work02, line_m_retaddr;
void line_m(int x1, int y1, int x2, int y2, int color, int logi) {
#asm
LINE01_ACPAGE	equ		0faf6h
LINE01_LINWRK	equ		0fc18h
LINE01_SCRMOD	equ		0fcafh

	pop	hl
	push	hl
	ld	(_line_m_retaddr),hl
	ld	hl,2
	add	hl,sp
	ld	c,(hl)     ;6th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)     ;5th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)     ;4th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)     ;third param to work01 
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_line_m_work01), bc
	ld	e,(hl)     ;second param to de
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)     ;first param to work02
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_line_m_work02), bc
	ld	hl,(_line_m_retaddr)
	push	hl
	ld	bc,(_line_m_work01)
	ld	hl,(_line_m_work02)

	

	pop	iy		;return address
	ld	(LINE01_LINWRK+4),hl	;save x start
	ld	a,(LINE01_SCRMOD)
	cp	7
	ld	a,(LINE01_ACPAGE)	;activ page
	jr	c,LINE1
	add	a,a
LINE1:
	add	a,d
	ld	(LINE01_LINWRK+7),a	;save y start
	ld	a,e
	ld	(LINE01_LINWRK+6),a
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
	ld	(LINE01_LINWRK+8),hl	;save nuber of data for x
	ld	(LINE01_LINWRK+13),a	;arg
	pop	hl		;hl..y end
	or	a
	sbc	hl,de		;hl..y end - y start
	jr	nc,LINE3
	ex	de,hl
	ld	hl,0
	or	a
	sbc	hl,de
	ld	a,(LINE01_LINWRK+13)
	set	3,a
	ld	(LINE01_LINWRK+13),a
LINE3:
	ld	de,(LINE01_LINWRK+8)	;number of dots for x
	or	a
	sbc	hl,de
	jr	nc,LINE4
	add	hl,de
	ld	(LINE01_LINWRK+10),hl	;save number of dots for x
	jr	LINE5
LINE4:
	ld	a,(LINE01_LINWRK+13)
	set	0,a
	ld	(LINE01_LINWRK+13),a
	ld	(LINE01_LINWRK+10),de	;save number of dots for x
	add	hl,de
	ld	(LINE01_LINWRK+8),hl	;number of dots for y
LINE5:
	pop	hl
	ld	a,l
	ld	(LINE01_LINWRK+12),a	;color code
	pop	hl		;logical operation
	ld	a,l
	or	01110000b	;LINE commmand
	ld	(LINE01_LINWRK+14),a
	push	hl
	push	hl
	push	hl
	push	iy
	ld	hl,LINE01_LINWRK+4
	di
	call	WAITVDP	
	ld	a,(_wrport)
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



	pop	hl
	pop	hl
	pop	hl
	pop	hl

#endasm
} 

int boxfil_work01, boxfil_work02, boxfil_retaddr;
void boxfil(int x1, int y1, int x2, int y2, int color, int logi) {
#asm


BOXFIL01_ACPAGE	equ	0faf6h
BOXFIL01_LINWRK	equ	0fc18h
BOXFIL01_SCRMOD	equ	0fcafh

	pop	hl
	push	hl
	ld	(_line_m_retaddr),hl
	ld	hl,2
	add	hl,sp
	ld	c,(hl)     ;6th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)     ;5th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)     ;4th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)     ;third param to work01 
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_line_m_work01), bc
	ld	e,(hl)     ;second param to de
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)     ;first param to work02
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_line_m_work02), bc
	ld	hl,(_line_m_retaddr)
	push	hl
	ld	bc,(_line_m_work01)
	ld	hl,(_line_m_work02)





	pop	iy
	ld	(BOXFIL01_LINWRK+4),hl	;return address
	ld	a,(BOXFIL01_SCRMOD)
	cp	7
	ld	a,(BOXFIL01_ACPAGE)
	jr	c,BOXFI1	;active page
	add	a,a
BOXFI1:
	add	a,d
	ld	(BOXFIL01_LINWRK+7),a	;save y start
	ld	a,e
	ld	(BOXFIL01_LINWRK+6),a
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
	ld	(BOXFIL01_LINWRK+8),hl	;save number of dots for x
	ld	(BOXFIL01_LINWRK+13),a	;arg
	pop	hl		;hl..y end
	or	a
	sbc	hl,de		;hl..end - y start
	jr	nc,BOXFI3
	ex	de,hl
	ld	hl,0
	or	a
	sbc	hl,de
	ld	a,(BOXFIL01_LINWRK+13)
	set	3,a
	ld	(BOXFIL01_LINWRK+13),a
BOXFI3:
	inc	hl
	ld	(BOXFIL01_LINWRK+10),hl	;save number of dots for y
	pop	hl
	ld	a,l
	ld	(BOXFIL01_LINWRK+12),a	;color code
	pop	hl		;logical operation
	ld	a,l
	or	10000000b	;LMMV command
	ld	(BOXFIL01_LINWRK+14),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	iy		;return address
	ld	hl,BOXFIL01_LINWRK+4
	di
	call	WAITVDP
	ld	a,(_wrport)
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


	pop	hl
	pop	hl
	pop	hl
	pop	hl


#endasm
}

int cpym2v_work01, cpym2v_work02, cpym2v_retaddr;
void cpym2v(char *sr, int dr, int dx, int dy, int dp, int lg) {
#asm
	pop hl
    push    hl
    ld  (_cpym2v_retaddr),hl
    ld  hl,2
    add hl,sp
    ld  c,(hl)     ;6th param to stack
    inc hl
    ld  b,(hl)
    inc hl
    push    bc
    ld  c,(hl)     ;5th param to stack
    inc hl
    ld  b,(hl)
    inc hl
    push    bc
    ld  c,(hl)     ;4th param to stack
    inc hl
    ld  b,(hl)
    inc hl
    push    bc
    ld  c,(hl)     ;third param to work01
    inc hl
    ld  b,(hl)
    inc hl
    ld  (_cpym2v_work01), bc
    ld  e,(hl)     ;second param to de
    inc hl
    ld  d,(hl)
    inc hl
    ld  c,(hl)     ;first param to work02
    inc hl
    ld  b,(hl)
    inc hl
    ld  (_cpym2v_work02), bc
    ld  hl,(_cpym2v_retaddr)
    push    hl
    ld  bc,(_cpym2v_work01)
    ld  hl,(_cpym2v_work02)



CPYM2V_LINWRK	equ	0fc18h
CPYM2V_SCRMOD	equ	0fcafh
cpym2v01:
	pop	iy	;return	address
	ld	a,e
	and	3
	rlca
	rlca
	ld	(CPYM2V_LINWRK+13),a	;arg
	ld	(CPYM2V_LINWRK+4),bc	;save destination x start
	pop	bc		;destination y start
	pop	de		;destination page
	ld	a,(CPYM2V_SCRMOD)
	cp	7
	ld	a,e
	jr	c,CPYMV1
	add	a,a
CPYMV1:
	add	a,b
	ld	(CPYM2V_LINWRK+7),a	;save destination y start
	ld	a,c
	ld	(CPYM2V_LINWRK+6),a
	pop	bc		;logop
	ld	a,c
	or	10110000b	;LMMC command
	ld	(CPYM2V_LINWRK+14),a
	ld	de,CPYM2V_LINWRK+8
	ld	bc,4
	ldir
	ld	a,(hl)
	ld	(CPYM2V_LINWRK+12),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	iy		;retun address
;
	push	hl
	ld	hl,CPYM2V_LINWRK+4
	di
	call	WAITVDP
	ld	a,(_wrport)
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
	dec	c		;C PORT No1s address
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



	pop	hl
	pop	hl
	pop	hl
	pop	hl

#endasm
}

int cpyv2v_work01, cpyv2v_m_work02, cpyv2v_m_retaddr;
void cpyv2v(int x1, int y1, int x2, int y2, int sp, int dx, int dy, int dp, int lg) {
#asm
	pop hl
    push    hl
    ld  (_cpym2v_retaddr),hl   ;save return address
    ld  hl,2
    add hl,sp
	ld	c,(hl)		;9th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)		;8th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)		;7th param to stack
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
    ld  c,(hl)     ;6th param to stack
    inc hl
    ld  b,(hl)
    inc hl
    push    bc
    ld  c,(hl)     ;5th param to stack
    inc hl
    ld  b,(hl)
    inc hl
    push    bc
    ld  c,(hl)     ;4th param to stack
    inc hl
    ld  b,(hl)
    inc hl
    push    bc
    ld  c,(hl)     ;third param to work01
    inc hl
    ld  b,(hl)
    inc hl
    ld  (_cpym2v_work01), bc
    ld  e,(hl)     ;second param to de
    inc hl
    ld  d,(hl)
    inc hl
    ld  c,(hl)     ;first param to work02
    inc hl
    ld  b,(hl)
    inc hl
    ld  (_cpym2v_work02), bc
    ld  hl,(_cpym2v_retaddr)
    push    hl
    ld  bc,(_cpym2v_work01)
    ld  hl,(_cpym2v_work02)



CPYV2V_LINWRK	equ	0fc18h
CPYV2V_SCRMOD	equ	0fcafh
cpyv2v01:
	pop	iy		;return address
	ld	(CPYV2V_LINWRK),hl	;save source x start
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
	ld	(CPYV2V_LINWRK+8),hl	;save number of dots for x
	ld	(CPYV2V_LINWRK+13),a	;arg
	pop	hl		;hl..sorce y end
	pop	bc		;c..sorce page
	ld	a,(CPYV2V_SCRMOD)
	cp	7
	ld	a,c
	jr	c,CPYVV2
	add	a,a
CPYVV2:
	add	a,d
	ld	(CPYV2V_LINWRK+3),a	;save sorce y start
	ld	a,e
	ld	(CPYV2V_LINWRK+2),a
	or	a
	sbc	hl,de		;hl.sy end - sy start
	jr	nc,CPYVV3
	ex	de,hl
	ld	hl,0
	or	a
	sbc	hl,de
	ld	a,(CPYV2V_LINWRK+13)
	set	3,a
	ld	(CPYV2V_LINWRK+13),a
CPYVV3:
	inc	hl
	ld	(CPYV2V_LINWRK+10),hl	;save number of dots for y
	pop	hl		;destination x start
	pop	de		;destination y start
	pop	bc		;destination page
	ld	(CPYV2V_LINWRK+4),hl
	ld	a,(CPYV2V_SCRMOD)
	cp	7
	ld	a,c
	jr	c,CPYVV4
	add	a,a
CPYVV4:
	add	a,d
	ld	(CPYV2V_LINWRK+7),a	;save destination y start
	ld	a,e
	ld	(CPYV2V_LINWRK+6),a
	pop	hl		;logical operation
	ld	a,l
	or	10010000b	;LMMV command
	ld	(CPYV2V_LINWRK+14),a
	push	hl		;adjust stack
	push	hl
	push	hl
	push	hl
	push	hl
	push	hl
	push	iy		;return address
	ld	hl,CPYV2V_LINWRK
	di
	call	WAITVDP
	ld	a,(_wrport)
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




	pop	hl
	pop	hl
	pop	hl
	pop	hl
	pop	hl
	pop	hl
	pop	hl
#endasm
}

int palett_param3, palett_retaddr;
void palett(int pl, int r, int g, int b) {
#asm
	pop		hl
	push	hl
	ld	(_palett_retaddr),hl
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	inc	hl
	push	bc
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	inc	hl
	ld	(_palett_param3),bc
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ld	hl,(_palett_retaddr)
	push	hl
	ld	l,c
	ld	h,b
	ld	bc,(_palett_param3)


	
palett01:
;set palette
;entry b..palette number @...red*8+blue d...green
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
	ld	a,(_wrport)
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
#endasm
}

int rnd_m_rndspd01[2] = {0x1991, 0xc204};
unsigned int rnd(unsigned int r) {
#asm
	ld	hl,2
	add	hl,sp
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ld	l,c
	ld	h,b

rnd01:
	ld	a,h
	or	l
	jp	z, RND3
	ex	de,hl		;save the range
	ld	hl,(_rnd_m_rndspd01)
	ld	b,h
	ld	c,l
	add	hl,hl
	add	hl,hl
	add	hl,bc
	ld	(_rnd_m_rndspd01),hl
	ld	hl,(_rnd_m_rndspd01+2)
	adc	hl,hl
	adc	hl,bc
	ld	(_rnd_m_rndspd01+2),hl
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
RND3:

#endasm
}

void calbio(unsigned int address, struct regs *r) {
	
#asm

CALBIO_CALSLT	equ	001ch
CALBIO_EXPTBL	equ	0fcc1h		;slot address of BIOS ROM

    ld	hl,2
	add	hl,sp
	ld	e,(hl)	;second param to de
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)	;first param to hl
	inc	hl
	ld	b,(hl)
	ld  l,c
	ld	h,b


CALBIO01:
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
	ld	iy,(CALBIO_EXPTBL-1)
	call	CALBIO_CALSLT
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


#endasm
}

