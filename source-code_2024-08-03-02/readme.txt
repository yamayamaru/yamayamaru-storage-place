https://i-satoh.hatenablog.com/entry/65438175
ここのサイトのMotionJPEGの再生プログラムを参考にして
FM TOWNSで動画再生のテストをしてみました。
動画のみで音声は再生されません

今回のはFM TOWNS TOWNS OS用320x240 32768色モード用です

JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder
のJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cを参考にして手を加えました
botmerさんがメンテナンスされているものがあるようですがかなり手を加えられているのでそれでは動作しません
また、botmerさんがメンテナンスされているものはRGB565専用になってるようなので使いません

HDDに保存されたMotionJPEGの動画を表示するものです。


動画は映像のみの再生です、音声は再生されません。


長時間動画の再生はできてます。


MotionJPEGの動画の他にBMPやJPEGの画像も表示できます


このプログラムには動画ファイルは付属しません
356行目あたりのloop関数の中身を編集してご自分の動画ファイルを再生するように変更してください
(拡張子は大文字の.MJPにしてください)

対応している動画形式はMotionJPEGです。
拡張子は大文字半角の.MJPで保存してください。
MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
ffmpeg -i test_in3.mp4 -b:v 750k  -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov
ffmpeg -i test_in4.mp4 -b:v 200k  -r 12.0 -s 120x90 -vcodec mjpeg -an test_o4.mov
ffmpeg -i test_in5.mp6 -b:v 70k    -r 4.0 -s 120x90 -vcodec mjpeg -an test_o5.mov
拡張子は大文字の.MJPで保存してください。




コンパイル方法：

wasm -3 -zq -fl=rmical01.lst rmical01.asm
wlink format dos com option map,quiet name rmical01 file { rmical01.obj }
wdis rmical01.obj > rmical01.lst


wcc386 -3r -zq -zk0 -ox -fpc -fo=picojpeg.obj JPEGDecoder\picojpeg.c
wpp386 -3r -zq -zk0 -ox -fpc -fo=JPEGDecoder.obj JPEGDecoder\JPEGDecoder.cpp
wpp386 -3r -zq -zk0 -ox -fpc -fo=framebuf_draw_mjpeg02.obj framebuf_draw_mjpeg02.cpp
wlink system pharlap runtime MAXREAL=131072,MINREAL=131072 option quiet,STACK=256K name mjpeg021 file{ framebuf_draw_mjpeg02.obj JPEGDecoder.obj picojpeg.obj }




作成されたrmical01.comとmjpeg021と動画ファイルや画像ファイルを同じフォルダに入れて実行します
mjpeg021と同じフォルダにrmical01.comがないと実行を中断します





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
