/*
	VRAM Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 0.9
*/
	.globl VRAM_putPixelW
	.globl VRAM_getPixelW
	.globl VRAM_putPixelB
	.globl VRAM_getPixelB

	.data
	.text

/*
	VRAM_putPixelW( int pos, color ) VRAMへのワード書き込み 
*/
	.align 16
VRAM_putPixelW:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	movl  16(%esp),%eax
	push  $0x104
	pop   %ds
	movw  %ax,(%edi)
	pop   %ds
	pop   %edi
	ret

/*
	VRAM_getPixelW( int pos )  VRAMデータのワード読込
*/
	.align 16
VRAM_getPixelW:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	push  $0x104
	pop   %ds
	movw  (%edi),%ax
	movzwl  %ax,%eax
	pop   %ds
	pop   %edi
	ret

/*
	VRAM_putPixelB( int pos, color ) VRAMへのバイト書き込み 
*/
	.align 16
VRAM_putPixelB:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	movl  16(%esp),%eax
	push  $0x104
	pop   %ds
	movb  %al,(%edi)
	pop   %ds
	pop   %edi
	ret

/*
	VRAM_getPixelB( int pos )  VRAMデータのバイト読込
*/
	.align 16
VRAM_getPixelB:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	push  $0x104
	pop   %ds
	movb  (%edi),%al
	movzbl  %al,%eax
	pop   %ds
	pop   %edi
	ret

	.end

