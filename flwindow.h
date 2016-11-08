// 2005 Hiroyuki Ogasawara
// vim:ts=4 sw=4:
/*!
	@file
	@author	Hiroyuki Ogasawara
	@brief
*/

#ifndef FL_FLWINDOW_H_
#define FL_FLWINDOW_H_


#if _flWIN32

//-----------------------------------------------------------------------------
// Window
//-----------------------------------------------------------------------------
class flWindow {
private:
	enum {
		WFLAG_WITHMENU	= (1<<0),
		//---
		MAX_WINDOW		= 32,
	};
	HWND	hWnd;
	HINSTANCE	Instance;
	int		WindowFlag;
	LRESULT	(*EventExec)( HWND hwnd, UINT mes, WPARAM wparam, LPARAM lparam );
	int		TableID;
private:
	static flWindow*	_ThisTable[MAX_WINDOW];
private:
	static int	_SearchAndSetTable( flWindow* win );
	static LRESULT CALLBACK	WindowExec( HWND hwnd, UINT mes, WPARAM wparam, LPARAM lparam );
public:
	flWindow( HINSTANCE isntance );
	virtual	~flWindow();
	//-------------------------------------------------------------------------
	//
	//-------------------------------------------------------------------------
	virtual LRESULT 	EventProc( HWND hwnd, UINT mes, WPARAM wparam, LPARAM lparam );
	operator HWND()
	{
		return	hWnd;
	}
	operator HINSTANCE()
	{
		return	Instance;
	}
	HWND 		GetHWND()
	{
		return	hWnd;
	}
	HINSTANCE 	GetInstance()
	{
		return	Instance;
	}
	void 	SetExec( LRESULT (*callback)( HWND, UINT, WPARAM, LPARAM )= NULL )
	{
		EventExec= callback;
	}
	HWND 	Open( flWindow* phwnd, const ACHAR* appname, int icon= 0, int menu= 0 );
	void 	SetStyle( int x, int y, int w, int h, int style, int exstyle, const ACHAR* title= NULL );
	static HWND	FindPrevInstance( const ACHAR* classname, const ACHAR* winname );
};

#endif







#endif


