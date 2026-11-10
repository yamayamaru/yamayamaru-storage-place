/*
	T-BIOS Graphics Library
		for GNU assembler
	            by Waku Factory
             90/03/20 Ver 1.0
 	     90/10/11     1.2
*/
	.globl EGB_region
	.globl EGB_copy
	.globl EGB_rotate
	.globl EGB_resolve

/**********************************
	block operation 5 / 5
 **********************************/
	.data
	.text
/*
	EGB_region( char *gwork, int *in,*work,*x,*y,*x2,*y2, char *poly ) óÃàÊê›íË
*/
	.align 16
EGB_region:
	push  %edi
	push  %esi
	push  %ebx
	movl  20(%esp),%edi
	movb  (%edi),%al
	movl  28(%esp),%edi
	movw  (%edi),%dx
	movl  32(%esp),%edi
	movw  (%edi),%bx
	movl  44(%esp),%esi
	movb  $0x2c,%ah
	movl  16(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	push  %ebp
	movl  20(%esp),%ebp
	movb  %al,(%ebp)
	movl  24(%esp),%ebp
	movl  %ecx,(%ebp)
	movl  28(%esp),%ebp
	movw  %dx,(%ebp)
	movl  32(%esp),%ebp
	movw  %bx,(%ebp)
	movl  36(%esp),%ebp
	movw  %si,(%ebp)
	movl  40(%esp),%ebp
	movw  %di,(%ebp)
	movsbl %ah,%eax
	pop   %ebp
	pop   %ebx
	pop   %esi
	pop   %edi
	ret

/*
	EGB_copy( char *gwork, int mode, char *param, char *work ) âÊñ ÇÃï°é 
*/
	.align 16
EGB_copy:
	push  %edi
	push  %esi
	push  %ebx
	movb  20(%esp),%al
	movl  24(%esp),%esi
	movl  28(%esp),%ebx
	movb  $0x2d,%ah
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
	EGB_rotate(char *gwork, int mode, char *param,  char *work ) âÊñ ÇÃâÒì]
*/
	.align 16
EGB_rotate:
	push  %edi
	push  %esi
	push  %ebx
	movb  20(%esp),%al
	movl  24(%esp),%esi
	movl  28(%esp),%ebx
	movb  $0x2e,%ah
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
	EGB_resolve( char *gwork, char *work ) âÊñ ÇÃÇ⁄Ç©Çµ
*/
	.align 16
EGB_resolve:
	push  %edi
	push  %ebx
	movl  16(%esp),%ebx
	movb  $0x2f,%ah
	movl  12(%esp),%edi
	push  $0x0110
	pop   %fs
	lcall %fs:(0x20)
	movsbl %ah,%eax
	pop   %ebx
	pop   %edi
	ret

	.end


