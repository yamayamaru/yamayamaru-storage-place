#!/bin/bash



as --32 LIB_EGB/EGB_S1.S -o LIB_EGB/egb_s1.o
as --32 LIB_EGB/EGB_S2.S -o LIB_EGB/egb_s2.o
as --32 LIB_EGB/EGB_S3.S -o LIB_EGB/egb_s3.o
as --32 LIB_EGB/EGB_S4.S -o LIB_EGB/egb_s4.o
as --32 LIB_EGB/EGB_S5.S -o LIB_EGB/egb_s5.o
as --32 LIB_EGB/EGB_B1.S -o LIB_EGB/egb_b1.o
as --32 LIB_EGB/EGB_B2.S -o LIB_EGB/egb_b2.o
as --32 LIB_EGB/EGB_B3.S -o LIB_EGB/egb_b3.o
as --32 LIB_EGB/EGB_B4.S -o LIB_EGB/egb_b4.o
as --32 LIB_EGB/EGB_B5.S -o LIB_EGB/egb_b5.o
as --32 LIB_EGB/EGB_G1.S -o LIB_EGB/egb_g1.o
as --32 LIB_EGB/EGB_G2.S -o LIB_EGB/egb_g2.o
as --32 LIB_EGB/EGB_G3.S -o LIB_EGB/egb_g3.o
as --32 LIB_EGB/EGB_G4.S -o LIB_EGB/egb_g4.o
as --32 LIB_EGB/EGB_F1.S -o LIB_EGB/egb_f1.o
as --32 LIB_EGB/EGB_F2.S -o LIB_EGB/egb_f2.o
clang -O2 -m32 -march=i386 -msoft-float -fno-pic -fno-pie -ffreestanding  -I./newlib-4.4.0.20231231/newlib/libc/include -c LIB_EGB/escreen.c -o LIB_EGB/escreen.o

as --32 LIB_IO/INTDOS.S -o LIB_IO/intdos.o
as --32 LIB_IO/IOPORT.S -o LIB_IO/ioport.o
as --32 LIB_IO/VRAM_1.S -o LIB_IO/vram_1.o
as --32 LIB_IO/VRAM_2.S -o LIB_IO/vram_2.o
as --32 LIB_IO/VRAM_3.S -o LIB_IO/vram_3.o

as --32 tdosalloc.s -o tdosalloc.o

rm libt.a
ar rcs  libt.a   LIB_EGB/egb_s1.o
ar rcs  libt.a   LIB_EGB/egb_s2.o
ar rcs  libt.a   LIB_EGB/egb_s3.o
ar rcs  libt.a   LIB_EGB/egb_s4.o
ar rcs  libt.a   LIB_EGB/egb_s5.o
ar rcs  libt.a   LIB_EGB/egb_b1.o
ar rcs  libt.a   LIB_EGB/egb_b2.o
ar rcs  libt.a   LIB_EGB/egb_b3.o
ar rcs  libt.a   LIB_EGB/egb_b4.o
ar rcs  libt.a   LIB_EGB/egb_b5.o
ar rcs  libt.a   LIB_EGB/egb_g1.o
ar rcs  libt.a   LIB_EGB/egb_g2.o
ar rcs  libt.a   LIB_EGB/egb_g3.o
ar rcs  libt.a   LIB_EGB/egb_g4.o
ar rcs  libt.a   LIB_EGB/egb_f1.o
ar rcs  libt.a   LIB_EGB/egb_f2.o
ar rcs  libt.a   LIB_EGB/escreen.o

ar rcs libt.a  LIB_IO/intdos.o
ar rcs libt.a  LIB_IO/ioport.o
ar rcs libt.a  LIB_IO/vram_1.o
ar rcs libt.a  LIB_IO/vram_2.o
ar rcs libt.a  LIB_IO/vram_3.o

ar rcs libt.a  tdosalloc.o 


