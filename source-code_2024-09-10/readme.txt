Pimoroni Pico Plus2用のPSRAMテスト用プログラムです
πの計算プログラムです

https://www.kurims.kyoto-u.ac.jp/~ooura/pi_fft-j.html
ここのサイトのpi_fftc6をArduinoに移植してみました

Pimononi Pico Plus2のPSRAMのCSはCircutPythonのソースコードを確認したところ
GPIO47が使われています

earlephilhowerのArduinoではWindowsでは
C:\Users\username\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\4.0.1\variants\generic_rp2350
にあるpins_arduino.hに

#define RP2350_PSRAM_CS         (47u)
#define RP2350_PSRAM_MAX_SCK_HZ (109*1000*1000)

を定義して、Arduino IDEのボードにGeneric RP2350を選択してコンパイルすると
Pimoroni Pico Plus2でArduinoからPSRAMを使うことができました

Linuxではお使いのユーザのホームディレクトリの.arduino15/packages/rp2040/hardware/rp2040/4.0.1/variants/generic_rp2350
にあるpins_arduino.hになると思います

上記行為はあくまで自己責任で行ってください

これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
