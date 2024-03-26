FM TOWNSのエミュレータ津軽でTOWNS互換BIOSを使ってOpenWatcom C Ver2.0で
マンデルブロ集合のプログラムを作ってみました

BIOSはインラインアセンブラで叩いてます

OpenWatcom C Ver2.0はx64のWindows版を使ってWindows10 PC上でコンパイルしました
OpenWatcom Cのインストール時にターゲットにDOSのターゲットを選択する必要があります

TOWNS OSはフリコレ6で起動して実行しました


コンパイル方法

wcl386 -lpharlap -3r -zq -zk0 -s mandel1.c





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






一部更新しました

実は28画面分のデータを仕込んではいたのですが実行時間が
長いため1画面の描画が終了したらプログラムを終わるようにしてましたが
何かキーを押すことで中断するようにして28画面分全部実行するように変更しました
文字も表示できた方がいいのでdraw_string()関数も追加しました



これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
