// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#include	"flCore.h"
#include	"flThreadCritical.h"




flThreadCritical::flThreadCritical()
{
#if _flWIN32
	InitFlag= false;
#endif
}

flThreadCritical::~flThreadCritical()
{
	Quit();
}



void flThreadCritical::Init()
{
#if _flWIN32
	flASSERT( !InitFlag );
	InitializeCriticalSection( &CriticalObject );
	InitFlag= true;
#endif

#if _flLINUX
	CriticalObject.Init();
#endif

}



void flThreadCritical::Quit()
{
#if _flWIN32
	if( InitFlag ){
		DeleteCriticalSection( &CriticalObject );
		InitFlag= false;
	}
#endif

#if _flLINUX
		CriticalObject.Quit();
#endif
}



