// 2005 Hiroyuki Ogasawara
// vim:ts=4 sw=4:
/*!
	@file
	@author	Hiroyuki Ogasawara
	@brief
*/

#ifndef	FL_FLCORE_H_
#define	FL_FLCORE_H_

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define		FLATLIB_VERSION_TEXT	"1.00"
#define		FLATLIB_VERSION			100


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include	"flPlatformMacro.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// WindowsPC
#if _flWIN32 || _flWIN64
# if _flVC8 // VC 2005
#  define	_WIN32_WINNT	0x500
#  define	_CRT_SECURE_NO_DEPRECATE
#  pragma	warning( disable : 4819 )
# endif
# if _flVC9 // VC 2008
#  define	_CRT_SECURE_NO_DEPRECATE
#  pragma	warning( disable : 4819 )
# endif
# include	<winsock2.h>
# include	<windows.h>
# if _flIA32 || _flX64
#  include	<intrin.h>
# endif
# if _flWINCE
#  include	<crtdefs.h>
# endif
# include	<stddef.h>
# include	<tchar.h>
#endif


// Linux
#if _flUNIX
# include	<stdio.h>
# include	<stdlib.h>
# include	<unistd.h>
# include	<errno.h>
# include	<string.h>
# include	<netdb.h>
# include	<netinet/in.h>
# include	<sys/types.h>
# include	<sys/socket.h>
# include	<pthread.h>
#endif


#include	"flDataType.h"
#include	"flLog.h"



#endif


