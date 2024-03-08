sdcc4.2.0用のマンデルブロ集合のプログラムです

コンパイル方法


Windowsの場合

set SDCC_HOME=  #ここにsdccのHOMEのパスを指定
set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC_HOME%\bin;%PATH%

del mandelc.com
del mandelc.rel
del mandelc.asm

sdcc -mz80 -c -o mandelc.rel mandelc.c

sdcc -mz80 -c -o msxclib.rel msxclib.c
sdasz80 -l -s -o msxalib.rel msxalib.asm
sdasz80 -o msxdos.rel msxdos.asm

sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o mandelc.ihx mycrt0_2.rel mandelc.rel msxclib.rel msxalib.rel msxdos.rel

makebin -s 14100 mandelc.ihx > mandelc.bin
python3 binout.py mandelc.bin mandelc.com


ここでbinout.pyは先頭の256バイトを削除するスクリプトです
Python3のスクリプトになってますのでPython3を入れてください



Linuxの場合

export SDCC_HOME=  #ここにsdccのHOMEのパスを指定
export SDCC_INCLUDE=$SDCC_HOME/include
export PATH=$SDCC_HOME/bin:$PATH

rm mandelc.com
rm mandelc.rel
rm mandelc.asm

sdcc -mz80 -c -o mandelc.rel mandelc.c

sdcc -mz80 -c -o msxclib.rel msxclib.c
sdasz80 -l -s -o msxalib.rel msxalib.asm
sdasz80 -o msxdos.rel msxdos.asm

sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o mandelc.ihx mycrt0_2.rel mandelc.rel msxclib.rel msxalib.rel msxdos.rel

makebin -s 14100 mandelc.ihx > mandelc.bin
python3 binout.py mandelc.bin mandelc.com



Linuxの場合上記コマンドをmandelc-build.shというスクリプトにしてあります


備考：
Linuxの場合、MSXのFDのイメージは次のようにマウントできます。
$ sudo mount -o loop fdimage01.dsk /mnt

cpでファイルをコピーして終わったらumountします。
$ sudo umount /mnt

FDのイメージは次のように作成できます。(ubuntuの場合mkdosfsはdosfstoolsというパッケージに入ってます)
$ mkdosfs -C fdimage01.dsk 720


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
