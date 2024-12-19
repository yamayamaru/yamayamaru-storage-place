rem hc386 -v -c gra.c -fsoft
rem hc386 -v -c setscrn.c -fsoft
rem hc386 -v -c scroll3.c -fsoft
rem tasm io.asm,,io.lst
rem rem run386 -nocrt f:\hc386\bin\tlinkp  @scroll3.lnk
rem run386 -nocrt f:\hc386\bin\386linkp  @scroll3.lnk

rem -3sは386の命令セットで関数パラメータのスタック呼び出し
rem zp1は1バイト単位でアラインメントする(要するにアラインメントしない)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=gra.obj gra.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=setscrn.obj setscrn.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=scroll3.obj scroll3.c
wasm -3 -fo=io.obj io.asm
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=262144 name scroll3 file{ scroll3.obj gra.obj setscrn.obj io.obj ..\lib\tbios.lib ..\lib\fmcfrb.lib }
