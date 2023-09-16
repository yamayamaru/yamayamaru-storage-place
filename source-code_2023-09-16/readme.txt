MSX-C ver1.2 およびMSX-C Liberary 1.1を使ったサンプルプログラムです

マンデルブロ集合を描きます
動作はかなり時間がかかります
何かキーを押すと実行が中断されます


MSX-Cでコンパイルするときはソースファイルの改行コードをCRLFにして保存してください

コンパイル方法
     cd prog
     ccc mandel

コンパイルするとMANDEL.COMができます


コンパイルするにはそれぞれのディスクからファイルをコピーして
ワーク用のディスクを作成する必要があります

コンパイルしたディスクのディレクトリ構成は下記のようになります

t-- AUTOEXEC.BAT
t-- BIN
| t-- AKID.COM
| t-- C.BAT
| t-- CC.BAT
| t-- CF.COM
| t-- CG.COM
| t-- CURSES.TCO
| t-- FPC.COM
| t-- GLIB.TCO
| t-- KID.COM
| t-- L80.COM
| t-- LIB.TCO
| t-- M80.COM
| t-- MATH.TCO
| t-- MLIB.TCO
| t-- MORE.COM
| t-- MSXBIOS.TCO
| t-- MX.COM
t-- COMMAND2.COM
t-- INCLUDE
| t-- BDOSFUNC.H
| t-- CONIO.H
| t-- CTYPE.H
| t-- CURSES.H
| t-- DIRECT.H
| t-- GLIB.H
| t-- IO.H
| t-- MALLOC.H
| t-- MATH.H
| t-- MEMORY.H
| t-- MSXBIOS.H
| t-- PROCESS.H
| t-- SETJMP.H
| t-- STDIO.H
| t-- STDLIB.H
| t-- STRING.H
| t-- TYPE.H
t-- LIB
| t-- CEND.REL
| t-- CK.REL
| t-- CLIB.REL
| t-- CRUN.REL
| t-- CURSES.REL
| t-- GLIB.REL
| t-- MATH.REL
| t-- MLIB.REL
| t-- MSXBIOS.REL
t-- MSXDOS2.SYS
t-- PROG
    t-- CCC.BAT
    t-- MANDEL.C
    t-- MANDEL.COM

4 directories, 49 files
