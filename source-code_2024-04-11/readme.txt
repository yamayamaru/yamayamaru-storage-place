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
            portに指定したポート番号で待ち受け状態になるのでFM TOWNSエミュレータの津軽のRS232Cのところで  
            このプログラムを起動したWindowsパソコンのIPアドレスと指定したport番号を設定してください  
            例えば、WindowsパソコンのIPアドレスが192.168.1.10、port番号が4242なら津軽に下記のように設定します  
            192.16.1.10:4242  

            WindowsパソコンのIPアドレスはコマンドプロンプトからipconfigと入力すると表示されます


  終了方法
      CTRL+Cキーを押して終了させます


RS232Cの出力をcom0comというRS232Cの仮想ループアダプタを使って
Hairless MIDIに入力し、loopMIDIを使ってvsthostにつなぐことで
vsthost上のSoundCanvasVA鳴らすことができます

また、RS232Cの出力をMIDI音源のRSMIDI入力に入れることによりRSMIDI音源を鳴らすこともできます

RS232Cの出力をcom0comというRS232Cの仮想ループアダプタを使って
Hairless MIDIに入力して、Hairless MIDIのMIDI出力をMIDIインターフェースにすることで
MIDIインターフェースにつないだMIDI音源を鳴らすこともできます

FM TOWNSエミュレータの津軽でもWindowsでMIDI出力がサポートされたようですが  
出力先のMIDIインターフェースを自由に設定できないのと
SysExが出力されないようなので今でも津軽ではこのプログラムを使ってMIDIを鳴らしています



追記：
MIDI出力のみでなくMIDI入力にも対応したバージョンを作成してみました
MIDI入力可能なバージョンはsocket_to_serial3になります


  コンパイル方法(ビルドにはVisual StudioもしくはVisual Studio Build Toolsが必要です。
                                       コンパイル済みの実行ファイルも用意しました)
      cl /O2 socket_to_serial3.cpp

  実行方法(コマンドプロンプトで実行します)
      socket_to_serial3.exe serial-device-No.  port

  実行例
      socket_to_serial3.exe  5 4242

  終了方法
      CTRL+Cキーを押して終了させます



互換BIOSをそのまま使うとEUP Playerでファイル選択が正常にできないです
うんづのところのMKOSROMでMSDOS Ver3.1から作成したFMT_DOS.ROMを
互換BIOSのROMの中のFMT_DOS.ROMと差し替えることによりファイル選択が正常にできるようになります
ただし、MSDOS Ver3.1から作成したFMT_DOS.ROMを使うと今のところ、HDDからしか起動しなくなるようです
なのでもともとの互換BIOSを用途によって使い分けてください
うんづのところのMKOSROMでMSDOS Ver3.1から作成したFMT_DOS.ROMも一緒に置いておきます



これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
