wasm -3 -zq -fl=count_timer_prog01.lst count_timer_prog01.asm
wlink format dos com option map,quiet name count01 file { count_timer_prog01.obj }
wdis count_timer_prog01.obj > count_timer_prog01.lst
