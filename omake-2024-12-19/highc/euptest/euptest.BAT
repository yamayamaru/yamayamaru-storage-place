rem hc386 -v -c gra.c -fsoft
rem hc386 -v -c setscrn.c -fsoft
rem hc386 -v -c scroll3.c -fsoft
rem tasm io.asm,,io.lst
rem rem run386 -nocrt f:\hc386\bin\tlinkp  @scroll3.lnk
rem run386 -nocrt f:\hc386\bin\386linkp  @scroll3.lnk

rem -3sは386の命令セットで関数パラメータのスタック呼び出し
rem zp1は1バイト単位でアラインメントする(要するにアラインメントしない)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=euptest.obj euptest.c
wasm -3 -fo=key_chec.obj key_chec.asm
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=262144 name euptest file{ euptest.obj key_chec.obj ..\lib\snd.lib ..\lib\tbios.lib }
