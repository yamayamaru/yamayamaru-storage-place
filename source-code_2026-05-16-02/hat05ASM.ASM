		.386p

		assume	cs:TEXT
TEXT		segment para public use32
; int map_physical_momory_at_end_of_segment(unsigned int segment, unsgined int base_addr, int number_of_pages, unsigned int *segment_off);
		public	map_physical_momory_at_end_of_segment
map_physical_momory_at_end_of_segment	proc
		push	ebp
		mov	ebp, esp

		pushfd
		push	es
		push	fs
		push	gs
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi

		mov	eax, [ebp+8]
		mov	es, ax
		mov	ebx, [ebp+12]
		mov	ecx, [ebp+16]

		mov	eax, 250ah
		int	21h

		jc	map_physical_momory_at_end_of_segment_jmp01

		mov	ebx, [ebp+20]		;success
		mov	[ebx], eax		;save segment offset address
		xor	eax, eax		;if success then eax = 0;

map_physical_momory_at_end_of_segment_jmp01:
						;if failure then eax = error code
		pop	edi
		pop	esi
		pop	edx
		pop	ecx
		pop	ebx
		pop	gs
		pop	fs
		pop	es
		popfd

		pop	ebp
		ret

map_physical_momory_at_end_of_segment	endp

		public	allocate_segment
allocate_segment	proc
; int allocate_segment(unsigned int allocate_pages, unsigned int *segment_sel);

		push	ebp
		mov	ebp, esp

		pushfd
		push	es
		push	fs
		push	gs
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi

		mov	ebx, [ebp+8]
		mov	ah, 48h
		int	21h

		jc	allocate_segment_jmp01

		xor	edx, edx		; success
		mov	dx, ax
		mov	ebx, [ebp+12]
		mov	[ebx], edx		; save segment selector
		xor	eax, eax		; if success then eax = 0
		jmp	short allocate_segment_jmp02
allocate_segment_jmp01:
						; if failure then eax = error code
		mov	edx, [ebp+12]
		mov	[edx], ebx		; save number of free pages of pysical momoery available

allocate_segment_jmp02:

		pop	edi
		pop	esi
		pop	edx
		pop	ecx
		pop	ebx
		pop	gs
		pop	fs
		pop	es
		popfd

		pop	ebp
		ret
allocate_segment	endp


		public	free_segment
free_segment	proc
; int free_segment(unsigned int segment_sel);

		push	ebp
		mov	ebp, esp

		pushfd
		push	es
		push	fs
		push	gs
		push	ebx
		push	ecx
		push	edx
		push	esi
		push	edi


		mov	eax, [ebp+8]
		mov	es, ax
		mov	ah, 49h
		int	21h


		jc	free_segnemt_jmp01

		xor	eax, eax		; if success then eax = 0;

free_segnemt_jmp01:
						; if failure then eax = error code
		pop	edi
		pop	esi
		pop	edx
		pop	ecx
		pop	ebx
		pop	gs
		pop	fs
		pop	es
		popfd

		pop	ebp
		ret
free_segment	endp

TEXT		ends


DATA		segment para public use32
DATA		ends
		end
