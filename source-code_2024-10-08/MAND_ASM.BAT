hc386 -v -c mand_asm.c -fsoft
tasm mand.asm,,mand.lst
run386 -nocrt f:\hc386\bin\386linkp  @mand_asm.lnk
