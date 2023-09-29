_print_str::
		push	ix

		push	af
		push	bc
		push	de
		push	hl
		push	iy

		ld		e, l
		ld		d, h
		ld		c, #9
		call	0x0005

		pop		iy
		pop		hl
		pop		de
		pop		bc
		pop		af

		pop		ix
		ret

_putchar::
		push	ix

		push	af
		push	bc
		push	de
		push	hl
		push	iy

		ld		e, l
		ld		c, #2
		call	0x0005

		pop		iy
		pop		hl
		pop		de
		pop		bc
		pop		af

		pop		ix
		ret

_getchar::
		push	ix

		push	bc
		push	hl
		push	iy

		ld		c,#1
		call	0x0005
		ld		e,a
		ld		d,#0

		pop		iy
		pop		hl
		pop		bc

		pop		ix
		ret

_print_hex16::
		push	ix
		ld		ix,#0
		add		ix,sp

		push	hl

		push	af
		push	bc
		push	de
		push	hl
		push	iy

		ld		a,#'0'
		call	_putchar
		ld		a,#'x'
		call	_putchar

		ld		a,-1(ix)
		rrca
		rrca
		rrca
		rrca
		and		#0xf
		call	_print_hex4
		ld		a,-1(ix)
		and		#0xf
		call	_print_hex4

		ld		a,-2(ix)
		rrca
		rrca
		rrca
		rrca
		and		#0xf
		call	_print_hex4
		ld		a,-2(ix)
		and		#0xf
		call	_print_hex4

		pop		iy
		pop		hl
		pop		de
		pop		bc
		pop		af

		pop		hl

		pop		ix
		ret

_print_hex4::
		push	ix

		and		#0xf
		cp		#0x0a
		jp		c,print_hex4_jmp01
		sub		#0x0a
		add		#'a'
		sub		#'0'
print_hex4_jmp01:
		add		#'0'
		call	_putchar

		pop		ix
		ret
