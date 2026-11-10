/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.1
	        10/11     1.2  	separate egbwork
*/
/****************************************
   setting operations  1/5
 *****************************************/
	.data
	.text
 .globl EGB_init
 .globl EGB_resolution
 .globl EGB_resolutionRAM
 .globl EGB_displayStart
 .globl EGB_displayOffset
 .globl EGB_screenOffset
 .globl EGB_screenZoom
 .globl EGB_screenWidth
 .globl EGB_viewport
 .globl EGB_viewportRect
 .globl EGB_palette
 .globl EGB_setPalette
 .globl EGB_writePage
 .globl EGB_displayPage

	.globl  EGB_param

	.align 16
	.comm EGB_param,512

/*
	EGB_init(char *gwork, int size) グラフィックBIOSの初期化
*/
	.align 16
EGB_init:
	push  %edi
	push  %ds
	pop   %gs
	push  %ds
	pop   %es
	movl  12(%esp),%ecx
	movl  8(%esp),%edi
	xorb  %ah,%ah
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_resolution( char *gwork, int page, int mode )  画面モードの設定
				  page = 0 or 1
                                  1 <= mode <= 18
*/
	.align 16
EGB_resolution:
	push  %edi
	movb  12(%esp),%al
	movw  16(%esp),%dx
	movb  $1,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_resolutionRAM( char *gwork, int page, pixel, width, height, char *ram )
		RAM領域仮想画面モードの設定
*/
	.align 16
EGB_resolutionRAM:
	push  %edi
	push  %esi
	push  %ebx
	movb  20(%esp),%al
	movb  24(%esp),%cl
	movw  28(%esp),%dx
	movw  32(%esp),%bx
	movl  36(%esp),%esi
	movb  $1,%ah
	movl  16(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %esi
	pop   %edi
	ret

/*
	EGB_displayStart( char *gwork, int mode, x, y ) 表示開始位置の設定
*/
	.align 16
EGB_displayStart:
	push  %edi
	push  %ebx
	movb  16(%esp),%al
	movw  20(%esp),%dx
	movw  24(%esp),%bx
	movb  $2,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

/*
*	EGB_displayOffset( char *gwork, int sx,sy ) *表示画面の画面枠内の移動
*/
	.align 16
EGB_displayOffset:
	push  %edi
	push  %ebx
	movw  16(%esp),%dx
	movw  20(%esp),%bx
	movw  $0x200,%ax
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret
	
/*
*	EGB_screenOffset( char *gwork,int sx,sy ) VRAM中の表示領域の開始座標
*/
	.align 16
EGB_screenOffset:
	push  %edi
	push  %ebx
	movw  16(%esp),%dx
	movw  20(%esp),%bx
	movw  $0x201,%ax
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

/*
*	EGB_screenZoom( char *gwork, int wx,wy ) 画面の拡大
*/
	.align 16
EGB_screenZoom:
	push  %edi
	push  %ebx
	movw  16(%esp),%dx
	movw  20(%esp),%bx
	movw  $0x202,%ax
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

/*
*	EGB_screenWidth( char *gwork, int wx,wy ) 画面の大きさ設定
*/
	.align 16
EGB_screenWidth:
	push  %edi
	push  %ebx
	movw  16(%esp),%dx
	movw  20(%esp),%bx
	movw  $0x203,%ax
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

/*
	EGB_viewport( char *gwork, char *param )
*/
	.align 16
EGB_viewport:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $3,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
*	EGB_viewportRect( char *gwork,int sx,sy,ex,ey ) ビューポート設定
*/
	.align 16
EGB_viewportRect:
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
	movb  $3,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_palette( char *gwork, int syncf, char *param ) パレットの設定
*/
	.align 16
EGB_palette:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $4,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
*	EGB_setPalette( char *gwork ,int num, int g, int r, ing b ) 1つのパレット設定
*/
		.align 16
EGB_setPalette:
	push  %edi
	push  %esi
	movl  $EGB_param,%esi
	movl  $1,(%esi)
	movl  16(%esp),%eax
	movl  %eax,4(%esi)
	movb  20(%esp),%al
	movb  %al,8(%esi)
	movb  24(%esp),%al
	movb  %al,9(%esi)
	movb  28(%esp),%al
	movb  %al,10(%esi)
	movw  $0x400,%ax
	movb  %al,11(%esi)
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_writePage( char *gwork, int page ) 書き込みページの設定
*/
	.align 16
EGB_writePage:
	push  %edi
	movb  12(%esp),%al
	movb  $5,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_displayPage( char *gwork, int prior,page ) 表示ページの設定
*/
	.align 16
EGB_displayPage:
	push  %edi
	movl  16(%esp),%edx
	movb  8(%esp),%al
	movb  $6,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

	.end

