wcc -2 -zq -zk0 -ox -ml -fo=testdemo.obj testdemo.c
wlink system dos option quiet,STACK=8K name testdemo file { testdemo.obj } library { graph98.lib }

