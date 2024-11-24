rem wcc386 -4s -zq -zk0 -ox -ot -oe -oi -fo=%1.obj %1.c
wcc -2 -zq -zk0 -ox -ml -fo=%1.obj %1.c
wlink system dos option quiet,STACK=8K name %1 file { %1.obj } library { graph98.lib }

