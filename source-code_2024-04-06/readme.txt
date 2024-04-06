FM TOWNSのエミュレータ津軽でTOWNS互換BIOSを使ってリアルモードBIOSを呼び出すことに成功したので
そのプログラムを公開します
リアルモードのTBIOSの日時取得BIOSを呼び出して、画面に日時を表示するだけのプログラムです


OpenWatcom C Ver2.0はx64のWindows版を使ってWindows10 PC上でコンパイルしました
OpenWatcom Cのインストール時にターゲットにDOSのターゲットを選択する必要があります

コンパイル方法


wasm -3 -zq -fl=rmical01.lst rmical01.asm
wlink format dos com option map,quiet name rmical01 file { rmical01.obj }

wcc386 -3r -zq -zk0 -ox -fo=trmical1.obj trmical1.c
wlink system pharlap runtime CALLBUFS=65536 option quiet name trmical1 file{ trmical1.obj }


出来たtrmical1.expとrmical01.comを同じ場所に置いて実行してください



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
