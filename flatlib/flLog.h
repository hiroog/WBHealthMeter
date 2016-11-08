// 2005 Hiroyuki Ogasawara
// vim:ts=4 sw=4:
/*!
	@file
	@author	Hiroyuki Ogasawara
	@brief
*/

#ifndef FL_FLLOG_H_
#define FL_FLLOG_H_

#include	"flDataType.h"
#include	<assert.h>


//----------------------------------------------------------------------------
#if _flDEBUG
//----------------------------------------------------------------------------

#define	flECHECK(n)					if(n)
#define	flASSERT(n)					assert(n)
#define	flASSERT_ALIGN16( n )		flASSERT( !(reinterpret_cast<intptr_t>(n) & (16-1)) )

//----------------------------------------------------------------------------
#else
//----------------------------------------------------------------------------

#define	flECHECK(n)					if(0)
#define	flASSERT(n)					
#define	flASSERT_ALIGN16( n )		

//----------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class flLog {
public:
	static void	Format( const char* msg ... );
	static void	FormatV( const char* msg, __VA_LIST_T args );
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


inline void flMESSAGE( const char* format ... )
{
	if( _flDEBUG ){
#if _flVALIST
		va_list	ap;
		va_start( ap, format );
		flLog::FormatV( format, ap );
		va_end( ap );
#else
		flLog::FormatV( format, reinterpret_cast<void*>(&format+1) );
#endif
	}
}


inline void flERROR( const char* format ... )
{
#if _flVALIST
	va_list	ap;
	va_start( ap, format );
	flLog::FormatV( format, ap );
	va_end( ap );
#else
	flLog::FormatV( format, reinterpret_cast<void*>(&format+1) );
#endif
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



#endif

