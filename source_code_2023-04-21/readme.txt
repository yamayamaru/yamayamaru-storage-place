このプログラムはシリアルポートからRS-MIDIのデータを受け取って
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
