rem -3sは386の命令セットで関数パラメータのスタック呼び出し
rem zp1は1バイト単位でアラインメントする(要するにアラインメントしない)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=spr2.obj spr2.c
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=262144 name spr2 file{ spr2.obj ..\lib\tbios.lib }
