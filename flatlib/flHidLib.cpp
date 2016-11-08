// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4 noet:

#include	"flCore.h"
#include	<windows.h>
#include	"flHidLib.h"



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef BOOLEAN _flSTDCALL	HIDD_GETATTRIBUTES ( HANDLE, flHidLib::HIDD_ATTRIBUTES* );

static HIDD_GETATTRIBUTES*	func_HIDD_GetAttributes= NULL;

BOOLEAN flHidLib::HidD_GetAttributes( HANDLE handle, HIDD_ATTRIBUTES* attr )
{
	return	(*func_HIDD_GetAttributes)( handle, attr );
}


//-----------------------------------------------------------------------------

typedef NTSTATUS _flSTDCALL	HIDP_GETCAPS ( flHidLib::HIDP_PREPARSED_DATA*, flHidLib::HIDP_CAPS* );

static HIDP_GETCAPS*	func_HIDP_GetCaps= NULL;

NTSTATUS flHidLib::HidP_GetCaps( HIDP_PREPARSED_DATA* pdp, HIDP_CAPS* caps )
{
	return	(*func_HIDP_GetCaps)( pdp, caps );
}


//-----------------------------------------------------------------------------

typedef BOOLEAN _flSTDCALL	HIDD_FREEPREPARSEDDATA ( flHidLib::HIDP_PREPARSED_DATA* );

static HIDD_FREEPREPARSEDDATA*	func_HIDD_FreePreparsedData= NULL;

BOOLEAN flHidLib::HidD_FreePreparsedData( HIDP_PREPARSED_DATA* pdp )
{
	return	(*func_HIDD_FreePreparsedData)( pdp );
}


//-----------------------------------------------------------------------------

typedef BOOLEAN _flSTDCALL	HIDD_GETPREPARSEDDATA ( HANDLE, flHidLib::HIDP_PREPARSED_DATA** );

static HIDD_GETPREPARSEDDATA*	func_HIDD_GetPreparsedData= NULL;

BOOLEAN flHidLib::HidD_GetPreparsedData( HANDLE handle, HIDP_PREPARSED_DATA** pdp )
{
	return	(*func_HIDD_GetPreparsedData)( handle, pdp );
}


//-----------------------------------------------------------------------------

typedef BOOLEAN _flSTDCALL	HIDD_SETOUTPUTREPORT ( HANDLE, void*, unsigned long );

static HIDD_SETOUTPUTREPORT*	func_HIDD_SetOutputReport= NULL;

BOOLEAN flHidLib::HidD_SetOutputReport( HANDLE handle, void* data, unsigned long length )
{
	return	(*func_HIDD_SetOutputReport)( handle, data, length );
}


//-----------------------------------------------------------------------------

typedef void _flSTDCALL	HIDD_GETHIDGUID ( LPGUID );

static HIDD_GETHIDGUID*	func_HIDD_GetHidGuid= NULL;

void flHidLib::HidD_GetHidGuid( LPGUID guid )
{
	(*func_HIDD_GetHidGuid)( guid );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


typedef flHidLib::HDEVINFO _flSTDCALL	SETUPDIGETCLASSDEVS ( const GUID*, const char* enumerator, HWND parent, DWORD flag );

static SETUPDIGETCLASSDEVS*	func_SetupDiGetClassDevs= NULL;

flHidLib::HDEVINFO flHidLib::SetupDiGetClassDevs( const GUID* guid, const char* enumerator, HWND parent, DWORD flag )
{
	return	(*func_SetupDiGetClassDevs)( guid, enumerator, parent, flag );
}



//-----------------------------------------------------------------------------

typedef BOOL _flSTDCALL	SETUPDIENUMDEVICEINTERFACES ( flHidLib::HDEVINFO, flHidLib::SP_DEVINFO_DATA*, const GUID*, DWORD, flHidLib::SP_DEVICE_INTERFACE_DATA* );

static SETUPDIENUMDEVICEINTERFACES*	func_SetupDiEnumDeviceInterfaces= NULL;

BOOL flHidLib::SetupDiEnumDeviceInterfaces( HDEVINFO hinfo, SP_DEVINFO_DATA* sp, const GUID* guid, DWORD index, SP_DEVICE_INTERFACE_DATA* data )
{
	return	(*func_SetupDiEnumDeviceInterfaces)( hinfo, sp, guid, index, data );
}



//-----------------------------------------------------------------------------

typedef BOOL _flSTDCALL	SETUPDIGETDEVICEINTERFACEDETAIL ( flHidLib::HDEVINFO, flHidLib::SP_DEVICE_INTERFACE_DATA*, flHidLib::SP_DEVICE_INTERFACE_DETAIL_DATA*, DWORD, DWORD*, flHidLib::SP_DEVINFO_DATA* );

static SETUPDIGETDEVICEINTERFACEDETAIL* func_SetupDiGetDeviceInterfaceDetail= NULL;

BOOL flHidLib::SetupDiGetDeviceInterfaceDetail( HDEVINFO hinfo, SP_DEVICE_INTERFACE_DATA* data, SP_DEVICE_INTERFACE_DETAIL_DATA* detail, DWORD size, DWORD* rsize, SP_DEVINFO_DATA* sp )
{
	return	(*func_SetupDiGetDeviceInterfaceDetail)( hinfo, data, detail, size, rsize, sp );
}


//-----------------------------------------------------------------------------

typedef BOOL _flSTDCALL	SETUPDIDESTROYDEVICEINFOLIST ( flHidLib::HDEVINFO );

static SETUPDIDESTROYDEVICEINFOLIST*	func_SetupDiDestroyDeviceInfoList= NULL;

BOOL flHidLib::SetupDiDestroyDeviceInfoList( HDEVINFO hinfo )
{
	return	(*func_SetupDiDestroyDeviceInfoList)( hinfo );
}





//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void*	flHidLib::hModule= NULL;
void*	flHidLib::sModule= NULL;

static void* GetAddress( void* handle, const char* fname )
{
	HMODULE	hm= reinterpret_cast<HMODULE>( handle );
	void*	func= reinterpret_cast<void*>( GetProcAddress( hm, fname ) );
#if 0
if(func){
printf("GETFUNC:%s\n", fname );
}
#endif
	if( !func ){
		flERROR( "ERROR: func %s\n", fname );
	}
	return	func;
}



int flHidLib::InitModule()
{
	HMODULE	HIDDLLModuleHandle= ::LoadLibrary( "hid.dll" );
	HMODULE	SETDLLModuleHandle= ::LoadLibrary( "setupapi.dll" );

	if( !HIDDLLModuleHandle || !SETDLLModuleHandle ){
		QuitModule();
		return	FALSE;
	}
	hModule= reinterpret_cast<void*>( HIDDLLModuleHandle );
	sModule= reinterpret_cast<void*>( SETDLLModuleHandle );

#if 0
	GETEXTATTRIB*	pget= (GETEXTATTRIB*)GetProcAddress( HIDDLLModuleHandle, "HidP_GetExtendAttributes" );
	INITREPORT*	pin= (INITREPORT*)GetProcAddress( HIDDLLModuleHandle, "HidP_InitializeReportForID" );
#endif

	func_HIDD_GetAttributes= reinterpret_cast<HIDD_GETATTRIBUTES*>( GetAddress(	hModule, "HidD_GetAttributes" ) );
	func_HIDP_GetCaps= reinterpret_cast<HIDP_GETCAPS*>( GetAddress(	hModule, "HidP_GetCaps" ) );
	func_HIDD_FreePreparsedData= reinterpret_cast<HIDD_FREEPREPARSEDDATA*>( GetAddress(	hModule, "HidD_FreePreparsedData" ) );
	func_HIDD_GetPreparsedData= reinterpret_cast<HIDD_GETPREPARSEDDATA*>( GetAddress(	hModule, "HidD_GetPreparsedData" ) );
	func_HIDD_SetOutputReport= reinterpret_cast<HIDD_SETOUTPUTREPORT*>( GetAddress(	hModule, "HidD_SetOutputReport" ) );
	func_HIDD_GetHidGuid= reinterpret_cast<HIDD_GETHIDGUID*>( GetAddress(	hModule, "HidD_GetHidGuid" ) );


	func_SetupDiGetClassDevs= reinterpret_cast<SETUPDIGETCLASSDEVS*>( GetAddress(	sModule, "SetupDiGetClassDevsA" ) );
	func_SetupDiEnumDeviceInterfaces= reinterpret_cast<SETUPDIENUMDEVICEINTERFACES*>( GetAddress(	sModule, "SetupDiEnumDeviceInterfaces" ) );
	func_SetupDiGetDeviceInterfaceDetail= reinterpret_cast<SETUPDIGETDEVICEINTERFACEDETAIL*>( GetAddress(	sModule, "SetupDiGetDeviceInterfaceDetailA" ) );
	func_SetupDiDestroyDeviceInfoList= reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>( GetAddress(	sModule, "SetupDiDestroyDeviceInfoList" ) );
	return	TRUE;
}




void flHidLib::QuitModule()
{
	if( hModule ){
		HMODULE	HIDDLLModuleHandle= reinterpret_cast<HMODULE>( hModule );
		::FreeLibrary( HIDDLLModuleHandle );
		hModule= NULL;
	}
	if( sModule ){
		HMODULE	SETDLLModuleHandle= reinterpret_cast<HMODULE>( sModule );
		::FreeLibrary( SETDLLModuleHandle );
		sModule= NULL;
	}
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

