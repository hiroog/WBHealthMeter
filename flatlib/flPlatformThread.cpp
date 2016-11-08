// 2004-2008 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#include	"flCore.h"
# include	<process.h>
#include	"flPlatformThread.h"




flThread::flThread()
{
	InitFlag= false;
#if _flWIN32
	Thread= NULL;
#endif
}


flThread::~flThread()
{
	Quit();
}


void flThread::Init()
{
	if( !InitFlag ){
		InitFlag= true;
		Control= 0;
		ControlCritical.Init();
#if _flWIN32
		Thread= NULL;
#endif
	}
}


void flThread::Quit()
{
	if( InitFlag ){
#if _flWIN32
		if( Thread ){
			CloseHandle( Thread );
			Thread= NULL;
		}
#endif
		ControlCritical.Quit();
		Control= 0;
		InitFlag= false;
	}
}


int flThread::TestControl( int control )
{
	ControlEnter();
	int	ret= Control & control;
	ControlLeave();
	return	ret;
}


int flThread::SetControl( int control )
{
	ControlEnter();
	int	ret= ( Control |= control );
	ControlLeave();
	return	ret;
}


int flThread::ResetControl( int control )
{
	ControlEnter();
	int	ret= ( Control &= ~control );
	ControlLeave();
	return	ret;
}


int flThread::WaitQuit()
{
	SetControl( CONTROLREQUEST_QUIT );
#if _flWIN32
	if( Thread ){
		WaitForSingleObject( Thread, INFINITE );
		CloseHandle( Thread );
		Thread= NULL;
	}
#endif
#if _flLINUX
	pthread_join( Thread, NULL );
#endif
	return	0;
}


bool	flThread::IsExecute()
{
#if _flWIN32
	if( Thread ){
		return	WaitForSingleObject( Thread, 0 ) == WAIT_TIMEOUT;
	}
#endif
#if _flLINUX
	//pthread_join( Thread, NULL );
	flASSERT( 0 );
#endif
	return	false;
}


#if _flWIN32
# if __USE_WINAPI_THREAD__
DWORD WINAPI
# else
unsigned int WINAPI
# endif
flThread::__ThreadExec( void* arg )
{
	flThread*	This= reinterpret_cast<flThread*>( arg );
	void*	retval= This->Exec( This->Arg );
	This->Exit( 0 );
	return	0;
}
#endif

#if _flLINUX
void* flThread::__ThreadExec( void* arg )
{
	flThread*	This= reinterpret_cast<flThread*>( arg );
	void*	retval= This->Exec( This->Arg );
	This->Exit( 0 );
	return	retval;
}
#endif





bool	flThread::CreateThread( void* (*exec)(void*), void* arg )
{
	Arg= arg;
	Exec= exec;
#if _flWIN32
# if __USE_WINAPI_THREAD__
	Thread= ::CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)__ThreadExec, (void*)this, 0, &ThreadID );
# else
	Thread= reinterpret_cast<HANDLE>( ::_beginthreadex( NULL, 0, &__ThreadExec, (void*)this, 0, &ThreadID ) );
# endif
#endif
#if _flLINUX
	int	err;
	if( (err= pthread_create( &Thread, NULL, (void* (*)(void*))__ThreadExec, (void*)this )) ){
		flError::Format( ZT("pthread_create: error %d"), err );
		return	false;
	}
#endif
	return	true;
}


void flThread::Exit( int code )
{
	SetControl( STATUS_EXIT );
#if _flWIN32
# if __USE_WINAPI_THREAD__
	ExitThread( code );
# else
	_endthreadex( code );
# endif
#endif
#if _flLINUX
	pthread_exit( (void*)code );
#endif
}





