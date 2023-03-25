NP2kaiのnp21kaiを使ってOpenWatcom C v2.0でPC9821の256色モードを使ってみました
256色だと写真も表示できます。

8bitのBMPファイルの写真からパレットデータと画像データを抜き出したものを描いてみました。
8bitBMPファイルは画像革命2でJPEGの写真を8bit BMPファイルに変換したものです。

VisionFive2上でnp21kaiを実行しましたがRaspberry Piなど他のSBC上でも動作するはずです。


OpenWatcom C v2.0のインストール方法はここを見てください
https://github.com/yamayamaru/yamayamaru-storage-place/blob/main/memo_2023-03-08/Using_OpenWatcom_C_V2.0_with_NP2kai_in_VisionFive2.txt


コンパイル方法

画像データが大きくてコンパイルするのに大きなメモリが必要です。
np21kaiでメモリを32.6MBにしてください

次に環境変数DOS16Mを次のように設定します

set DOS16M=1 @ 0 - 32m


あとはコンパイルするだけです

dos4gw b:\watcom\binw\wcc386 -4s -zq tstpic01.c
dos4gw b:\watcom\binw\wlink name tstpic01 file { tstpic01.obj }

b:\watcom\binwの部分はご自分の環境に合わせて変更してください。


dos4gw %WATCOM%\binw\wcc386 -4s -zq tstpic01.c
dos4gw %WATCOM%\binw\wlink name tstpic01 file { tstpic01.obj }
これでもいいはずです。


-4sは486でスタック渡しのパラメータ設定です。
486でレジスタ渡しの場合は-4r
Pentiumでスタック渡しの場合は-5s
Pentiumでレジスタ渡しの場合は-5rのようです。



実行方法

dos4gw tstpic01.exe
