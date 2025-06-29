;  PI		T.WAKAMATSU	FEB.21.1190

		extrn		gtm11:proc, gtm21:proc, gtm31:proc, tmdp1:proc, putchar1:proc
		public		main01


;dcml		equ		65640h              ;100万桁         int(1000000 / log10(256) + 55)     (必ず4の倍数にすること)
;dcml		equ		0a240h              ;10万桁
dcml		equ		 1040h              ;1万桁
;dcml		equ		  1a8h              ;1000桁


;stsg		segment		stack
;		dw		200h	dup(?)
;stsg		ends

_DATA		segment

		align		16
tim1		db		"start  00:00:00.00", 0dh, 0ah
tim2		db		"end_1  00:00:00.00", 0dh, 0ah
tim3		db		"end_2  00:00:00.00", 0dh, 0ah, "$"

		align		16
		db		0d0004h dup(0)                       ;workarea 100万桁の時に0d0003h
pi		dd		     1h dup(0)
_DATA		ends

_TEXT		segment

main01:
		push		rax
		push		rbx
		push		rcx
		push		rdx
		push		rsi
		push		rdi
		push		rbp
		push		r10
		push		r10				;アラインメント合わせのダミー

		mov		rax, offset pi
		mov		r10, rax                  ;r10←pi
		mov		r11, rax
		mov		rax, dcml
		sub		r11, rax                  ;r11←pi-dcml
		call		gtm1
		call		frst
		call		scnd
		call		thrd
		call		gtm2
		call		prnt
		call		gtm3
		call		tmdp

		pop		r10
		pop		r10
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

;		mov		ax, 4c00h
;		int		21h

frst:		xor		rsi, rsi
		sub		rsi, dcml
		mov		dword ptr [r10+rsi], 0c0h
		mov		ecx, 1
lfr1:		call		div8
		jz		short sfr1
		call		diva
		add		ecx, 2
		call		div8
		jz		short sfr1
		call		divs
		add		ecx, 2
		jmp		short lfr1
sfr1:		ret

scnd:		xor		rsi, rsi
		sub		rsi, dcml
		mov		dword ptr [r10+rsi], 1c8h
		mov		ebx, 0cb1h
		mov		ecx, 1
lsc1:		call		div1
		jz		short ssc1
		call		diva
		add		ecx, 2
		call		div1
		jz		short ssc1
		call		divs
		add		ecx, 2
		jmp		short lsc1
ssc1:		ret

thrd:		xor		rsi, rsi
		sub		rsi, dcml
		mov		dword ptr [r10+rsi], 3bch
		mov		ebx, 0df21h
		mov		ecx, 1
lth1:		call		div1
		jz		short sth1
		call		diva
		add		ecx, 2
		call		div1
		jz		short sth1
		call		divs
		add		ecx, 2
		jmp		short lth1
sth1:		ret

div8:		mov		eax, [r10+rsi]
		xor		ebx, ebx
		rept		6
		rcr		eax, 1
		rcr		ebx, 1
		endm
		mov		[r10+rsi], eax
		xor		r11, r11			;r11←-4
		sub		r11, 4
		cmp		rsi, r11
		jz		sd81
		mov		[r10+rsi+4], ebx
sd81:		or		eax, eax
		jnz		sd82
		add		rsi, 4
sd82:		ret

		align		4
div1:		mov		rdi, rsi
		xor		edx, edx
ld11:		mov		eax, [r10+rdi]
		div		ebx
		mov		[r10+rdi], eax
		add		rdi, 4
		jnz		short ld11
		cmp		dword ptr [r10+rsi], 0
		jnz		short sd11
		add		rsi, 4
sd11:		ret

		align		4
diva:		mov		rdi, rsi
		xor		edx, edx
lda1:		mov		eax, [r10+rdi]
		div		ecx
		add		[r10+rdi-dcml], eax
		jnc		short sda1
		mov		rax, rdi
lda2:		sub		rax, 4
		inc		dword ptr [r10+rax-dcml]
		jz		short lda2
sda1:		add		rdi, 4
		jnz		short lda1
		ret

		align		4
divs:		mov		rdi, rsi
		xor		edx, edx
lds1:		mov		eax, [r10+rdi]
		div		ecx
		sub		[r10+rdi-dcml], eax
		jnc		short sds1
		mov		rax, rdi
lds2:		sub		rax, 4
		sub		dword ptr [r10+rax-dcml], 1
		jc		short lds2
sds1:		add		rdi, 4
		jnz		short lds1
		ret

		align		4
trns:
		push		r11				;アラインメント合わせのダミー
		xor		r11, r11			;r11←-4
		sub		r11, 4
		mov		rsi, r11
		mov		rdi, 4
		sub		rdi, dcml
ltr1:		mov		eax, [r10+rsi-dcml]
		mov		[r10+rdi], eax
		sub		rsi, 4
		add		rdi, 4
		jnz		short ltr1
		mov		edx, [r10+rsi-dcml]
		add		dl, 30h
		call		pout
		mov		dl, 2eh
		call		pout
		call		otlf
		pop		r11
		ret

		nop
		nop
prnt:
		push		r11				;アラインメント合わせのダミー
		call		trns
		xor		edi, edi
		mov		rsi, 4
		sub		rsi, dcml
lpr1:		mov		bh, 5
lpr2:		mov		bl, 20
lpr3:		mov		ch, 5
lpr4:		mov		cl, 10
lpr5:		call		cvrt
		add		dl, 30h
		call		pout
		add		edi, 1a934f09h
		jc		short spr1
lpr6:		dec		cl
		jnz		short lpr5
		mov		dl, 20h
		call		pout
		dec		ch
		jnz		short lpr4
		call		otlf
		dec		bl
		jnz		short lpr3
		call		otlf
		dec		bh
		jnz		short lpr2
;		mov		dl, 0ch
;		call		pout
		jmp		short lpr1
spr1:		add		rsi, 4
		jnz		short lpr6
spr2:		call		otlf
		pop		r11
		ret

otlf:
		push		r11
;		mov		dl, 0dh
;		call		pout
		mov		dl, 0ah
		call		pout
		pop		r11
		ret
;pout:		mov		ah, 2
;		int		21h
;		ret

pout:
		push		rax
		push		rbx
		push		rcx
		push		rdx
		push		rsi
		push		rdi
		push		rbp
		push		r10
		push		r10				;アラインメント合わせのダミー

		xor		rcx, rcx
		mov		cl, dl
		push		rcx				;アラインメント合わせのダミー
		push		rcx

		call		putchar1

		pop		rcx
		pop		rcx

		pop		r10
		pop		r10
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
cvrt:		push		rsi
		push		rbx
		push		rcx
		mov		ecx, 0ah
		xor		ebx, ebx
lcv1:		mov		eax, [r10+rsi]
		mul		ecx
		add		eax, ebx
		adc		edx, 0
		mov		ebx, edx
		mov		[r10+rsi], eax
		add		rsi, 4
		jnz		short lcv1
		pop		rcx
		pop		rbx
		pop		rsi
		ret

;gtm1:		mov		edi, offset tim1
;		jmp		short gttm
;gtm2:		mov		edi, offset tim2
;		jmp		short gttm
;gtm3:		mov		edi, offset tim3
;gttm:		mov		ah, 2ch
;		int		21h
;		mov		al, ch
;		call		btod
;		mov		7[edi], ax
;		mov		al, cl
;		call		btod
;		mov		10[edi], ax
;		mov		al, dh
;		call		btod
;		mov		13[edi], ax
;		mov		al, dl
;		call		btod
;		mov		16[edi], ax
;		ret
;
;btod:		xor		ah, ah
;		mov		bh, 0ah
;		div		bh
;		add		ax, 3030h
;		ret
;
;tmdp:		mov		edx, offset tim1
;		mov		ah, 9
;		int		21h
;		ret

gtm1:
		push		rax
		push		rbx
		push		rcx
		push		rdx
		push		rsi
		push		rdi
		push		rbp
		push		r10
		push		r10				;アラインメント合わせのダミー

		call		gtm11

		pop		r10
		pop		r10
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

gtm2:
		push		rax
		push		rbx
		push		rcx
		push		rdx
		push		rsi
		push		rdi
		push		rbp
		push		r10
		push		r10				;アラインメント合わせのダミー

		call		gtm21

		pop		r10
		pop		r10
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

gtm3:
		push		rax
		push		rbx
		push		rcx
		push		rdx
		push		rsi
		push		rdi
		push		rbp
		push		r10
		push		r10				;アラインメント合わせのダミー

		call		gtm31

		pop		r10
		pop		r10
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

tmdp:
		push		rax
		push		rbx
		push		rcx
		push		rdx
		push		rsi
		push		rdi
		push		rbp
		push		r10
		push		r10				;アラインメント合わせのダミー

		call		tmdp1

		pop		r10
		pop		r10
		pop		rbp
		pop		rdi
		pop		rsi
		pop		rdx
		pop		rcx
		pop		rbx
		pop		rax
		ret

_TEXT		ends
		end
