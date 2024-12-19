/***************************************************************/
/*                                                             */
/* XLD.H                                                       */
/*                                                             */
/*   ダイナミックリンクサポートライブラリヘッダファイル        */
/*                                                             */
/*            Copyright (c) 1992,1994 FUJITSU LIMITED.         */
/*            All rights reserved.                             */
/*                                                             */
/***************************************************************/

#ifndef	_XLD_H
#define	_XLD_H

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef unsigned int size_t;
#endif

/*
**	type define
*/
typedef	struct {
	unsigned long 	headerID; 		// header ID ("DLL0")
	unsigned short	minorVer;		// minor version
	unsigned short	majorVer;		// major version
	unsigned long	userRsv;		// reserve area for user
	unsigned long	sysRsv;			// reserve area for system
} stLibHeader;						// DLL header

/*
**	functions
*/
extern void		*_XLD_init(void *pRsc);
extern void		_XLD_setMemFunc(void *(*malloc)(size_t), void (*free)(void *));
extern void		*_XLD_callMalloc(size_t size);
extern void		_XLD_callFree(void *ptr);
extern int		_XLD_setLinkPath(const char *s);
extern char		*_XLD_getLinkPath(char *s);
extern char		*_XLD_getLinkPath2(char *s);
extern void		_XLD_load();
extern size_t	_XLD_unlink();
extern int		_XLD_isLinked();
extern int		_XLD_lockLib();
extern int		_XLD_unlockLib();
extern int		_XLD_getError(void);
extern void		_XLD_setError(int);
extern void		_XLD_clearError(void);
extern void		_XLD_debugStart(int mode);
extern void		_XLD_debugEnd(void);
extern void		_XLD_setErrorFunc(int (*errFunc)());
extern int		(*_XLD_getErrorFunc(void))();
extern int		_XLD_getLibHeader(char *moduleName, stLibHeader *libHeader);
extern int		_XLD_loadLibrary(char *moduleName);
extern int		_XLD_freeLibrary(int handle);
extern void		*_XLD_getProcAddress(int handle, char *libName);

/*
**	Error information
*/
#define _XLD_ERR_OPENLIB	  (-1)	// Can't open DLL
#define _XLD_ERR_MALLOC		  (-2)	// Can't allocate memory for DLL
#define _XLD_ERR_LOADLIB	  (-3)	// Can't load DLL
#define _XLD_ERR_NOEXTRSC	  (-4)	// External resource not found
#define _XLD_ERR_NOLIB		  (-5)	// Specified library not found
#define _XLD_ERR_LOCKED		  (-6)	// Library locked
#define _XLD_ERR_FREEMEM	  (-7)	// Can't release sufficient memory
#define _XLD_ERR_TOOLONGPATH  (-8)	// Pathname too long
#define _XLD_ERR_NOLIBINMAIN  (-9)	// Specified library not found in main
#define _XLD_ERR_FREELIB	 (-10)	// Specified library can't be released
#define _XLD_ERR_NOTLINKED	 (-11)	// Specified Library not linked
#define _XLD_ERR_ILLPARM	 (-12)	// Illegal parameter
#define _XLD_ERR_NOHEADER	 (-13)	// DLL header not found

/*
**	Error handler status
*/
#define _XLD_CALL_CONTINUE	   (0)	// Continue function call
#define _XLD_CALL_RETRY		  (-1)	// Retry function call
#define	_XLD_CALL_SYSRETRY	  (-2)	// SHELLSystem call(for SHELL application)
#define	_XLD_CALL_HALT		  (-3)	// Can't continue  (for SHELL application)

#endif /* _XLD_H */
