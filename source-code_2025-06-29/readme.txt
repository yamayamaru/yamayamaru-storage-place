Oh! FM 1990年7月号のp88からp96に若松登志樹さんのπの計算プログラムが掲載されています

このπの計算プログラムはRUN386を使ったMSDOS用のオールアセンブラで記述されているのですが
Windows10やWindows11でも動くようにしてみました

ソースコードはVisual Studio Community用です
Visual Studio Community 2019でコンパイルできることを確認してます



コンパイル方法

32bit版
ml /c pimil.asm
cl /O2 /F1048576 /Fe:pimil pimil_main.c pimil.obj


x64版
ml64 /c /Fl pimil.asm
cl /O2 /F1048576 /Fe:pimil pimil_main.c pimil.obj
