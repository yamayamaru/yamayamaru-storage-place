		.intel_syntax noprefix

		.extern		gtm11, gtm21, gtm31, tmdp1, putcar1
		.globl		main01


#		.equ		dcml,0x65640              #100万桁         int(1000000 / log10(256) + 55)     (必ず4の倍数にすること)
#		.equ		dcml, 0xa240              #10万桁
		.equ		dcml, 0x1040              #1万桁
#		.equ		dcml,  0x1a8              #1000桁


		.data
		.align		16
tim1:		.string		"start  00:00:00.00"
		.byte		0xd, 0xa
tim2:		.string		"end_1  00:00:00.00"
		.byte		0xd, 0xa
tim3:		.string		"end_2  00:00:00.00"
		.byte		0xd, 0xa, 0

		.align		16
		.skip		0xd0004, 0                       #workarea 100万桁の時に0xd0003
pi:		.skip		4, 0


		.text
main01:
		push	rax
		push	rbx
		push	rcx
		push	rdx
		push	rsi
		push	rdi
		push	rbp
		push	r12
		push	r12						#アラインメント合わせのダミー

		lea		rax, pi[rip]
		mov		r12, rax                  #r12←pi
		mov		r11, rax
		mov		rax, dcml
		sub		r11, rax                  #r11←pi-dcml
		call	gtm1
		call	frst
		call	scnd
		call	thrd
		call	gtm2
		call	prnt
		call	gtm3
		call	tmdp

		pop		r12
		pop		r12
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

#		mov		ax, 0x4c00
#		int		0x21

frst:	xor		rsi, rsi
		sub		rsi, dcml
		mov		dword ptr [r12+rsi], 0xc0
		mov		ecx, 1
lfr1:	call	div8
		jz		short sfr1
		call	diva
		add		ecx, 2
		call	div8
		jz		short sfr1
		call	divs
		add		ecx, 2
		jmp		short lfr1
sfr1:	ret

scnd:	xor		rsi, rsi
		sub		rsi, dcml
		mov		dword ptr [r12+rsi], 0x1c8
		mov		ebx, 0x0cb1
		mov		ecx, 1
lsc1:	call		div1
		jz		short ssc1
		call	diva
		add		ecx, 2
		call	div1
		jz		short ssc1
		call	divs
		add		ecx, 2
		jmp		short lsc1
ssc1:	ret

thrd:	xor		rsi, rsi
		sub		rsi, dcml
		mov		dword ptr [r12+rsi], 0x03bc
		mov		ebx, 0xdf21
		mov		ecx, 1
lth1:	call	div1
		jz		short sth1
		call	diva
		add		ecx, 2
		call	div1
		jz		short sth1
		call	divs
		add		ecx, 2
		jmp		short lth1
sth1:	ret

div8:	mov		eax, [r12+rsi]
		xor		ebx, ebx

		rcr		eax, 1
		rcr		ebx, 1
		rcr		eax, 1
		rcr		ebx, 1
		rcr		eax, 1
		rcr		ebx, 1
		rcr		eax, 1
		rcr		ebx, 1
		rcr		eax, 1
		rcr		ebx, 1
		rcr		eax, 1
		rcr		ebx, 1

		mov		[r12+rsi], eax
		xor		r11, r11			#r11←-4
		sub		r11, 4
		cmp		rsi, r11
		jz		sd81
		mov		[r12+rsi+4], ebx
sd81:	or		eax, eax
		jnz		sd82
		add		rsi, 4
sd82:	ret

		.align		4
div1:	mov		rdi, rsi
		xor		edx, edx
ld11:	mov		eax, [r12+rdi]
		div		ebx
		mov		[r12+rdi], eax
		add		rdi, 4
		jnz		short ld11
		cmp		dword ptr [r12+rsi], 0
		jnz		short sd11
		add		rsi, 4
sd11:	ret

		.align		4
diva:	mov		rdi, rsi
		xor		edx, edx
lda1:	mov		eax, [r12+rdi]
		div		ecx
		add		[r12+rdi-dcml], eax
		jnc		short sda1
		mov		rax, rdi
lda2:	sub		rax, 4
		inc		dword ptr [r12+rax-dcml]
		jz		short lda2
sda1:	add		rdi, 4
		jnz		short lda1
		ret

		.align		4
divs:	mov		rdi, rsi
		xor		edx, edx
lds1:	mov		eax, [r12+rdi]
		div		ecx
		sub		[r12+rdi-dcml], eax
		jnc		short sds1
		mov		rax, rdi
lds2:	sub		rax, 4
		sub		dword ptr [r12+rax-dcml], 1
		jc		short lds2
sds1:	add		rdi, 4
		jnz		short lds1
		ret

		.align		4
trns:
		push	r11				#アラインメント合わせのダミー
		xor		r11, r11			#r11←-4
		sub		r11, 4
		mov		rsi, r11
		mov		rdi, 4
		sub		rdi, dcml
ltr1:	mov		eax, [r12+rsi-dcml]
		mov		[r12+rdi], eax
		sub		rsi, 4
		add		rdi, 4
		jnz		short ltr1
		mov		edx, [r12+rsi-dcml]
		add		dl, 0x30
		call	pout
		mov		dl, 0x2e
		call	pout
		call	otlf
		pop		r11
		ret

		nop
		nop
prnt:
		push	r11				#アラインメント合わせのダミー
		call	trns
		xor		edi, edi
		mov		rsi, 4
		sub		rsi, dcml
lpr1:	mov		bh, 5
lpr2:	mov		bl, 20
lpr3:	mov		ch, 5
lpr4:	mov		cl, 10
lpr5:	call	cvrt
		add		dl, 0x30
		call	pout
		add		edi, 0x1a934f09
		jc		short spr1
lpr6:	dec		cl
		jnz		short lpr5
		mov		dl, 0x20
		call	pout
		dec		ch
		jnz		short lpr4
		call	otlf
		dec		bl
		jnz		short lpr3
		call	otlf
		dec		bh
		jnz		short lpr2
#		mov		dl, 0x0c
#		call	pout
		jmp		short lpr1
spr1:	add		rsi, 4
		jnz		short lpr6
spr2:	call	otlf
		pop		r11
		ret

otlf:
		push	r11
#		mov		dl, 0x0d
#		call	pout
		mov		dl, 0x0a
		call	pout
		pop		r11
		ret
#pout:	mov		ah, 2
#		int		0x21
#		ret

pout:
		push	rax
		push	rbx
		push	rcx
		push	rdx
		push	rsi
		push	rdi
		push	rbp
		push	r12
		push	r12				#アラインメント合わせのダミー

		xor		rcx, rcx
		mov		cl, dl
		mov		rdi, rcx
		push	rdi				#アラインメント合わせのダミー
		push	rdi

		call	putchar1@PLT

		pop		rcx
		pop		rcx

		pop		r12
		pop		r12
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret


		nop
		nop
cvrt:	push		rsi
		push		rbx
		push		rcx
		mov		ecx, 0x0a
		xor		ebx, ebx
lcv1:	mov		eax, [r12+rsi]
		mul		ecx
		add		eax, ebx
		adc		edx, 0
		mov		ebx, edx
		mov		[r12+rsi], eax
		add		rsi, 4
		jnz		short lcv1
		pop		rcx
		pop		rbx
		pop		rsi
		ret

#gtm1:		mov		edi, offset tim1
#		jmp		short gttm
#gtm2:		mov		edi, offset tim2
#		jmp		short gttm
#gtm3:		mov		edi, offset tim3
#gttm:		mov		ah, 0x2c
#		int		0x21
#		mov		al, ch
#		call		btod
#		mov		7[edi], ax
#		mov		al, cl
#		call		btod
#		mov		10[edi], ax
#		mov		al, dh
#		call		btod
#		mov		13[edi], ax
#		mov		al, dl
#		call		btod
#		mov		16[edi], ax
#		ret
#
#btod:		xor		ah, ah
#		mov		bh, 0x0a
#		div		bh
#		add		ax, 0x3030
#		ret
#
#tmdp:		mov		edx, offset tim1
#		mov		ah, 9
#		int		0x21
#		ret

gtm1:
		push	rax
		push	rbx
		push	rcx
		push	rdx
		push	rsi
		push	rdi
		push	rbp
		push	r12
		push	r12				#アラインメント合わせのダミー

		call		gtm11@PLT

		pop		r12
		pop		r12
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

gtm2:
		push	rax
		push	rbx
		push	rcx
		push	rdx
		push	rsi
		push	rdi
		push	rbp
		push	r12
		push	r12				#アラインメント合わせのダミー

		call		gtm21@PLT

		pop		r12
		pop		r12
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

gtm3:
		push	rax
		push	rbx
		push	rcx
		push	rdx
		push	rsi
		push	rdi
		push	rbp
		push	r12
		push	r12				#アラインメント合わせのダミー

		call		gtm31@PLT

		pop		r12
		pop		r12
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

tmdp:
		push	rax
		push	rbx
		push	rcx
		push	rdx
		push	rsi
		push	rdi
		push	rbp
		push	r12
		push	r12				#アラインメント合わせのダミー

		call		tmdp1@PLT

		pop		r12
		pop		r12
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

		.end
