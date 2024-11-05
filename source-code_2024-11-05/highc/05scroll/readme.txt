Oh! FM 1991年1月号のP.152からP.153で解説されていた
0.5ドットスクロールをするためのサンプルプログラムです
scroll.cが掲載されていたプログラムです

scroll2.c, scroll3.cは私が少し改変したものです
FM TOWNSエミュレータの津軽でしか動作確認していません

0.5ドットスクロールとなってますがFM TOWNSエミュレータの津軽では1ドット単位のスクロールになります

コンパイル方法
コンパイルするのにFM TOWNS用のHigh Cが必要です

scroll.lnk, scroll2.lnk, scroll3.lnkの中のf:\hc386\small\の部分はご自分の環境に合わせて変更してください

scroll.bat, scroll2.bat, scroll3.batを実行してください
(scroll.bat, scroll2.bat, scroll3.batの中のf:\hc386\bin\の部分はご自分の環境に合わせて変更してください
また、386LINKが無いHIGH Cの場合はTLINKを使ってください)

TurboAssemblerを使ってますがTurboAssemblerがない場合は無料のOpenWatcom C Ver2.0のWindows用のクロス開発版に付属のwasmでアセンブル可能です  
アセンブル方法は下記のようになります  
wasm -3 -fo=io.obj io.asm


実行方法

run386 scroll

run386 scroll2

run386 scroll3
