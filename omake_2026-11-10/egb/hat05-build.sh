#!/bin/bash



clang -O2 -m32 -march=i386 -m80387 -fno-pic -fno-pie -ffreestanding -I./newlib-4.4.0.20231231/newlib/libc/include -c hat05.c -o hat05.o
clang -O2 -m32 -march=i386 -m80387 -fno-pic -fno-pie -ffreestanding -I./newlib-4.4.0.20231231/newlib/libc/include -c newlib_stub.c -o stub.o
as --32  -o startup.o startup.s



/home/ubuntu/2.43.1/usr/bin/ld -N -m elf_i386 -static -nostdlib  -T aout.ld \
-o hat05.elf \
startup.o stub.o \
hat05.o \
libt.a \
libc.a libm.a \
-Map hat05.map



/home/ubuntu/2.43.1/usr/bin/objcopy -S -O a.out-i386 hat05.elf hat05.bin

/home/ubuntu/2.43.1/usr/bin/objdump -b a.out-i386 -d hat05.elf > hat05.elf.txt

