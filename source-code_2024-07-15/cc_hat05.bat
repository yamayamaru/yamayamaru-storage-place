del *.err
wpp -3 -zq -zk0 -ox -fo=hat05_w.obj hat05_w.cpp
wlink system windows option quiet,STACK=32K name hat05_w file { hat05_w.obj }
