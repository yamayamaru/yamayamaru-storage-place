set SDCC_HOME="C:\Program Files\SDCC"

set SDCC_INCLUDE=%SDCC_HOME%\include
#set SDCC_LIBRARY=%SDCC_HOME%\lib
set PATH=%SDCC%\bin;%PATH%



del mandel_super_aki80_sio1_loop2.ihx
del mandel_super_aki80_sio1_loop2.rel
del mandel_super_aki80_sio1_loop2_asm.rel
del mycrt0_2_super_aki80.rel


sdcc -mz80 -c -o mandel_super_aki80_sio1_loop2.rel mandel_super_aki80_sio1_loop2.c
sdasz80 -o mycrt0_2_super_aki80.rel mycrt0_2_super_aki80.asm
sdasz80 -o mandel_super_aki80_sio1_loop2_asm.rel mandel_super_aki80_sio1_loop2_asm.asm
sdcc -mz80 --code-loc 0x8000 --data-loc 0 --no-std-crt0 -o mandel_super_aki80_sio1_loop2.ihx mycrt0_2_super_aki80.rel  mandel_super_aki80_sio1_loop2.rel  mandel_super_aki80_sio1_loop2_asm.rel
