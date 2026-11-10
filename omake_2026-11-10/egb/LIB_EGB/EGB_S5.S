/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.1
	     90/10/11     1.2
*/

/****************************************
   setting operations  5/5
*****************************************/
	.data
	.text
 .globl EGB_textDirection
 .globl EGB_textDisplayDirection
 .globl EGB_textSpace
 .globl EGB_textZoom
 .globl EGB_fontStyle
 .globl EGB_textAttrib

 .globl EGB_superImpose
 .globl EGB_dezitize
 .globl EGB_digitize
 
/*
	EGB_textDirection( char *gwork, int dir ) 文字方向の設定
*/
	.align 16
EGB_textDirection:
	push  %edi
	movb  12(%esp),%al
	movb  $0x15,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_textDisplayDirection( char *gwork, int dir ) 文字表示方向の設定
*/
	.align 16
EGB_textDisplayDirection:
	push  %edi
	movb  12(%esp),%al
	movb  $0x16,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_textSpace( char  *gwork, int space ) 文字間空白の設定
*/
	.align 16
EGB_textSpace:
	push  %edi
	movw  12(%esp),%dx
	movb  $0x17,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_textZoom( char *gwork,  int mode, x, y ) 文字拡大率の設定
*/
	.align 16
EGB_textZoom:
	push  %edi
	push  %ebx
	movb  16(%esp),%al
	movw  20(%esp),%dx
	movw  24(%esp),%bx
	movb  $0x18,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

/*
	EGB_fontStyle( char *gwork, int style) 字体の設定
*/
	.align 16
EGB_fontStyle:
	push  %edi
	movw  12(%esp),%dx
	movb  $0x19,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret
/*
*	EGB_textAttrib( char *gwork, int mx,my, style, dir,ddir,space ) 文字属性の設定
*/
	.align 16
EGB_textAttrib:
	push  %edi
	push  %ebx
	movl  12(%esp),%edi
	movw  16(%esp),%dx
	movw  20(%esp),%bx
	movw  $0x1800,%ax
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	addw  %dx,%dx
	movw  $0x1801,%ax
	lcall %fs:(0x20)
	movw  24(%esp),%dx
	movb  $0x19,%ah
	lcall %fs:(0x20)
	movb  28(%esp),%al
	movb  $0x15,%ah
	movb  32(%esp),%al
	movb  $0x16,%ah
	lcall %fs:(0x20)
	movw  34(%esp),%dx
	movb  $0x17,%ah
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

/*
	EGB_superImpose( char *gwork, int mode ) スーパーインポーズ
*/
	.align 16
EGB_superImpose:
	push  %edi
	movb  12(%esp),%al
	movb  $0x1a,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_digitize( char *gwork,int mode) ディジタイズ
*/
	.align 16
EGB_dezitize:
EGB_digitize:
	push  %edi
	movb  12(%esp),%al
	movb  $0x1b,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

	.end

