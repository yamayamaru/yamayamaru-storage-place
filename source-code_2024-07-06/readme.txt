FM TOWNSのMSDOS Ver3.1上でOpenWatcom C Ver2.0付属のDOSエクステンダーの
DOS4GWを使って動くプログラムを作ってみました

FM TOWNSではMSDOS Ver3.1を使うことでFMR-50用のMSDOSアプリが走ります
OpenWatcom C Ver2.0付属のDOSエクステンダーのDOS4GWを試してみたら普通に使えました

プログラムではDOS4GW上でDPMIのAPIを使ってFMR-50用のBIOSをコールしてます
BIOSで使うMSDOSメモリはDPMIのAPIのDPMIのAllocate DOS Memory BlockでMSDOSメモリを確保してます
BIOSコールはDPMIのAPIのSimulate Real Mode Interruptを使って行ってます

このプログラムはBIOSコールのためのサンプルプログラムのようなものです

FM R-50用のグラフィックBIOSのGDS-BASEを使って簡単なグラフィック描画をしてます
最後に、メキシカンハットの描画もしてます



OpenWatcom Cでのコンパイル方法
wcc386 -3s -zq -zk0 -ox -fo=gdsdemo1.obj gdsdemo1.c
wlink system dos4g option quiet,STACK=256K name gdsdemo1 file { gdsdemo1.obj }






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



FM TOWNSのMSDOSでDOS4GWを使うには
CONFIG.SYSを編集してEMM386の欄をREMでコメントアウトします(EMM386が起動してるとDOS4GWが動作しない)
AUTOEXEC.BATを編集して下記を追加します

SET PATH=%PATH%;d:\watcom\binw
SET DOS16M=5 @ 0 - 10m
SET WATCOM=d:\watcom

DOS4GW.EXEはd:\watcom\binwにコピーします
DOS4GW.EXEはOpenWatcom Cのbinwディレクトリの中に入ってます

再起動します



津軽で実行する場合
CONFIG.SYSとAUTOEXEC.BATを編集したら一度津軽を終了させます
・Enable 80387のチェックをつけます
・CPU Fidelity:はHIGH_FIDELITYにします



これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
