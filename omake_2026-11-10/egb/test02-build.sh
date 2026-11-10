#!/bin/bash



clang -O2 -m32 -march=i386 -m80387 -fno-pic -fno-pie -ffreestanding -I./newlib-4.4.0.20231231/newlib/libc/include -c test02.c -o test02.o
clang -O2 -m32 -march=i386 -m80387 -fno-pic -fno-pie -ffreestanding -I./newlib-4.4.0.20231231/newlib/libc/include -c newlib_stub.c -o stub.o
as --32  -o startup.o startup.s


/home/ubuntu/2.43.1/usr/bin/ld -N -m elf_i386 -static -nostdlib  -T aout.ld \
-o test02.elf \
startup.o stub.o \
test02.o \
libt.a \
libc.a libm.a \
-Map test02.map



/home/ubuntu/2.43.1/usr/bin/objcopy -S -O a.out-i386 test02.elf test02.bin

/home/ubuntu/2.43.1/usr/bin/objdump -b a.out-i386 -d test02.elf > test02.elf.txt

