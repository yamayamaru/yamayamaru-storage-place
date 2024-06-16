FM TOWNSのVRAM直接アクセスのテストプログラムです

OpenWatcom C Ver2.0用です

OpenWatcom C Ver2.0はx64のWindows版を使ってWindows10 PC上でコンパイルしました
OpenWatcom Cのインストール時にターゲットにDOSのターゲットを選択する必要があります


scroll2.cはTOWNSの画面スクロールの機能ではなく
CPUの力業でバックバッファの内容をスクロールしてからVRAMに描画してます
640x480 256色モードを使って縦横の倍率を2倍にして
1024x512の仮想画面の画面表示位置を変更して
FM TOWNSの画面モードにはない320x240 256色表示を実現してます

画面の縦横の倍率の設定はset_screen_ratio()関数で実現してます
仮想画面の中の画面表示位置を変更する機能は
set_screen_move_virtual_screen()関数で実現してます
共にFM TOWNSのグラフィックスBIOSを呼び出してるだけです



bmpdata2.datとbmppale2.datはscroll2.cで使う画像データです
実行時にscroll2.expと同じ場所に置いてください


コンパイル方法

wcl386 -lpharlap -3r -zq -zk0 -ox scroll2.c

または

wcc386 -3r -zq -zk0 -ox -fo=scroll2.obj scroll2.c
wlink system pharlap option quiet,STACK=262144 name scroll2 file{ scroll2.obj }



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
