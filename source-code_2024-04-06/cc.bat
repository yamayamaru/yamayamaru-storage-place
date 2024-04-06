wcc386 -3r -zq -zk0 -ox -fo=%1.obj %1.c
wlink system pharlap runtime CALLBUFS=65536 option quiet name %1 file{ %1.obj }
