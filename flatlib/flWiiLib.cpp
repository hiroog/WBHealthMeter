// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#define	_CRT_SECURE_NO_DEPRECATE
#pragma	warning( disable : 4819 )
#pragma	warning( disable : 4995 )

#include	"flCore.h"
#include	<wtypes.h>
#include	<Vector>
#include	<process.h>
#include	"flWiiLib.h"
#include	"flHidLib.h"
#include	"flWiiRemote.h"
#include	"flSimpleMath.h"


#define	ENABLE_SIXAXIS		0


#if ENABLE_SIXAXIS
# include	"flSixAxis.h"
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define	__USE_BATTERYUPDATE__	0
#define	__OVERLAP_WRITE			1
#define	__USE_WRITEFILE__		0

#define	__USE_RAWDATA			1
#define	__USE_APICHECKER		1

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// debug move to flWIiLibCommon.h
//#define	__USE_DUMPTEST			0
//#define	__USE_DUMPTEST2			0
//#define	__USE_SENDDUMP			1




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


class flWiiLibImp : public flWiiLib {
private:
	enum {
		MAX_THREAD_EVENT	= 2,
	};
private:
	std::vector<flWiiLibDevice*>		DevList;
//	int			ApiFlag;
	int			rVid;
	int			rPid;
	HANDLE		_Handle;
	HANDLE		_Event[MAX_THREAD_EVENT];	// 0=Exit, 1=Update
private:
	int 	IsTimeoutDevice();
	void 	RetryMode();
	void 	FixMode();
	int 	_OpenTestHidDevice( const char* path, int vid, int pid );
	static unsigned int	WINAPI	_DeviceLoopEntry( void* arg );
public:
	flWiiLibImp()
	{
		_Handle= INVALID_HANDLE_VALUE;
		for( int i= 0 ; i< MAX_THREAD_EVENT ; i++ ){
			_Event[i]= INVALID_HANDLE_VALUE;
		}
//		ApiFlag= 0;
	}
	~flWiiLibImp()
	{
		Quit();
	}
	template<class T>
	int 	AppendDevice( const char* path );
	template<class T>
	int 	CheckDeviceOpen( const char* path );
	int 	Init();
	void 	Quit();
	int 	FindDevicePath( unsigned int vid, unsigned int pid );
	void 	DeviceLoop();
	void 	WaitExit();
	// interface
	void 	Update();
	void 	Release();
//	void 	DumpAll();
	void 	BeginLoop( unsigned int vid, unsigned int pid );
#if __USE_APICHECKER
	void 	APICheckWait( flWiiLibDevice* dev );
#endif
	int 	GetDeviceCount() const
	{
		return	static_cast<int>( DevList.size() );
	}
	flWiiLibDevice* 	GetDevice( unsigned int id )
	{
		//std::vector<flWiiDevice*>::iterator	it= DevList.begin();
		if( id >= 0 && id < DevList.size() ){
			return	DevList[ id ];
		}
		return	NULL;
	}
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if 0
void flWiiLibImp::DumpAll()
{
	std::vector<flWiiDevice*>::iterator	it= DevList.begin();
	for(; it != DevList.end() ; it++ ){
		flWiiDevice*	wp= *it;
		if( wp ){
			wp->Dump();
		}
	}
}
#endif

int flWiiLibImp::Init()
{
	if( ! flHidLib::InitModule() ){
		flWiiLibCommon::DebugPrint( "ERROR: InitModule\n" );
		return	FALSE;
	}
	return	TRUE;
}


void flWiiLibImp::Quit()
{
	{
		std::vector<flWiiLibDevice*>::iterator	it= DevList.begin();
		for(; it != DevList.end() ; it++ ){
			flWiiLibDevice*	wp= *it;
			if( wp ){
				wp->StopReport();
			}
		}
	}


	DevList.clear();
	if( _Handle != INVALID_HANDLE_VALUE ){
		WaitExit();
		CloseHandle( _Handle );
		_Handle= INVALID_HANDLE_VALUE;
	}

	for( int i= 0 ; i< MAX_THREAD_EVENT ; i++ ){
		if( _Event[i] != INVALID_HANDLE_VALUE ){
			CloseHandle( _Event[i] );
			_Event[i]= INVALID_HANDLE_VALUE;
		}
	}

	std::vector<flWiiLibDevice*>::iterator	it= DevList.begin();
	for(; it != DevList.end() ; it++ ){
		flWiiLibDevice*	wp= *it;
		if( wp ){
			wp->Quit();
			delete wp;
			wp= NULL;
			*it= NULL;
		}
	}
	DevList.clear();

	flHidLib::QuitModule();
}


void flWiiLibImp::Release()
{
	if( this ){
		Quit();
		delete this;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class T>
int flWiiLibImp::AppendDevice( const char* path )
{
	T*	wp= new T;
	int	id= static_cast<int>( DevList.size() );
	if( !wp->Open( path, id ) ){
		delete	wp;
		return	FALSE;
	}
	flWiiLibCommon::DebugPrint( "AddOpen %s", path );

#if __USE_APICHECKER
	flMESSAGE( "API Check Start" );
	APICheckWait( wp );
	flMESSAGE( "API Check End" );
#endif

	wp->Start();

	DevList.push_back( wp );
	return	TRUE;
}


template<class T>
int flWiiLibImp::CheckDeviceOpen( const char* path )
{
	std::vector<flWiiLibDevice*>::iterator	it= DevList.begin();
	for( int i= 0 ; it != DevList.end() ; it++, i++ ){
		flWiiLibDevice*	wp= *it;
		if( !wp ){
			continue;
		}
		if( wp->_EqName( path ) ){
			if( wp->IsValid() ){
				return	FALSE;
			}
			wp->Open( path, i );
			wp->Start();
			flWiiLibCommon::DebugPrint( "ReOpen %s", path );
			return	FALSE;
		}
	}
	return	AppendDevice<T>( path );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int flWiiLibImp::_OpenTestHidDevice( const char* path, int vid, int pid )
{
	HANDLE	hdev= CreateFile(
					path,
					GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ|FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					0,
					NULL
				);
	int	found= FALSE;

	if( hdev == INVALID_HANDLE_VALUE ){
		return	FALSE;
	}

	flHidLib::HIDD_ATTRIBUTES	attr;
	if( flHidLib::HidD_GetAttributes( hdev, &attr ) ){
#if __USE_SENDDUMP
flWiiLibCommon::DebugPrint( "vid=%x pid=%d", attr.VendorID, attr.ProductID );
#endif
		if( attr.VendorID == vid && attr.ProductID == pid ){
			found= TRUE;
#if __USE_SENDDUMP
flWiiLibCommon::DebugPrint( "Found vid=%x pid=%x", attr.VendorID, attr.ProductID );
#endif
		}
	}

	CloseHandle( hdev );
	return	found;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int flWiiLibImp::FindDevicePath( unsigned int vid, unsigned int pid )
{
//4d1e55b2 f16f 11cf  88 cb 00 11 11 00  0 30
	GUID	hidGuid;
	flHidLib::HidD_GetHidGuid( &hidGuid );

//flWiiLibCommon::DebugPrint( "FindDevice\n" );

#if 0
flWiiLibCommon::DebugPrint(
"%08x %04x %04x  %02x %02x %02x %02x %02x %02x %2x %02x\n",
	hidGuid.Data1,
	hidGuid.Data2,
	hidGuid.Data3,
	hidGuid.Data4[0],
	hidGuid.Data4[1],
	hidGuid.Data4[2],
	hidGuid.Data4[3],
	hidGuid.Data4[4],
	hidGuid.Data4[5],
	hidGuid.Data4[6],
	hidGuid.Data4[7] );
#endif

	flHidLib::HDEVINFO	hdevinfo;
	hdevinfo= flHidLib::SetupDiGetClassDevs(
				&hidGuid,
				NULL,
				NULL,
				flHidLib::DIGCF_PRESENT
				|flHidLib::DIGCF_DEVICEINTERFACE
				|flHidLib::DIGCF_ALLCLASSES
			);


//	flWiiDevice::ResetAPIFlag( APIFLAG_SELECTED|APIFLAG_WRITEMODE );


	flHidLib::SP_DEVICE_INTERFACE_DETAIL_DATA*	funcClassData= NULL;

	const int	DEVICELIST_LIMIT= 32;
	int			retCode= TRUE;
	for( int i= 0 ; i< DEVICELIST_LIMIT ; i++ ){
		flHidLib::SP_DEVICE_INTERFACE_DATA	deviceInfoData;
		memset( &deviceInfoData, 0,
							sizeof(flHidLib::SP_DEVICE_INTERFACE_DATA) );
		deviceInfoData.cbSize= sizeof( flHidLib::SP_DEVICE_INTERFACE_DATA );
		if( flHidLib::SetupDiEnumDeviceInterfaces(
				hdevinfo,
				0,
				&hidGuid,
				i,
				&deviceInfoData
				) ){
			ULONG	reqLength= 0;
			flHidLib::SetupDiGetDeviceInterfaceDetail(
					hdevinfo,
					&deviceInfoData,
					NULL,
					0,
					&reqLength,
					NULL );

#if __USE_SENDDUMP
flWiiLibCommon::DebugPrint( "**START\nreq=%d %d", reqLength, sizeof(flHidLib::SP_DEVICE_INTERFACE_DETAIL_DATA) );
#endif

			funcClassData=
				reinterpret_cast<flHidLib::SP_DEVICE_INTERFACE_DETAIL_DATA*>(
									flWiiLibCommon::dAlloc( reqLength ) );
			//funcClassData->cbSize= reqLength;
			if( sizeof(void*) == 4 ){
				funcClassData->cbSize= 5;
			}else{
				funcClassData->cbSize= 
					sizeof(flHidLib::SP_DEVICE_INTERFACE_DETAIL_DATA);
			}
			if( !funcClassData ){
				retCode= FALSE;
				break;
			}
			if( !flHidLib::SetupDiGetDeviceInterfaceDetail(
					hdevinfo,
					&deviceInfoData,
					funcClassData,
					reqLength,
					&reqLength,
					NULL ) ){
				retCode= FALSE;
#if _flDEBUG
flWiiLibCommon::DebugPrint( "SetupDiGetDeviceInterfaceDetail ERROR\n" );
#endif
				break;
			}
#if __USE_SENDDUMP
flWiiLibCommon::DebugPrint( "OK= %d %d", i, reqLength );
flWiiLibCommon::DebugPrint( "%s", funcClassData->DevicePath );
#endif
			if( _OpenTestHidDevice( funcClassData->DevicePath, flWiiDevice::VENDOR_ID, flWiiDevice::PRODUCT_ID ) ){
				CheckDeviceOpen<flWiiDevice>( funcClassData->DevicePath );
#if ENABLE_SIXAXIS
			}else if( _OpenTestHidDevice( funcClassData->DevicePath, flSixAxis::VENDOR_ID, flSixAxis::PRODUCT_ID ) ){
				CheckDeviceOpen<flSixAxis>( funcClassData->DevicePath );
#endif
			}
			if( funcClassData ){
				flWiiLibCommon::dFree( funcClassData );
				funcClassData= NULL;
			}
		}else{
			if( GetLastError() == ERROR_NO_MORE_ITEMS ){
				break;
			}

flWiiLibCommon::DebugPrint( "SetupDiEnumDeviceInterfaces ERROR\n" );
		}
	}

	flHidLib::SetupDiDestroyDeviceInfoList( hdevinfo );


	if( funcClassData ){
		flWiiLibCommon::dFree( funcClassData );
		funcClassData= NULL;
	}
	return	retCode;
}




int flWiiLibImp::IsTimeoutDevice()
{
#if 0
	std::vector<flWiiDevice*>::iterator	it= DevList.begin();
	for(; it != DevList.end() ; it++ ){
		flWiiDevice*	wp= *it;
		if( wp ){
			if( wp->TestFlag( flWiiDevice::DEVICEFLAG_TIMEOUT ) ){
				return	TRUE;
			}
		}
	}
#endif
	return	FALSE;
}


void flWiiLibImp::RetryMode()
{
#if 0
	flMESSAGE( "RetryMode" );
	std::vector<flWiiDevice*>::iterator	it= DevList.begin();
	for(; it != DevList.end() ; it++ ){
		flWiiDevice*	wp= *it;
		if( wp ){
			if( wp->TestFlag( flWiiDevice::DEVICEFLAG_FIRSTREQ ) ){
				if( !wp->TestFlag( flWiiDevice::DEVICEFLAG_WRITEMODE ) ){
					wp->SetFlag( flWiiDevice::DEVICEFLAG_WRITEMODE );
					wp->ResetFlag( flWiiDevice::DEVICEFLAG_FIRSTREQ
								|flWiiDevice::DEVICEFLAG_TIMEOUT );
					wp->SendCommand( "\x11\x00", 2 );		// LED off
					wp->SendCommand( "\x15\x00", 2 );		// ReqStatus
				}
			}
		}
	}
#endif
}

void flWiiLibImp::FixMode()
{
#if 0
	std::vector<flWiiDevice*>::iterator	it= DevList.begin();
	for(; it != DevList.end() ; it++ ){
		flWiiDevice*	wp= *it;
		if( wp ){
			if( wp->TestFlag(
						flWiiDevice::DEVICEFLAG_FIRSTREQ
						|flWiiDevice::DEVICEFLAG_TIMEOUT ) ){
				wp->ResetFlag( flWiiDevice::DEVICEFLAG_FIRSTREQ
								|flWiiDevice::DEVICEFLAG_TIMEOUT );
			}
		}
	}
#endif
}



void flWiiLibImp::APICheckWait( flWiiLibDevice* dev )
{
#if __USE_APICHECKER
	if( dev->TestAPIFlag( APIFLAG_SELECTED ) ){
		return;
	}

	if( !dev->APIChecker() ){
		return;
	}

	DWORD	tout= 1000*2;	// 2sec
	switch( WaitForMultipleObjectsEx( 2, _Event, FALSE, tout, TRUE ) ){
	case WAIT_OBJECT_0:	// EXIT
	case WAIT_OBJECT_0+1:	// UPDATE
		break;
	case WAIT_TIMEOUT:
		dev->SetAPIFlag( APIFLAG_WRITEMODE|APIFLAG_SELECTED );
		dev->CancelIO();
		flMESSAGE( " --- [timeout] select API WriteFile" );
		break;
	case WAIT_IO_COMPLETION:
		dev->SetAPIFlag( APIFLAG_SELECTED );
		dev->ResetAPIFlag( APIFLAG_WRITEMODE );
		flMESSAGE( " --- [ret] select API SetOutputReport" );
		break;
	}
#endif
}



void flWiiLibImp::DeviceLoop()
{
	FindDevicePath( rVid, rPid );
//	unsigned int	_DTime= GetTickCount();
	for(;;){
		DWORD	tout= INFINITE;
#if 0 //__USE_WRITEFILE__
		if( IsTimeoutDevice() ){
			tout= 1000*2;
		}
#endif
		switch( WaitForMultipleObjectsEx( 2, _Event, FALSE, tout, TRUE ) ){
		case WAIT_OBJECT_0:	// EXIT
			flWiiLibCommon::DebugPrint( "Req Exit!!" );
			return;
		case WAIT_OBJECT_0+1:	// UPDATE
			{
				FindDevicePath( rVid, rPid );
#if 0
				const unsigned int	timeOut= 1000*5;
				flWiiLibCommon::DebugPrint( "Update!!\n" );
				unsigned int	ntime= GetTickCount();
				if( ntime > _DTime ){
					unsigned int	difft= ntime - _DTime;
					if( difft > timeOut ){
						FindDevicePath( rVid, rPid );
						_DTime= GetTickCount();
					}
				}
#endif
			}
			break;
		case WAIT_TIMEOUT:
//			flWiiLibCommon::DebugPrint( "Timeout" );
			flMESSAGE( "timeout" );
//			RetryMode();
			break;
		case WAIT_IO_COMPLETION:
#if __USE_DUMPTEST
			flMESSAGE( "io cmpl" );
#endif
#if 0 //__USE_WRITEFILE__
			FixMode();
#endif
			//flWiiLibCommon::DebugPrint( "IO Signal!!" );
			break;
		}
	}
}




unsigned int WINAPI flWiiLibImp::_DeviceLoopEntry( void* arg )
{
	flWiiLibImp*	_This= reinterpret_cast<flWiiLibImp*>( arg );
	_This->DeviceLoop();
	return	0;
}


void flWiiLibImp::BeginLoop( unsigned int vid, unsigned int pid )
{
	Init();

	rVid= vid;
	rPid= pid;
	for( int i= 0 ; i< MAX_THREAD_EVENT ; i++ ){
		_Event[i]= CreateEvent( NULL, FALSE, FALSE, NULL );
	}
	_Handle= reinterpret_cast<HANDLE>( _beginthreadex(
					NULL,
					NULL,
					_DeviceLoopEntry,
					reinterpret_cast<void*>( this ),
					NULL,
					NULL
				) );
}


void flWiiLibImp::WaitExit()
{
	SetEvent( _Event[0] );
	WaitForSingleObject( _Handle, INFINITE );
}


void flWiiLibImp::Update()
{
	SetEvent( _Event[1] );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

flWiiLib* flWiiLib::Create()
{
	return	new flWiiLibImp;
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


