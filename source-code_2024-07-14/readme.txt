https://i-satoh.hatenablog.com/entry/65438175
ここのサイトのMotionJPEGの再生プログラムを参考にして
PC9821の256色パックドモードでで動画再生のテストをしてみました。
動画のみで音声は再生されません

今回のはPC9821の256色パックドモード用です
実行にはOpenWatcom C Ver2.0のDOSエクステンダーDOS4GWを使います
DOSエクステンダーを使うことで1MB以上のメモリへのアクセスが可能になり、
PC9821の256色パックドモードの場合、VRAMを0x00F00000に出現させることができるので、
512KBのVRAMへのリニアアクセスが可能になります

JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder
のJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cを参考にして手を加えました
botmerさんがメンテナンスされているものがあるようですがかなり手を加えられているのでそれでは動作しません
また、botmerさんがメンテナンスされているものはRGB565専用になってるようなので使いません

HDDに保存されたMotionJPEGの動画を表示するものです。
動画は映像のみの再生です、音声は再生されません。
長時間動画の再生はできてます。

このプログラムには動画ファイルは付属しません
255行目あたりのloop関数の中身を編集してご自分の動画ファイルを再生するように変更してください
(拡張子は大文字の.MJPにしてください)

対応している動画形式はMotionJPEGです。
拡張子は大文字半角の.MJPで保存してください。
MotionJPEGの動画ファイルはパソコンでffmpegを使って下記のように作成できます。
ffmpeg -i 'test01.mp4' -b:v 750k -r 10.0 -s 320x240 -vcodec mjpeg -an V4_10FPS.mov
ffmpeg -i 'test02.mp4' -b:v 125k -r 5.0   -s 120x90 -vcodec mjpeg -an V5_05FPS.mov
ffmpeg -i 'test03.mp4' -b:v 200k -r 15.0  -s 120x90 -vcodec mjpeg -an V6_15FPS.mov
ffmpeg -i 'test04.mp4' -b:v 140k -r 8.0   -s 120x90 -vcodec mjpeg -an V7_08FPS.mov
ffmpeg -i 'test05.mp4' -b:v 200k -r 12.0  -s 120x90 -vcodec mjpeg -an V8_08FPS.mov
拡張子は大文字の.MJPで保存してください。




コンパイル方法：

wcc386 -3r -zq -zk0 -ox -fpc -fo=picojpeg.obj JPEGDecoder\picojpeg.c
wpp386 -3r -zq -zk0 -ox -fpc -fo=JPEGDecoder.obj JPEGDecoder\JPEGDecoder.cpp
wpp386 -3r -zq -zk0 -ox -fpc -fo=framebuf_draw_mjpeg02-web216_02.obj framebuf_draw_mjpeg02-web216_02.cpp
wlink system dos4g option quiet,STACK=256K name mjpeg032 file{ framebuf_draw_mjpeg02-web216_02.obj JPEGDecoder.obj picojpeg.obj }



count_timer_prog01.asmはPC98のタイマBIOS用のハンドラプログラムです
動画再生のソースにはマシン語コードをそのまま配列で定義して使ってるので
コンパイルには使わないのですがハンドラを書き換えたい人は参考にしてください
実行にはOpenWatcom C Ver2.0に付属のdos4gw.exeが必要です

PC98のMSDOSでDOS4GWを使うには
CONFIG.SYSを編集してEMM386の欄をREMでコメントアウトします(EMM386が起動してるとDOS4GWが動作しない)
AUTOEXEC.BATを編集して下記を追加します

SET PATH=%PATH%;d:\watcom\binw
SET DOS16M=1 @ 0 - 10m
SET WATCOM=a:\watcom

DOS4GW.EXEはa:\watcom\binwにコピーします
DOS4GW.EXEはOpenWatcom Cのbinwディレクトリの中に入ってます

再起動します



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
