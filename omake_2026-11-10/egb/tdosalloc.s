		.globl	allocate_msdos_memory_block, free_msdos_memory_block
		.globl	pokeb_far, pokew_far, pokel_far
		.globl	peekb_far, peekw_far, peekl_far
		.globl	tdoscal_data_start, tdoscal_data_end
		.text

		.intel_syntax noprefix		

		.align	16
allocate_msdos_memory_block:
		push	ebp
		mov	ebp,esp

		push	esi
		push	edi
		push	ebx

		push	edx
		push	ecx
		push	ds                                            

	        mov     ebx, [ebp + 8]		#  segsize
		shr	ebx, 4                  #  segsize / 16
	        mov     ax, 0x25c0
	        int     0x21

	        jc      allocate_msdos_memory_block_jmp01

	        pop     ds
		and	eax, 0xffff
		mov	edx, [ebp + 12]         #  第二パラメータ
	        mov	[edx], eax
	        mov     eax, 0                  # #成功

	        jmp     allocate_msdos_memory_block_jmp02
allocate_msdos_memory_block_jmp01:

	        pop     ds
		and	ebx, 0xffff
		mov	edx, [ebp + 12]         #  第二パラメータ
	        mov	[edx], ebx
		and	eax, 0xffff             # #失敗 : eax = error code

allocate_msdos_memory_block_jmp02:

		pop	ecx
		pop	edx

		pop	ebx
		pop	edi
		pop	esi

		mov	esp,ebp
		pop	ebp
		ret


# 
#     int free_msdos_memory_block(unsigned int segment01)# 
#     ret: success = 0, failed != 0
# 


		.align	16
free_msdos_memory_block:
		push	ebp
		mov	ebp,esp

		push	esi
		push	edi
		push	ebx

		push	edx
		push	ecx
		push	ds

       		mov     ecx, [ebp + 8]
		and	ecx, 0xffff
        	mov     ax, 0x25c1
        	int     0x21

        	jc      free_msdos_memory_block_jmp01

        	pop     ds
        	mov     eax, 0                  # #成功

        	jmp     free_msdos_memory_block_jmp02
free_msdos_memory_block_jmp01:

        	pop     ds
        	and	eax, 0xffff             # #失敗 : eax = error code

free_msdos_memory_block_jmp02:

		pop	ecx
		pop	edx

		pop	ebx
		pop	edi
		pop	esi

		mov	esp,ebp
		pop	ebp
		ret


		.align	16
pokeb_far:
		push	ebp
		mov	ebp,esp

		push	edx

		push	fs
		mov	ax, [ebp + 8]
		mov	fs, ax
		mov	edx, [ebp + 12]
		mov	al, [ebp + 16]
		mov	fs:[edx], al
		pop	fs

		pop	edx

		mov	esp,ebp
		pop	ebp
		ret

		.align	16
pokew_far:
		push	ebp
		mov	ebp,esp

		push	edx

		push	fs
		mov	ax, [ebp + 8]
		mov	fs, ax
		mov	edx, [ebp + 12]
		mov	ax, [ebp + 16]
		mov	fs:[edx], ax
		pop	fs

		pop	edx

		mov	esp,ebp
		pop	ebp
		ret

		.align	16
pokel_far:
		push	ebp
		mov	ebp,esp

		push	edx

		push	fs
		mov	ax, [ebp + 8]
		mov	fs, ax
		mov	edx, [ebp + 12]
		mov	eax, [ebp + 16]
		mov	fs:[edx], eax
		pop	fs

		pop	edx

		mov	esp,ebp
		pop	ebp
		ret


		.align	16
peekb_far:
		push	ebp
		mov	ebp,esp

		push	edx

		push	fs
		mov	ax, [ebp + 8]
		mov	fs, ax
		mov	edx, [ebp + 12]
		xor	eax, eax
		mov	al, fs:[edx]
		pop	fs

		pop	edx

		mov	esp,ebp
		pop	ebp
		ret

peekw_far:
		push	ebp
		mov	ebp,esp

		push	edx

		push	fs
		mov	ax, [ebp + 8]
		mov	fs, ax
		mov	edx, [ebp + 12]
		xor	eax, eax
		mov	ax, fs:[edx]
		pop	fs

		pop	edx

		mov	esp,ebp
		pop	ebp
		ret

peekl_far:
		push	ebp
		mov	ebp,esp

		push	edx

		push	fs
		mov	ax, [ebp + 8]
		mov	fs, ax
		mov	edx, [ebp + 12]
		mov	eax, fs:[edx]
		pop	fs

		pop	edx

		mov	esp,ebp
		pop	ebp
		ret


#  int issue_realmode_interrupt(int intno, struct rmi_call_reg *reg_in, struct rmi_call_reg *reg_out);
		.globl	issue_realmode_interrupt

		.equ	issue_realmode_interrupt_rmi_ds,	 2
 		.equ	issue_realmode_interrupt_rmi_es,	 4
 		.equ	issue_realmode_interrupt_rmi_fs,	 6
 		.equ	issue_realmode_interrupt_rmi_eax,	 8
 		.equ	issue_realmode_interrupt_rmi_ebx,	12
 		.equ	issue_realmode_interrupt_rmi_ecx,	16
 		.equ	issue_realmode_interrupt_rmi_edx,	20
 		.equ	issue_realmode_interrupt_rmi_esi,	24
 		.equ	issue_realmode_interrupt_rmi_edi,	28
 		.equ	issue_realmode_interrupt_rmi_ebp,	32
 		.equ	issue_realmode_interrupt_rmi_gs,	36
 		.equ	issue_realmode_interrupt_rmi_carry,	38

issue_realmode_interrupt:
		push	ebp
		mov	ebp, esp
		
	        push    es
        	push    fs
        	push    gs

		pushfd
        	push    ebx
        	push    ecx
        	push    edx
        	push    esi
        	push    edi

		mov	edx, [ebp + 16]
		mov	[issue_realmode_interrupt_rmic_call_reg_addr_out], edx
		mov	edx, [ebp + 12]

		mov	eax, [ebp + 8]

		mov	[issue_realmode_interrupt_rmic_intno], ax

		mov	ax, [edx+issue_realmode_interrupt_rmi_ds]
		mov	[issue_realmode_interrupt_rmic_ds], ax
		mov	ax, [edx+issue_realmode_interrupt_rmi_es]
		mov	[issue_realmode_interrupt_rmic_es], ax
		mov	ax, [edx+issue_realmode_interrupt_rmi_fs]
		mov	[issue_realmode_interrupt_rmic_fs], ax
		mov	ax, [edx+issue_realmode_interrupt_rmi_gs]
		mov	[issue_realmode_interrupt_rmic_gs], ax

		mov	eax, [edx+issue_realmode_interrupt_rmi_eax]
		mov	[issue_realmode_interrupt_rmic_eax], eax
		mov	eax, [edx+issue_realmode_interrupt_rmi_edx]
		mov	[issue_realmode_interrupt_rmic_edx], eax

		mov	ebx, [edx+issue_realmode_interrupt_rmi_ebx]
		mov	ecx, [edx+issue_realmode_interrupt_rmi_ecx]
		mov	esi, [edx+issue_realmode_interrupt_rmi_esi]
		mov	edi, [edx+issue_realmode_interrupt_rmi_edi]

		push	ebp
		mov	ebp, [edx+issue_realmode_interrupt_rmi_ebp]
		push	ds

		mov	edx, offset issue_realmode_interrupt_rmic_intno
		mov	ax, 0x2511
		int	0x21

        	pop     ds
		mov	edx, [issue_realmode_interrupt_rmic_call_reg_addr_out]
		mov	[edx + issue_realmode_interrupt_rmi_ebp], ebp
		pop	ebp

		mov	[edx + issue_realmode_interrupt_rmi_eax], eax


		jc	issue_realmode_interrupt_jmp01
		mov	al, 0xff
		mov	[edx + issue_realmode_interrupt_rmi_carry], al
		jmp	issue_realmode_interrupt_jmp02
issue_realmode_interrupt_jmp01:
		xor	al, al
		mov	[edx + issue_realmode_interrupt_rmi_carry], al
issue_realmode_interrupt_jmp02:
		mov	[edx + issue_realmode_interrupt_rmi_ebx], ebx
		mov	[edx + issue_realmode_interrupt_rmi_ecx], ecx
		mov	[edx + issue_realmode_interrupt_rmi_esi], esi
		mov	[edx + issue_realmode_interrupt_rmi_edi], edi

		mov	ax, [issue_realmode_interrupt_rmic_ds]
		mov	[edx + issue_realmode_interrupt_rmi_ds], ax
		mov	ax, [issue_realmode_interrupt_rmic_es]
		mov	[edx + issue_realmode_interrupt_rmi_es], ax
		mov	ax, [issue_realmode_interrupt_rmic_fs]
		mov	[edx + issue_realmode_interrupt_rmi_fs], ax
		mov	ax, [issue_realmode_interrupt_rmic_gs]
		mov	[edx + issue_realmode_interrupt_rmi_gs], ax

		mov	eax, [issue_realmode_interrupt_rmic_edx]
		mov	[edx + issue_realmode_interrupt_rmi_edx], eax

		xor	eax, eax

        	pop     edi
        	pop     esi
        	pop     edx
        	pop     ecx
        	pop     ebx
        	popfd

        	pop     gs
        	pop     fs
        	pop     es
        
        	mov	esp, ebp
       		pop	ebp
        	ret


		.data

		.align	16
tdoscal_data_start:
issue_realmode_interrupt_rmic_intno:	.short	0
issue_realmode_interrupt_rmic_ds:	.short	0
issue_realmode_interrupt_rmic_es:	.short	0
issue_realmode_interrupt_rmic_fs:	.short	0
issue_realmode_interrupt_rmic_gs:	.short  0
issue_realmode_interrupt_rmic_eax:	.long	0
issue_realmode_interrupt_rmic_edx:	.long	0

issue_realmode_interrupt_rmic_call_reg_addr_out:	.long	0
issue_realmode_interrupt_rmic_call_esp_save:	.long	0
tdoscal_data_end:

		.att_syntax prefix
		.end


