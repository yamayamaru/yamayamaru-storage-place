rem ** FPU Ç…ÇÊÇÈïÇìÆè¨êîì_ââéZ **
run386  %GCCROOT%/usr/bin/cpp  -D__GNUC__ -DTOWNS -I. -I %GCCROOT%/usr/include %1.c %TMP%/_tmp.i
if errorlevel 1 goto exit
run386  %GCCROOT%/usr/bin/cc1 -O -m80387 -quiet %TMP%/_tmp.i
if errorlevel 1 goto exit
run386  %GCCROOT%/usr/bin/gas -o %TMP%/_tmp.o %TMP%/_tmp.s
if errorlevel 1 goto exit
run386  %GCCROOT%/usr/bin/ld -N -S -o %TMP%/_tmp.bin %GCCROOT%/usr/lib/crt0.o %TMP%/_tmp.o %2 %3 %4 %5 %6 %7 %8 %9 -L%GCCROOT%/usr/lib  -lc
if errorlevel 1 goto exit
genexp  %TMP%/_tmp.bin %1.exp
:exit
