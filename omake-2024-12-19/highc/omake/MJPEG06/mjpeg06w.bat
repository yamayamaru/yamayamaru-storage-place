rem -3s��386�̖��߃Z�b�g�Ŋ֐��p�����[�^�̃X�^�b�N�Ăяo��
rem zp1��1�o�C�g�P�ʂŃA���C�������g����(�v����ɃA���C�������g���Ȃ�)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=picojpeg.obj jpegdec\picojpeg.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=JPEGDeco.obj jpegdec\JPEGDeco.c
wcc386 -3s -zp1 -zq -zk0 -ox -fo=mjpeg06w.obj mjpeg06w.c
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=524288 name mjpeg06w file{ mjpeg06w.obj jpegdeco.obj picojpeg.obj ..\..\lib\tbios.lib ..\..\lib\fmcfrb.lib }
