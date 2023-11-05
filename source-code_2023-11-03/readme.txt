このプログラムはLinuxのフレームバッファ上でMotionJPEGの画像を表示するプログラムです

https://i-satoh.hatenablog.com/entry/65438175
上記サイトのプログラムを参考にして作ったteensy 4.1用のプログラムを改変して作ってみました
映像のみで音声はありません


Linuxのフレームバッファ出力にはffmpegが対応してるのでこのプログラム自体はあまり意味がないです
あくまで実験です




コンパイル方法

chmod u+x framebuf_draw_mjpeg02-build.sh

./framebuf_draw_mjpeg02-build.sh


実行方法

./framebuf_draw_mjpeg02




追記
https://www.pexels.com/ja-jp/search/videos/%E8%87%AA%E7%84%B6/
こちらのサイトからダウンロードした無料動画を付けてみました



動画ファイルは付属してないので実行するにあたっては150行目くらいにあるloop関数で
ご自分で用意したMotionJPEGのファイルを指定してください
拡張子は大文字の.MJP固定です

動画のエンコードはffmpegでビットレート、フレームレートや解像度を指定してエンコードできます
ffmpeg -i test_in1.mp4 -b:v 2700k -r 13.0 -s 426x320 -vcodec mjpeg -an test_o1.mov    
ffmpeg -i test_in2.mp4 -b:v 1500k -r 22.0 -s 320x240 -vcodec mjpeg -an test_o2.mov
ffmpeg -i test_in3.mp4 -b:v 750k -r 38.0 -s 240x160 -vcodec mjpeg -an test_o3.mov


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません



