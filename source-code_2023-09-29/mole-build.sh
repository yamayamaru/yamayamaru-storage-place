#!/bin/bash

export SDCC_HOME=  #ここにsdccのHOMEのパスを指定
export SDCC_INCLUDE=$SDCC_HOME/include
export PATH=$SDCC_HOME/bin:$PATH

rm mole.com
rm mole.rel
rm mole.asm
sdcc -mz80 -c -o mole.rel mole.c
sdcc -mz80 -c -S -o mole.asm mole.c
sdcc -mz80 -c -o msxclib.rel msxclib.c

sdasz80 -l -s -o msxalib.rel msxalib.asm
sdasz80 -o msxdos.rel msxdos.asm
sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o mole.ihx mycrt0_2.rel mole.rel msxclib.rel msxalib.rel msxdos.rel

makebin -s 15600 mole.ihx > mole.bin
python3 binout.py mole.bin mole.com

