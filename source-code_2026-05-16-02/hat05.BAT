rem hc386 -v -c hat05.c -fsoft
hc386 -v -c hat05.c -f387

tasm hat05asm.asm

exe386 a:\hc386\bin\386linkp @hat05
