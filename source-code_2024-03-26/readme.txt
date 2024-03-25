FM TOWNSのエミュレータ津軽でTOWNS互換BIOSを使ってOpenWatcom C Ver2.0で
マンデルブロ集合のプログラムを作ってみました

BIOSはインラインアセンブラで叩いてます

OpenWatcom C Ver2.0はx64のWindows版を使ってWindows10 PC上でコンパイルしました
OpenWatcom Cのインストール時にターゲットにDOSのターゲットを選択する必要があります

TOWNS OSはフリコレ6で起動して実行しました


コンパイル方法

wcl386 -lpharlap -3r -zq -zk0 -s mandel1.c
