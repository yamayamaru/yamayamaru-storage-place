#!/bin/bash

. ../../set-env-sdcc.source


rm piob_out_super_aki80.ihx
rm piob_out_super_aki80.rel
rm mycrt0_2_super_aki80.rel

sdcc -mz80 -c -o piob_out_super_aki80.rel piob_out_super_aki80.c
sdasz80 -o mycrt0_2_super_aki80.rel mycrt0_2_super_aki80.asm
sdcc -mz80 --code-loc 0x8000 --data-loc 0 --no-std-crt0 -o piob_out_super_aki80.ihx mycrt0_2_super_aki80.rel  piob_out_super_aki80.rel 

