cf -r10:9:4 %2 %1
fpc lib mlib %1
cg -k %3 %1
m80 =%1/z
del %1.mac
l80 %lib%ck,%1,%lib%mlib/s,%lib%clib/s,%lib%crun/s,%lib%cend,%1/n/y/e:xmain
del %1.rel
del %1.sym

