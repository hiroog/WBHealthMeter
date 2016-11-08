// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#ifndef	FL_FLHIDLIB_H_
#define	FL_FLHIDLIB_H_



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class flHidLib {
public:
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	typedef unsigned int	HIDP_PREPARSED_DATA;
	struct HIDD_ATTRIBUTES {
		unsigned long	Size;
		unsigned short	VendorID;
		unsigned short	ProductID;
		unsigned short	VersionNumber;
	};
	struct HIDP_CAPS {
		unsigned short	Usage;
		unsigned short	UsagePage;
		unsigned short	InputReportByteLength;
		unsigned short	OutputReportByteLength;
		unsigned short	FeatureReportByteLength;
		unsigned short	Reserved[17];
		unsigned short	NumberLinkCollectionNodes;
		unsigned short	NumberInputButtonCaps;
		unsigned short	NumberInputValueCaps;
		unsigned short	NumberInputDataIndices;
		unsigned short	NumberOutputButtonCaps;
		unsigned short	NumberOutputValueCaps;
		unsigned short	NumberOutputDataIndices;
		unsigned short	NumberFeatureputButtonCaps;
		unsigned short	NumberFeatureputValueCaps;
		unsigned short	NumberFeatureputDataIndices;
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	enum {
		DIGCF_DEFAULT			= 0x00000001,
		DIGCF_PRESENT			= 0x00000002,
		DIGCF_ALLCLASSES		= 0x00000004,
		DIGCF_PROFILE			= 0x00000008,
		DIGCF_DEVICEINTERFACE	= 0x00000010,
	};
	typedef PVOID	HDEVINFO;
	struct SP_DEVINFO_DATA {
		DWORD	cbSize;
		GUID	ClassGuid;
		DWORD	DevInst;
		uintptr_t	Reserved;
	};
	struct SP_DEVICE_INTERFACE_DATA {
		DWORD	cbSize;
		GUID	InterfaceClassGuid;
		DWORD	Flags;
		uintptr_t	Reserved;
	};
	struct SP_DEVICE_INTERFACE_DETAIL_DATA {
		DWORD	cbSize;
		char	DevicePath[1];
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
private:
	static void*	hModule;
	static void*	sModule;
public:
	static int	InitModule();
	static void	QuitModule();
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	static BOOLEAN	HidD_GetAttributes( HANDLE, HIDD_ATTRIBUTES* );
	static NTSTATUS	HidP_GetCaps( HIDP_PREPARSED_DATA*, HIDP_CAPS* );
	static BOOLEAN	HidD_GetPreparsedData( HANDLE, HIDP_PREPARSED_DATA** );
	static BOOLEAN	HidD_FreePreparsedData( HIDP_PREPARSED_DATA* );
	static BOOLEAN	HidD_SetOutputReport( HANDLE, void*, unsigned long );
	static void		HidD_GetHidGuid( LPGUID );
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	static HDEVINFO	SetupDiGetClassDevs( const GUID*, const char* enumerator, HWND parent, DWORD flag );
	static BOOL		SetupDiEnumDeviceInterfaces( HDEVINFO, SP_DEVINFO_DATA*, const GUID*, DWORD index, SP_DEVICE_INTERFACE_DATA* );
	static BOOL		SetupDiGetDeviceInterfaceDetail( HDEVINFO, SP_DEVICE_INTERFACE_DATA*, SP_DEVICE_INTERFACE_DETAIL_DATA*, DWORD, DWORD*, SP_DEVINFO_DATA* );
	static BOOL		SetupDiDestroyDeviceInfoList( HDEVINFO );
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
};


#endif


