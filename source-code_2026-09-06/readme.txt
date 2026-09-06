ChatGPTでインベーダー風ゲームの生成を試してみました

「C言語で架空のグラフィック関数(DrawPixel(int x, int y, int color)、
fillrect(int x1, int y1, int x2, int y2, int color)、draw_image(int x, int y, char *data))などを使ってスペースインベーダー風のゲームを作って」
と指示して生成してもらいました

指示してないが、必要な関数key_pressed(key)、wait_vsync()も生成してくれました
/*
 * 架空のグラフィック・入力関数
 *
 * DrawPixel(x, y, color);
 * fillrect(x1, y1, x2, y2, color);
 * draw_image(x, y, data);
 * clear_screen(color);
 * key_pressed(key);
 * wait_vsync();
 */
 
 最初に生成されたコードはキャラクタの描画はfillrect()で行い、バリアもないものでした
 最初にAIが生成したC言語のソースプログラムは693行ありました
 
 それをFM TOWNSで自分で移植しました
  
 何度か指示をだして完成させました
 
 ここのプログラムは最初、FM TOWNS用のHigh C用に作り、完成した後、GNU for TOWNS Rel2のgcc用に移植したものです
 
 ESCキーを押すとプログラムは終了します
 
 
 GNU for TOWNS Rel2でのコンパイル方法
 
set PATH=q:\;q:\usr\bin;q:\gnuish\futil14;%PATH%
set gccroot=q:

GNU for TOWNS Rel2のCDROMをセットします
上記のようにq:\、q:\user\bin、q:\gnuish\futil14にパスを通す
環境変数gccrootにq:を設定

q:\usr\bin\gcle invader q:\usr\lib\libt.a

上記のようにBATファイルを実行します




インターネットで公開されているgccでもコンパイルできると思います
https://anikun.kutami.jp/towns-gcc/


インターネットに公開されているgccでビルド方法

set GCC=i:/gnugcc/usr/bin
set GINC=i:/gnugcc/usr/include
set GLIB=i:/gnugcc/usr/lib
PATH=%PATH%;i:\gnugcc\usr\bin
set gccroot=i:/gnugcc

上記のように環境変数を設定(i:\GNUGCCがある場合を想定してます、ドライブ番号はご自分の環境に合わせて設定してください)

下記のようにしてコンパイルします
set DOSX=-nocrt
gcc -O -o invader invader.c -lt
set DOSX=

ここでDOSXに-nocrtを設定しているのはエラーメッセージを読みやすいように画面をリセットしない設定をRUN386に指示してます



実行方法
run386 -nocrt invader


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。




追記
FMT_DOS.ROMはうんづのところのmkosrom.exeを使って
FM TOWNS用のMSDOS Ver3.1から作ったものです
FM TOWNSエミュレータの津軽やうんづで
津軽の互換ROMセットと使うことができます
(FMT_DOS.ROMを差し替えて使います。
ただし、差し替えるとHDDからしか起動できなくなるため、
TOWNSシステムソフトウェアV2.1が必須になります)
http://ysflight.in.coocan.jp/FM/towns/FreeTOWNS/ROMS.zip

今のところこのROMイメージを使わないで互換BIOSだけを使うと
OpenWatcom C Ver2.0でprintf()、sprintf(), fprintf()などを使うとプログラムがハングします
MSDOS 3.1から作ったFMT_DOS.ROMを差し替えるとprintf()、sprintf(), fprintf()を使っても
作成したプログラムがハングしなくなります
OpenWatcom C Ver2.0で作成したプログラムを津軽で動作させるときに
プログラムによっては80387をオンにしてるとハングする場合があるのでそのときは80387をオフにしてください
FM TOWNS用のgccであるTOWNS-gppもこのROMイメージを使わないで互換BIOSだけを使うとうまく動作しないようです
実機のBIOSであれば特に差し替える必要はありません

