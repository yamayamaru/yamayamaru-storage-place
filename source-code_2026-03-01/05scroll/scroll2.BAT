hc386 -v -c gra.c -fsoft
hc386 -v -c setscrn.c -fsoft
hc386 -v -c scroll2.c -fsoft
tasm io.asm,,io.lst
rem run386 -nocrt f:\hc386\bin\tlinkp  @scroll2.lnk
run386 -nocrt f:\hc386\bin\386linkp  @scroll2.lnk
