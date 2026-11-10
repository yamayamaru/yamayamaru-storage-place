/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.data
	.text

	.globl	VRAM_clear
/*
	VRAM_clear( int page ) VRAMÇÃÉ[ÉçÉNÉäÉA
*/
	.align 16
VRAM_clear:
	push  %edi
	push  %es
	push  $0x104
	pop   %es
	xor   %edi,%edi
	movl  $0x40000,%ecx
	movb  12(%esp),%al
	jz    _L1
	movl  %ecx,%edi   
_L1:
	rep
	stosl
	pop   %es
	pop   %edi
	ret

	.end

