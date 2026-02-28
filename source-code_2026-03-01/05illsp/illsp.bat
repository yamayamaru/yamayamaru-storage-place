hc386 -v -c gra.c -fsoft
hc386 -v -c setscrn.c -fsoft
hc386 -v -c extspdef.c -fsoft
hc386 -v -c illsp.c -fsoft
tasm io.asm,,io.lst
rem run386 -nocrt f:\hc386\bin\tlinkp  @illsp.lnk
run386 -nocrt f:\hc386\bin\386linkp  @illsp.lnk
