PC上でHairless MIDIで変換したRS-MIDIのデータをRaspberry Pi Pico Wを使ってLinuxマシンにに飛ばしてみました
LinuxマシンににUSB-RS232C変換アダプタを付けてRS232Cケーブルで音源のMU80につないでます


Raspberry Pi Pico W用のサーバプログラムが
raspberry_pi_pico_w_arduino_rs-midi_serial_to_wifi_server.ino
です
Earle F.PhilhowerさんのArduino用のプログラムです

シリアルポートで受けたRS-MIDIのデータをWiFiに飛ばします

Hairless MIDIのSerial PortにはPicoのUSBのシリアルポートを指定します
この場合、プログラム25行目のコメントアウトを外してSERIAL0をSerialに設定してください

また、3.3VのUSB-Serialを使ってPicoのUART1に入力する場合は
Hairless MIDIのSerial Portには3.3VのUSB-SerialのCOMポートを指定して
プログラム25行目をコメントアウトして26行のコメントアウトを外してSERIAL0をSerial1に設定してください

たまに途中で止まってその後にデータが一気に流れることがあります
あくまで実験なのでご了承ください



Linuxマシン用のサーバプログラムが
raspberry_pi_rs-midi-serial_to_wifi_server.c
です
3.3VのUSB-Serialからの入力をネットワークに飛ばします

コンパイル方法は
   gcc -O2 -o raspberry_pi_rs-midi-serial_to_wifi_server raspberry_pi_rs-midi-serial_to_wifi_server.c -lpthread

実行方法は
   ./raspberry_pi_rs-midi-serial_to_wifi_server  serial_device

実行例は
   ./raspberry_pi_rs-midi-serial_to_wifi_server  /dev/ttyUSB0



クライアントはLinuxマシン用しか用意していません
クライアントプログラムが
raspberry_pi_rs-midi-serial_to_wifi_client.c
です
サーバプログラムが飛ばしたネットワークからのデータを受け取り
USB-RS232Cポートに出力してUSB-RS232CにつながっているRS-MIDI対応のMIDI音源を鳴らします

USB-RS232Cでなくて3.3VUSB-Serialの場合はMIDI音源のシリアルポートにつなぐには
RS232C用に電圧レベルを上げるコンバータモジュールが必要です

コンパイル方法は
    gcc -O2 -o raspberry_pi_rs-midi-serial_to_wifi_client raspberry_pi_rs-midi-serial_to_wifi_client.c

起動するとサーバのIPアドレスを聞いてくるのでサーバのIPアドレスを入力してください


実行方法は
    ./raspberry_pi_rs-midi-serial_to_wifi_client  serial_device

実行例は
    ./raspberry_pi_rs-midi-serial_to_wifi_client  /dev/ttyUSB0
