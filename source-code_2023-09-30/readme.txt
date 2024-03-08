Z88DK用のマンデルブロ集合のプログラムです

コンパイル方法


Windowsの場合

set Z88DK_HOME= #ここにZ88DKのHOMEのパスを指定する
set ZCCCFG=%Z88DK_HOME%\lib\config
set PATH=%Z88DK_HOME%\bin;%PATH%

zcc +msx -DNODELAY -lm -subtype=msxdos mandelc.c msxclib.c msxalib.c -o mandelc.com




Linuxの場合
export Z88DK_HOME= #ここにZ88DKのHOMEのパスを指定する
export ZCCCFG=${Z88DK_HOME}/lib/config
export PATH=${Z88DK_HOME}/bin:${PATH}

zcc +msx -DNODELAY -lm -subtype=msxdos mandelc.c msxclib.c msxalib.c -o mandelc.com




Linuxの場合上記コマンドをmandelc-build.shというスクリプトにしてあります




備考：
Linuxの場合、MSXのFDのイメージは次のようにマウントできます。
$ sudo mount -o loop fdimage01.dsk /mnt

cpでファイルをコピーして終わったらumountします。
$ sudo umount /mnt

FDのイメージは次のように作成できます。(ubuntuの場合mkdosfsはdosfstoolsというパッケージに入ってます)
$ mkdosfs -C fdimage01.dsk 720


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません
