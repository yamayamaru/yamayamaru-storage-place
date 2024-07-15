Windows 3.1用のメキシカンハットのプログラムです

hat01_w.cppが通常のメキシカンハットプログラム

hat05_w.cppがグラデーション付きのメキシカンハットのプログラムです

Borland C++を持ってない人のためにOpenWatcom C++ Ver2.0でもコンパイルできるようにしました


OpenWatcom C++ Ver2.0はWindows 3.1用のプログラムの作成もできます
OpenWatcom C++ Ver2.0でのコンパイル方法は下記になります

wpp -3 -zq -zk0 -ox -fo=hat01_w.obj hat01_w.cpp
wlink system windows option quiet,STACK=32K name hat01_w file { hat01_w.obj }


wpp -3 -zq -zk0 -ox -fo=hat05_w.obj hat05_w.cpp
wlink system windows option quiet,STACK=32K name hat05_w file { hat05_w.obj }









OpenWatcom C Ver2.0はここからダウンロードできます
https://github.com/open-watcom/open-watcom-v2/releases

Windowsでクロス開発するために64bitWindowsなら
open-watcom-2_0-c-win-x64.exe
32bitWindowsなら
open-watcom-2_0-c-win-x86.exe
をダウンロードしてください

Linux版は実験段階のようですが
とりあえず32bitのx86または64bitのx64用に用意されています

インストール時にターゲットでDOSを選択しないと
DOS用のコンパイラが入らないので要注意


環境変数はc:\watcomにインストールした場合下記のように設定する必要があるようです

PATHには下記を追記
C:\WATCOM\BINNT64;C:\WATCOM\BINNT

以下のように環境変数を設定
INCLUDE=C:\WATCOM\H;C:\WATCOM\H\NT;C:\WATCOM\H\NT\DIRECTX;C:\WATCOM\H\NT\DDK
WATCOM=C:\WATCOM
EDPATH=C:\WATCOM\EDDAT
WHTMLHELP=C:\WATCOM\BINNT\HELP
WIPFC=C:\WATCOM\WIPFC


64bitのx64LinuxではPATHに下記を追記する必要があるようです
watcom cをインストールした場所のPATH/watcom/binl64

これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
