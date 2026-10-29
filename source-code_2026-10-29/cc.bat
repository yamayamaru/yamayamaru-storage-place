del hat05p_w.exe
del hat05p_w.obj
del hat05p_w.rex

rem wpp386 -bt=win386 -3s -zW -zq -zk0 -ox -fpc -fo=hat05p_w.obj hat05p_w.cpp
wpp386 -bt=win386 -3s -zW -zq -zk0 -ox -fpi87 -fo=hat05p_w.obj hat05p_w.cpp

rem wlink system win386 option quiet,STACK=256k name hat05p_w file{ hat05p_w.obj }

wlink @hat05p_w.lnk

wbind -n hat05p_w
