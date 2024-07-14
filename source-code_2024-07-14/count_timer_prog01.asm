        .386

	public	count_timer_prog, timer_count_data01
	assume  cs:code, ds:code
code	segment use16

	org	0000h
count_timer_prog:
	push	es
	push	ds
	push	eax
	push	ebx
	push	ecx
	push	edx
	push	edi
	push	esi
	push	ebp

	push	cs       ;es, ds‚Ì‰Šú‰»‚ğ‚µ‚È‚¢‚Æ‚¢‚¯‚È‚¢
	pop	es
	push	cs
	pop	ds

	mov	edx, 0
	mov	eax, 1
	add	eax, timer_count_data01
	adc	edx, timer_count_data02
	mov	timer_count_data01, eax
	mov	timer_count_data02, edx

	mov	eax, flag01
	cmp	eax, 0
	jz	count_timer_prog_jmp01

        mov	ah, 02h
	mov	cx, interval01
	push	cs
	pop	es
	lea	bx, count_timer_prog
	int	1ch

count_timer_prog_jmp01:
	pop	ebp
	pop	esi
	pop	edi
	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
	pop	ds
	pop	es
	iret

	org	1000H
timer_count_data01	dd      0
timer_count_data02	dd      0
flag01		        dd      0
interval01		dw	0
			dw	0
code	ends    count_timer_prog
	end
