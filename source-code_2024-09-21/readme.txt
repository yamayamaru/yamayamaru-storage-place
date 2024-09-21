Raspberry Pi Pico2のRISC-Vコアでクロック数計測を
どうやってやればいいのか調べたところできるようになりました

riscv-privilegedのマニュアルを確認したところ
CSRレジスタのうち、mcounterenレジスタのbit1をセットすることで
カウンタを有効にすることができるようです

ただ、CSRレジスタのmcountinhibitレジスタのbit1が1になっているとカウントアップされないので
mcounterenレジスタのbit1をクリアします

インラインアセンブラでこんな感じでできました

    asm volatile (
        "csrr t0, mcountinhibit\n\t"
        "li   t1, 1\n\t"
        "not  t1, t1\n\t"
        "and  t0, t0, t1\n\t"
        "csrw mcountinhibit, t0\n\t"
        "csrr t0, mcounteren\n\t"
        "ori  t0, t0, 1\n\t"
        "csrw mcounteren, t0\n\t"
        :
        :
        :"t0", "t1"
    );

あとは、rdcycle疑似命令を使ってカウンタの値を読み取るだけです

このやり方で正しいのかはわからないので試す方は自己責任で行ってください

これらのプログラムの使用に当たってはご自分の責任において使用してください
これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
