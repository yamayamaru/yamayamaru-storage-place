#!/bin/bash

export SDCC_HOME=  #ここにsdccのHOMEのパスを指定
export SDCC_INCLUDE=$SDCC_HOME/include
export PATH=$SDCC_HOME/bin:$PATH

rm mandelc.com
rm mandelc.rel
rm mandelc.asm
sdcc -mz80 -c -o mandelc.rel mandelc.c
sdcc -mz80 -c -S -o mandelc.asm mandelc.c
sdcc -mz80 -c -o msxclib.rel msxclib.c

sdasz80 -l -s -o msxalib.rel msxalib.asm
sdasz80 -o msxdos.rel msxdos.asm
sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o mandelc.ihx mycrt0_2.rel mandelc.rel msxclib.rel msxalib.rel msxdos.rel

makebin -s 14100 mandelc.ihx > mandelc.bin
python3 binout.py mandelc.bin mandelc.com

