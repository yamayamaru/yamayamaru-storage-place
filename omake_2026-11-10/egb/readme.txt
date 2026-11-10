GNU for TOWNSのグラフィックライブラリをclang用に移植したライブラリを使ったサンプルプログラムです

メキシカンハットのプログラムのhat05.cとリアルモードのBIOSコールのサンプルプログララムのtest02.cを用意しました

最初に、tbios-build.shを実行してlibt.aを作成してください

-I./newlib-4.4.0.20231231/newlib/libc/includeの部分はnewlibのヘッダーファイルを指定してます
ご自分の環境に合わせて変更してください

newlibを自前でビルドしてできたlibc.a、libm.a、libg.aを同じディレクトリに置いてください


コンパイル方法
bash hat05-build.sh

上記を実行してください
(hat05-build.shの中身を確認して
-I./newlib-4.4.0.20231231/newlib/libc/includeの部分や自分でビルドしたbinutilsのldやobjcopy、objdumpのパスの部分を
ご自分の環境に合わせて変更してください
ubuntu標準のldやobjdump、objcopyはa.out-i386に対応してないので自分でビルドしたbinutilsのものを使用してください)


できたhat05.binをFM TOWNSエミュレータの津軽に持っていき下記のようにbenexp3.expを実行します
GENEXP3.EXPのみ使えればいいのでTOWNS-gccを使えるようにインストールする必要はありません

run386 -nocrt f:\tgcc\usr\bin\genexp3 hat05.bin hat05.exp

f:\tgcc\usr\binはgenexp3.expへのパスです。ご自分の環境に合わせて変更してください)



test02.cの場合はgenexp3.expを実行する場合、-minreal、-maxrealの指定が必要です

run386 -nocrt f:\tgcc\usr\bin\genexp3 test02.bin test02.exp -minreal 512 -maxreal 512

f:\tgcc\usr\binはgenexp3.expへのパスです。ご自分の環境に合わせて変更してください)
-minrealや-maxrealはコンベンショナルメモリのMSDOS領域のメモリ確保する場合に指定が必要です
パラグラフ(16バイト)単位での指定で512を指定すると8192バイトまで確保可能になります

津軽で互換BIOSのみの環境でTOWNSメニューからgenexp3.expを実行したい場合はrun386から始まる上記を
test02.batという名前でテキストファイルに書き込み保存します
TOWNSメニュー上からtest02.batをダブルクリックすることでgenexp3.expでtest02.binをtest02.expに変換可能です
