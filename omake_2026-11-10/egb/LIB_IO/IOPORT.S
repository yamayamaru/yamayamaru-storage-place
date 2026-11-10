/*
	functions for GNU assembler
		by Waku Factory
*/
	.globl INPB
	.globl INPW
	.globl OUTPB
	.globl OUTPW
	.globl CRTC
	.globl INKEY
	.globl LAMP

/******************************************
	hardware access functions 
 ******************************************/
	.data
	.text
/*
	int INPB( int addr ) input byte from port
*/
	.align 16
INPB:
	movw 4(%esp),%dx
	inb %dx,%al
	movzbl %al,%eax
	ret

/*
	int INPW( int addr ) input word form port
*/
	.align 16
INPW:
	movw 4(%esp),%dx
	inw %dx,%ax
	movzwl %ax,%eax
	ret

/*
	void OUTPB( int addr, int data ) output byte to port
*/
	.align 16
OUTPB:
	movw 4(%esp),%dx
	movb 8(%esp),%al
	outb %al,%dx
	ret

/*
	void OUTPW( int addr, int data ) output word to port
*/
	.align 16
OUTPW:
	movw 4(%esp),%dx
	movw 8(%esp),%ax
	outw %ax,%dx
	ret

/*
	void CRTC( int reg, int data ) set data to CRTC
*/
	.align 16
CRTC:
	movw $0x440,%dx
	movb 4(%esp),%al
	outb %al,%dx
	movw 8(%esp),%ax
	leal 2(%edx),%edx
	outw %ax,%dx
	ret

/*
	int INKEY()  read keyboard matrix data 
*/
	.align 16
INKEY:
	xorl %eax,%eax
	movw $0x600,%dx
	inb  %dx,%al
	ret

/*
	void LAMP( int data ) debug lamp   ( TOWNS model 1/2 only )
*/
	.align 16
LAMP:
	movb 4(%esp),%al
	movw $0x4EC,%dx
	outb %al,%dx
	ret

	.end

