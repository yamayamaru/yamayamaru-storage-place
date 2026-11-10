/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.globl EGB_paint
	.globl EGB_closePaint
	.globl EGB_point

/*********************************
	graphics operation 3 / 4 
 *********************************/
	.data
	.text

/*
	EGB_paint( char *gwork, char *param ) ペイント(境界色指定)
*/
	.align 16
EGB_paint:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x4d,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_closePaint( char *gwork, char *param ) ペイント(色変更)
*/
	.align 16
EGB_closePaint:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x4e,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_point( char *gwork, int mode, int x,y , int *result) ポイント色を得る
*/
	.align 16
EGB_point:
	push  %edi
	push  %ebx
	movb  16(%esp),%al
	movl  20(%esp),%edx
	movl  24(%esp),%ebx
	movb  $0x4f,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	movl  28(%esp),%edi
	movl  %edx,(%edi)
	pop   %ebx
	pop   %edi
	ret

	.end

