https://i-satoh.hatenablog.com/entry/65438175
ここのサイトのMotionJPEGの再生プログラムを参考にして
Raspberry Pi Pico2でPico-DVIを使って動画再生のテストをしてみました。

Pimoroni Pico Plus2でも動作しました

SDカードに保存されたMotionJPEGの動画を表示するものです
Raspberry Pi Pico用のArduinoのearlephilhowerのArduino用です
動画は映像のみの再生です、音声は再生されません
FPSは遅いですが、長時間動画の再生はできてます

Raspberry Pi Pico2、Adafuit DVIブレークアウトボードの他に、別途SDカードのアダプタが必要です



Adafuit DVIブレークアウトボードのピンの接続は下記を参照してください
https://learn.adafruit.com/adafruit-dvi-breakout-board/raspberry-pi-pico-demo-code

Adafuit DVIブレークアウトボードのライブラリのインストール方法は下記を参照してください
https://learn.adafruit.com/picodvi-arduino-library-video-out-for-rp2040-boards/installation

JPEGDecoderフォルダをArduinoのライブラリフォルダに入れてビルドしてください


付属の動画ファイルや画像ファイルはSDカードに保存して使うためのものです
