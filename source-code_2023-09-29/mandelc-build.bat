rem SDCC_HOME‚Ésdcc‚ÌHOME‚ÌƒpƒX‚ðŽw’è‚·‚é
set SDCC_HOME=c:\sdcc-4.2.0

set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC_HOME%\bin;%PATH%

del mandelc.com
del mandelc.rel
del mandelc.asm
sdcc -mz80 -c -o mandelc.rel mandelc.c
sdcc -mz80 -c -S -o mandelc.asm mandelc.c
sdcc -mz80 -c -o msxclib.rel msxclib.c

sdasz80 -l -s -o msxalib.rel msxalib.asm
sdasz80 -o msxdos.rel msxdos.asm
sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o mandelc.ihx mycrt0_2.rel mandelc.rel msxclib.rel msxalib.rel msxdos.rel

makebin -s 14100 mandelc.ihx > mandelc.bin
python binout.py mandelc.bin mandelc.com

