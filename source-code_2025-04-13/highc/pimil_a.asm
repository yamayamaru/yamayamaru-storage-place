		.386p

		public		mul32_mul_32_to_64, div64_div_32

CGROUP		group		code1
DGROUP		group		data1

code1		segment		dword public use32 'CODE'
		assume		cs:CGROUP, ds:DGROUP
mul32_mul_32_to_64	proc		near

		push		ebp
		mov		ebp, esp

		push		ebx
		push		edx

		mov		eax, [ebp + 8]
		mov		ebx, [ebp + 12]
		mul		ebx
		mov		ebx, [ebp + 16]
		mov		[ebx], edx

		pop		edx
		pop		ebx

		mov		esp, ebp
		pop		ebp
		ret

mul32_mul_32_to_64		endp

div64_div_32	proc		near

		push		ebp
		mov		ebp, esp

		push		ebx
		push		edx

		mov		eax, [ebp + 8]
		mov		ebx, [ebp + 12]
		mov		edx, [ebp + 16]
		div		ebx
		mov		ebx, [ebp + 20]
		mov		[ebx], edx

		pop		edx
		pop		ebx

		mov		esp, ebp
		pop		ebp
		ret

div64_div_32		endp

code1		ends

data1		segment		dword public use32 'DATA'
data1		ends
		end
