/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
		90/03/20 Ver 1.0
		90/10/11     1.2
*/
	.globl EGB_getRectColor
	.globl EGB_putRectColor
	.globl EGB_getRect
	.globl EGB_putRect

/**************************************
	block operation  3 / 5
 **************************************/
	.data
	.text
/*
*	EGB_getRectColor( char *gwork, char *buf,int sx,sy,ex,ey,color) 矩形領域読込(色指定)
*/
	.align 16
EGB_getRectColor:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movl  16(%esp),%eax
	movl  %eax,(%esi)
	movw  %ds,%ax
	movw  %ax,4(%esi)
	movl  20(%esp),%eax
	movw  %ax,6(%esi)
	movl  24(%esp),%eax
	movw  %ax,8(%esi)
	movl  28(%esp),%eax
	movw  %ax,10(%esi)
	movl  32(%esp),%eax
	movw  %ax,12(%esi)
	movw  $1,14(%esi)
	movl  36(%esp),%eax
	movl  %eax,16(%esi)

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
*	EGB_putRectColor( char *gwork, int clip, char *buf, int sx,sy,ex,ey)
		矩形領域書き込み(単色)
*/
	.align 16
EGB_putRectColor:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movl  20(%esp),%eax
	movl  %eax,(%esi)
	movw  %ds,%ax
	movw  %ax,4(%esi)
	movl  24(%esp),%eax
	movw  %ax,6(%esi)
	movl  28(%esp),%eax
	movw  %ax,8(%esi)
	movl  32(%esp),%eax
	movw  %ax,10(%esi)
	movl  36(%esp),%eax
	movw  %ax,12(%esi)
	movb  16(%esp),%al

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
*	EGB_getRect( char *gwork, char *buf, int sx,sy,ex,ey ) 矩形領域読込(フルカラー)
*/
	.align 16
EGB_getRect:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movl  16(%esp),%eax
	movl  %eax,(%esi)
	movw  %ds,%ax
	movw  %ax,4(%esi)
	movl  20(%esp),%eax
	movw  %ax,6(%esi)
	movl  24(%esp),%eax
	movw  %ax,8(%esi)
	movl  28(%esp),%eax
	movw  %ax,10(%esi)
	movl  32(%esp),%eax
	movw  %ax,12(%esi)

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
*	EGB_putRect( char *gwork, int clip, char *buf, int sx,sy,ex,ey ) 矩形領域書き込み
*/
	.align 16
EGB_putRect:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movl  20(%esp),%eax
	movl  %eax,(%esi)
	movw  %ds,%ax
	movw  %ax,4(%esi)
	movl  24(%esp),%eax
	movw  %ax,6(%esi)
	movl  28(%esp),%eax
	movw  %ax,8(%esi)
	movl  32(%esp),%eax
	movw  %ax,10(%esi)
	movl  36(%esp),%eax
	movw  %ax,12(%esi)
	movb  16(%esp),%al

	movb  $0x25,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

	.end
 
