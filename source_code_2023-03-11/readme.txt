NP2kaiのnp21kaiを使ってOpenWatcom C v2.0でマンデルブロ集合を描いてみました。

VisionFive2上でnp21kaiを実行しましたがRaspberry Piなど他のSBC上でも動作するはずです。


OpenWatcom C v2.0のインストール方法はここを見てください
https://github.com/yamayamaru/yamayamaru-storage-place/blob/main/memo_2023-03-08/Using_OpenWatcom_C_V2.0_with_NP2kai_in_VisionFive2.txt


コンパイル方法

dos4gw b:\watcom\binw\wcc386 -4s -zq mande1.c
dos4gw b:\watcom\binw\wlink name mandel1 file { mandel1.obj }

b:\watcom\binwの部分はご自分の環境に合わせて変更してください。


dos4gw %WATCOM%\binw\wcc386 -4s -zq mandel1.c
dos4gw %WATCOM%\binw\wlink name mandel1 file { mandel1.obj }
これでもいいはずです。


-4sは486でスタック渡しのパラメータ設定です。
486でレジスタ渡しの場合は-4r
Pentiumでスタック渡しの場合は-5s
Pentiumでレジスタ渡しの場合は-5rのようです。



実行方法

dos4gw mandel1.exe
