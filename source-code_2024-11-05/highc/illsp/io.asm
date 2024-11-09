;
;		各種サンプル機械語ルーチン
;

		.386p
code		segment		dword public use32 'code'
		assume		cs:code

;
;		CRTC操作ルーチン
;

		public		CRTC
		db		'CRTC', 4
CRTC		proc		near

		mov		dx, 440h
		mov		al, [esp+4]
		out		dx, al
		add		dx, 2
		mov		ax, [esp+8]
		out		dx, ax
		xor		eax, eax
		ret

CRTC		endp


;
;		スプライトＲＡＭ操作ルーチン
;

		public		SPRamPokeB
		db		'SPRamPokeB',10
SPRamPokeB	proc		near

		push		fs
		push		114h
		pop		fs
		mov		edx, [esp+8]
		mov		eax, [esp+12]
		mov		fs:[edx], al
		xor		eax, eax
		pop		fs
		ret

SPRamPokeB	endp


		public		SPRamPokeW
		db		'SPRamPokeW',10
SPRamPokeW	proc		near

		push		fs
		push		114h
		pop		fs
		mov		edx, [esp+8]
		mov		eax, [esp+12]
		mov		fs:[edx], ax
		xor		eax, eax
		pop		fs
		ret

SPRamPokeW	endp


		public		SPRamPeekB
		db		'SPRamPeekB',10
SPRamPeekB	proc		near

		push		fs
		push		114h
		pop		fs
		mov		edx, [esp+8]
		xor		eax, eax
		mov		al, fs:[edx]
		pop		fs
		ret

SPRamPeekB	endp


		public		SPRamPeekW
		db		'SPRamPeekW',10
SPRamPeekW	proc		near

		push		fs
		push		114h
		pop		fs
		mov		edx, [esp+8]
		xor		eax, eax
		mov		ax, fs:[edx]
		pop		fs
		ret

SPRamPeekW	endp

code		ends
		end
