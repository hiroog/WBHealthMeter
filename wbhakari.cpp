// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:


#include	"flCore.h"
#include	"flWindow.h"
#include	"flWiiLib.h"
#include	<stdio.h>
#include	"flBluetoothLib.h"
#include	"flPlatformThread.h"
#include	"flSimpleMath.h"
#include	"resource.h"
#include	<Vector>
#include	<shlobj.h>
#include	"RealtimeLog.h"


#define		WIIBALANCE_VERSION	"v1.13"
#define		WIIBALANCE_TITLE	"WBHealthMeter"

#define		_LOGFILENAME_FILE	"WBHealthMeter_log.txt"
#define		RT_LOGFILENAME_FILE	"WBHealthMeter_realtime.txt"


#define		TEXT_COLOR			RGB( 20, 20, 20)
#define		BTEXT_COLOR			RGB(  0,  0,  0)
//#define		LOGTEXT_COLOR		RGB(120,120,115)
#define		LOGTEXT_COLOR		RGB( (int)(240/1.5f), (int)(240/1.5f), (int)(230/1.5f))
#define		WALKTEXT_COLOR		RGB( 20, 20,220)
#define		BMITEXT_COLOR		RGB(192,165,142)
#define		BG_COLOR			RGB(240,240,230)
#define		METER_COLOR			RGB(255,255,255)
#define		METERLINE_COLOR		RGB( 55, 55, 25)
#define		POW_COLOR			RGB( 80,120,255)
#define		BOX_COLOR			RGB( 80, 80,255)



enum {
	SCREEN_WIDTH		= 600,
	SCREEN_HEIGHT		= 400,
	S_FONTSIZE			= 12,
	B_FONTSIZE			= 96,
	M_FONTSIZE			= 24,
	//---
	CALIBRATION_TIME	= 60,
	CT_MAX_PATH			= 1024*4,
	FIXMAX_USER			= 4,
	//---
	DIFF_LIMIT			= 30,
	FIX_LIMIT			= 30 * 10,
	MAX_MEMLOGLINE		= 8,
	MAX_LOGLINECHAR		= 64,
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct LogData {
public:
	SYSTEMTIME	Time;
	float		Weight;
};

class LogUser {
private:
	unsigned int			Color;
	std::vector<LogData>	Data;
public:
};

class LogFile {
public:
public:
	LogUser	User[FIXMAX_USER];
public:
};

struct LogLine {
	// 0 2008-01-01 12:34:56 123.4		27byte
	// log 0 2008-01-01 12:34:56 123.4		27byte
	char	Log[MAX_LOGLINECHAR];
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


class AppWindow : public flWindow {
private:
	enum {
		MAX_DRAWBUFFER	= 512,
		MAX_LOCALPARAM	= 32,
	};
	enum {
		FLAG_ADDDEVICE	= (1<<2),
		FLAG_UPDATE		= (1<<3),
		FLAG_UPDATE2	= (1<<4),
		FLAG_FIXSTART	= (1<<5),
	};
	struct LocalParam {
		float			BigWeight;
		float			TotalWeight;
		float			FixWeight[FIXMAX_USER];
		float			FixDiff;
		float			Point[4];
		float			Balance[2];
		int				AdjustTimer;
		unsigned int	LocalFlag;
		int				DeviceType;
		flWiiLibDevice*	iDevice;
	public:
		void 	Format();
		void 	Calibration();
		//---------------------------------------------------------------------
		int 		IsAdjusting() const
		{
			return	AdjustTimer;
		}
		//---------------------------------------------------------------------
		void 	SetLocalFlag( unsigned int f )
		{
			LocalFlag|= f;
		}
		void 	ResetLocalFlag( unsigned int f )
		{
			LocalFlag&= ~f;
		}
		unsigned int 	TestLocalFlag( unsigned int f ) const
		{
			return	LocalFlag & f;
		}
	};
private:
	HFONT			hFont;
	HFONT			hBigFont;
	HFONT			hMidFont;
	HDC				hDC;
	HDC				hBack;
	HBITMAP			hBitmap;
	HBRUSH			hBGColor;
	HBRUSH			hMeterColor;
	HBRUSH			hPowColor;
	HPEN			hMeterPen;
	HPEN			hBoxPen;
	int				ScreenWidth;
	int				ScreenHeight;
	flWiiLib*		iWiiLib;
	int				OptionFlag;
//	int				DeviceCount;
//	int				ActiveCount;
	//---------------------------------------------
	//---------------------------------------------
	int				Stature;	// mm
	int				User;
	int				FixTimer;
	int				DiffTimer;
	int				PrevButton;
	int				BlinkTimer;
	int				StartupWait;
	//---------------------------------------------
	LocalParam		Local[MAX_LOCALPARAM];
	LocalParam*		GlobalBalance;
	LogFile			File;
	LogLine			Line[MAX_MEMLOGLINE];
	//---------------------------------------------
	flThread		_WorkThread;
	RealtimeLog		RT_Log;
	//---------------------------------------------
public:
	AppWindow( HINSTANCE hInstance ) : flWindow( hInstance )
	{
		Format();
	}
	~AppWindow()
	{
		Quit();
	}
	//-------------------------------------------------------------------------
	LRESULT 	EventProc( HWND hwnd, UINT mes, WPARAM wparam, LPARAM lparam );
	//-------------------------------------------------------------------------
	void 	Quit();
	void 	Init();
	void 	Render();
	void 	Render_Active();
	void 	DrawBluetooth();
	void 	DrawMessage( const char** message );
	void 	DrawNotFound();
	void 	Format();
	//-------------------------------------------------------------------------
	void 	SetFlag( int f )
	{
		OptionFlag|= f;
	}
	void 	ResetFlag( int f )
	{
		OptionFlag&= ~f;
	}
	int 		TestFlag( int f ) const
	{
		return	OptionFlag & f;
	}
	//-------------------------------------------------------------------------
	void 	Clear();
	void 	SetFontColor( unsigned int color );
	void 	SetBGColor( unsigned int color );
	void 	Print( int x, int y, const char* format ... );
	//-------------------------------------------------------------------------
	void 	DrawBox( int x, int y, int w, int h );
	void 	DrawLine( const POINT* plist, int pcount );
	void 	Poly( int basex, int basey, const flMatrix* mat, const flVect3* ipos, flVect3* vpos, int vcount, const int* _index, int icount );
	//-------------------------------------------------------------------------
	void 	UpdateBalance( flWiiLibDevice* dev, LocalParam* lp );
	//-------------------------------------------------------------------------
	void 	DrawFix( LocalParam* lp );
	void 	DrawWeight( LocalParam* lp );
	void 	DrawMeter( LocalParam* lp );
	void 	DrawBalanceBar( LocalParam* lp );
	void 	DrawHelp( int basex, int basey );
	void 	DrawBMIBar( LocalParam* lp );
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	static void* 	_AddDeviceFunc( void* arg );
	void 	AddDevice();
	void 	CalibrationAll();
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	FILE* 	OpenLog( const char* mode );
	void 	AppendLog( LocalParam* lp );
	void 	LoadLog();
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	void	StartRealtimeLog();
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	void 	BeginFix( int user );
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void AppWindow::LocalParam::Calibration()
{
	BigWeight= 0.0f;
	TotalWeight= 0.0f;
	for( int i= 0 ; i< 4 ; i++ ){
		Point[i]= 0.0f;
	}
	Balance[0]= 0.0f;
	Balance[1]= 0.0f;
	AdjustTimer= CALIBRATION_TIME;
}




void AppWindow::LocalParam::Format()
{
	Calibration();

	for( int i= 0 ; i< FIXMAX_USER ; i++ ){
		FixWeight[i]= 0.0f;
	}
	LocalFlag= 0;

	DeviceType= 0;
	iDevice= NULL;
}




void AppWindow::Format()
{
	hFont= NULL;
	hBigFont= NULL;
	hMidFont= NULL;
	hDC= NULL;
	hBack= NULL;
	hBitmap= NULL;
	hBGColor= NULL;
	hMeterColor= NULL;
	hPowColor= NULL;
	hMeterPen= NULL;
	hBoxPen= NULL;

	iWiiLib= NULL;
	OptionFlag= 0;
//	DeviceCount= 0;
//	ActiveCount= 0;

	Stature= 1600;
	User= 0;
	FixTimer= 0;
	DiffTimer= 0;
	PrevButton= 0;
	BlinkTimer= 0;
	StartupWait= 30*4;

	for( int i= 0 ; i< MAX_LOCALPARAM ; i++ ){
		Local[i].Format();
	}
	GlobalBalance= NULL;

	for( int i= 0 ; i< MAX_MEMLOGLINE ; i++ ){
		*Line[i].Log= '\0';
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void AppWindow::Init()
{
	ScreenWidth= SCREEN_WIDTH;
	ScreenHeight= SCREEN_HEIGHT;

	static const LOGFONT	logf= {
		S_FONTSIZE,
		S_FONTSIZE/2,
		0,		// escape
		0,		// orient
		FW_DONTCARE,	// weight
		FALSE,		// italic
		FALSE,		// underline
		FALSE,		// strikeout
		SHIFTJIS_CHARSET, //ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("ＭＳ ゴシック")
	};
	hFont= CreateFontIndirect( &logf );
	if( !hFont ){
		flMESSAGE( "font ERROR" );
	}

	static const LOGFONT	big_logf= {
		B_FONTSIZE,
		B_FONTSIZE/2,
		0,		// escape
		0,		// orient
		FW_DONTCARE,	// weight
		FALSE,		// italic
		FALSE,		// underline
		FALSE,		// strikeout
		SHIFTJIS_CHARSET, //ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("ＭＳ ゴシック")
	};
	hBigFont= CreateFontIndirect( &big_logf );
	if( !hBigFont ){
		flMESSAGE( "font ERROR" );
	}


	static const LOGFONT	mid_logf= {
		M_FONTSIZE,
		M_FONTSIZE/2,
		0,		// escape
		0,		// orient
		FW_DONTCARE,	// weight
		FALSE,		// italic
		FALSE,		// underline
		FALSE,		// strikeout
		SHIFTJIS_CHARSET, //ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("ＭＳ ゴシック")
	};
	hMidFont= CreateFontIndirect( &mid_logf );
	if( !hMidFont ){
		flMESSAGE( "font ERROR" );
	}


	hDC= GetDC( *this );
	hBack= CreateCompatibleDC( hDC );
	hBitmap= CreateCompatibleBitmap( hDC, ScreenWidth, ScreenHeight );
	SelectObject( hBack, hBitmap );

	ReleaseDC( *this, hDC );

	hBGColor= CreateSolidBrush( BG_COLOR );
	hMeterColor= CreateSolidBrush( METER_COLOR );
	hPowColor= CreateSolidBrush( POW_COLOR );
	hMeterPen= CreatePen( PS_SOLID, 0, METERLINE_COLOR );
	hBoxPen= CreatePen( PS_SOLID, 0, BOX_COLOR );

	_WorkThread.Init();


	LoadLog();


	iWiiLib= flWiiLib::Create();
	iWiiLib->BeginLoop();

}

void AppWindow::Quit()
{
	_WorkThread.Quit();

	if( hFont ){
		DeleteObject( hFont );
		hFont= NULL;
	}
	if( hBigFont ){
		DeleteObject( hBigFont );
		hBigFont= NULL;
	}
	if( hMidFont ){
		DeleteObject( hMidFont );
		hMidFont= NULL;
	}
	if( hBGColor ){
		DeleteObject( hBGColor );
		hBGColor= NULL;
	}
	if( hMeterColor ){
		DeleteObject( hMeterColor );
		hMeterColor= NULL;
	}
	if( hPowColor ){
		DeleteObject( hPowColor );
		hPowColor= NULL;
	}
	if( hBoxPen ){
		DeleteObject( hBoxPen );
		hBoxPen= NULL;
	}
	if( hMeterPen ){
		DeleteObject( hMeterPen );
		hMeterPen= NULL;
	}
	if( hBitmap ){
		DeleteObject( hBitmap );
		hBitmap= NULL;
	}
	if( hBack ){
		DeleteDC( hBack );
		hBack= NULL;
	}
	if( iWiiLib ){
		ZRELEASE( iWiiLib );
	}

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


static void Execute( const TCHAR *command, const TCHAR *param= NULL )
{
	SHELLEXECUTEINFO	info;
	memset( &info, 0, sizeof(SHELLEXECUTEINFO) );
	info.cbSize= sizeof(SHELLEXECUTEINFO);
	info.lpFile= command;
	info.lpParameters= param;
//	info.nShow= SW_SHOWNORMAL;
	ShellExecuteEx( &info );
}


static void OnlineHelp()
{
	Execute( _T("http://dench.flatlib.jp/vec/more/WBHealthMeter.txt") );
}


static void OpenWeb()
{
	Execute( _T("http://hp.vector.co.jp/authors/VA004474/win/win32.html#wbhealthmeter") );
}


static void OpenBlog()
{
	Execute( _T("http://wlog.flatlib.jp/") );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


LRESULT AppWindow::EventProc( HWND hwnd, UINT mes, WPARAM wparam, LPARAM lparam )
{
	switch( mes ){
	case WM_DESTROY:
		PostQuitMessage( 0 );
		return	FALSE;
	case WM_KEYDOWN:
		switch( wparam ){
#if 0
		case VK_UP:
			if( Stature < 3000 ){
				Stature++;
			}
			break;
		case VK_DOWN:
			if( Stature > 1 ){
				Stature--;
			}
			break;
#endif
		case 'A':
			AddDevice();
			break;
		case 'C':
			CalibrationAll();
			break;
		}
		break;
	case WM_COMMAND:
		switch( LOWORD( wparam ) ){
		case ID_FILE_EXIT:
			PostQuitMessage( 0 );
			return	FALSE;
		case ID_ADDDEVICE:
			AddDevice();
			return	FALSE;
		case ID_CALIBRATION:
			CalibrationAll();
			return	FALSE;
		case ID_HELP_ONLINEMANUAL:
			OnlineHelp();
			return	FALSE;
		case ID_HELP_WEB:
			OpenWeb();
			return	FALSE;
		case ID_HELP_BLOG:
			OpenBlog();
			return	FALSE;
		case ID_USER0:
			BeginFix( 0 );
			return	FALSE;
		case ID_USER1:
			BeginFix( 1 );
			return	FALSE;
		case ID_USER2:
			BeginFix( 2 );
			return	FALSE;
		case ID_USER3:
			BeginFix( 3 );
			return	FALSE;
		case ID_START_LOG:
			StartRealtimeLog();
			return	FALSE;
		case ID_STOP_LOG:
			RT_Log.Stop();
			return	FALSE;
		}
		break;
	}
	return	TRUE;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void	AppWindow::StartRealtimeLog()
{
	char	documentpath[CT_MAX_PATH];
	*documentpath= '\0';
	if( SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, documentpath ) == S_OK ){
		strcat( documentpath, "\\" RT_LOGFILENAME_FILE );
		flMESSAGE( "Document Path=%s", documentpath );
		RT_Log.Start( documentpath );
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


FILE* AppWindow::OpenLog( const char* mode )
{
	char	documentpath[CT_MAX_PATH];
	*documentpath= '\0';
	if( SHGetFolderPath(
				NULL,
				CSIDL_PERSONAL,
				NULL,
				0,
				documentpath ) == S_OK ){
	}
	strcat( documentpath, "\\" _LOGFILENAME_FILE );
	flMESSAGE( "Document Path=%s", documentpath );
	return	fopen( documentpath, mode );
}


void AppWindow::LoadLog()
{
	FILE*	fp= OpenLog( "rb" );
	if( fp ){
		const int	lineMax= MAX_LOGLINECHAR;
		char	_logline[ lineMax+2 ];
		LogLine	linebuf[ MAX_MEMLOGLINE ];
		memset( linebuf, 0, sizeof(LogLine) * MAX_MEMLOGLINE );
		int	index= 0;
		int	count= 0;
		for(; fgets( _logline, lineMax, fp ) ;){
			_logline[ lineMax-1 ]= '\0';
			if( !strncmp( _logline, "log ", 4 ) ){
				strcpy( linebuf[index].Log, _logline+4 );
				if( ++index >= MAX_MEMLOGLINE ){
					index= 0;
				}
				count++;
			}
		}
		fclose( fp );
		for( int i= 0 ; i< count ; i++ ){
			char*	ptr= linebuf[i].Log;
			for(; *ptr ; ptr++ ){
				if( *ptr == '\n' || *ptr == '\r' ){
					*ptr= '\0';
					break;
				}
			}
		}
		if( count < MAX_MEMLOGLINE ){
			for( int i= 0 ; i< count ; i++ ){
				strcpy( Line[i].Log, linebuf[i].Log );
			}
		}else{
			for( int i= 0 ; i< MAX_MEMLOGLINE ; i++ ){
				strcpy( Line[i].Log, linebuf[index].Log );
				if( ++index >= MAX_MEMLOGLINE ){
					index= 0;
				}
			}
		}
	}
}

void AppWindow::AppendLog( LocalParam* lp )
{
	char	_logline[ 256 ];
	SYSTEMTIME	systime;
	GetLocalTime( &systime );
	sprintf( _logline,
		"%d %04d-%02d-%02d %02d:%02d:%02d %5.1f",
			User,
			systime.wYear,
			systime.wMonth,
			systime.wDay,
			systime.wHour,
			systime.wMinute,
			systime.wSecond,
			lp->FixWeight[User]
		);

	int	freecount= 0;
	for( int i= 0 ; i< MAX_MEMLOGLINE ; i++ ){
		if( !*Line[i].Log ){
			freecount++;
		}
	}
	if( !freecount ){
		for( int i= 0 ; i< MAX_MEMLOGLINE-1 ; i++ ){
			strcpy( Line[i].Log, Line[i+1].Log );
		}
		*Line[MAX_MEMLOGLINE-1].Log= '\0';
	}

	for( int i= 0 ; i< MAX_MEMLOGLINE ; i++ ){
		if( !*Line[i].Log ){
			strcpy( Line[i].Log, _logline );
			break;
		}
	}

	FILE*	fp= OpenLog( "a" );
	if( fp ){
		fprintf( fp, "log %s\n", _logline );
		fclose( fp );
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void AppWindow::Clear()
{
	RECT	rect;
	rect.left= 0;
	rect.top= 0;
	rect.right= ScreenWidth;
	rect.bottom= ScreenHeight;
	FillRect(
		hBack,
		&rect,
		hBGColor
		);
}


void AppWindow::SetFontColor( unsigned int textcolor )
{
	SetTextColor( hBack, textcolor );
}

void AppWindow::SetBGColor( unsigned int textcolor )
{
	SetBkColor( hBack, textcolor );
}

void AppWindow::Print( int x, int y, const char* format ... )
{
	char	_drawBuffer[ MAX_DRAWBUFFER ];
	vsprintf( _drawBuffer, format, reinterpret_cast<va_list>( &format + 1 ) );

	RECT	rect;
	rect.left= x;
	rect.top= y;
	rect.right= x + ScreenWidth;
	rect.bottom= y + ScreenHeight;
	DrawText( hBack,
		_drawBuffer,
		-1,
		&rect,
		DT_LEFT|DT_TOP|DT_NOCLIP
		);

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------









void AppWindow::UpdateBalance( flWiiLibDevice* dev, LocalParam* lp )
{
	dev->DecodePacket();

	if( lp->IsAdjusting() ){
		dev->CalibrationBalance( TRUE );
	}else{
		dev->CalibrationBalance( FALSE );
	}
	if( lp->AdjustTimer ){
		lp->AdjustTimer--;
	}

	float	total= dev->GetWeight();

	lp->TotalWeight= total;
	lp->Balance[0]= dev->GetWeightBalance( 0 );
	lp->Balance[1]= dev->GetWeightBalance( 1 );

	for( int i= 0 ; i< 4 ; i++ ){
		lp->Point[i]= dev->GetBalancePressure( i );
	}

	lp->BigWeight= total * 0.10f + lp->BigWeight * 0.90f;
}





void AppWindow::DrawFix( LocalParam* lp )
{
	const int	basex= 32;
	const int	basey= 32 + 20;
	const int	logy= 32;

	SelectObject( hBack, hMidFont );
	if( TestFlag( FLAG_FIXSTART ) ){
		SetFontColor( BTEXT_COLOR );
		if( !(BlinkTimer & 0x10) ){
			Print(
				basex,
				basey + M_FONTSIZE,
				"User%d 測定中",
						User );
		}
		Print(
				basex,
				basey + M_FONTSIZE *3,
				"台に乗ってください。測定値を記録します。"
				);
		if( lp->BigWeight >= 5.0f ){
			float	diff= lp->FixWeight[User] - lp->BigWeight;
			lp->FixDiff= lp->FixDiff * 0.5f + diff * 0.5f;
			lp->FixWeight[User]= lp->BigWeight;
			if( flMath::Abs( lp->FixDiff ) < 0.1f ){
				if( ++DiffTimer >= DIFF_LIMIT ){
					ResetFlag( FLAG_FIXSTART );
					AppendLog( lp );
				}
			}else{
				DiffTimer= 0;
			}
			FixTimer= FIX_LIMIT/2;
		}else{
			lp->FixDiff= 0.0f;
			lp->FixWeight[User]= 0.0f;
		}
		if( ++FixTimer > FIX_LIMIT ){
			ResetFlag( FLAG_FIXSTART );
		}
	}else{
		SetFontColor( LOGTEXT_COLOR );
		for( int i= 0 ; i< MAX_MEMLOGLINE ; i++ ){
			const char*	memp= Line[i].Log;
			if( *memp ){
				Print(
					basex,
					logy + M_FONTSIZE * i,
					"User%skg",
						memp );
			}
		}
	}

	if( lp->iDevice ){
		int	button= lp->iDevice->GetButton();
		int	push= (PrevButton ^ button) & button;
		PrevButton= button;
		if( push & flWiiLibDevice::BT_TA ){
			if( TestFlag( FLAG_FIXSTART ) ){
				int	user= User+1;
				if( user >= FIXMAX_USER ){
					user= 0;
				}
				BeginFix( user );
			}else{
				BeginFix( 0 );
			}
		}
	}

}


void AppWindow::BeginFix( int user )
{
	User= user;
	SetFlag( FLAG_FIXSTART );
	FixTimer= 0;
	DiffTimer= 0;
	BlinkTimer= 0;
}





void AppWindow::DrawWeight( LocalParam* lp )
{
	if( lp->DeviceType != flWiiLibDevice::EXTENSIONTYPE_BALANCE ){
		return;
	}

	SelectObject( hBack, hBigFont );
	SetFontColor( BTEXT_COLOR );

	// 100.2kg
	float		dispWeight= lp->BigWeight < 0.0f ? 0.0f : lp->BigWeight;
	if( lp->AdjustTimer ){
		const int	bigsize= B_FONTSIZE * 8 / 2;
		Print( (ScreenWidth - bigsize)/2,
			ScreenHeight - B_FONTSIZE - 20,
			"0kg調整中" );
	}else{
		const int	bigsize= B_FONTSIZE * 7 / 2;
		Print( (ScreenWidth - bigsize)/2,
			ScreenHeight - B_FONTSIZE - 20,
			"%5.1fkg", dispWeight );

		if( RT_Log.IsLogging() ){
			RT_Log.Add( lp->Point[0], lp->Point[1], lp->Point[2], lp->Point[3], dispWeight );
		}
	}
}






void AppWindow::DrawBalanceBar( LocalParam* lp )
{
	struct bar_pos {
		int	x, y;
		int	basex;
	};
	const int	width= 100;
	const int	height= 48;
	const int	xoffset= 16;
	int			yoffset= ScreenHeight - ( height * 2 + 16);

	int			rightpos= ScreenWidth - xoffset - width;
	const int	leftpos= xoffset;
	int			uppos= yoffset;
	int			downpos= yoffset + height + 10;
	bar_pos	_Pos[]= {
	{	rightpos,		uppos,		1,	},
	{	rightpos,		downpos,	1,	},
	{	leftpos,		uppos,		0,	},
	{	leftpos,		downpos,	0,	},
	};

	const bar_pos*	pp= _Pos;
	for( int i= 0 ; i< 4 ; i++, pp++ ){
		int	size= static_cast<int>( width * lp->Point[i] / 136.0f );
		BitBlt( hBack,
				pp->x + (width - size) * pp->basex,
				pp->y,
				size,
				height,
				NULL, 0, 0, WHITENESS );
	}
	DrawMeter( lp );
}



void AppWindow::DrawBox( int x, int y, int w, int h )
{
	SelectObject( hBack, hBoxPen );

	POINT	pbox[5];
	pbox[0].x= x;
	pbox[0].y= y;
	pbox[1].x= x + w;
	pbox[1].y= y;
	pbox[2].x= x + w;
	pbox[2].y= y + h;
	pbox[3].x= x;
	pbox[3].y= y + h;
	pbox[4].x= x;
	pbox[4].y= y;
	Polyline(
		hBack,
		pbox,
		5
		);
}




void AppWindow::DrawLine(
		const POINT*	plist,
		int	pcount
		)
{
	Polyline(
		hBack,
		plist,
		pcount
		);
}


void AppWindow::Poly( int basex, int basey,
			const flMatrix* mat,
			const flVect3* ipos,
			flVect3* vpos,
			int vcount,
			const int* _index,
			int icount
			)
{
	for( int i= 0 ; i< vcount ; i++ ){
		mat->Transformation( &vpos[i], &ipos[i] );
		float	rhv= 32.0f/ vpos[i].z;
		vpos[i].x=  vpos[i].x * rhv;
		vpos[i].y= -vpos[i].y * rhv;
	}
	for( int i= 0 ; i< icount ; i+= 4 ){
		POINT	point[4];
		POINT*	pp= point;
		int	i0= _index[i+0];
		int	i1= _index[i+1];
		int	i2= _index[i+2];
		int	i3= _index[i+3];
		pp->x= basex + static_cast<int>( vpos[i0].x );
		pp->y= basey + static_cast<int>( vpos[i0].y );
		pp++;
		pp->x= basex + static_cast<int>( vpos[i1].x );
		pp->y= basey + static_cast<int>( vpos[i1].y );
		pp++;
		pp->x= basex + static_cast<int>( vpos[i2].x );
		pp->y= basey + static_cast<int>( vpos[i2].y );
		pp++;
		pp->x= basex + static_cast<int>( vpos[i3].x );
		pp->y= basey + static_cast<int>( vpos[i3].y );
		pp++;

		Polygon(
			hBack,
			point,
			4
				);
	}
}




void AppWindow::DrawMeter( LocalParam* lp )
{
	struct _Point {
		float	x;
		float	y;
		float	z;
	};

	static _Point	_meter[]= {
		{	(- 5.0f),		(-10.0f),		(0.0f),	},
		{	(  5.0f),		(-10.0f),		(0.0f),	},
		{	( -1.0f),		(110.0f),		(0.0f),	},
		{	(  1.0f),		(110.0f),		(0.0f),	},
	};
	static int		_index[]= {
		0, 1, 3, 2,
	};

	flMatrix	matrix;
	matrix.SetIdentity();

	// 0-136
	// -120 .. +120
	float	angle= flMath::DEGtoRAD( (240.0f)* lp->TotalWeight / (136.0f) - (120.0f) );
	matrix.RotationZD( -angle );

	matrix._43= (20.0f);

	SelectObject( hBack, hMeterColor );
	SelectObject( hBack, hMeterPen );

	flVect3	_vect[4];
	Poly(
		ScreenWidth/2,
		240 + 50 - 80,
		&matrix,
		reinterpret_cast<flVect3*>( _meter ),
		_vect,
		4,
		_index,
		4 );

}




void AppWindow::DrawBMIBar( LocalParam* lp )
{
	SelectObject( hBack, hFont );

	// 60
	// 120 - 200
	// 12 .. 22 .. 32 .. 42
	// kg / len
	const int	MAX_BMI_AREA= 27;
	const int	offsetx= 64;
	const int	offsetxr= 10;
	int			barwidth= ScreenWidth - offsetx + offsetxr;
	const int	offsety= 250;
	const int	barsize= 10;
	const int	STARTBMI= 15;
	const int	MAXBMI= STARTBMI + MAX_BMI_AREA;
	const int	ypos_bmi= offsety + barsize;
	const int	ypos_str= offsety - S_FONTSIZE*1;
	POINT	plist[MAX_BMI_AREA*2*2];
	POINT*	pp= plist;
	for( int i= 0 ; i< MAX_BMI_AREA+1 ; i++ ){
		int	xpos= offsetx + barwidth * i / MAX_BMI_AREA;
		plist[1].x=
		plist[0].x= xpos;
		plist[0].y= offsety;
		plist[1].y= offsety + barsize;
		DrawLine( plist, 2 );
		float	startStature= sqrt(lp->BigWeight / (i+STARTBMI) ) * 100;
		Print(
			xpos - S_FONTSIZE/2 + 1,
			ypos_bmi,
				"%d", i+ STARTBMI );
		if( lp->BigWeight >= (2.0f) ){
			Print(
				xpos - S_FONTSIZE/2 - S_FONTSIZE/4,
				ypos_str,
					"%3d", static_cast<int>( startStature ) );
		}
	}
	Print(
		4,
		ypos_bmi,
			"BMI" );
	Print(
		4,
		ypos_str,
			"身長(cm)" );
}






//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



void* AppWindow::_AddDeviceFunc( void* arg )
{
	flMESSAGE( "BeginAdd" );
	AppWindow*	_This= reinterpret_cast<AppWindow*>( arg );
	flBluetoothLib*	iBluetooth= flBluetoothLib::Create();
	if( iBluetooth->AddDevice( 5 ) ){
		_This->SetFlag( FLAG_UPDATE );
		flMESSAGE( "Wait" );
		Sleep( 1000 * 10 );
		_This->SetFlag( FLAG_UPDATE2 );
		flMESSAGE( "Update" );
		_This->iWiiLib->Update();
		_This->CalibrationAll();
	}
	iBluetooth->Release();
	//_This->_WorkThread.Exit( 0 );
	flMESSAGE( "EndAdd" );
	_This->ResetFlag( FLAG_UPDATE | FLAG_UPDATE2 | FLAG_ADDDEVICE );
	return	NULL;
}

void AppWindow::AddDevice()
{
	SetFlag( FLAG_ADDDEVICE );
	_WorkThread.Quit();
	_WorkThread.Init();
	_WorkThread.CreateThread( _AddDeviceFunc, this );
}


void AppWindow::CalibrationAll()
{
	for( int i= 0 ; i< MAX_LOCALPARAM ; i++ ){
		Local[i].Calibration();
	}
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void AppWindow::Render_Active()
{
	int	devcount= iWiiLib->GetDeviceCount();
	if( devcount >= MAX_LOCALPARAM ){
		devcount= MAX_LOCALPARAM;
	}
//	DeviceCount= devcount;

	GlobalBalance= NULL;
	int	busyCount= 0;
	if( StartupWait ){
		busyCount= StartupWait--;
	}
	for( int i= 0 ; i< devcount ; i++ ){
		LocalParam*	lp= &Local[ i ];
		flWiiLibDevice*	dev= iWiiLib->GetDevice( i );
		lp->iDevice= NULL;
		if( !dev->IsValid() ){
			continue;
		}
		if( dev->GetReadType() ){
			busyCount++;
			continue;
		}
		lp->iDevice= dev;
		int	type= dev->GetExtensionType();
		lp->DeviceType= type;
		switch( type ){
		case flWiiLibDevice::EXTENSIONTYPE_BALANCE:
			UpdateBalance( dev, lp );
			GlobalBalance= lp;
			break;
		}
	}

	if( GlobalBalance ){
		DrawFix( GlobalBalance );
		DrawWeight( GlobalBalance );
		DrawBalanceBar( GlobalBalance );
		DrawBMIBar( GlobalBalance );
	}else{
		LocalParam	zero;
		memset( &zero, 0, sizeof(LocalParam) );
		DrawWeight( &zero );
		DrawBalanceBar( &zero );
		if( !busyCount ){
			DrawNotFound();
		}
	}
}



void AppWindow::DrawMessage( const char** message )
{
	int	pos= 64 + M_FONTSIZE * 1;
	for(; *message ; pos+= M_FONTSIZE*2 ){
		Print( 32, pos, *message++ );
	}
}



void AppWindow::DrawBluetooth()
{
	SetFontColor( WALKTEXT_COLOR );
	SelectObject( hBack, hMidFont );

	const static char*	_msg2[]= {
		"デバイス情報取得中です。",
		"",
		"(登録したら最初は 0kg 調整してください)",
		NULL,
	};
	const static char*	_msg_update[]= {
		"デバイス情報の更新待ちをしています。",
		"しばらくお待ちください。",
		"(登録したら最初は 0kg 調整してください)",
		NULL,
	};
	const static char*	_msg_sync[]= {
		"コントローラペアリング待ち",
		"バランスWiiボード の",
		"SYNC ボタンを押してください",
		NULL,
	};

	if( TestFlag( FLAG_UPDATE2 ) ){
		DrawMessage( _msg2 );
	}else if( TestFlag( FLAG_UPDATE ) ){
		DrawMessage( _msg_update );
	}else{
		DrawMessage( _msg_sync );
	}
}




void AppWindow::DrawNotFound()
{
	SetFontColor( WALKTEXT_COLOR );
	SelectObject( hBack, hMidFont );

	const static char*	_msg2[]= {
		"バランスWiiボード が見つかりません。",
		"ペアリング＋接続 してください。",
		"(詳しくはマニュアルをご参照ください)",
		NULL,
	};
	DrawMessage( _msg2 );
}


void AppWindow::Render()
{
	hDC= GetDC( *this );

	Clear();
	SetFontColor( TEXT_COLOR );
	SetBGColor( BG_COLOR );

	SelectObject( hBack, hFont );

	Print( 10, 10,
		"バランスWiiボードで 体重計 (http://wlog.flatlib.jp/)" );


	if( TestFlag( FLAG_ADDDEVICE ) ){
		DrawBluetooth();
	}else{
		Render_Active();
	}

	BlinkTimer++;

	BitBlt( hDC, 0, 0, ScreenWidth, ScreenHeight, hBack, 0, 0, SRCCOPY );
	ReleaseDC( *this, hDC );
	Sleep( 33 );
}





int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrev, LPTSTR cmdline, int nCmdShow )
{
	AppWindow*	wp= new AppWindow( hInstance );
	wp->Open(
			NULL,
			WIIBALANCE_TITLE " " WIIBALANCE_VERSION,
			NULL,
			IDR_MENU1
		);
	wp->SetStyle(
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			WS_OVERLAPPEDWINDOW,
			WS_EX_APPWINDOW,
			WIIBALANCE_TITLE " " WIIBALANCE_VERSION
		);
	wp->Init();

//	hAccelTable= LoadAccelerators( hInstance, (LPCTSTR)IDC_FVIEW );
	MSG	msg;
	for(;;){
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ){
			if( msg.message == WM_QUIT ){
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}else{
			wp->Render();
		}
	}

	wp->Quit();
	ZDELETE( wp );

	return	0;
}




