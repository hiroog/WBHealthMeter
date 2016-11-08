// 2005-2008 Hiroyuki Ogasawara
// vim:ts=4 sw=4:
/*!
	@file
	@author	Hiroyuki Ogasawara
	@brief
*/

#ifndef	FL_FLTHREADCRITICAL_H_
#define	FL_FLTHREADCRITICAL_H_



class flThreadCritical {
private:
#if _flWIN32
	CRITICAL_SECTION	CriticalObject;
	bool				InitFlag;
#endif
#if _flLINUX
	flThreadMutex		CriticalObject;
#endif
public:
	flThreadCritical();
	~flThreadCritical();
	void	Init();
	void	Quit();
	void	Enter()
	{
#if _flWIN32
		EnterCriticalSection( &CriticalObject );
#endif
#if _flLINUX
		CriticalObject.Lock();
#endif
	}
	void	Leave()
	{
#if _flWIN32
		LeaveCriticalSection( &CriticalObject );
#endif
#if _flLINUX
		CriticalObject.Unlock();
#endif
	}
	bool	TryLock()
	{
#if _flWIN32
		return	!!TryEnterCriticalSection( &CriticalObject );
#endif
#if _flLINUX
		return	!!CriticalObject.TryLock();
#endif
	}
};



#endif


