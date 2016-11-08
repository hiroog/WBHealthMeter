// 2005 Hiroyuki Ogasawara
// vim:ts=4 sw=4:
/*!
	@file
	@author	Hiroyuki Ogasawara
	@brief
*/

#ifndef	FL_FLPLATFORMMACRO_H_
#define	FL_FLPLATFORMMACRO_H_


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if	0
// CPU
# define	_flIA32			0
# define	_flIA64			0
# define	_flX64			0
# define	_flPPC64		0
# define	_flARM			0

// OS
# define	_flWIN32		0
# define	_flWIN64		0
# define	_flWINCE		0
# define	_flLINUX		0
# define	_flUNIX			0

// GraphicLib
# define	_flDXM			0
# define	_flDX8			0
# define	_flDX9			0
# define	_flDX10			0
# define	_flDX101		0
# define	_flRH			0

// Default Data type
# define	_flFIXED12		0
# define	_flFIXED16		0
# define	_flFLOAT		0
# define	_flDOUBLE		0

// Encoding
# define	_flSYSWIDE		0	// Wide Char
# define	_flVALIST		0	//
# define	_flBIGENDIAN	0	// Endian

// Compiler
# define	_flVC8			0	// VisualC++ v8 (VS2005)
# define	_flVC9			0	// VisualC++ v9 (VS2008)
# define	_flGCC			0	// GNU C
#endif



//-----------------------------------------------------------------------------
// WindowsPC DX10
//-----------------------------------------------------------------------------
#if	_flPC10
# ifdef _WIN64
#  error "use _flPC649 or _flPC6410"
# endif
# define	_flIA32			1
# define	_flWIN32		1
# define	_flDX10			1
# define	_flFLOAT		1
# define	_flVC9			1
# define	_flSYSTYPE		1
# define	_flVALIST		1
#endif


//-----------------------------------------------------------------------------
// WindowsPC
//-----------------------------------------------------------------------------
#if	_flPC9 || _flPC
# ifdef _WIN64
#  error "use _flPC649 or _flPC6410"
# endif
# define	_flIA32			1
# define	_flWIN32		1
# define	_flDX9			1
# define	_flFLOAT		1
# define	_flVC9			1
# define	_flSYSTYPE		1
# define	_flVALIST		1
#endif



//-----------------------------------------------------------------------------
// WindowsPC64 DX10
//-----------------------------------------------------------------------------
#if	_flPC6410
# ifndef _WIN64
#  error "use _flPC9 or _flPC10"
# endif
# define	_flX64			1
# define	_flWIN32		1
# define	_flWIN64		1
# define	_flDX10			1
# define	_flFLOAT		1
# define	_flVC9			1
# define	_flSYSTYPE		1
# define	_flVALIST		1
#endif

//-----------------------------------------------------------------------------
// WindowsPC64
//-----------------------------------------------------------------------------
#if	_flPC649
# ifndef _WIN64
#  error "use _flPC9 or _flPC10"
# endif
# define	_flX64			1
# define	_flWIN32		1
# define	_flWIN64		1
# define	_flDX9			1
# define	_flFLOAT		1
# define	_flVC9			1
# define	_flSYSTYPE		1
# define	_flVALIST		1
#endif


//-----------------------------------------------------------------------------
// WindowsCE
//-----------------------------------------------------------------------------
#if	_flCE
# define	_flARM			1
# define	_flWIN32		1
# define	_flWINCE		1
# define	_flDXM			1
# define	_flFIXED16		1
# define	_flFLOAT		1
# define	_flSYSWIDE		1
# define	_flVC8			1
# define	_flSYSTYPE		1
#endif






//-----------------------------------------------------------------------------
// Linux
//-----------------------------------------------------------------------------
#if	_flLINUX
# define	_flIA32			1
# define	_flLINUX		1
# define	_flUNIX			1
# define	_flFLOAT		1
# define	_flGCC			1
# define	_flSYSTYPE		1
#endif


//-----------------------------------------------------------------------------
// fix
//-----------------------------------------------------------------------------

#if !_flSYSTYPE
# error "systype undefined, use _flPC10=1 or _flPC6410=1"
#endif

// CPU
#ifndef	_flIA32
# define	_flIA32			0
#endif
#ifndef	_flIA64
# define	_flIA64			0
#endif
#ifndef	_flX64
# define	_flX64			0
#endif
#ifndef	_flPPC64
# define	_flPPC64		0
#endif
#ifndef	_flARM
# define	_flARM			0
#endif

// OS
#ifndef	_flWIN32
# define	_flWIN32		0
#endif
#ifndef	_flWIN64
# define	_flWIN64		0
#endif
#ifndef	_flWINCE
# define	_flWINCE		0
#endif
#ifndef	_flLINUX
# define	_flLINUX		0
#endif
#ifndef	_flUNIX
# define	_flUNIX			0
#endif

// GraphicLibrary
#ifndef	_flDXM
# define	_flDXM			0
#endif
#ifndef	_flDX8
# define	_flDX8			0
#endif
#ifndef	_flDX9
# define	_flDX9			0
#endif
#ifndef	_flDX10
# define	_flDX10			0
#endif
#ifndef	_flRH
# define	_flRH			0
#endif

// Default DataType
#ifndef	_flFIXED12
# define	_flFIXED12		0
#endif
#ifndef	_flFIXED16
# define	_flFIXED16		0
#endif
#ifndef	_flFLOAT
# define	_flFLOAT		0
#endif
#ifndef	_flDOUBLE
# define	_flDOUBLE		0
#endif


// System Encoding
#ifndef	_flSYSWIDE
# define	_flSYSWIDE		0
#endif
#ifndef	_flVALIST
# define	_flVALIST		0
#endif
#ifndef	_flBIGENDIAN
# define	_flBIGENDIAN	0
#endif


// Compiler
#ifndef	_flVC8
# define	_flVC8			0
#endif
#ifndef	_flVC9
# define	_flVC9			0
#endif
#ifndef	_flGCC
# define	_flGCC			0
#endif



//-----------------------------------------------------------------------------
// Debug
//-----------------------------------------------------------------------------
#ifndef	_flDEBUG
# if defined(NDEBUG)
#  define	_flDEBUG		0
# else
#  define	_flDEBUG		1
# endif
#endif

#ifndef _flRETAIL
# define	_flRETAIL		0
#endif


#if _flDX10
# define	_flDX101		1
#endif


#endif


