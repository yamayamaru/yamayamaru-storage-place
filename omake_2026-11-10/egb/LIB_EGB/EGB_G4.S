/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.globl EGB_singlePset
	.globl EGB_singleLine
	.globl EGB_box
	.globl EGB_maru

/**************************************
	graphcs operation  4 / 4
   original fnuntions
 **************************************/

	.data
	.text
/*
*	EGB_singlePset( char *gwork, int x,y ) àÍì_ï`âÊ
*/
	.align 16
EGB_singlePset:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movw  $1,(%esi)
	movl  16(%esp),%eax
	movw  %ax,2(%esi)
	movl  20(%esp),%eax
	movw  %ax,4(%esi)
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
*	EGB_singleLine( char *gwork, int sx,sy,ex,ey ) àÍñ{ê¸ï`âÊ
*/
	.align 16
EGB_singleLine:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movw  $2,(%esi)
	movl  16(%esp),%eax
	movw  %ax,2(%esi)
	movl  20(%esp),%eax
	movw  %ax,4(%esi)
	movl  24(%esp),%eax
	movw  %ax,6(%esi)
	movl  28(%esp),%eax
	movw  %ax,8(%esi)
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
*	EGB_box( char *gwork, int sx,sy,ex,ey ) éläpÇï`Ç≠
*/
	.align 16
EGB_box:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movl  16(%esp),%eax
	movw  %ax,(%esi)
	movl  20(%esp),%eax
	movw  %ax,2(%esi)
	movl  24(%esp),%eax
	movw  %ax,4(%esi)
	movl  28(%esp),%eax
	movw  %ax,6(%esi)
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
*	EGB_maru( char *gwork, int x,y,r )  Ç‹ÇÈÇï`Ç≠
*/
	.align 16
EGB_maru:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movl  16(%esp),%eax
	movw  %ax,(%esi)
	movl  20(%esp),%eax
	movw  %ax,2(%esi)
	movl  24(%esp),%eax
	movw  %ax,4(%esi)
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

