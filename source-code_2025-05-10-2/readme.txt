super aki-80用のプログラムをZ80用のCコンパイラのsdcc-4.2.0で作れるか
試してみたところ作れました

プログラムは秋月電子で販売されているAKI-80用のROMをSuper AKI-80に装着した
Super AKI-80のRAM上で動きます

マンデルブロ集合を描くプログラムを作ってみました


mandel_super_aki80_sio1_loop2.ihx      コンパイル済みのインテルHEXファイル

mandel_super_aki80_sio1_loop2.c        メインプログラムのソース
mandel_super_aki80_sio1_loop2_asm.asm  簡単な補助的な関数のアセンブラのソース
mycrt0_2_super_aki80.asm               sdcc-4.2.0を使うためのスタートアップルーチン




自分でビルドしたい方は下記の方法でビルドできます

sdcc -mz80 -c -o mandel_super_aki80_sio1_loop2.rel mandel_super_aki80_sio1_loop2.c
sdasz80 -o mycrt0_2_super_aki80.rel mycrt0_2_super_aki80.asm
sdasz80 -o mandel_super_aki80_sio1_loop2_asm.rel mandel_super_aki80_sio1_loop2_asm.asm
sdcc -mz80 --code-loc 0x8000 --data-loc 0 --no-std-crt0 -o mandel_super_aki80_sio1_loop2.ihx mycrt0_2_super_aki80.rel  mandel_super_aki80_sio1_loop2.rel  mandel_super_aki80_sio1_loop2_asm.rel

簡単にビルドできるようにmandel_super_aki80_sio1_loop2-build.batを用意しました
ファイルの中で環境変数SDCC_HOMEにSDCCをインストールしたフォルダのパスを設定してください
あとはコマンドプロンプトでmandel_super_aki80_sio1_loop2-build.batを実行するだけです



プログラムの実行方法
秋月電子でAKI-80用のROMをSuper AKI-80に挿して
パソコンにつないだUSB-SerialのアダプタにAKI-80のシリアルポートをつなぎます
パソコンではTeratermを使ってシリアルスピードを9600bpsにして待機します

AKI-80に電源を入れてAKI-80をリセットします
@と表示されたらROMのモニタが起動した状態です
Teratermのメニューの設定→端末の画面で
改行コードの送信をLFにしてローカルエコーにチェックを付けます

この状態で何かキーを押すと
RS
と表示されます

LHと入力してEnterキーを押します
$と表示されるのでTeratermのメニューのファイル→ファイル送信で
mandel_super_aki80_sio1_loop2.ihxを送信します
送信が終わると@が表示されます
一度Enterキーを押します

GO8000と入力してEnterキーを押すとプログラムの実行が開始されます






SDCC-4.2.0のインストール方法

sdccはここからダウンロードできます
https://sourceforge.net/projects/sdcc/files/

Windows x64版のsdcc-4.2.0はここです
https://sourceforge.net/projects/sdcc/files/sdcc-win64/4.2.0/
ここのsdcc-4.2.0-x64-setup.exeをダウンロードしてください


署名がないので実行するとWindowsによってPCが保護されましたと出ます
詳細情報をクリックして実行をクリックします

ユーザアカウント制御の画面が出るので「はい」をクリックします




コンパイル時は下記のようにSDCCのコンパイルに必要な環境変数の設定が必要です

環境変数SDCC_HOMEにはSDCCをインストールしたフォルダのパスを指定します
デフォルトではC:\Program Files\SDCCにインストールされます

set SDCC_HOME="C:\Program Files\SDCC"
set SDCC_INCLUDE=%SDCC_HOME%\include
set PATH=%SDCC%\bin;%PATH%



これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
