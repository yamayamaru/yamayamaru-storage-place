rem 環境変数SDCC_HOMEにSDCCをインストールしたフォルダを指定してください
rem デフォルトではC:\Program Files\SDCCにインストールされます
set SDCC_HOME="C:\Program Files\SDCC"


set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC%\bin;%PATH%


del hat02_super_aki80_sio1_tek_emu.ihx
del hat02_super_aki80_sio1_tek_emu.rel
del mycrt0_2_super_aki80.rel

sdcc -mz80 -c -o hat02_super_aki80_sio1_tek_emu.rel hat02_super_aki80_sio1_tek_emu.c
sdasz80 -o mycrt0_2_super_aki80.rel mycrt0_2_super_aki80.asm
sdcc -mz80 --code-loc 0x8000 --data-loc 0 --no-std-crt0 -o hat02_super_aki80_sio1_tek_emu.ihx mycrt0_2_super_aki80.rel  hat02_super_aki80_sio1_tek_emu.rel

