/*
	real mode & DOS system call support

		for GAS & GCC
				By Waku Factory
	1990/10/12   V1.0
*/
	.globl intdos
	.globl intreal
	.globl nativetoreal
	.globl segread

	.data
	.text
/*
	int intdos( union REGS cregs, union REGS rregs ) 
		if error occures return -1 
*/
	.align 16
intdos:
	enter $0,$0
	push  %edi
	push  %esi
	push  %ebx

	push  %ebp
	movl  8(%ebp),%esi
	pushl  (%esi)
	pushl  8(%esi)
	pushl  12(%esi)
	pushl  4(%esi)
	pushl  %esp
	pushl  %ebp
	pushl  16(%esi)
	pushl  20(%esi)
	popal
        int   $0x21
	pop   %ebp
	pushal
	movl  12(%ebp),%edi
	popl  20(%edi)
	popl  16(%edi)
	pop   %eax
	pop   %eax
	popl  4(%edi)
	popl  12(%edi)
	popl  8(%edi)
	popl  (%edi)
	mov   $0,%eax
	jae   __int_ret
	dec   %eax
__int_ret:
	pop   %ebx
	pop   %esi
	pop   %edi
	leave
	ret

/*
	intreal(int num, union REGS *cregs, union REGS *rregs, 
			struct SREGS *sregs ) 
*/ 

	.align 16
__intpacket:	.space 20

intreal:
	enter $0,$0
	push  %edi
	push  %esi
	push  %ebx

	movl  $__intpacket,%edx
	movl  8(%ebp),%eax
	movw  %ax,(%edx)
	movl  20(%ebp),%esi
	movw  (%esi),%ax
	movw  %ax,4(%edx)
	movw  6(%esi),%ax
	movw  %ax,2(%edx)
	movw  8(%esi),%ax
	movw  %ax,6(%edx)
	movw  10(%esi),%ax
	movw  %ax,8(%edx)

	movl  12(%ebp),%esi
	movl  (%esi),%eax
	movl  %eax,10(%edx)
	movl  12(%esi),%eax
	movl  %eax,14(%edx)

	push   %ebp
	pushl  $0x2511
	pushl  8(%esi)
	pushl  %edx
	pushl  4(%esi)
	pushl  %esp
	pushl  %ebp
	pushl  16(%esi)
	pushl  20(%esi)
	popal
	int    $0x21
	pop   %ebp
	pushal
	pushfl
	movl  16(%ebp),%edi
	popl  24(%edi)
	popl  20(%edi)
	popl  16(%edi)
	pop   %eax
	pop   %eax
	popl  4(%edi)
	pop   %edx
	popl  8(%edi)
	pop   %eax

	movl  %eax,(%edi)
	movl  14(%edx),%eax
	movl  %eax,12(%edi)

	pop   %ebx
	pop   %esi
	pop   %edi
	leave
	ret

/*
	load segment regs
*/

	.align 16
segread:
	push  %edi
	movl  8(%esp),%edi
	movw  %es,%ax
	movw  %ax,(%edi)
	movw  %cs,%ax
	movw  %ax,2(%edi)
	movw  %ss,%ax
	movw  %ax,4(%edi)
	movw  %ds,%ax
	movw  %ax,6(%edi)
	movw  %fs,%ax
	movw  %ax,8(%edi)
	movw  %gs,%ax
	movw  %ax,10(%edi)
	pop   %edi
	ret

/*
	address convert
	int nativetoreal( char *native,int len, int *seg, int *ofs )
*/
	.align 16
nativetoreal:
	push %ebx
	push %es

	push  %ds
	pop  %es
	movl 12(%esp),%ebx
	movl 16(%esp),%ecx
	movw $0x250f,%ax
	int  $0x21
	jb   _err_ret
	movl 24(%esp),%ebx
	movw %cx,(%ebx)
	movl 20(%esp),%ebx
	shrl $16,%ecx
	movw %cx,(%ebx)
	xorl %eax,%eax
	pop  %es
	pop  %ebx
	ret

_err_ret:
	movl $-1,%eax
	pop  %es
	pop  %ebx
	ret

	.end

