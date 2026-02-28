hc386 -v -c gra.c -fsoft
hc386 -v -c setscrn.c -fsoft
hc386 -v -c spsave.c -fsoft
tasm io.asm,,io.lst
rem run386 -nocrt f:\hc386\bin\tlinkp  @spsave.lnk
run386 -nocrt f:\hc386\bin\386linkp  @spsave.lnk
