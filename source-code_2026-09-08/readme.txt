PC9801やPC9821でFM TOWNS用のGNU for TOWNS Rel2が使えないか試してみました

 
ここのプログラムは最初、FM TOWNS用のHigh C用に作り、完成した後、GNU for TOWNS Rel2のgcc用に移植したものです
PC9821の256色パックドモードを使うので256色パックドモードに対応した機種でしか動作しません。
このプログラムはメキシカンハットを描画するプログラムです


set PATH=%PATH%;i:\;i:\usr\bin;i:\gnuish\futil14;
set gccroot=i:

(i:はCDROMドライブのドライブ名です。i:の部分はご自分の環境に合わせて変更してください)

GNU for TOWNS Rel2のCDROMをセットします
上記のようにi:\、i:\user\bin、i:\gnuish\futil14にパスを通す
環境変数gccroot=i:を設定

run386 i:\usr\bin\gas -o hat05asm.o hat05asm.s
i:\usr\bin\gclf.bat hat05 hat05asm.o -lt

上記のようにBATファイルを実行します
gclf.batは80387命令を生成します
gcle.batは浮動小数点演算には浮動小数点エミュレーションを使います




インターネットで公開されているgccでもコンパイルできると思います
https://anikun.kutami.jp/towns-gcc/


インターネットに公開されているgccでビルド方法

set GCC=a:/tgcc/usr/bin
set GINC=a:/tgcc/usr/include
set GLIB=a:/tgcc/usr/lib
PATH=%PATH%;a:\tgcc\usr\bin
set gccroot=a:/tgcc

上記のように環境変数を設定(a:\TGCCがある場合を想定してます、ドライブ番号はご自分の環境に合わせて設定してください)

exe386 a:\tgcc\usr\bin\gas -o hat05asm.o hat05asm.s
gcc -O -msoft-float -o hat05 hat05.c hat05asm.o -lt
または
gcc -O -m80387 -o hat05 hat05.c hat05asm.o -lt

実行方法
exe386 hat05


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
