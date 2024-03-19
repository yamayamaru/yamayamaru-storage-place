rem SDCC_HOME‚Ésdcc‚ÌHOME‚ÌƒpƒX‚ðŽw’è‚·‚é
set SDCC_HOME=c:\sdcc-4.2.0

set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC_HOME%\bin;%PATH%

del hipman.com
del hipman.rel
del hipenemy.rel
del hipinit.rel
del hipscore.rel

sdcc -mz80 -c -o msxclib.rel msxclib.c
sdasz80 -l -s -o msxalib.rel msxalib.asm
sdasz80 -o msxdos.rel msxdos.asm
sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 -c -o hipman.rel hipman.c 
sdcc -mz80 -c -o hipenemy.rel hipenemy.c 
sdcc -mz80 -c -o hipinit.rel hipinit.c
sdcc -mz80 -c -o hipscore.rel hipscore.c

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o hipman.ihx mycrt0_2.rel hipman.rel hipinit.rel hipenemy.rel hipscore.rel msxclib.rel msxalib.rel msxdos.rel


makebin -s 13000 hipman.ihx > hipman.bin
python binout.py hipman.bin hipman.com

