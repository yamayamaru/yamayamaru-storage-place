	public	key_check

;---------------------------------------------------------------------
;
code	segment
		assume	cs:code
;
;---------------------------------------------------------------------
;
;	initialize routine
;
;
;
key_check	proc	near

		push	edx
		mov	ah,06h
		mov	dl,0ffh
		int	021h
		movzx	eax,al
		pop	edx
		ret

key_check	endp

code		ends
;

	end
