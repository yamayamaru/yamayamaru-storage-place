/************************************************/
/*                                              */
/* FNTB.H      COPYRIGHT FUJITSU LIMITED        */
/*                  1991                        */
/*                                              */
/************************************************/

#define FNTDEFAULT	0
#define MIN12		1
#define MIN16		2
#define MIN24		3
#define GOTH24		4
#define KYOU24		5
#define MARU24		6
#define MINN32		7
#define GOTH32		8
#define MINN48		9
#define GOTH48		10
#define MINN60		11
#define GOTH60		12
#define MOUH48		13

#define VCTMIN		16
#define VCTGOTH		17
#define VCTKYO		18
#define VCTKAI		19
#define VCTMARU		20
#define VCTGYO		21
#define VCTHIK		22
#define VCTMARUG	23

typedef struct {
    short int fontId;		/* 文字種 */
    short int sizeX;		/* 文字サイズX */
    short int sizeY;		/* 文字サイズY */
    short int attribute;	/* 文字スタイル */
    short int space;		/* 文字間隔 */
    short int margin;		/* 文字列調整幅 */
} FONTTYPE;

typedef struct {
    unsigned char     *offset;	/* データ格納領域オフセット */
    unsigned short int segment; /* データ領域セグメント */
    short int          rowBytes;/* データ横サイズ */
    short int          sizeX;	/* データ横ドット数 */
    short int          sizeY;	/* データ縦ドット数 */
} BITMAP;

#ifndef FNTB_H

int FNT_init() ;
int FNT_end() ;

int FNT_readString(char *, BITMAP *, Rect *, FONTTYPE *);
int FNT_readOffsetString(char *, int, int, BITMAP *, Rect *, FONTTYPE *);
int FNT_stringSize(char *, BITMAP *, FONTTYPE *);
int FNT_offsetStringSize(char *, int, int, BITMAP *, FONTTYPE *);

int FNT_setMemoryFunc(void *(*)(), void (*)()) ;
int FNT_getMemoryFunc(void *(**)(), void (**)()) ;

int FNT_setGetFntPatternFunc(int (*)(int, int, BITMAP *), 
							int (*)(), int (*)()) ;
int FNT_getGetFntPatternFunc(int (**)(int, int, BITMAP *), 
							int (**)(), int (**)()) ;

char *FNT_setFontPath(char *) ;

int FNT_SetUpFontFile(void) ;

/* フォント管理(FTM)の各ルーチンの登録 */
extern	int	FNT_SetUpFontFTM() ;
/* フォント管理ワーク領域のアドレスを取得 */
extern	int	FNT_GetFTMWorkPtr(char **) ;

#endif

#define FNT_ISPRINT		0x01
#define FNT_ISUPPER		0x02
#define FNT_ISLOWER		0x04
#define FNT_ISDIGIT		0x08
#define FNT_ISXDIGIT	0x10
#define FNT_ISKANA		0x20
#define FNT_ISKANJI		0x40
#define FNT_ISKANJI2	0x80

extern unsigned char	FNT_isTable[256] ;

#define FNT_isprint(c)  (FNT_isTable[(c)] & FNT_ISPRINT)
#define FNT_isupper(c)  (FNT_isTable[(c)] & FNT_ISUPPER)
#define FNT_islower(c)  (FNT_isTable[(c)] & FNT_ISLOWER)
#define FNT_isdigit(c)  (FNT_isTable[(c)] & FNT_ISDIGIT)
#define FNT_isxdigit(c) (FNT_isTable[(c)] & FNT_ISXDIGIT)
#define FNT_iskana(c)   (FNT_isTable[(c)] & FNT_ISKANA)
#define FNT_iskanji(c)  (FNT_isTable[(c)] & FNT_ISKANJI)
#define FNT_iskanji2(c) (FNT_isTable[(c)] & FNT_ISKANJI2)
#define FNT_isalpha(c)  (FNT_isTable[(c)] & (FNT_ISUPPER|FNT_ISLOWER))
#define FNT_isalnum(c)  (FNT_isTable[(c)] & \
                            (FNT_ISUPPER|FNT_ISLOWER|FNT_ISDIGIT))
#define FNT_isalkana(c) (FNT_isTable[(c)] & \
                            (FNT_ISUPPER|FNT_ISLOWER|FNT_ISKANA))
#define FNT_isalnmkana(c) (FNT_isTable[(c)] & \
                            (FNT_ISUPPER|FNT_ISLOWER|FNT_ISDIGIT|FNT_ISKANA))
#define FNT_isprkana(c) (FNT_isTable[(c)] & \
                            (FNT_ISPRINT|FNT_ISKANA))

#define FNT_toupper(c)	(FNT_islower((c)) ? ((c) - 'a' + 'A') : (c))
#define FNT_tolower(c)	(FNT_isupper((c)) ? ((c) - 'A' + 'a') : (c))

