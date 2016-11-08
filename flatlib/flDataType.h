// 2005 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#ifndef	FL_FLDATATYPE_H_
#define	FL_FLDATATYPE_H_

//-----------------------------------------------------------------------------
// Win32 x64
//-----------------------------------------------------------------------------
#if _flWIN64
typedef		_int8				SI8;
typedef		_int16				SI16;
typedef		_int32				SI32;
typedef		_int64				SI64;
typedef		unsigned _int8		UI8;
typedef		unsigned _int16		UI16;
typedef		unsigned _int32		UI32;
typedef		unsigned _int64		UI64;
//# define	FSIZE_TO_MSIZE32(n)	(static_cast<UI32>(n))
//# define	FSIZE_TO_MSIZE(n)	(n)
# define	_flFASTCALL
# define	_flCDECL		__cdecl
# define	_flSTDCALL		__stdcall
# define	_flINLINE		inline
# define	_flSINLINE		inline
# define	_flNOINLINE		__declspec(noinline)
# define	_flNAKED
# define	_flNORETURN		__declspec(noreturn)
# define	_flTHREAD		__declspec(thread)
# define	_flALIGN128		__declspec(align(128))
# define	_flALIGN64		__declspec(align(64))
# define	_flALIGN32		__declspec(align(32))
# define	_flALIGN16		__declspec(align(16))
# define	_flALIGN8		__declspec(align(8))
# define	_flALIGN4		__declspec(align(4))
# define	_flRESTRICT		__restrict
# define	_flFNCRESTRICT	__declspec(restrict)
# define	_flNOALIAS		__declspec(noalias)
# define	_flDEPRECATE	__declspec(deprecated)
# define	_flASSUME( n )	__assume( n )
# define	_flSYSALIGNSIZE		8
# define	flOVERRIDE		override
#endif


//-----------------------------------------------------------------------------
// Win32 x86
//-----------------------------------------------------------------------------
#if _flWIN32 && !_flWIN64
typedef		_int8				SI8;
typedef		_int16				SI16;
typedef		_int32				SI32;
typedef		_int64				SI64;
typedef		unsigned _int8		UI8;
typedef		unsigned _int16		UI16;
typedef		unsigned _int32		UI32;
typedef		unsigned _int64		UI64;
#if _flWINCE
//typedef		UI32				FSIZE_T;
//# define	FSIZE_TO_MSIZE(n)	(n)
#else
//typedef		UI64				FSIZE_T;
//# define	FSIZE_TO_MSIZE32(n)	(static_cast<UI32>(n))
//# define	FSIZE_TO_MSIZE(n)	FSIZE_TO_MSIZE32(n)
#endif
//typedef		unsigned int		LOOP_T;
# define	_flFASTCALL
# define	_flCDECL		__cdecl
# define	_flSTDCALL		__stdcall
# define	_flINLINE		inline
# define	_flSINLINE		inline
# define	_flNOINLINE		__declspec(noinline)
# define	_flNAKED		__declspec(naked)
# define	_flNORETURN		__declspec(noreturn)
# define	_flTHREAD		__declspec(thread)
# define	_flALIGN128		__declspec(align(128))
# define	_flALIGN64		__declspec(align(64))
# define	_flALIGN32		__declspec(align(32))
# define	_flALIGN16		__declspec(align(16))
# define	_flALIGN8		__declspec(align(8))
# define	_flALIGN4		__declspec(align(4))
# if _flVC8 || _flVC9
#  define	_flRESTRICT		__restrict
#  define	_flFNCRESTRICT	__declspec(restrict)
#  define	_flNOALIAS		__declspec(noalias)
#  define	_flDEPRECATE	__declspec(deprecated)
# define	_flASSUME( n )	__assume( n )
# else
#  define	_flRESTRICT
#  define	_flFNCRESTRICT
#  define	_flNOALIAS
#  define	_flDEPRECATE
# define	_flASSUME( n )
# endif
# define	_flSYSALIGNSIZE		4
# define	flOVERRIDE		override
#endif





//-----------------------------------------------------------------------------
// default
//-----------------------------------------------------------------------------
#ifndef	_flFASTCALL
typedef		char				SI8;
typedef		short int			SI16;
typedef		long int			SI32;
typedef		long long int		SI64;
typedef		unsigned char		UI8;
typedef		unsigned short		UI16;
typedef		unsigned int		UI32;
typedef		unsigned long long	UI64;
typedef		char				TCHAR;
typedef		unsigned short		WCHAR;
typedef		unsigned int		DWORD;
//typedef		int					LOOP_T;
//typedef		UI32				FSIZE_T;
//# define	FSIZE_TO_MSIZE(n)	(n)
# define	_flFASTCALL
# define	_flCDECL
# define	_flSTDCALL
# define	_flINLINE		inline
# define	_flNOINLINE
# define	_flNAKED
# define	_flNORETURN
# define	_flTHREAD
# define	_flALIGN128
# define	_flALIGN64
# define	_flALIGN32
# define	_flALIGN16
# define	_flALIGN8
# define	_flALIGN4
# define	_flRESTRICT		restrict
# define	_flFNCRESTRICT
# define	_flNOALIAS
# define	_flDEPRECATE
# define	flOVERRIDE
#endif

//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------
#define		_flWIDE				0

//-----------------------------------------------------------------------------
// type
//-----------------------------------------------------------------------------
typedef		char				ACHAR;

#ifndef	_TCHAR_DEFINED
#define	_TCHAR_DEFINED
typedef		char				TCHAR;
#endif

#if _flVALIST
typedef		va_list				__VA_LIST_T;
#else
typedef		const void*			__VA_LIST_T;
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef		float		DVALF;
#define		DVF( n )	((DVALF)( n ))


#ifndef	DV
#  define	_flFIXED	0
#  define	DV( n )		DVF( n )
typedef		DVALF		DVAL;
#endif


#define		DVAL( n )	____DVAL_CAST_ERROR____##n


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef	TEXT
# define	TEXT(n)		(n)
#endif
#ifndef	ZT
//# define	ZT(n)		TEXT(n)
# define	ZT(n)		n
#endif



#ifndef	DCOL_ARGB
# define	DCOL_ARGB(a,r,g,b)	((DCOL)(((a)<<24)|((r)<<16)|((g)<<8)|(b)))
# define	DCOL_RGBA(r,g,b,a)	DCOL_ARGB(a,r,g,b)
# define	DCOL_RGB(r,g,b)		DCOL_ARGB(0,r,g,b)
# define	DCOL_GETA(col)		(((col)>>24)&0xff)
# define	DCOL_GETR(col)		(((col)>>16)&0xff)
# define	DCOL_GETG(col)		(((col)>> 8)&0xff)
# define	DCOL_GETB(col)		(((col)    )&0xff)
#endif


#ifndef	DBYTE4_RGBA
# define	DBYTE4_RGBA(r,g,b,a)	((DBYTE4)(((a)<<24)|((b)<<16)|((g)<<8)|(r)))
# define	DBYTE4_GETA(col)		(((col)>>24)&0xff)
# define	DBYTE4_GETB(col)		(((col)>>16)&0xff)
# define	DBYTE4_GETG(col)		(((col)>> 8)&0xff)
# define	DBYTE4_GETR(col)		(((col)    )&0xff)
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef	TRUE
# define	TRUE		1
#endif
#ifndef	FALSE
# define	FALSE		0
#endif

#ifndef	NULL
# define	NULL		0
#endif

#ifndef	S_OK
# define	S_OK		0 // S_OK	= ((HRESULT)0x00000000L)
# define	S_FALSE		1 // S_FALSE= ((HRESULT)0x00000001L)
#endif
#ifndef	DD_OK
# define	DD_OK	S_OK
#endif

#ifndef	CONST_TYPE
# define	CONST_TYPE	enum
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define		ZRELEASE(n)			((n)?((n)->Release(),(n)=NULL):0)
#define		ZFREE(n)			{if(n){flFree(n);(n)=NULL;}}
#define		ZDELETE(n)			{if(n){delete (n);(n)=NULL;}}




#endif


