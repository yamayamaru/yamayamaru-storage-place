;*************************************************************************
;*                                                                       *
;*    RS232C MIDI LIBRALY                                                *
;*                EUPHONY ドライバサンプルプログラム                     *
;*                                                                       *
;*************************************************************************

RS_TIME_OVER	equ	16000

	extrn	SND_eup_driver	:near

	public	SND_rs_midi_init
	public	rs_midi_init
	public	SND_rs_midi_end
	public	rs_midi_exit

joedata	segment	dword public use32 'DATA'
joedata	ends
joecode	segment	dword public use32 'CODE'
joecode	ends
DGROUP	group	joedata
CGROUP	group	joecode
;---------------------------------------
joedata	segment

rs_midi_ent_sav	dd	?
modem_sav	db	?
run_stat	db	?

joedata	ends
		page
;---------------------------------------------------------------------
joecode	segment
		assume	cs:CGROUP,ds:DGROUP
;---------------------------------------------------------------------
;	initialize routine
;---------------------------------------------------------------------
SND_rs_midi_init	proc	near
rs_midi_init:

		pushfd
		cli
		pushad

		mov	dx,0a0ah
		in	al,dx		;Get modem status
		mov	modem_sav,al
		mov	al,080h		;Set rs232c
		out	dx,al
		mov	dx,0a08h
		mov	al,0c0h		;Ext. sync
		out	dx,al
		mov	dx,0a02h	;init i8251
		xor	al,al
		call	rs_write
		call	rs_write
		call	rs_write
		mov	al,040h
		call	rs_write
		mov	al,04dh
		call	rs_write
		mov	al,031h
		call	rs_write
		mov	ah,0ch
		call	SND_eup_driver
		mov	rs_midi_ent_sav,ebx
		mov	ebx,offset rs_drive_entry
		mov	ah,0dh
		call	SND_eup_driver
		popad
		popfd
		xor	eax,eax
		ret

SND_rs_midi_init	endp

WAIT_REG	equ	06ch
rs_write	proc	near
		out	dx,al
		out	WAIT_REG,al
		out	WAIT_REG,al
		out	WAIT_REG,al
		out	WAIT_REG,al
		ret
rs_write	endp

SND_rs_midi_end	proc	near
rs_midi_exit:

		pushad
		mov	ebx,rs_midi_ent_sav
		mov	ah,0dh
		call	SND_eup_driver
		mov	dx,0a0ah
		mov	al,modem_sav
		out	dx,al
		popad
		xor	eax,eax
		ret

SND_rs_midi_end	endp
;--------------------------------------------------------------
		align	4
rs_drive_entry	proc	near

		push	ecx
		push	edx
		cmp	al,080h			;Status?
		jb	short #data		;No, jump
		cmp	al,0f8h			;Real time status ?
		jae	short #data		;Yes, jump
		cmp	al,0f0h			;Common status?
		jae	short #status		;Yes, jump
		cmp	al,run_stat		;Same command?
		je	short #exit		;Yes, jump
#status:
		mov	run_stat,al		;No, set new command
#data:
		push	eax
		mov	dx,0a02h
		mov	ecx,RS_TIME_OVER
loop1:
		in	al,dx
		and	al,01h
		jne	short #ready
		loop	short loop1
		pop	eax
		jmp	short error1
#ready:
		pop	eax
		mov	dx,0a00h
		out	dx,al
#exit:
		pop	edx
		pop	ecx
		xor	ah,ah
		clc
		ret

error1:
		pop	edx
		pop	ecx
		mov	ah,0f0h
		stc
		ret
rs_drive_entry	endp

joecode		ends

		end
