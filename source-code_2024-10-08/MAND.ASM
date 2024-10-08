;
;    int mand(int a1, int b1, int depth1);
;    ret:  loop count
;
		.386p
		extrn	SMALL?:word

		public	mand, imul01

mand_segment	segment dword public use32 'code' 


CGROUP		group	mand_segment
		assume	cs:CGROUP

mand_i1		equ	-4[ebp]
mand_x1		equ	-8[ebp]
mand_y1		equ	-12[ebp]
mand_x2		equ	-16[ebp]
mand_y2		equ	-20[ebp]

mand_a1		equ	8[ebp]
mand_b1		equ	12[ebp]
mand_depth1	equ	16[ebp]

		db		'mand'
		db		4

mand		proc	near
		push	ebp
		mov		ebp, esp
		sub		esp, 20			;ローカルスタックフレームサイズ*5
		push	esi
		push	edi
		push	ebx
		push	ecx
		push	edx


		xor	eax, eax
		mov	mand_i1, eax
		mov	mand_x1, eax
		mov	mand_y1, eax
		mov	mand_x2, eax
		mov	mand_y2, eax

mand_loop01:	mov	eax, mand_i1		;(i1 < depth1) && (x2+y2 <= (4 << 24))
		cmp	eax, mand_depth1
		jge	mand_loop01_end
		mov	eax, mand_x2
		add	eax, mand_y2
		mov	ebx, 4000000H
		cmp	eax, ebx
		jg	mand_loop01_end
mand_jmp01:
		mov	eax, mand_x2		;tmp1 = x2 - y2 + a1;
		sub	eax, mand_y2
		add	eax, mand_a1
		mov	ecx, eax		;一時退避

		mov	eax, mand_x1		;y1 = imul01(2 * x1, y1) + b1;
		shl	eax, 1
		mov	ebx, mand_y1
		call	imul01_asm
		add	eax, mand_b1
		mov	mand_y1, eax

		mov	mand_x1, ecx		;x1 = tmp1;

		mov	eax, mand_x1		;x2 = imul01(x1, x1);
		mov	ebx, eax
		call	imul01_asm
		mov	mand_x2, eax

		mov	eax, mand_y1		;y2 = imul01(y1, y1);
		mov	ebx, eax
		call	imul01_asm
		mov	mand_y2, eax

		inc	dword ptr mand_i1	;i1++;
		jmp	mand_loop01
mand_loop01_end:

		mov	eax, mand_i1		;if(i1 == depth1) return 0;
		cmp	eax, mand_depth1	;else             return ++i;
		jnz	mand_jmp02
		xor	eax, eax
		jmp	mand_jmp03
mand_jmp02:
		inc	eax
mand_jmp03:

		pop		edx
		pop		ecx
		pop		ebx
		pop		edi
		pop		esi
		mov		esp,ebp
		pop		ebp
		ret
mand		endp


;	edx:eax = eax * ebx
;       eax = (edx:eax >> 24) & 0xffffffff
imul01_asm	proc	near
		imul	ebx
		shl	edx, 8
		shr	eax, 24
		or	eax, edx
		ret
imul01_asm	endp


		db		'imul01'
		db		6

imul01		proc	near
		push	ebp
		mov	ebp, esp
		push	ebx
		push	edx

		mov	eax, 8[ebp]
		mov	ebx, 12[ebp]
		call	imul01_asm

		pop	edx
		pop	ebx
		pop	ebp
		ret
imul01		endp

mand_segment	ends
		end
