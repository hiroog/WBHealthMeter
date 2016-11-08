// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4 noet:

//#define	_CRT_SECURE_NO_DEPRECATE
//#pragma	warning( disable : 4819 )
//#pragma	warning( disable : 4995 )


#include	"flCore.h"
#include	<Bthdef.h>
#include	<BluetoothAPIs.h>
#include	"flBluetoothLib.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


class flBluetoothLibImp : public flBluetoothLib {
public:
	flBluetoothLibImp()
	{
	}
	~flBluetoothLibImp()
	{
		Quit();
	}
	void	Quit();
	void	Release();
	int		AddDevice( int timeout );
};



void flBluetoothLibImp::Quit()
{
}


void flBluetoothLibImp::Release()
{
	if( this ){
		Quit();
		delete this;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

flBluetoothLib* flBluetoothLib::Create()
{
	return	new flBluetoothLibImp;
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


int flBluetoothLibImp::AddDevice( int timeout )
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS	search;
	memset( &search, 0, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) );
	search.dwSize= sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	//search.fReturnAuthenticated= TRUE;
	search.fReturnRemembered= TRUE;
	//search.fReturnConnected= TRUE;
	search.fReturnUnknown= TRUE;
	search.fIssueInquiry= TRUE;
	search.cTimeoutMultiplier= timeout;


	BLUETOOTH_DEVICE_INFO	dinfo;
	memset( &dinfo, 0, sizeof(BLUETOOTH_DEVICE_INFO) );
	dinfo.dwSize= sizeof(BLUETOOTH_DEVICE_INFO);

	const wchar_t*	_BT_NAME0=	L"Nintendo RVL-CNT-01";
	const wchar_t*	_BT_NAME1=	L"Nintendo RVL-WBC-01";

	HBLUETOOTH_DEVICE_FIND	shandle;
	int	ret= 0;
	if( shandle= BluetoothFindFirstDevice( &search, &dinfo ) ){
		do{
			if( wcscmp( _BT_NAME0, dinfo.szName )
							&& wcscmp( _BT_NAME1, dinfo.szName ) ){
				continue;
			}

			if( !dinfo.fConnected ){
				BluetoothRemoveDevice( &dinfo.Address );
#if 0
				BluetoothSetServiceState(
						hradio,
						&dinfo,
						&HumanInterfaceDeviceServiceClass_UUID,
						BLUETOOTH_SERVICE_DISABLE
						);
#endif
				int	bret= BluetoothSetServiceState(
						NULL,
						&dinfo,
						&HumanInterfaceDeviceServiceClass_UUID,
						BLUETOOTH_SERVICE_ENABLE
						);
#if _flDEBUG
				switch( bret ){
				case ERROR_SUCCESS:
					flMESSAGE( "success\n" );
					break;
				case ERROR_INVALID_PARAMETER:
					flMESSAGE( "invalid params\n" );
					break;
				case ERROR_SERVICE_DOES_NOT_EXIST:
					flMESSAGE( "not exist\n" );
					break;
				case E_INVALIDARG:
					flMESSAGE( "e_invalid arg\n" );
					break;
				}
#endif
				ret++;
			}

		}while( BluetoothFindNextDevice( shandle, &dinfo) );
		BluetoothFindDeviceClose( shandle );
	}else{
#if _flDEBUG
		int	err= GetLastError();
		if( err == ERROR_INVALID_PARAMETER ){
			flMESSAGE( "err invalid\n" );
		}
		if( err == ERROR_REVISION_MISMATCH ){
			flMESSAGE( "err mismatch\n" );
		}
#endif
	}
	return	ret;
}





