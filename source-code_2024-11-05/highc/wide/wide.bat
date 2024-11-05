hc386 -v -c gra.c -fsoft
hc386 -v -c setscrn.c -fsoft
hc386 -v -c wide.c -fsoft
tasm io.asm,,io.lst
rem run386 -nocrt f:\hc386\bin\tlinkp  @wide.lnk
run386 -nocrt f:\hc386\bin\386linkp  @wide.lnk
