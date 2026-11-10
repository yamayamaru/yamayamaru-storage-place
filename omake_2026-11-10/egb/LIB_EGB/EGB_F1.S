/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.globl EGB_sjisString
	.globl EGB_connectSjisString
	.globl EGB_asciiString
	.globl EGB_connectAsciiString
	.globl EGB_jisString
	.globl EGB_connectJisString
	.globl EGB_anyChar

/***************************************
	font operations
 ***************************************/
	.data
	.text
/*
	EGB_sjisString( char *gwork, char *param ) 漢字文字列
*/
	.align 16
EGB_sjisString:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x60,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_connectSjisString( char *gwork, char *param ) 追加漢字文字列
*/
	.align 16
EGB_connectSjisString:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x61,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_asciiString( char *gwork, int font, char *param ) JIS8ビット文字列
*/
	.align 16
EGB_asciiString:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $0x62,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_connectAsciiString( char *gwork, int font, char *param ) 追加JIS文字列
*/
	.align 16
EGB_connectAsciiString:
	push  %edi
	push  %esi
	movb  16(%esp),%al
	movl  20(%esp),%esi
	movb  $0x63,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_jisString( char *gwork, char *param ) JISコード文字列
*/
	.align 16
EGB_jisString:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x64,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_connectJisString( char *gwork, char *param ) 追加JISコード文字列
*/
	.align 16
EGB_connectJisString:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x65,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_anyChar( char *gwork, int wx,wy,  char *pattern ) 任意文字表示
*/
	.align 16
EGB_anyChar:
	push  %edi
	push  %esi
	push  %ebx
	movl  16(%esp),%edx
	movl  20(%esp),%ebx
	movl  24(%esp),%esi
	movb  $0x66,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %esi
	pop   %edi
	ret

	.end

