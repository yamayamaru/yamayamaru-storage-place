        .386

	public	rmical, rmi_call_data, timer_count_up, timer_count_data
	assume  cs:code, ds:code
code	segment use16

	org	100h
start:
rmical:
	jmp	rmical01
	nop


;     rmi_data_table
	org	180h
rmi_call_data:
	dw	rmi_call_data01



	org	200h

rmical01:
	push	esi
	push	edi
	push	ebp

	push	ds
	pop	gs
	mov	ax, rmi_es
	mov	es, ax
	mov	ax, rmi_fs
	mov	fs, ax
	lea	bx, int_label01
	mov	al, rmi_int
	mov	[bx + 1], al
	mov	ebx, rmi_ebx
	mov	ecx, rmi_ecx
	mov	edx, rmi_edx
	mov	esi, rmi_esi
	mov	edi, rmi_edi
	mov	ebp, rmi_ebp
	mov	ax,  rmi_ds
	mov	ds,  ax
	mov	eax, gs:rmi_eax
	push	gs
int_label01:
	int	21h
	pop	gs
	mov	gs:rmi_eax, eax
	mov	ax, ds
	mov	gs:rmi_ds, ax
	push	gs
	pop	ds
	mov	ax, es
	mov	rmi_es, ax
	mov	ax, fs
	mov	rmi_fs, ax
	mov	rmi_ebx, ebx
	mov	rmi_ecx, ecx
	mov	rmi_edx, edx
	mov	rmi_esi, esi
	mov	rmi_edi, edi
	mov	rmi_ebp, ebp

        pop	ebp
	pop	edi
	pop	esi
        retf

	align	4
rmi_call_data01:
rmi_int db      21h
	db	0
rmi_ds	dw	0
rmi_es	dw	0
rmi_fs	dw	0
rmi_eax	dd	0
rmi_ebx	dd	0
rmi_ecx	dd	0
rmi_edx	dd	0
rmi_esi	dd	0
rmi_edi	dd	0
rmi_ebp	dd	0

code	ends
	end	rmical
