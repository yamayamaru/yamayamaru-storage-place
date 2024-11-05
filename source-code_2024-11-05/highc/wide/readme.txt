Oh! FM 1991年1月号のP.150からP.152で解説されていた
スプライト画面や背景を横長にするサンプルプログラムです


コンパイル方法
コンパイルするのにFM TOWNS用のHigh Cが必要です

wide.lnkの中のf:\hc386\small\の部分はご自分の環境に合わせて変更してください

wide.batを実行してください
(wide.batの中のf:\hc386\bin\の部分はご自分の環境に合わせて変更してください
また、386LINKが無いHIGH Cの場合はTLINKを使ってください)

TurboAssemblerを使ってますがTurboAssemblerがない場合は無料のOpenWatcom C Ver2.0のWindows用のクロス開発版に付属のwasmでアセンブル可能です  
アセンブル方法は下記のようになります  
wasm -3 -fo=io.obj io.asm

実行方法

run386 wide パラメーター

パラメータは0から5の数字になります
