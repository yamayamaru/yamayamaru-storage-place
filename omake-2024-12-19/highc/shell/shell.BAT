rem hc386 -v -c gra.c -fsoft
rem hc386 -v -c setscrn.c -fsoft
rem hc386 -v -c scroll3.c -fsoft
rem tasm io.asm,,io.lst
rem rem run386 -nocrt f:\hc386\bin\tlinkp  @scroll3.lnk
rem run386 -nocrt f:\hc386\bin\386linkp  @scroll3.lnk

rem -3sは386の命令セットで関数パラメータのスタック呼び出し
rem zp1は1バイト単位でアラインメントする(要するにアラインメントしない)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=loader.obj loader.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=shell.obj shell.c
wasm -3 -fo=rsmidi.obj rs_midi.asm
wasm -3 -fo=key_chec.obj key_chec.asm
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=262144 name shell file{ shell.obj key_chec.obj loader.obj ..\lib\snd.lib rsmidi.obj ..\lib\tbios.lib }
