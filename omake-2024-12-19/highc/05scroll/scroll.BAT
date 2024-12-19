rem hc386 -v -c gra.c -fsoft
rem hc386 -v -c setscrn.c -fsoft
rem hc386 -v -c scroll.c -fsoft
rem tasm io.asm,,io.lst
rem rem run386 -nocrt f:\hc386\bin\tlinkp  @scroll.lnk
rem run386 -nocrt f:\hc386\bin\386linkp  @scroll.lnk

rem -3sは386の命令セットで関数パラメータのスタック呼び出し
rem zp1は1バイト単位でアラインメントする(要するにアラインメントしない)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=gra.obj gra.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=setscrn.obj setscrn.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=scroll.obj scroll.c
wasm -3 -fo=io.obj io.asm
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=262144 name scroll file{ scroll.obj gra.obj setscrn.obj io.obj ..\lib\tbios.lib ..\lib\fmcfrb.lib }
