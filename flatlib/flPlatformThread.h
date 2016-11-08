// 2005-2008 Hiroyuki Ogasawara
// vim:ts=4 sw=4:
/*!
	@file
	@author	Hiroyuki Ogasawara
	@brief
*/

#ifndef	FL_FLPLATFORMTHREAD_H_
#define	FL_FLPLATFORMTHREAD_H_

#include	"flThreadCritical.h"


#define	__USE_WINAPI_THREAD__	0

//-----------------------------------------------------------------------------
// Thread
//-----------------------------------------------------------------------------

class flThread {
public:
	enum {
		CONTROLREQUEST_NONE			= 0,
		CONTROLREQUEST_QUIT			= (1<<0),
		STATUS_EXIT					= (1<<1),
		CONTROLREQUESTSHIFT_USER	= 16,
	};
private:
	bool		InitFlag;
#if _flWIN32
	HANDLE		Thread;
# if __USE_WINAPI_THREAD__
	DWORD		ThreadID;
# else
	unsigned int	ThreadID;
# endif
#endif
#if _flLINUX
	pthread_t	Thread;
#endif
	volatile int	Control;
	void*			(*Exec)( void* arg );
	void*			Arg;
	flThreadCritical	ControlCritical;
private:
#if _flWIN32
# if __USE_WINAPI_THREAD__
	static DWORD WINAPI	__ThreadExec( void* arg );
# else
	static unsigned int WINAPI	__ThreadExec( void* arg );
# endif
#endif
#if _flLINUX
	static void*	__ThreadExec( void* arg );
#endif
public:
	flThread();
	~flThread();
	void 	Init();
	void 	Quit();
	bool 	CreateThread( void* (*exec)( void* arg ), void* arg );
	void 	Exit( int code );
	int 	TestControl( int control );
	int 	SetControl( int control );
	int 	ResetControl( int control );
	void	ControlEnter()
	{
		ControlCritical.Enter();
	}
	void	ControlLeave()
	{
		ControlCritical.Leave();
	}
	int 	WaitQuit();
	bool 	IsExecute();
	int		TestQuit()
	{
		return	TestControl(CONTROLREQUEST_QUIT);
	}
	int		IsActive()
	{
#if _flWIN32
		return	IsExecute();
#endif
#if _flLINUX
		return	!TestControl(STATUS_EXIT);
#endif
	}
};



#endif




