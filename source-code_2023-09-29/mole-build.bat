rem SDCC_HOME‚Ésdcc‚ÌHOME‚ÌƒpƒX‚ðŽw’è‚·‚é
set SDCC_HOME=c:\sdcc-4.2.0

set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC_HOME%\bin;%PATH%

del mole.com
del mole.rel
del mole.asm
sdcc -mz80 -c -o mole.rel mole.c
sdcc -mz80 -c -S -o mole.asm mole.c
sdcc -mz80 -c -o msxclib.rel msxclib.c

sdasz80 -l -s -o msxalib.rel msxalib.asm
sdasz80 -o msxdos.rel msxdos.asm
sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o mole.ihx mycrt0_2.rel mole.rel msxclib.rel msxalib.rel msxdos.rel

makebin -s 15600 mole.ihx > mole.bin
python binout.py mole.bin mole.com

