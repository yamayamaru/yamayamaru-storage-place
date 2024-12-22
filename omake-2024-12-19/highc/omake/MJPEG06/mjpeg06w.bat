rem -3sは386の命令セットで関数パラメータのスタック呼び出し
rem zp1は1バイト単位でアラインメントする(要するにアラインメントしない)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=picojpeg.obj jpegdec\picojpeg.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=JPEGDeco.obj jpegdec\JPEGDeco.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=mjpeg06w.obj mjpeg06w.c
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=524288 name mjpeg06w file{ mjpeg06w.obj jpegdeco.obj picojpeg.obj ..\..\lib\tbios.lib ..\..\lib\fmcfrb.lib }
