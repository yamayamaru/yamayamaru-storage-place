/*
	VRAM Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 0.9
*/
	.globl VRAM_putPixelW2
	.globl VRAM_getPixelW2
	.globl VRAM_putPixelB2
	.globl VRAM_getPixelB2

/*
	VRAM access for single page mode
*/
	.data
	.text

/*
	VRAM_putPixelW2( int pos, color ) VRAMへのワード書き込み 
*/
	.align 16
VRAM_putPixelW2:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	movl  16(%esp),%eax
	push  $0x10c
	pop   %ds
	movw  %ax,(%edi)
	pop   %ds
	pop   %edi
	ret

/*
	VRAM_getPixelW2( int pos )  VRAMデータのワード読込
*/
	.align 16
VRAM_getPixelW2:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	push  $0x10c
	pop   %ds
	movw  (%edi),%ax
	movzwl  %ax,%eax
	pop   %ds
	pop   %edi
	ret

/*
	VRAM_putPixelB2( int pos, color ) VRAMへのバイト書き込み 
*/
	.align 16
VRAM_putPixelB2:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	movl  16(%esp),%eax
	push  $0x10c
	pop   %ds
	movb  %al,(%edi)
	pop   %ds
	pop   %edi
	ret

/*
	VRAM_getPixelB2( int pos )  VRAMデータのバイト読込
*/
	.align 16
VRAM_getPixelB2:
	push  %edi
	push  %ds
	movl  12(%esp),%edi
	push  $0x10c
	pop   %ds
	movb  (%edi),%al
	movzbl  %al,%eax
	pop   %ds
	pop   %edi
	ret

	.end

