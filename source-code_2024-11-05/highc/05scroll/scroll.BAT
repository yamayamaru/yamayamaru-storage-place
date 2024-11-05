hc386 -v -c gra.c -fsoft
hc386 -v -c setscrn.c -fsoft
hc386 -v -c scroll.c -fsoft
tasm io.asm,,io.lst
rem run386 -nocrt f:\hc386\bin\tlinkp  @scroll.lnk
run386 -nocrt f:\hc386\bin\386linkp  @scroll.lnk
