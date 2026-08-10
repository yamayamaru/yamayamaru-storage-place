https://i-satoh.hatenablog.com/entry/65438175
ここのサイトのMotionJPEGの再生プログラムを参考にしてRaspberry Pi Picoで動画再生のテストをしてみました。
Pimoroni Pico Plus2でも動作しました
Raspberry Pi Pico及び、Pimoroni Pico Plus2用です。
Raspberry Pi Pico2でも動作します。

ILI9341のLCD専用になってます。
ILI9341はSPI接続です。
他にSPI接続のSDカードのアダプタが必要です
SDカードはSPIで指定していてSPIはデフォルトでは下記のピンになってます
    MISO  GPIO16(Pin番号21)
    MOSI  GPIO19(Pin番号25)
    SCK   GPIO18(Pin番号24)
    SS    GPIO17(Pin番号22)

ファイルが読めない場合は、SDカードのSPIの周波数を調整してください
LCDが真っ白になって表示がおかしい場合はLCDのSPIの周波数を下げてみてください

Windowsの場合ArduinoのArduino coreは下記のフォルダに格納されてます
C:\Users\username\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\4.0.1

Linuxの場合は下記にあります
~/.arduino15/packages/rp2040/hardware/rp2040/4.0.1

pico専用ライブラリは下記にあります
C:\Users\username\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\4.0.1\libraries
  

今回はテスト用の動画も用意しました
SDカードに書き込んで実行してください。

SDカードに保存されたMotionJPEGの動画を表示するものです。
earlephilhowerのArduino動きました。
https://github.com/earlephilhower/arduino-pico

動画は映像のみの再生です、音声は再生されません。

FPSは遅いですが、長時間動画の再生はできてます。


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
