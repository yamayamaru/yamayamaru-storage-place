;*************************************************************************
;*                                                                       *
;*    EUPHONY LIBRARY SAMPLE PROGRAM                                     *
;*               ‹@”\ : ·°“ü—ÍÁª¯¸—p»ÌÞÙ°ÁÝ                              *
;*                                                                       *
;*************************************************************************

	public	key_check

joedata	segment	dword public use32 'DATA'
joedata	ends
joecode	segment	dword public use32 'CODE'
joecode	ends
DGROUP	group	joedata
CGROUP	group	joecode
		page
;---------------------------------------------------------------------
joecode	segment
		assume	cs:CGROUP,ds:DGROUP
;---------------------------------------------------------------------
key_check	proc	near

		push	edx
		mov	ah,06h
		mov	dl,0ffh
		int	021h
		movzx	eax,al
		pop	edx
		ret

key_check	endp

joecode		ends
		end
