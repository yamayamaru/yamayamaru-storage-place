HI-TECH C Ver 3.09に
MSX-C Library互換ライブラリ for HI-TECH Cを使って
メキシカンハットのプログラムを移植してみました


コンパイルにはHI-TECH C Ver 3.09と
MSX-C Library互換ライブラリ for HI-TECH CのGLIB.H、MSXBIOS.H、LIBM.LIBが必要です
HI-TECH C Ver 3.09ファイルととGLIB.H、MSXBIOS.H、LIBM.LIBを同じ場所に置きます
msxalib.h, msxalib.asm, msxclib.h, msxclib.c, msxc_def.h, hat02.cのこれらのファイルも同じ場所に置きます


コンパイルはCP/M program EXEcutor for Win32の環境でコンパイルし、
blueMSXで動作確認しました


コンパイル方法

c -V -C hat02.c
c -V -C msxclib.c
ZAS -J -N -omsxalib.OBJ msxalib.asm
c -V hat02.c msxclib.obj msxalib.obj LIBM.LIB -LF 




CP/M program EXEcutor for Win32環境ではこんな感じでコンパイルします

cpm c -V -C hat02.c
cpm c -V -C msxclib.c
cpm ZAS -J -N -omsxalib.OBJ msxalib.asm
cpm c -V hat02.c msxclib.obj msxalib.obj LIBM.LIB -LF 



これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。


HI-TECH C Ver3.09
http://web.archive.org/web/20040414090318/http://www.htsoft.com/software/cpm/index.html

MSX-C Library互換ライブラリ for HI-TECH C
https://www.vector.co.jp/soft/other/msx/se148476.html

CP/M program EXEcutor for Win32
https://www.vector.co.jp/soft/win95/util/se378130.html

HI-TECH Cを使ってみた
https://qiita.com/Stosstruppe/items/ffc96fe921a17061a4e5
