/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.1
	     90/10/11     1.2
*/
/****************************************
   setting operations  2/5
*****************************************/
	.data
	.text
 .globl EGB_color
 .globl EGB_foreColor
 .globl EGB_backColor
 .globl EGB_paintColor
 .globl EGB_transColor
 .globl EGB_colorIGRB
 .globl EGB_foreColorIGRB
 .globl EGB_backColorIGRB
 .globl EGB_paintColorIGRB
 .globl EGB_transColorIGRB
 .globl EGB_pastel
 .globl EGB_writeMode

/*
	EGB_color( char *gwork, int mode, int color )  描画色の設定(パレットコード)
*/
	.align 16
EGB_color:
	push  %edi
	movb  12(%esp),%al
	movl  16(%esp),%edx
	movb  $7,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_foreColor( char *gwork,int col ) 前景色の設定(パレットコード)
*/
	.align 16
EGB_foreColor:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x700,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_backColor( char *gwork, int col ) 背景色の設定(パレットコード)
*/
	.align 16
EGB_backColor:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x701,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_paintColor( char *gwork,int col ) 面塗り色の設定(パレットコード)
*/
	.align 16
EGB_paintColor:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x702,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_transColor( char *gwork, int col ) 透過色の設定(パレットコード)
*/
	.align 16
EGB_transColor:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x703,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_colorIGRB( char *gwork,int mode, int col ) 描画色の設定(IGRBコード)
*/
	.align 16
EGB_colorIGRB:
	push  %edi
	movb  12(%esp),%al
	movl  16(%esp),%edx
	movb  $8,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_foreColorIGRB( char *gwork,int col ) 前景色の設定(IGRBコード)
*/
	.align 16
EGB_foreColorIGRB:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x800,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_backColorIGRB( char *gwork,int col ) 背景色の設定(IGRBコード)
*/
	.align 16
EGB_backColorIGRB:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x801,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_paintColorIGRB( char *gwork, int col ) 面塗り色の設定(IGRBコード)
*/
	.align 16
EGB_paintColorIGRB:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x802,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
*	EGB_transColorIGRB( char *gwork, int col ) 透過色の設定(IGRBコード)
*/
	.align 16
EGB_transColorIGRB:
	push  %edi
	movl  12(%esp),%edx
	movw  $0x803,%ax
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_pastel( char *gwork, int level ) Pastelモード描画の混色比
*/
	.align 16
EGB_pastel:
	push  %edi
	movw  12(%esp),%dx
	movb  $9,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_writeMode( char *gwork, int mode )  描画モードの設定
                         0 <= mode <= 15
*/
	.align 16
EGB_writeMode:
	push  %edi
	movb  12(%esp),%al
	movb  $10,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

	.end

