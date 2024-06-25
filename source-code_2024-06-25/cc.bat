wcc -0 -zq -zk0 -s -fo=%1.obj %1.c
wlink system dos name %1 option quiet file{ %1.obj }
