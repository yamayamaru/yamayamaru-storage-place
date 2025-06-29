;  PI		T.WAKAMATSU	FEB.21.1190

		.386p

		.model		flat

		extrn		_gtm11:proc, _gtm21:proc, _gtm31:proc, _tmdp1:proc, _putchar1:proc
		public		_main01


;dcml		equ		65640h              ;100ñúåÖ         int(1000000 / log10(256) + 55)     (ïKÇ∏4ÇÃî{êîÇ…Ç∑ÇÈÇ±Ç∆)
;dcml		equ		0a240h              ;10ñúåÖ
dcml		equ		 1040h              ;1ñúåÖ
;dcml		equ		  1a8h              ;1000åÖ


;stsg		segment		stack
;		dw		200h	dup(?)
;stsg		ends

_DATA		segment
		align		16
tim1		db		"start  00:00:00.00", 0dh, 0ah
tim2		db		"end_1  00:00:00.00", 0dh, 0ah
tim3		db		"end_2  00:00:00.00", 0dh, 0ah, "$"

		align		16
		db		0d0004h dup(0)                       ;workarea 100ñúåÖÇÃéûÇ…0d0003h
pi		dd		     1h dup(0)
_DATA		ends

_TEXT		segment

_main01:
		push		eax
		push		ebx
		push		ecx
		push		edx
		push		esi
		push		edi
		push		ebp

		call		gtm1
		call		frst
		call		scnd
		call		thrd
		call		gtm2
		call		prnt
		call		gtm3
		call		tmdp

		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret

;		mov		ax, 4c00h
;		int		21h

frst:		xor		esi, esi
		sub		esi, dcml
		mov		dword ptr pi[esi], 0c0h
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

scnd:		xor		esi, esi
		sub		esi, dcml
		mov		dword ptr pi[esi], 1c8h
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

thrd:		xor		esi, esi
		sub		esi, dcml
		mov		dword ptr pi[esi], 3bch
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

div8:		mov		eax, pi[esi]
		xor		ebx, ebx
		rept		6
		rcr		eax, 1
		rcr		ebx, 1
		endm
		mov		pi[esi], eax
		cmp		esi, -4
		jz		sd81
		mov		pi[esi+4], ebx
sd81:		or		eax, eax
		jnz		sd82
		add		esi, 4
sd82:		ret

		align		4
div1:		mov		edi, esi
		xor		edx, edx
ld11:		mov		eax, pi[edi]
		div		ebx
		mov		pi[edi], eax
		add		edi, 4
		jnz		short ld11
		cmp		dword ptr pi[esi], 0
		jnz		short sd11
		add		esi, 4
sd11:		ret

		align		4
diva:		mov		edi, esi
		xor		edx, edx
lda1:		mov		eax, pi[edi]
		div		ecx
		add		pi[edi-dcml], eax
		jnc		short sda1
		mov		eax, edi
lda2:		sub		eax, 4
		inc		dword ptr pi[eax-dcml]
		jz		short lda2
sda1:		add		edi, 4
		jnz		short lda1
		ret

		align		4
divs:		mov		edi, esi
		xor		edx, edx
lds1:		mov		eax, pi[edi]
		div		ecx
		sub		pi[edi-dcml], eax
		jnc		short sds1
		mov		eax, edi
lds2:		sub		eax, 4
		sub		dword ptr pi[eax-dcml], 1
		jc		short lds2
sds1:		add		edi, 4
		jnz		short lds1
		ret

		align		4
trns:		mov		esi, -4
		mov		edi, 4
		sub		edi, dcml
ltr1:		mov		eax, pi[esi-dcml]
		mov		pi[edi], eax
		sub		esi, 4
		add		edi, 4
		jnz		short ltr1
		mov		edx, pi[esi-dcml]
		add		dl, 30h
		call		pout
		mov		dl, 2eh
		call		pout
		call		otlf
		ret

		nop
		nop
prnt:		call		trns
		xor		edi, edi
		mov		esi, 4
		sub		esi, dcml
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
spr1:		add		esi, 4
		jnz		short lpr6
spr2:		call		otlf
		ret

otlf:
;		mov		dl, 0dh
;		call		pout
		mov		dl, 0ah
		call		pout
		ret
;pout:		mov		ah, 2
;		int		21h
;		ret

pout:
		push		eax
		push		ebx
		push		ecx
		push		edx
		push		esi
		push		edi
		push		ebp

		xor		eax, eax
		mov		al, dl
		push		eax

		call		_putchar1

		pop		eax

		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret


		nop
		nop
cvrt:		push		esi
		push		ebx
		push		ecx
		mov		ecx, 0ah
		xor		ebx, ebx
lcv1:		mov		eax, pi[esi]
		mul		ecx
		add		eax, ebx
		adc		edx, 0
		mov		ebx, edx
		mov		pi[esi], eax
		add		esi, 4
		jnz		short lcv1
		pop		ecx
		pop		ebx
		pop		esi
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
		push		eax
		push		ebx
		push		ecx
		push		edx
		push		esi
		push		edi
		push		ebp

		call		_gtm11

		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret

gtm2:
		push		eax
		push		ebx
		push		ecx
		push		edx
		push		esi
		push		edi
		push		ebp

		call		_gtm21

		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret

gtm3:
		push		eax
		push		ebx
		push		ecx
		push		edx
		push		esi
		push		edi
		push		ebp

		call		_gtm31

		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret

tmdp:
		push		eax
		push		ebx
		push		ecx
		push		edx
		push		esi
		push		edi
		push		ebp

		call		_tmdp1

		pop		ebp
		pop		edi
		pop		esi
		pop		edx
		pop		ecx
		pop		ebx
		pop		eax
		ret

_TEXT		ends
		end
