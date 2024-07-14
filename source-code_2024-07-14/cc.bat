wcc386 -3r -zq -zk0 -ox -fpc -fo=picojpeg.obj JPEGDecoder\picojpeg.c
wpp386 -3r -zq -zk0 -ox -fpc -fo=JPEGDecoder.obj JPEGDecoder\JPEGDecoder.cpp
wpp386 -3r -zq -zk0 -ox -fpc -fo=framebuf_draw_mjpeg02-web216_02.obj framebuf_draw_mjpeg02-web216_02.cpp
wlink system dos4g option quiet,STACK=256K name mjpeg032 file{ framebuf_draw_mjpeg02-web216_02.obj JPEGDecoder.obj picojpeg.obj }
