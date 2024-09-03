https://i-satoh.hatenablog.com/entry/65438175
ここのサイトのMotionJPEGの再生プログラムを参考にしてRaspberry Pi Picoで動画再生のテストをしてみました。
Pimoroni Pico Plus2でも動作しました(DMAでは動作しませんでした)
Raspberry Pi Pico及び、Pimoroni Pico Plus2用です。
動作確認はしてないですがRaspberry Pi Pico2でも動作すると思います。

ILI9341のLCD専用になってます。
ILI9341はSPI接続です。
他にSPI接続のSDカードのアダプタが必要です

今回はテスト用の動画も用意しました
SDカードに書き込んで実行してください。

SDカードに保存されたMotionJPEGの動画を表示するものです。
earlephilhowerのArduino動きました。
https://github.com/earlephilhower/arduino-pico

動画は映像のみの再生です、音声は再生されません。

FPSは遅いですが、長時間動画の再生はできてます。


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
