/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
	     90/10/11     1.2
*/
	.globl EGB_putSjis
	.globl EGB_putSjisAdd

/************************************
	strings display
 ************************************/
	.data
	.text

/*
*	EGB_putSjis( char *gwork, int x,y,  char *str ) •¶Žš—ñ‚Ì•\Ž¦
*/
	.align 16
EGB_putSjis:
	push  %edi
	push  %esi
	movl  $EGB_param,%edi
	movl  16(%esp),%eax
	stosw
	movl  20(%esp),%eax
	stosw
	addl  $2,%edi
	movl  24(%esp),%esi
	xor   %cx,%cx
_loop:
        lodsb
	test  %al,%al
        jz    _eloop
        stosb
	inc   %cx
	jmp   _loop
_eloop:
        movl  $EGB_param,%esi
	movw  %cx,4(%esi)
 
	movb  $0x060,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

/*
*	EGB_putSjisAdd( char *gwork, char *str ) •¶Žš—ñ‚Ì’Ç‰Á•\Ž¦
*/
	.align 16
EGB_putSjisAdd:
	push  %edi
	push  %esi
	movl  $EGB_param,%edi
	addl  $2,%edi
	movl  16(%esp),%esi
	xor   %cx,%cx
_loop2:
        lodsb
	test  %al,%al
        jz    _eloop2
        stosb
	inc   %cx
	jmp   _loop2
_eloop2:
        movl  $EGB_param,%esi
	movw  %cx,(%esi)
 
	movb  $0x61,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %esi
	pop   %edi
	ret

	.end

