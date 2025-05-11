rem 環境変数SDCC_HOMEにSDCCをインストールしたフォルダを指定してください
rem デフォルトではC:\Program Files\SDCCにインストールされます
set SDCC_HOME="C:\Program Files\SDCC"


set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC%\bin;%PATH%



del piob_out_super_aki80.ihx
del piob_out_super_aki80.rel
del mycrt0_2_super_aki80.rel

sdcc -mz80 -c -o piob_out_super_aki80.rel piob_out_super_aki80.c
sdasz80 -o mycrt0_2_super_aki80.rel mycrt0_2_super_aki80.asm
sdcc -mz80 --code-loc 0x8000 --data-loc 0 --no-std-crt0 -o piob_out_super_aki80.ihx mycrt0_2_super_aki80.rel  piob_out_super_aki80.rel 
