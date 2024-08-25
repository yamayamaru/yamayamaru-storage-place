del *.err
wpp -2 -zq -zk0 -ox -ms -fo=imeonoff.obj imeonoff.cpp
wlink system windows option quiet,STACK=8K name imeonoff file { imeonoff.obj } library { winnls.lib }
