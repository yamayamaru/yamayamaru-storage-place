https://i-satoh.hatenablog.com/entry/65438175
ここのサイトのMotionJPEGの再生プログラムを参考にして
FM TOWNSで動画再生のテストをしてみました。
動画のみで音声は再生されません

今回のはFM TOWNS TOWNS OS用 FM TOWNS用High C移植版です
High Cに移植するためにC++で書かれていた部分をC言語用に書き直してます
High C V1.7L13でコンパイルしてFMTOWNのエミュレータの津軽で動作確認してます
画面モードは320x240の32768色モードを使ってます

JPegDecorderはemIDE_STM32F103C8_KMR1p8_MotionJPEG_test\Arduino\Libs\JPEGDecoder
のJPEGDecoder.hとJPEGDecoder.cppとpicojpeg.hとpicojpeg.cを参考にして手を加えました
botmerさんがメンテナンスされているものがあるようですがかなり手を加えられているのでそれでは動作しません
また、botmerさんがメンテナンスされているものはRGB565専用になってるようなので使いません

HDDに保存されたMotionJPEGの動画を表示するものです。


動画は映像のみの再生です、音声は再生されません。


長時間動画の再生はできてます。



MotionJPEGの動画の他にBMPやJPEGの画像も表示できます




このプログラムには動画ファイルは付属しません
232行目あたりのloop関数の中身を編集してご自分の動画ファイルを再生するように変更してください
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

MJPEG06.LNKをご自分の環境用に編集してからFM TOWNS上でcc.batを実行してください

MJPEG06.LNKの内容は下記のようになってます
mjpeg06
jpegdeco
picojpeg
  -lib f:\hc386\small\HCe.lib f:\hc386\small\na.lib f:\hc386\small\tbios.lib f:\hc386\small\fmcfrb.lib
  -stack 524288
  -exe mjpeg06

ここで-libの欄のHigh C用のライブラリの場所をご自分の環境に合わせてパスを変更してください
