/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.globl EGB_getBlockColor
	.globl EGB_putBlockColor
	.globl EGB_getBlock
	.globl EGB_putBlock
	.globl EGB_getBlockZoom
	.globl EGB_putBlockZoom


	.data
	.text
/****************************************
     EGB  block operation  2 / 3
 ****************************************/

/*
	EGB_getBlockColor( char *gwork, char *param ) 矩形領域読みだし(色指定)
*/
	.align 16
EGB_getBlockColor:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x22,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_putBlockColor( char *gwork,int clip, char *param ) 矩形領域書き込み(色指定)
*/
	.align 16
EGB_putBlockColor:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $0x23,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_getBlock( char *gwork,char *param ) 矩形領域読みだし(フルカラー)
*/
	.align 16
EGB_getBlock:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x24,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_putBlock( char *gwork, int clip, char *param ) 矩形領域書き込み(フルカラー)
*/
	.align 16
EGB_putBlock:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $0x25,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_getBlockZoom( char *gwork, char *param ) 矩形領域読みだし(ズーム付)
*/
	.align 16
EGB_getBlockZoom:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x26,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_putBlockZoom( char *gwork,int clip, char *param ) 矩形領域書き込み(ズーム付)
*/
	.align 16
EGB_putBlockZoom:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $0x27,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

	.end

