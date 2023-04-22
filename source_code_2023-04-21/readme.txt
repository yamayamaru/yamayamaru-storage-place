raspberry_pi_serial_to_midi.cはシリアルポートからRS-MIDIのデータを受け取って
そのデータをそのままMIDIインターフェースに出力してMIDI音源を鳴らします

コンパイル方法
      gcc -O2 -o raspberry_pi_serial_to_midi raspberry_pi_serial_to_midi.c

実行方法
      ./raspberry_pi_serial_to_midi  serial_device  midi_device
実行例
      ./raspberry_pi_serial_to_midi  /dev/ttyUSB0  /dev/snd/midiC2D0


このプログラムで生じた損害などは一切保障しません
自己責任で行ってください

Windows上でX68000エミュレータのXM6typeGでtantanさんのFDイメージを使って
3.3V USB-Serialから出力してRaspberry Piにつないだ3.3V USB-Serialで受け取ったデータを
MIDIインターフェースに直接データをそのまま流してます

PCの3.3V USB-SerialとRaspberry Pi側の3.3V USB-Serialの接続方法は
まず、両方のGNDをつなぎ、次にPC側の3.3V USB-SerialのTXをRaspberry Pi側の3.3V USB-SerialのRXにつなぎます
これだけでOKです

Raspberry Pi側はRaspberry PiでUARTを使える設定を行えば3.3V USB-Serialでなくても
Raspberry PiのUARTのポートのttyAMA0を指定すれば行けるはずです



MIDIインターフェースはUM-ONEmk2を使ってます

Windows上でTMIDIで演奏してloopMIDIとHairless MIDI<->Serial Bridgeで3.3VのUSB-Serialから出力してもいいです


tantanさんのFDイメージはここから
https://github.com/tantanGH/distribution/blob/main/x68000z/uart-midi-raspi.md

XM6typeGで使うときはZMUSICを選択してください

FDの中にはmzpが入っていてSMFやRCPのデータからZMUSIC用のデータZMDを生成できます
単に
mzp hoge.mid
とすると自動的にZMDに変換後演奏されます

mzp -O hoge.mid
とするとSMFやRCPのデータからZMDを生成します


ZMUSICのトラックバッファが少ないとmzpでの変換に失敗します
-Tオプションでトラックバッファの容量を指定できます
autoexec.batを編集して
ZMUSICRS.X
の部分を
ZMUSICRS.X  -T1024
としてください
-T1024でトラックバッファの容量を1MB確保します




追記：

シリアルからRS-MIDIのデータを受けてRaspberry Piに接続したUSB-RS232Cポートに出力してRS-MIDI対応MIDI音源を鳴らすプログラムの
raspberry_pi_serial_to_serial.cと

LANからRS-MIDIのデータを受けてRaspberry Piに接続したUSB-MIDIインターフェースに出力してMIDI音源を鳴らすプログラムの
raspberry_pi_rs-midi-wifi_to_midi_client.c

2つのプログラムを追加しました



raspberry_pi_rs-midi-wifi_to_midi_client.cのサーバプログラムは
https://github.com/yamayamaru/yamayamaru-storage-place/tree/main/source_code_2023-04-17
raspberry_pi_rs-midi-serial_to_wifi_server.c
になります

あらかじめサーバ側のSBCでraspberry_pi_rs-midi-serial_to_wifi_serverを立ち上げてから
raspberry_pi_rs-midi-wifi_to_midi_clientで接続します。
