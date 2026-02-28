hc386 -v -c gra.c -fsoft
hc386 -v -c setscrn.c -fsoft
hc386 -v -c scroll3.c -fsoft
tasm io.asm,,io.lst
rem run386 -nocrt f:\hc386\bin\tlinkp  @scroll3.lnk
run386 -nocrt f:\hc386\bin\386linkp  @scroll3.lnk
