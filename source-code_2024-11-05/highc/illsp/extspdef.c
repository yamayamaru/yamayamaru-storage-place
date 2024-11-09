/*
 *    スプライト８９３定義関数
 *
 *    注）ｘｘが使われていないというワーニングは無視してください。
 *        パラメータの範囲チェックはありません。
 */

extern SPRamPokeB(), SPRamPokeW(), SPRamPeekB(), SPRamPeekW();

void ESPR_define(int iro, int topPat, int xNo, int yNo, char *pat)
{
    unsigned int Saddr;
    int num, i;

    Saddr = topPat * 128;
    num = xNo * yNo * 128;
    if (iro == 1) num = num * 4;

    for (i = 0; i < num; i++, pat++) {
        SPRamPokeB(Saddr + i, *pat);
    }
}

void ESPR_setPaletteBlock(int topPal, int num, short *pal)
{
    unsigned int Saddr;
    int i;

    Saddr = topPal * 32;
    for (i = 0; i < 16 * num; i++, pal++) {
        SPRamPokeW(Saddr + i * 2, *pal);
    }

}

void ESPR_setAttribute(int topPat, int xNo, int yNo, int att, int colTable)
{
    unsigned int Saddr;
    Saddr = topPat * 8;
    SPRamPokeW(Saddr + 4, att);
    SPRamPokeW(Saddr + 6, colTable);
}

void ESPR_readAttribute(int topPat, int xNo, int yNo, char *addr)
{
    unsigned int Saddr;
    int i;

    Saddr = topPat * 8;
    for (i = 0; i < xNo * yNo * 8; i++, addr++) {
        *addr = SPRamPeekB(Saddr + i);
    }
}
