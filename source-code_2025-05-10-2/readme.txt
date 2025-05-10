super aki-80用のプログラムをZ80用のCコンパイラのsdcc-4.2.0で作れるか
試してみたところ作れました

プログラムは秋月電子で販売されているAKI-80用のROMをSuper AKI-80に装着した
Super AKI-80のRAM上で動きます

マンデルブロ集合を描くプログラムを作ってみました


mandel_super_aki80_sio1_loop2.ihx      コンパイル済みのインテルHEXファイル

mandel_super_aki80_sio1_loop2.c        メインプログラムのソース
mandel_super_aki80_sio1_loop2_asm.asm  補助的なアセンブラのソース
mycrt0_2_super_aki80.asm               sdcc-4.2.0を使うためのスタートアップルーチン




自分でビルドしたい方は下記の方法でビルドできます

sdcc -mz80 -c -o mandel_super_aki80_sio1_loop2.rel mandel_super_aki80_sio1_loop2.c
sdasz80 -o mycrt0_2_super_aki80.rel mycrt0_2_super_aki80.asm
sdasz80 -o mandel_super_aki80_sio1_loop2_asm.rel mandel_super_aki80_sio1_loop2_asm.asm
sdcc -mz80 --code-loc 0x8000 --data-loc 0 --no-std-crt0 -o mandel_super_aki80_sio1_loop2.ihx mycrt0_2_super_aki80.rel  mandel_super_aki80_sio1_loop2.rel  mandel_super_aki80_sio1_loop2_asm.rel





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
