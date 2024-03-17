PC9821の256色のパックドモードで16MB空間にVRAMを出現させるデモプログラムです

OpenWatcom C Ver2.0およびWatcom C++ Ver10.5Jでコンパイルを確認しました


OpenWatcom C Ver2.0の環境変数の設定やコンパイル方法は下記を参照してください
下記URLはPC9821エミュレータのNP21kaiでの使い方の説明ですが環境変数の設定やコンパイル方法などはそのまま使えます
https://github.com/yamayamaru/yamayamaru-storage-place/blob/main/memo_2023-03-08/Using_OpenWatcom_C_V2.0_with_NP2kai_in_VisionFive2.txt




コンパイル方法は
wcc386 -4s -zq -zk0 -ox -ot -oe -oi -fo=vramwat2.obj vramwat2.c
wlink name vramwat2 file { vramwat2.obj }



-4sは486でパラメータをスタック渡し
-zqはメッセージ抑制
-zk0はSHIFT JIS対応
-oxは-oi,-ol,-om,-orとスタックオーバーフロー抑制
-otは速度最適化
-oeはnumより少ない演算子の数の関数のインライン展開:numのデフォルト値は20
-oiは特定のライブラリ関数のインライン展開




OpenWatcom C Ver2.0だとdos4gwを付ける必要があります(wcc386やwlinkの絶対パス名を指定する必要があるかもしれないです)

dos4gw c:\watcom\binw\wcc386 -4s -zq -zk0 -ox -ot -oe -oi -fo=vramwat2.obj vramwat2.c
dos4gw c:\watcom\binw\wlink name vramwat2 file { vramwat2.obj }

ここでc:\watcom\binw\はOpenWatcom C Ver2.0の実行ファイルが収められているディレクトリ





実行方法
OpenWatcom C Ver2.0では実行する時にdos4gwを付けて実行する必要があります
Watcom C++ Ver10.5Jではそのままファイル名のみで実行できます

dos4gw vramwat0




OpenWatcom C Ver2.0ではソースファイルの最後にEOFが付加されているとハングします
EOFを取り除くプログラム rmeof.cも一緒にアップしておきます

コンパイル方法は
dos4gw c:\watcom\binw\wcc -4 -zq -fo=rmeof.obj rmeof.c
dos4gw c:\watcom\binw\wlink name rmeof file { rmeof.obj }

使い方
rmeof ファイル名

ファイル名で指定したファイルのEOFを取り除きます
できればバックアップを取ってから実行してください





これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
