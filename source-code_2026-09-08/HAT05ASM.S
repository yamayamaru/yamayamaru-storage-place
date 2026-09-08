
	.globl	_map_physical_memory_at_end_of_segment
	.globl	_allocate_segment
	.globl	_free_segment

	.data
	.text

/*
; int map_physical_memory_at_end_of_segment(unsigned int segment, unsgined int base_addr, int number_of_pages, unsigned int *segment_off);
*/
	.align	4
_map_physical_memory_at_end_of_segment:
	pushl	%ebp
	movl	%esp, %ebp

	pushfl
	pushw	%es
	pushw	%fs
	pushw	%gs
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi

	movl	8(%ebp), %eax
	movw	%ax, %es
	movl	12(%ebp), %ebx
	movl	16(%ebp), %ecx

	movl	$0x250a, %eax
	int	$0x21

	jc	map_physical_memory_at_end_of_segment_jmp01

	movl	20(%ebp), %ebx		/* success */
	movl	%eax, (%ebx)		/* save segment offset address */
	xorl	%eax, %eax		/* if success then eax = 0 */

map_physical_memory_at_end_of_segment_jmp01:
					/* if failure then eax = error code */
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popw	%gs
	popw	%fs
	popw	%es
	popfl

	popl	%ebp
	ret



/*
; int allocate_segment(unsigned int allocate_pages, unsigned int *segment_sel);
*/
	.align	4
_allocate_segment:
	pushl	%ebp
	movl	%esp, %ebp

	pushfl
	pushw	%es
	pushw	%fs
	pushw	%gs
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi

	movl	8(%ebp), %ebx
	movb	$0x48, %ah
	int	$0x21

	jc	allocate_segment_jmp01

	xorl	%edx, %edx		/* success */
	movw	%ax, %dx
	movl	12(%ebp), %ebx
	movl	%edx, (%ebx)
	xorl	%eax, %eax
	jmp	allocate_segment_jmp02

allocate_segment_jmp01:
					/* if failure then eax = error code */
	movl	12(%ebp), %edx
	movl	%ebx, (%edx)		/* save number of free pages of pysical momoery available */

allocate_segment_jmp02:

	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popw	%gs
	popw	%fs
	popw	%es
	popfl

	popl	%ebp
	ret


/*
; int free_segment(unsigned int segment_sel);
*/
	.align	4
_free_segment:
	pushl	%ebp
	movl	%esp, %ebp

	pushfl
	pushw	%es
	pushw	%fs
	pushw	%gs
	pushl	%ebx
	pushl	%ecx
	pushl	%edx
	pushl	%esi
	pushl	%edi

	movl	8(%ebp), %eax
	movb	%ax, %es
	movb	$0x49, %ah
	int	$0x21

	jc	free_segnemt_jmp01

	xorl	%eax, %eax		/* if success then eax = 0; */

free_segnemt_jmp01:
					/* if failure then eax = error code */
	popl	%edi
	popl	%esi
	popl	%edx
	popl	%ecx
	popl	%ebx
	popw	%gs
	popw	%fs
	popw	%es
	popfl

	popl	%ebp
	ret
