誤差拡散法によって24bitのBitmap画像から
MSXのSCREEN 8の8bit画像を作るプログラムを作ってみました

プログラム本体はerror_diffusion01.cです
コマンドラインのプログラムで起動方法は下記になります

  error_diffusion01 in-file out-file

in-fileに24bitのbitmapのRAWデータを指定します
out-fileに生成するMSXのSCREEN 8用の8bit画像のファイル名を指定します

24bitのRGBのbitmapのRAWデータから
MSXのSCREEN 8の8bit画像を生成します。
生成するSCREEN 8の8bit画像はBASICのBLOADで読み込めるようにBSAVEのヘッダーが付いてます
データだけ必要な場合はBSAVEのヘッダー部分の先頭から7バイトを読み飛ばしてください

Windowsでコンパイルする場合はVisual StudioのBuild Toolsもしくは、MinGW-w64が必要です
Build Toolsでコンパイルする場合はDeveloper Command Pronpt for VSxxxxで
error_diffusion01-build.batを実行してください
MinGW-w64でコンパイルする場合は
  chmod u+x *.sh
  error_diffusion01-build.sh
になります。

Linuxでコンパイルする場合は
  chmod u+x *.sh
  error_diffusion01-build.sh
になります。



おまけで24bitのBMP画像から24bitのbitmapのRAWデータを生成するPythonのスクリプトも付けました
実行方法は下記になります

  python3 bmp24_data_to_rgb24bit_raw_data.py in-file out-file

in-fileに24bitのBMP画像を指定します
out-fileに24bit bitmapのRAW画像のファイル名を指定します
あくまでおまけです。
24bit BMP画像はWindowsに付属のpaintで一度開いてから保存しないと変換できないかもしれないです



python3がインストールされてPATHが通ってる状態で
bmp24_data_to_msx_screen8_data_bsave.bat(Windows用)もしくは
bmp24_data_to_msx_screen8_data_bsave.sh(LinuxまたはMingw-w64用)
を実行すると24bit BMP画像からMSX SCREEN 8の8bit画像への変換のサンプルを生成できます。


おまけの
plot_dat-256x212-24bit.pyはPython3のtkinter用の24bit bitmap RAW画像を表示するためのプログラムです
msx_screen8_plot.pyはPython3のtkinter用のMSX 8bit画像を表示するめのプログラムです
これらのプログラムの実行にはPythonのpillowモジュールのインストールが必要です。pipで入れてください。
tkinterはPythonをインストールすると自動でインストールされます。


bmp24_data_to_rgb24bit_raw_data.pyは
https://www.tutimogura.com/python-bitmap-read/
を参考にしてます
error_diffusion01.cは
https://zenn.dev/baroqueengine/books/a19140f2d9fc1a/viewer/531f1d
を参考にして作りました。


BASICでMSX SCREEN 8用に変換した5枚のサンプル画像を表示するプログラムとしてPUTSCRN8.BASを用意しました
このプログラムのようにBASICのBLOADに,Sオプションを付けてBASICでVRAMに読み込むことが可能です
