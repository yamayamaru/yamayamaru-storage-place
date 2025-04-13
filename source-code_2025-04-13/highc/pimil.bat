hc386 -v -c pimil.c -softfp
tasm pimil_a.asm,,pimil_a.lst
run386 -nocrt f:\hc386\bin\386linkp @pimil
