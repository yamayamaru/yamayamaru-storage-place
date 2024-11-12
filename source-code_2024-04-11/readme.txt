FM TOWNSエミュレータの津軽にはRS232Cの出力を指定したIPアドレスのネットワークに飛ばすことができます
このプログラムはFM TOWNSエミュレータの津軽でRS232Cのデータをネットワークに飛ばしたものを
このプログラムで受けてデータをそのままRS232Cインターフェースへ出力するものです
今回はWindowsのコマンドプロンプト上で動くものを作ってみました

  コンパイル方法(ビルドにはVisual StudioもしくはVisual Studio Build Toolsが必要です。
                                       コンパイル済みの実行ファイルも用意しました)
      cl /O2 socket_to_serial.cpp

  実行方法(コマンドプロンプトで実行します)
      socket_to_serial.exe serial-device-No.  port

  実行例
      socket_to_serial.exe  5 4242

      注意: 5の部分はご自分のRS232Cインターフェースのデバイスを指定してください  
            (COM5なら数値の5をCOM4なら数値の4を指定します)

            このプログラムを実行してから津軽を起動してください



RS232Cの出力をcom0comというRS232Cの仮想ループアダプタを使って
Hairless MIDIに入力し、loopMIDIを使ってvsthostにつなぐことで
vsthost上のSoundCanvasVA鳴らすことができます

また、RS232Cの出力をMIDI音源のRSMIDI入力に入れることによりRSMIDI音源を鳴らすこともできます

RS232Cの出力をcom0comというRS232Cの仮想ループアダプタを使って
Hairless MIDIに入力して、Hairless MIDIのMIDI出力をMIDIインターフェースにすることで
MIDIインターフェースにつないだMIDI音源を鳴らすこともできます


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
