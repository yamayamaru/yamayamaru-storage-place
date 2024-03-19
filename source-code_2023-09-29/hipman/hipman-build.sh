#!/bin/bash

export SDCC_HOME=  #‚±‚±‚Ésdcc‚ÌHOME‚ÌƒpƒX‚ðŽw’è

export SDCC_HOME=/home/ubuntu/mnt/ubuntu-20.04-x64-build-z88dk/sdcc-4.2.0

export SDCC_INCLUDE=$SDCC_HOME/include
export PATH=$SDCC_HOME/bin:$PATH

rm hipman.com
rm hipman.rel
rm hipenemy.rel
rm hipinit.rel
rm hipscore.rel

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
python3 binout.py hipman.bin hipman.com

