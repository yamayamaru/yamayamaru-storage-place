High C V1.7L13に付属のForRBIOS用のFMCFRBライブラリです

OpenWatcom C Ver2.0付属のwdisで逆アセンブルする場合はオブジェクトファイルを取り出す必要があります


ライブラリに含まれているオブジェクトファイルのリストを出すには
wlib FMCFRB.LIB  > list.txt

オブジェクトファイルを取り出すには(取り出したいオブジェクトファイルの前に*アスタリスクを付ける)
wlib FMCFRB.LIB *取り出したいオブジェクトファイル名

例) wlib FMCFRB.LIB *f8e00-1


リスティングファイル出力例
wdis f8e00-1.obj

アセンブルソース出力例
wdis -a f8e00-1.obj
