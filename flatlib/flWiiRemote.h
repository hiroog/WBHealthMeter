// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#ifndef	FL_FLWIIREMOTE_H_
#define	FL_FLWIIREMOTE_H_


#define	_CRT_SECURE_NO_DEPRECATE
#pragma	warning( disable : 4819 )
#pragma	warning( disable : 4995 )

#include	"flCore.h"
#include	"flWiiLibCommon.h"
#include	"flWiiLib.h"
#include	"flHidLib.h"
#include	"flSimpleMath.h"



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define	__USE_BATTERYUPDATE__	0
#define	__OVERLAP_WRITE			1
#define	__USE_WRITEFILE__		0

#define	__USE_RAWDATA			1
#define	__USE_APICHECKER		1

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// debug -> flWIiLibCommon.h
//#define	__USE_DUMPTEST			0
//#define	__USE_DUMPTEST2			0
//#define	__USE_SENDDUMP			1




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class flWiiDevice : public flWiiLibDevice {
public:
	enum {
//		DEVICEFLAG_WRITEMODE	= (1<<0),
//		DEVICEFLAG_FIRSTREQ		= (1<<1),
//		DEVICEFLAG_TIMEOUT		= (1<<2),
		DEVICEFLAG_RESERVED		= (1<<2),
	};
	enum {
		VENDOR_ID	=	0x057e,
		PRODUCT_ID	=	0x0306,
	};
private:
	struct tWiiRemote {
		unsigned int	Button;
		unsigned int	Battery;
		unsigned int	EncodeType;
		unsigned int	ExtensionType;
		float			AccelF[3];
		int				_Dummy;
		unsigned int	IR[8];	// xy x4
		unsigned char	Calibration[16];
		unsigned char	ExtMemory[256];
	};
	struct tWiiNunchuk {
		unsigned int	DButton;
		float			AnalogF[2];
		float			AccelF[3];
		unsigned char	RawData[6];
	};
	struct tWiiClassic {
		unsigned int	DButton;
		float			AnalogLF[2];
		float			AnalogRF[2];
		float			AnalogTF[2];
		unsigned char	RawData[6];
	};
	struct tWiiBalance {
		float			PressureF[4];
		int				Calibration[12];
		int				Adjust[4];
		int				CalibrationMode;
		unsigned char	RawData[8];
	public:
		int	IsCalibrationMode() const
		{
			return	CalibrationMode;
		}
		void	SetCalibrationMode( int mode )
		{
			CalibrationMode= mode;
		}
	};
	enum {
		READTYPE_NOP		= 0,
		READTYPE_DEVICETYPE,
		READTYPE_EXTMEMORY,
		READTYPE_MEMORY,
		READTYPE_CALIBRATIONDATA,
		//--
		ENCODETYPE_NOP		= 0,
		ENCODETYPE_X17,
		//--
		EXTENSIONTYPE_NONE		= flWiiLibDevice::EXTENSIONTYPE_NONE,
		EXTENSIONTYPE_NUNCHUK	= flWiiLibDevice::EXTENSIONTYPE_NUNCHUK,
		EXTENSIONTYPE_CLASSIC	= flWiiLibDevice::EXTENSIONTYPE_CLASSIC,
		EXTENSIONTYPE_BALANCE	= flWiiLibDevice::EXTENSIONTYPE_BALANCE,
		EXTENSIONTYPE_GUITAR	= flWiiLibDevice::EXTENSIONTYPE_GUITAR,
		//--
		IRMODE_BASIC		= 1,
		IRMODE_EXTENDED		= 3,
		IRMODE_FULL			= 5,
		//--
		STATEREQ_TIMER		= 1000*60*10,
		STATEREQ_COUNT_MAX	= 33*10,
		//--
		MAX_SENDQUEUESIZE	= 32,
	};
	class _QueueT {
	private:
		char*	_Queue;
		int		QueueSize;
		int		Unitsize;
		int		WritePointer;
		int		ReadPointer;
	public:
		void	Format()
		{
			_Queue= NULL;
			WritePointer= 0;
			ReadPointer= 0;
		}
		_QueueT()
		{
			Format();
		}
		~_QueueT()
		{
			Quit();
		}
		void	Init( int qsize, int usize )
		{
			Unitsize= usize;
			QueueSize= qsize;
			_Queue= new char[usize * qsize];
		}
		void	Quit()
		{
			if( _Queue ){
				delete[] _Queue;
				_Queue= NULL;
			}
		}
		char*	WriteAlloc()
		{
			int	next= WritePointer + 1;
			if( next >= QueueSize ){
				next= 0;
			}
			if( next != ReadPointer ){
				char*	ptr= &_Queue[ WritePointer * Unitsize ];
				WritePointer= next;
				return	ptr;
			}
			return	NULL;
		}
		void	Read()
		{
			if( ReadPointer != WritePointer ){
				if( ++ReadPointer >= QueueSize ){
					ReadPointer= 0;
				}
			}
		}
	};
private:
	struct DecodeFunc {
		void	(*Exec)( flWiiDevice* _This, unsigned char* command, unsigned int length );
	};
	union tExtension {
		tWiiNunchuk		WiiNunchuk;
		tWiiClassic		WiiClassic;
		tWiiBalance		WiiBalance;
	}	Ext;
private:
	char*				Path;
	HANDLE				hDev;
	OVERLAPPED			Overlap;
#if __OVERLAP_WRITE
	OVERLAPPED			WriteOverlap;
#endif
	flHidLib::HIDP_PREPARSED_DATA*	Ppd;
	unsigned int		InputByteLength;
	unsigned int		OutputByteLength;
	void*				ReadBuffer;
	int					ReadType;
	int					PlayerID;
	void*				_ReadMemory;
	unsigned int		ReadSize;
	unsigned int		ReqTimer;
	unsigned int		ReqCount;
	unsigned int		DeviceFlag;
	_QueueT				Queue;
	tWiiRemote			WiiRemote;
	static DecodeFunc	_FuncTable[];
	int				APIFlag;
public:
	//-------------------------------------------------------------------------
	void	SetFlag( unsigned int flag )
	{
		DeviceFlag|= flag;
	}
	void	ResetFlag( unsigned int flag )
	{
		DeviceFlag&= ~flag;
	}
	unsigned int	TestFlag( unsigned int flag ) const
	{
		return	DeviceFlag & flag;
	}
private:
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	void	Format();
	//-------------------------------------------------------------------------
	static void CALLBACK	_ReadFileCB_Nop( DWORD dwErrorCode, DWORD dwNumberOfByteTransfered, LPOVERLAPPED lpOverlapped );
	static void CALLBACK	_ReadFileCB( DWORD dwErrorCode, DWORD dwNumberOfByteTransfered, LPOVERLAPPED lpOverlapped );
	static void CALLBACK	_WriteFileCB( DWORD dwErrorCode, DWORD dwNumberOfByteTransfered, LPOVERLAPPED lpOverlapped );
	void	ReadExec( unsigned char* command, unsigned int len );
	static void	DecodeX17( unsigned char* command, unsigned int len );
	int		_IsValid() const
	{
		return	hDev != INVALID_HANDLE_VALUE;
	}
	//-------------------------------------------------------------------------
	static void	f_Nop( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvStatus( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvRead( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvWrite( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvB( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBA( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBE8( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBAI12( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBE19( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBAE16( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBI10E9( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBAI10E6( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvE21( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBAI36A( flWiiDevice* _This, unsigned char* command, unsigned int len );
	static void	f_RecvBAI36B( flWiiDevice* _This, unsigned char* command, unsigned int len );
	//-------------------------------------------------------------------------
	void	_SetID( int id )
	{
		PlayerID= id;
	}
	int	_GetID() const
	{
		return	PlayerID;
	}
	void	_SetReadType( unsigned int type )
	{
		ReadType= type;
	}
	unsigned int	_GetReadType() const
	{
		return	ReadType;
	}
	void	SetEncodeType( unsigned int type )
	{
		WiiRemote.EncodeType= type;
	}
	int	IsX17() const
	{
		return	WiiRemote.EncodeType == ENCODETYPE_X17;
	}
	//-------------------------------------------------------------------------
	static unsigned short	GetShort( const unsigned char* command )
	{
		return	(command[0] << 8) | command[1];
	}
	//-------------------------------------------------------------------------
	void DecodeButton( const unsigned char* command );
	void DecodeAccel( const unsigned char* command );
	void DecodeExtension( unsigned char* command, unsigned int len );
	void DecodeIR10( const unsigned char* command, unsigned int len );
	void DecodeIR12( const unsigned char* command, unsigned int len );
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	void SetupExtensionMode();
	void _EnableIR( int flag, int mode );
	void _SetPlayerLED( int ledid );
	void _Send( const char* command, unsigned int length );
	void Read();
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	static float Calibration( int mi, int ma, int ce, int v );
	static float CalibrationA( int g, int ce, int v );
	//-------------------------------------------------------------------------
	void _SetButton( unsigned int button )
	{
		WiiRemote.Button= button;
	}
	unsigned int _GetButton() const
	{
		return	WiiRemote.Button;
	}
	void CalibrationCore( unsigned int x, unsigned int y, unsigned int z );
	float _GetAccel( int id ) const
	{
		if( id >= 0 && id < 3 ){
			return	WiiRemote.AccelF[ id ];
		}
		return	0.0f;
	}
	void _SetIR( int id, unsigned int x, unsigned int y )
	{
		int	nid= id << 1;
		WiiRemote.IR[nid + 0]= x;
		WiiRemote.IR[nid + 1]= y;
	}
	unsigned int _GetIR( int id ) const
	{
		if( id >= 0 && id < 8 ){
			return	WiiRemote.IR[id];
		}
		return	0;
	}
	void _SetBatteryLevel( unsigned int level )
	{
		WiiRemote.Battery= level;
	}
	unsigned int _GetBatteryLevel() const
	{
		return	WiiRemote.Battery;
	}
	void _SetExtensionType( unsigned int ta )
	{
		WiiRemote.ExtensionType= ta;
	}
	unsigned int _GetExtensionType() const
	{
		return	WiiRemote.ExtensionType;
	}
	//-------------------------------------------------------------------------
#if 0
	void _SetBalancePressure(
								unsigned int a,
								unsigned int b,
								unsigned int c,
								unsigned int d )
	{
		Ext.WiiBalance.Pressure[0]= a;
		Ext.WiiBalance.Pressure[1]= b;
		Ext.WiiBalance.Pressure[2]= c;
		Ext.WiiBalance.Pressure[3]= d;
	}
#endif
	float _GetBalancePressure( int id ) const	// interface
	{
		return	Ext.WiiBalance.PressureF[ id ];
	}
	void _SetCalibrationMode( int mode )
	{
		Ext.WiiBalance.SetCalibrationMode( mode );
	}
	void DoCalibrationBalance( const int* _a );
	void _CalibrationBalance( unsigned int a, unsigned int b, unsigned int c, unsigned int d );
	//-------------------------------------------------------------------------
	void _SetNunchukButton( unsigned int a )
	{
		Ext.WiiNunchuk.DButton= a;
	}
	unsigned int _GetNunchukButton() const
	{
		return	Ext.WiiNunchuk.DButton;
	}
	float _GetNunchukAnalog( int id ) const
	{
		if( id >= 0 && id < 2 ){
			return	Ext.WiiNunchuk.AnalogF[id];
		}
		return	0.0f;
	}
	float _GetNunchukAccel( int id ) const
	{
		if( id >= 0 && id < 3 ){
			return	Ext.WiiNunchuk.AccelF[id];
		}
		return	0;
	}
	void CalibrationNunchuk( unsigned int x, unsigned int y, unsigned int ax, unsigned int ay, unsigned int az );
	//-------------------------------------------------------------------------
	void _SetClassicButton( unsigned int a )
	{
		Ext.WiiClassic.DButton= a;
	}
	unsigned int _GetClassicButton() const
	{
		return	Ext.WiiClassic.DButton;
	}
	float 	_GetClassicAnalog( int id ) const
	{
		if( id >= 0 && id < 6 ){
			return	Ext.WiiClassic.AnalogLF[ id ];
		}
		return	0;
	}
	void CalibrationClassic( unsigned int lx, unsigned int ly, unsigned int rx, unsigned int ry, unsigned int tl, unsigned int tr );
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
public:
	flWiiDevice()
	{
		Format();
	}
	~flWiiDevice()
	{
		Quit();
	}
	void	Quit();
	void	SetPath( const char* path );
	int		Open( const char* path, int id );
	void	Start();
	void	Close();
//	void	Dump();
	void	StopReport();
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
#if __USE_APICHECKER
	int	APIChecker();
	void	CancelIO();
#endif
	int	TestAPIFlag( int flag ) const
	{
		return	APIFlag & flag;
	}
	void	SetAPIFlag( int flag )
	{
		APIFlag|= flag;
	}
	void	ResetAPIFlag( int flag )
	{
		APIFlag&= ~flag;
	}
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	const char*	_GetName() const
	{
		return	Path;
	}
	int	_EqName( const char* name ) const
	{
		if( Path ){
			return	!_strcmpi( Path, name );
		}
		return	FALSE;
	}
	//-------------------------------------------------------------------------
public:
	//-------------------------------------------------------------------------
	// Interfaces
	//-------------------------------------------------------------------------
	int	IsValid() const
	{
		return	_IsValid();
	}
	int	GetID() const
	{
		return	_GetID();
	}
	//-------------------------------------------------------------------------
	void	SendCommand( const char* cmd, unsigned int len ) flOVERRIDE
	{
		return	_Send( cmd, len );
	}
	unsigned int	GetExtensionType() const flOVERRIDE
	{
		return	_GetExtensionType();
	}
	unsigned int	GetBatteryLevel() const flOVERRIDE
	{
		return	_GetBatteryLevel();
	}
	int ReadMemory( unsigned int address, char* buffer, unsigned int size ) flOVERRIDE;
	unsigned int	GetReadType() const flOVERRIDE
	{
		return	_GetReadType();
	}
	void	ReadExtension( unsigned char* buffer ) const flOVERRIDE
	{
		memcpy( buffer, WiiRemote.ExtMemory, 256 );
	}
	void	DecodePacket();
	//-------------------------------------------------------------------------
	unsigned int	GetButton() const flOVERRIDE
	{
		return	_GetButton();
	}
	float	GetAccel( int id ) const flOVERRIDE
	{
		return	_GetAccel( id );
	}
	unsigned int	GetIR( int id ) const flOVERRIDE
	{
		return	_GetIR( id );
	}
	//-------------------------------------------------------------------------
	unsigned int	GetNunchukButton() const flOVERRIDE
	{
		return	_GetNunchukButton();
	}
	float	GetNunchukAnalog( int id ) const flOVERRIDE
	{
		return	_GetNunchukAnalog( id );
	}
	float	GetNunchukAccel( int id ) const flOVERRIDE
	{
		return	_GetNunchukAccel( id );
	}
	//-------------------------------------------------------------------------
	unsigned int	GetClassicButton() const flOVERRIDE
	{
		return	_GetClassicButton();
	}
	float	GetClassicAnalog( int id ) const flOVERRIDE
	{
		return	_GetClassicAnalog( id );
	}
	//-------------------------------------------------------------------------
	float	GetBalancePressure( int id ) const flOVERRIDE
	{
		return	_GetBalancePressure( id );
	}
	void	CalibrationBalance( int mode )
	{
		_SetCalibrationMode( mode );
	}
	float	GetWeight() const	flOVERRIDE
	{
		return	(_GetBalancePressure( 0 ) + _GetBalancePressure( 1 ) + _GetBalancePressure( 2 ) + _GetBalancePressure( 3 ) ) * 0.25f;
	}
	float	GetWeightBalance( int id ) const flOVERRIDE
	{
		if( !id ){
			return	(_GetBalancePressure( 0 ) + _GetBalancePressure( 1 ) - _GetBalancePressure( 2 ) - _GetBalancePressure( 3 ) ) * 0.5f;
		}else{
			return	(_GetBalancePressure( 1 ) + _GetBalancePressure( 3 ) - _GetBalancePressure( 0 ) - _GetBalancePressure( 2 ) ) * 0.5f;
		}
	}
	//-------------------------------------------------------------------------
};


#endif

