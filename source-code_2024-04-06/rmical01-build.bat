
del rmical01.obj
del rmical01.map
del rmical01.com
del rmical01.lst

wasm -3 -zq -fl=rmical01.lst rmical01.asm
wlink format dos com option map,quiet name rmical01 file { rmical01.obj }
