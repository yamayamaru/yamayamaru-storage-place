rem -3s��386�̖��߃Z�b�g�Ŋ֐��p�����[�^�̃X�^�b�N�Ăяo��
rem zp1��1�o�C�g�P�ʂŃA���C�������g����(�v����ɃA���C�������g���Ȃ�)
wcc386 -3s -zp1 -zq -zk0 -ox -fo=spr2.obj spr2.c
wlink system pharlap runtime CALLBUFS=32768 option quiet,STACK=262144 name spr2 file{ spr2.obj ..\lib\tbios.lib }