wcc386 -3r -zq -zk0 -ox -fpc -fo=picojpeg.obj JPEGDecoder\picojpeg.c
wpp386 -3r -zq -zk0 -ox -fpc -fo=JPEGDecoder.obj JPEGDecoder\JPEGDecoder.cpp
wpp386 -3r -zq -zk0 -ox -fpc -fo=framebuf_draw_mjpeg02.obj framebuf_draw_mjpeg02.cpp
wlink system pharlap runtime MAXREAL=131072,MINREAL=131072 option quiet,STACK=256K name mjpeg021 file{ framebuf_draw_mjpeg02.obj JPEGDecoder.obj picojpeg.obj }
