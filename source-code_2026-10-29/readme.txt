Watcom C/C++ 10.5JでWindows3.1上で動作する32bitのプログラムが作成できるwin386を試してみました

サンプルプログラムとして以前作ってあったWindows3.1用のメキシカンハットのプログラムをwin386に移植してみました
移植といってもWinProcのfar宣言を取っただけです
OpenWatcom C/C++ Ver2.0でもコンパイルすることが可能です


コンパイル方法
違う点はwpp386に-bt=win386を付けます
wlinkではsystem win386を指定します
最期にwbindを実行することでwlinkが生成した
hat05p_w.rexとwin386.extが組み込まれてexeファイルが作成されます

wpp386 -bt=win386 -3s -zW -zq -zk0 -ox -fpi87 -fo=hat05p_w.obj hat05p_w.cpp

wlink system win386 option quiet,STACK=256k name hat05p_w file{ hat05p_w.obj }

wbind -n hat05p_w

これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
