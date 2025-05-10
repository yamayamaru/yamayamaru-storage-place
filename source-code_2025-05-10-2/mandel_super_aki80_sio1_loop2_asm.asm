sioa_data   .equ 0x18
sioa_cmd    .equ 0x19

.globl _ctc0_intcall_func


_ctc0_intcall::
    push		ix
	push		iy
	push		af
	push		hl
	push		bc
	push		de

	call		_ctc0_intcall_func

	pop			de
	pop			bc
	pop			hl
	pop			af
	pop			iy
	pop			ix
	ei
	reti

_putchar::
        push    ix

		ld		e, l
		ld		d, #0

putchar_loop0001:
    	in  	a, (sioa_cmd)
    	and 	#0x04
    	jp  	z, putchar_loop0001
    	ld  	a, e
    	out 	(sioa_data), a

		pop		ix
		ret


_putchar01::
        push    ix

		ld		e, l
		ld		d, #0

putchar01_loop0001:
    	in  	a, (sioa_cmd)
    	and 	#0x04
    	jp  	z, putchar01_loop0001
    	ld  	a, e
    	out 	(sioa_data), a

		pop		ix
		ret

_outp::
        push    ix
        ld      c, a
        out     (c), l
        pop     ix
        ret

_inp::
        push    ix
        ld      c, a
        in      a, (c)
        pop     ix
        ret

_iretmon::
		ei	
		reti

