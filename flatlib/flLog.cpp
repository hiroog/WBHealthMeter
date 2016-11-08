// 2005-2008 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#include	"flCore.h"
#include	"flLog.h"
#include	<stdio.h>

#define	__USE_CRLF__		1



static void MessageExec_DebugConsole( const char* ptr )
{
#if _flWIN32
	OutputDebugString( ptr );
#else
	fputs( ptr, stdout );
#endif
}


void flLog::FormatV( const char* format, __VA_LIST_T args )
{
	enum {
		FORMAT_BUFSIZE	= 1024 * 2,
	};
	char	formatbuf[FORMAT_BUFSIZE];


#if _flVALIST
	vsprintf( formatbuf, format, args );
#else
	vsprintf( formatbuf, format, (va_list)args );
#endif


#if __USE_CRLF__
	strcat( formatbuf, "\r\n" );
#else
	strcat( formatbuf, "\n" );
#endif

	MessageExec_DebugConsole( formatbuf );
}



void _flCDECL flLog::Format( const char* format ... )
{
#if _flVALIST
	va_list	arg;
	va_start( arg, format );
	FormatV( format, arg );
	va_end( arg );
#else
	FormatV( format, (const void*)(&format+1) );
#endif
}




