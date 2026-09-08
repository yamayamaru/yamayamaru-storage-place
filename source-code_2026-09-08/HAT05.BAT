rem exe386.exeをrun386.exeとしてコピーしてカレントディレクトリに置く

run386 i:\usr\bin\gas -o hat05asm.o hat05asm.s
a:\command /c i:\usr\bin\gclf.bat hat05 hat05asm.o -lt
