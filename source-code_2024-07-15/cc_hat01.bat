del *.err
wpp -3 -zq -zk0 -ox -fo=hat01_w.obj hat01_w.cpp
wlink system windows option quiet,STACK=32K name hat01_w file { hat01_w.obj }
