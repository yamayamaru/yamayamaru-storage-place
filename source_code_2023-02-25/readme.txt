コンパイル方法

sdcc -mz80 -c -DTIME -o dhry-1.rel dhry-1.c
sdcc -mz80 -c -DTIME -o dhry-2.rel dhry-2.c
sdasz80 -o mycrt0_2.rel mycrt0_2.asm

sdcc -mz80 --code-loc 0x100 --data-loc 0 --no-std-crt0 -o dhry21.ihx mycrt0_2.rel dhry-1.rel dhry-2.rel

makebin -s 15000 dhry21.ihx > dhry21.bin
gcc -o binout bintou.c
./binout dhry21.bin > dhry21.com



備考：
Linuxの場合、MSXのFDのイメージは次のようにマウントできます。
$ sudo mount -o loop fdimage01.dsk /mnt

cpでファイルをコピーして終わったらumountします。
$ sudo umount /mnt

FDのイメージは次のように作成できます。(ubuntuの場合mkdosfsはdosfstoolsというパッケージに入ってます)
$ mkdosfs -C fdimage01.dsk 720
