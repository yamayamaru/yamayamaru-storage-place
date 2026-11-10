/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.globl EGB_pset
	.globl EGB_connect
	.globl EGB_unConnect
	.globl EGB_polygon
	.globl EGB_rectangle
	.globl EGB_circle

/*********************************
	graphics operation 1 / 4
 *********************************/
	.data
	.text

/*
	EGB_pset( char *gwork, char *parm ) ポイント
*/
	.align 16
EGB_pset:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x40,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_connect( char *gwork, char *param) 連続線分
*/
	.align 16
EGB_connect:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x41,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_unConnect( char *gwork, char *param ) 不連続線分
*/
	.align 16
EGB_unConnect:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x42,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_polygon( char *gwork, char *param ) ポリゴン
*/
	.align 16
EGB_polygon:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x43,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_rectangle( char *gwork, char *param ) 矩形
*/
	.align 16
EGB_rectangle:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x46,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_circle( char *gwork, char *param ) ○
*/
	.align 16
EGB_circle:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x47,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

	.end

