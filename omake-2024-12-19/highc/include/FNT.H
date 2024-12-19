/***************************************************************/
/*                                                             */
/* FNT.H                                                       */
/*            COPYRIGHT (C) 1994 FUJITSU LIMITED               */
/*                                                             */
/***************************************************************/

/* ANKのアドレス */
int FNT_ankAddr(int sizeX,int sizeY,int ank,int *seg,int *off);
/* ANKのコピー */
int FNT_ankRead(int sizeX,int sizeY,int ank,int seg,char *off);
/* KANJIのアドレス */
int FNT_kanjiAddr(int sizeX,int sizeY,int ank,int *seg,int *off);
/* KANJIのコピー */
int FNT_kanjiRead(int sizeX,int sizeY,int ank,int seg,char *off);
/* Sjis to jis */
int FNT_sjisToJis(int code);
/* Sjis to jis */
int FNT_jisToSjis(int code);
