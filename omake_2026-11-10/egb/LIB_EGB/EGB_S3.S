/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.1
             90/10/11     1.2
*/
/****************************************
   setting operations  3/5
*****************************************/
	.data
	.text
 .globl EGB_linePattern
 .globl EGB_paintMode
 .globl EGB_hatchingPattern
 .globl EGB_tilePattern
 .globl EGB_maskRegion
 .globl EGB_mask
 .globl EGB_maskBit


/*
	EGB_linePattern( char *gwork,int mode, pattern ) 線分パターンの設定
                    mode = 0 or 1 
*/
	.align 16
EGB_linePattern:
	push  %edi
	movb  12(%esp),%al
	movl  16(%esp),%edx
	movb  $11,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_paintMode( char *gwork, int mode ) ペイントモードの設定
*/
	.align 16
EGB_paintMode:
	push  %edi
	movw  12(%esp),%dx
	movb  $12,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

/*
	EGB_hatchingPattern( char *gwork, int mode,x,y, char *pat ) ハッチングパターン設定
*/
	.align 16
EGB_hatchingPattern:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movb  20(%esp),%bh
	movb  24(%esp),%bl
	movl  28(%esp),%esi
	movb  $13,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_tilePattern( char *gwork, int mode, x, y, char *pat ) タイルパターン設定
*/
	.align 16
EGB_tilePattern:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movb  20(%esp),%bh
	movb  24(%esp),%bl
	movl  28(%esp),%esi
	movb  $14,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_maskRegion(char *gwork, char *pat ) 画面マスク領域の設定
*/
	.align 16
EGB_maskRegion:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $15,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_mask( char *gwork,int mode ) マスクモードの設定
*/
	.align 16
EGB_mask:
	push  %edi
	movb  12(%esp),%al
	movb  $16,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret
/*
	EGB_maskBit( char *gwork, int bit ) マスクビットの設定
*/
	.align 16
EGB_maskBit:
	push  %edi
	movl  12(%esp),%edx
	movb  $0x14,%ah
	movl  8(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %edi
	ret

        .end

