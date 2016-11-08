// 2005-2006 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#include	"flCore.h"
#include	"flwindow.h"


flWindow* flWindow::_ThisTable[MAX_WINDOW];


int flWindow::_SearchAndSetTable( flWindow* win )
{
	for( int i= 0 ; i< MAX_WINDOW ; i++ ){
		if( !_ThisTable[i] ){
			_ThisTable[i]= win;
			return	i;
		}
	}
	return	-1;
}



flWindow::flWindow( HINSTANCE ins )
{
	hWnd= NULL;
	Instance= ins;
	WindowFlag= 0;
	EventExec= NULL;
	TableID= -1;
}


flWindow::~flWindow()
{
	if( hWnd ){
		DestroyWindow( hWnd );
		hWnd= NULL;
	}
	if( TableID >= 0 ){
		_ThisTable[TableID]= NULL;
		TableID= -1;
	}
	EventExec= NULL;
}


LRESULT flWindow::EventProc( HWND hwnd, UINT mes, WPARAM wparam, LPARAM lparam )
{
	switch( mes ){
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return	FALSE;
	}
	return	TRUE;
}


LRESULT CALLBACK flWindow::WindowExec( HWND hwnd, UINT mes, WPARAM wparam, LPARAM lparam )
{
	flWindow*	wThis= _ThisTable[ GetWindowLongPtr( hwnd, GWLP_USERDATA ) ];
	if( wThis ){
		LRESULT	ret= TRUE;
		if( wThis->EventExec ){
			ret= wThis->EventExec( hwnd, mes, wparam, lparam );
		}
		if( ret ){
			ret= wThis->EventProc( hwnd, mes, wparam, lparam );
		}
		if( !ret ){
			return	FALSE;
		}
	}
	return	DefWindowProc( hwnd, mes, wparam, lparam );
}


HWND flWindow::Open( flWindow* pwp, const ACHAR* appname, int icon, int menu )
{
	flASSERT( appname );

	WNDCLASS	wc;
	wc.style= CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc= WindowExec;
	wc.cbClsExtra= 0;
	wc.cbWndExtra= 0;
	wc.hInstance= Instance;
	wc.hIcon= icon ? LoadIcon( Instance, MAKEINTRESOURCE(icon) ) : NULL;
	wc.hCursor= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground= (HBRUSH)GetStockObject( BLACK_BRUSH );
	if( menu ){
		wc.lpszMenuName= MAKEINTRESOURCE( menu );
		WindowFlag|= WFLAG_WITHMENU;
	}else{
		wc.lpszMenuName= NULL;
	}
	wc.lpszClassName= appname;
	RegisterClass( &wc );

	hWnd= CreateWindowEx( 0,//exstyle
					appname, appname,
					WS_POPUP|WS_VISIBLE,	//style
					CW_USEDEFAULT, CW_USEDEFAULT, 256, 32,
					pwp ? *pwp : NULL, NULL, Instance, NULL );
	GetSubMenu( GetMenu( hWnd ), 0 );

	if( hWnd ){
		TableID= _SearchAndSetTable( this );
		if( TableID >= 0 ){
			SetWindowLongPtr( hWnd, GWLP_USERDATA, TableID );
		}
	}
	return	hWnd;
}


void flWindow::SetStyle( int x, int y, int w, int h, int style, int exstyle, const ACHAR* title )
{
	if( !hWnd ){
		return;
	}
	RECT		rect;
	int			posFlag= SWP_NOZORDER;
	if( x == CW_USEDEFAULT ){
		SetRect( &rect, 0, 0, w, h );
		posFlag|= SWP_NOMOVE|SWP_NOREPOSITION;
	}else{
		SetRect( &rect, x, y, w, h );
	}
	if( w == CW_USEDEFAULT ){
		posFlag|= SWP_NOSIZE;
	}
	if( WindowFlag & WFLAG_WITHMENU ){
		AdjustWindowRectEx( &rect, style, TRUE, exstyle );//with menu
	}else{
		AdjustWindowRectEx( &rect, style, FALSE, exstyle );//without menu
	}
	SetWindowLong( hWnd, GWL_STYLE, style );
	SetWindowLong( hWnd, GWL_EXSTYLE, exstyle );
	if( title ){
		SetWindowText( hWnd, title );
	}
	SetWindowPos( hWnd, 0,
			rect.left,
			rect.top,
			rect.right-rect.left,
			rect.bottom-rect.top,
			posFlag
			);
	ShowWindow( hWnd, SW_SHOWNORMAL );
	UpdateWindow( hWnd );
}




//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

HWND flWindow::FindPrevInstance( const ACHAR* classname, const ACHAR* winname )
{
	HWND	wp= FindWindowEx( NULL, NULL, classname, winname );
	if( wp ){
		SetForegroundWindow( wp );
	}
	return	wp;
}




