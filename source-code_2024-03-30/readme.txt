FM TOWNSエミュレータの津軽にはRS232Cの出力を指定したIPアドレスのネットワークに飛ばすことができます
このプログラムはFM TOWNSエミュレータの津軽でRS232Cのデータをネットワークに飛ばしたものを
Raspberry Piで受けてそのデータをそのままMIDIインターフェースへ出力するものです


  コンパイル方法
      gcc -O2 -o raspberrypi_midi_data_wifi_accept_to_midi_server raspberrypi_midi_data_wifi_accept_to_midi_server.c  -lpthread

  実行方法
      ./raspberrypi_midi_data_wifi_accept_to_midi_server  midi_device
  実行例
      ./raspberrypi_midi_data_wifi_accept_to_midi_server  /dev/snd/midiC2D0

      注意: /dev/snd/midiC2D0の部分はご自分のMIDIインターフェースのデバイスを指定してください

Raspberry PiにつないだMIDIインターフェースの出力をWindows PCのMIDIインターフェースに入力して
Windows上のvsthostにMIDIデータを送れます
vsthostでVSTプラグインをMIDIで演奏させることができるフリーソフトです
これを使ってVSTプラグインのSound Canvas VAを鳴らすことができます



使い方
    津軽でVMを起動する前にRaspberry Piで実行してください
    パラメータにmidiデバイスを指定します
実行例
    ./raspberrypi_midi_data_wifi_accept_to_midi_server  /dev/snd/midiC2D0

midiデバイスは/dev/sndの中にあります

津軽の方ではRS232Cの設定欄にRaspberry PiのIPアドレスを入力します




MIDIではなく、Serialに出力するものも作りました
こちらはWindows PCとRaspberry PiをSerialで接続して
そのデータをHairless MIDIでMIDIポートに出力することができます
仮想MIDIポートのloopMIDIを使えばHairlessMIDIの出力を
Windows上のvsthostに送ることができます

Windows PCとRaspberry Piの接続はMIDIよりもSerialでつなぐ方がお手軽なのでこれを作りました


  コンパイル方法
      gcc -O2 -o raspberrypi_midi_data_wifi_accept_to_serial_server raspberrypi_midi_data_wifi_accept_to_serial_server.c  -lpthread

  実行方法
      ./raspberrypi_midi_data_wifi_accept_to_serial_server  serial_device
  実行例
      ./raspberrypi_midi_data_wifi_accept_to_serial_server  /dev/ttyUSB0

      注意: /dev/ttyUSB0の部分はご自分のMIDIインターフェースのデバイスを指定してください



これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
