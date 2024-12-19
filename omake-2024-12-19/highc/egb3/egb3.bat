rem -3sは386の命令セットで関数パラメータのスタック呼び出し
rem zp1は1バイト単位でアラインメントする(要するにアラインメントしない)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=egb3.obj egb3.c
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=262144 name egb3 file{ egb3.obj ..\lib\tbios.lib }
