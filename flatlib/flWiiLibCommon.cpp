// 2007-2008 Hiroyuki Ogasawara
// vim:ts=4 sw=4 noet:

#include	"flCore.h"
#include	"flWiiLibCommon.h"


void* flWiiLibCommon::dAlloc( size_t size )
{
	return	malloc( size );
}


void flWiiLibCommon::dFree( void* ptr )
{
	free( ptr );
}


void flWiiLibCommon::DebugPrint( const char* format ... )
{
#if _flDEBUG
# if _flVALIST
	va_list	arg;
	va_start( arg, format );
	flLog::FormatV( format, arg );
	va_end( arg );
# else
	flLog::FormatV( format, (const void*)(&format+1) );
# endif
#endif
}




