wcc386 -3s -zq -zk0 -ox -fo=testdemo.obj testdemo.c
wlink system dos4g option quiet,STACK=256K name testdemo file { testdemo.obj } library { graph98.lib }

