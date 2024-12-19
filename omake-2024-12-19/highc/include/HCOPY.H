/***************************************************************/
/*                                                             */
/* HCOPY.H                                                     */
/*            COPYRIGHT (C) 1994 FUJITSU LIMITED               */
/*                                                             */
/***************************************************************/

#ifndef _HCOPY_DEFINED_

#define _HCOPY_DEFINED_
/************************/
/* ハードコピーの開始用 */
/************************/
#define	HS_PAGE0	0x00000001	/*	ページ0を出力						*/
#define	HS_PAGE1	0x00000002	/*	ページ1を出力						*/
#define	HS_PAGE2	0x00000004	/*	ページ2を出力(Full Color VRAM Page0)*/
#define	HS_PAGE3	0x00000008	/*	ページ3を出力(Full Color VRAM Page1)*/
#define	HS_DSPPAGE	0x00000000	/*	指定ページの内表示ページを出力	 	*/
#define	HS_APPPAGE	0x00000080	/*	表示ページに関係なく指定ページを出力*/

#define	HS_PG0(x)	((x & 0x0000008f) | HS_PAGE0)
#define	HS_PG1(x)	((x & 0x0000008f) | HS_PAGE1)
#define	HS_PG2(x)	((x & 0x0000008f) | HS_PAGE2)
#define	HS_PG3(x)	((x & 0x0000008f) | HS_PAGE3)
#define	HS_PGP(x)	((x & 0x00000080) | HS_PAGE0 | HS_PAGE1 | HS_PAGE2 | HS_PAGE3)

#define	HS_PRN		0x00000000		/*	プリンタへ出力					*/
#define	HS_FILE		0x00000001		/*	ファイルへ出力					*/
#define	HS_IPU		0x00000002		/*	カラー複写機へ出力				*/

#define	HS_OUTMODEOFF	0x00000000	/*	出力モードの指定無効			*/
#define	HS_OUTMODEON	0x00000100	/*	出力モードの指定有効			*/

/************************/
/* 出力モード設定用		*/
/************************/
#define	HO_SETVIEW		0x80		/*	範囲指定をする					*/
#define	HO_DEFVIEW		0x00		/*	範囲指定をしない				*/
#define	HO_SETPAGE0ZOOM	0x20		/*	ページ0の拡大倍率を指定する		*/
#define	HO_DEFPAGE0ZOOM	0x00		/*	ページ0の拡大倍率を指定しない	*/
#define	HO_SETPAGE1ZOOM	0x40		/*	ページ1の拡大倍率を指定する		*/
#define	HO_DEFPAGE1ZOOM	0x00		/*	ページ1の拡大倍率を指定しない	*/
#define	HO_SETPAGE2ZOOM	0x08		/*	ページ2の拡大倍率を指定する		*/
#define	HO_DEFPAGE2ZOOM	0x00		/*	ページ2の拡大倍率を指定しない	*/
#define	HO_SETPAGE3ZOOM	0x10		/*	ページ3の拡大倍率を指定する		*/
#define	HO_DEFPAGE3ZOOM	0x00		/*	ページ3の拡大倍率を指定しない	*/
/**********************/
/* ハードコピーの開始 */
/**********************/
extern int	HCP_start(int, int, char*) ;

/**********************/
/* ハードコピーの中止 */
/**********************/
extern int	HCP_stop(void) ;

/**********************/
/* 印刷開始位置の設定 */
/**********************/
extern int	HCP_setPrnStart(int, int) ;

/**********************/
/* 出力モードの設定   */
/**********************/
extern int	HCP_setOutMode(char*) ;

/**************/
/* エラー定義 */
/**************/
#define HCP_ENOERR	0x00	/* 正常終了									*/
#define HCP_EUNSEL	0x02	/* プリンタポート指定の誤り					*/
#define HCP_EPAPER	0x05	/* プリンタの用紙切れ						*/
#define HCP_EUNMOV	0x06	/* プリンタが動作状態でない					*/
#define HCP_EPTMOV	0x07	/* プリンタに動作エラーが発生した			*/
#define HCP_EPAGE	0x12	/* ページ指定の誤り							*/
#define HCP_EBOUND	0x13	/* 範囲指定の誤り							*/
#define HCP_EZOOM	0x14	/* 拡大指定の誤り							*/
#define HCP_EOUT	0x15	/* 出力指定の誤り							*/
#define HCP_EMODE	0x17	/* 出力モード指定の誤り						*/
#define HCP_EPATH	0x18	/* パス名の誤り								*/
#define HCP_EFOUT	0x19	/* ファイル出力時のエラー					*/
#define HCP_EHCMOV	0x1A	/* ハードコピー動作中						*/
#define HCP_ENOMAP	0x1B	/* ＶＲＡＭが使用出来ない					*/
#define HCP_ENOSPT	0x1C	/* 未サポート解像度							*/
#define HCP_ENOPG	0x1D	/* 指定ページが存在しない					*/
#define HCP_EDSP	0x1E	/* 指定ページが表示していない				*/
#define HCP_EBREAK	0x1F	/* 強制終了ファンクションにより中断された	*/
#define HCP_ENINST	0x80	/* 出力指定されたハードコピーが常駐されてない*/

#endif
