/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.1
	     90/10/11     1.2
*/
/****************************************
   setting operations  4/5
*****************************************/
	.data
	.text
 .globl EGB_pen
 .globl EGB_penSize
 .globl EGB_penStyle


/*
	EGB_pen( char *gwork, int penmode ) 描画ペンのモード設定
*/
	.align 16
EGB_pen:
	push  %edi
	movb  12(%esp),%al
	movb  $0x11,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_penSize( char *gwork, int pensize ) 描画ペンの太さ設定
                0 <= pensize <= 32
*/
	.align 16
EGB_penSize:
	push  %edi
	movb  12(%esp),%al
	movb  $0x12,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_penStyle( char *gwork, char *pattern ) 描画ペン形状定義
*/
	.align 16
EGB_penStyle:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x13,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

	.end

