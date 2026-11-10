/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.globl EGB_rotatePolygon
	.globl EGB_triangle
	.globl EGB_arc
	.globl EGB_fan
	.globl EGB_ellipse
	.globl EGB_ellipticArc
	.globl EGB_ellipticFan
	.globl EGB_bow
	.globl EGB_semiBow

/*********************************
	graphics operation 2 / 4
 *********************************/
	.data
	.text
/*
	EGB_rotatePolygon( char *gwork, char *param ) âÒì]ëÃ
*/
	.align 16
EGB_rotatePolygon:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x44,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_triangle( char *gwork, char *param ) éOäpå`
*/
	.align 16
EGB_triangle:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x45,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_arc( char *gwork, char *param ) â~å 
*/
	.align 16
EGB_arc:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x48,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_fan( char *gwork, char *param ) êÓå`
*/
	.align 16
EGB_fan:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x49,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_ellipse( char *gwork, char *param ) ë»â~
*/
	.align 16
EGB_ellipse:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x4a,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_ellipticArc( char *gwork, char *param ) ë»â~å 
*/
	.align 16
EGB_ellipticArc:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x4b,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_ellipticFan( char *gwork, char *param ) ë»êÓå`
*/
	.align 16
EGB_ellipticFan:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x4c,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_bow( char *gwork, char *param ) ã|å`(í âﬂ3ì_éwíË)
*/
	.align 16
EGB_bow:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x51,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
	EGB_semiBow( char *gwork, char *param ) ã|å`(îºåaéwíË)
*/
	.align 16
EGB_semiBow:
	push  %edi
	push  %esi
	movl  16(%esp),%esi
	movb  $0x51,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

	.end

