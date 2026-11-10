/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.data
	.text
/***********************************
	block operation  4 / 5
 ***********************************/
	.globl EGB_graphicCursor
	.globl EGB_maskData
	.globl EGB_scroll
	.globl EGB_partScroll
/*
	EGB_graphicCursor(char *gwork, int clip, char *param) グラフィックカーソル
*/
	.align 16
EGB_graphicCursor:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $0x28,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_maskData( char *gwork, int clip, char *param ) マスクデータの書き込み
*/
	.align 16
EGB_maskData:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $0x29,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_scroll( char *gwork, int mode, dx, dy ) 全画面スクロール
*/
	.align 16
EGB_scroll:
	push  %edi
	push  %ebx
	movb  16(%esp),%al
	movl  20(%esp),%edx
	movl  24(%esp),%ebx
	movb  $0x2A,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

/*
	EGB_partScroll( char *gwork, int mode, dx, dy, char *param ) 部分スクロール
*/
	.align 16
EGB_partScroll:
	push  %edi
	push  %esi
	push  %ebx
	movb  20(%esp),%al
	movl  24(%esp),%edx
	movl  28(%esp),%ebx
	movl  32(%esp),%esi
	movb  $0x2B,%ah
	movl  16(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %esi
	pop   %edi
	ret


	.end

