FM TOWNS用のCコンパイラのHigh CとTurboAssemblerを使って
C言語とアセンブラを使った簡単なテストプログラムを作りました

TurboAssemblerでもHigh Cのリンカ、386LINKやTLINKでリンクできることがわかります

マンデルブロ集合を描画するプログラムですがマンデルブロ集合の判定をする関数をアセンブラで記述しました
適当に書いたものなので最適化はされてないです

コンパイル方法は下記になります
hc386 -v -c mand_asm.c -fsoft
tasm mand.asm,,mand.lst
run386 -nocrt f:\hc386\bin\386linkp  @mand_asm.lnk

f:\hc386\binの部分は自分の環境に合わせて変更してください
また、MAND_ASM.LNKの中のf:\hc386\small\na.libやf:\hc386\small\tbios.libも
ご自分の環境に合わせてパスを変更してください


アセンブラは無料のOpenWatcom C Ver2.0に付属するwasmでもアセンブルすることができます
アセンブル方法は下記になります

wasm -3 -fl=mand.lst -fo=mand.obj mand.asm


これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
