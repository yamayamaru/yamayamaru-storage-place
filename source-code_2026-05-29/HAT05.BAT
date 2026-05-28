wcc386 -3s -zq -zk0 -ox -fo=hat05.obj hat05.c
wasm hat05asm.asm
wlink system pharlap option quiet,STACK=256K name hat05 file{ hat05.obj hat05asm.obj }
