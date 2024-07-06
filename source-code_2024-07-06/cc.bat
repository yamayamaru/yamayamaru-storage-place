wcc386 -3s -zq -zk0 -ox -fo=%1.obj %1.c
wlink system dos4g option quiet,STACK=256K name %1 file { %1.obj }
