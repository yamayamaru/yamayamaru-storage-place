FM TOWNSのF-BASIC386用のマシン語ルーチンのREXモジュールをOpenWatcom C Ver2.0で作成することができました

OpenWatcom C Ver2.0はx64のWindows版を使ってWindows10 PC上でコンパイルしました
OpenWatcom Cのインストール時にターゲットにDOSのターゲットを選択する必要があります

コンパイル方法


wcc386 -3r -zq -zk0 -ox -fpc -fofunc06.obj func06.c
wlink format pharlap rex option quiet,stack=8192,map name func06 file{ func06.obj } library { C:\WATCOM\lib386\dos\clib3r.lib C:\WATCOM\lib386\math3r.lib }


作成されたfunc06.mapの内容を見て_func06のアドレスを確認します
このアドレスにBASIC側からCALLMで関数を呼び出します
この例では_func06のアドレスが0x00000010になってます

作成されたfunc06.rexをfunc06.basと同じ場所に置きます

BASICでREXモジュールを使う場合まずCLEAR文でREXモジュールを配置するメモリを確保します
次に、LOADM文でREXモジュールを読み込みます
次に、CALLM文でREXモジュールを呼び出します

BASICプログラムは次のようになります

1000 SCREEN@2:CLS
1010 CLEAR,,1024,1024,65536:' CLEAR ,,スタック領域,配列領域,プロシージャ領域
1020 LOADM "func06.rex",0
1030 RESULT&=0
1040 PA&=40
1050 PB&=20
1060 PCA&=100
1070 PCB&=100
1080 PSTEP01&=5
1090 TIME01=TIME
1100 CALLM &H10,VARPTR(RESULT&),PA&,PB&,PCA&,PCB&,PSTEP01&
1110 TIME02=TIME
1120 IF INKEY$="" THEN 1110
1130 CONSOLE 0,25,0
1140 PRINT "TIME = ", (TIME02-TIME01)
1150 END





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















追記：
FM TOWNSのエミュレータのうんづのところにTOWNS用のMSDOS 3.1と
TOWNSシステムソフトウェアからTOWNSのBIOSの一部のFMT_DOS.ROMを作るツールのMKOSROM.EXEがあるのですが
これで作ったFMT_DOS.ROMを置いておきます

いまのところ、津軽のところにある互換ROMのFMT_DOS.ROMを置き換えると
HDDイメージからしか起動できなくなりますがいろいろ快適に使えるようになります
