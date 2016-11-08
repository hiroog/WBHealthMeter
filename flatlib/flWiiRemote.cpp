// 2007-2008 Hiroyuki Ogasawara
// vim:ts=4 sw=4 noet:

#include	"flCore.h"
#include	<wtypes.h>
#include	<Vector>
#include	<process.h>
#include	"flWiiRemote.h"



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//int		flWiiDevice::APIFlag= 0;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void flWiiDevice::Close()
{
	if( hDev != INVALID_HANDLE_VALUE ){
		CloseHandle( hDev );
		hDev= INVALID_HANDLE_VALUE;
	}
	if( Ppd ){
		flHidLib::HidD_FreePreparsedData( Ppd );
		Ppd= NULL;
	}
}


void flWiiDevice::Quit()
{
	if( Path ){
		flWiiLibCommon::dFree( Path );
		Path= NULL;
	}
	if( hDev != INVALID_HANDLE_VALUE ){

//		StopReport();

		CloseHandle( hDev );
		hDev= INVALID_HANDLE_VALUE;
	}
	if( Ppd ){
		flHidLib::HidD_FreePreparsedData( Ppd );
		Ppd= NULL;
	}
	Queue.Quit();
	if( ReadBuffer ){
		flWiiLibCommon::dFree( ReadBuffer );
		ReadBuffer= NULL;
	}
}


void flWiiDevice::Format()
{
	APIFlag= 0;

	Path= NULL;
	hDev= INVALID_HANDLE_VALUE;
	memset( &Overlap, 0, sizeof(OVERLAPPED) );
#if __OVERLAP_WRITE
	memset( &WriteOverlap, 0, sizeof(OVERLAPPED) );
#endif
	Ppd= NULL;
	InputByteLength= 0;
	OutputByteLength= 0;
	ReadBuffer= NULL;
	ReadType= READTYPE_NOP;
	PlayerID= 0;
	_ReadMemory= NULL;
	ReadSize= 0;
	ReqTimer= GetTickCount() + STATEREQ_TIMER;
	ReqCount= 0;

	DeviceFlag= 0;//DEVICEFLAG_TIMEOUT;
//DeviceFlag|= DEVICEFLAG_WRITEMODE;

	Queue.Format();

	memset( &WiiRemote, 0, sizeof(tWiiRemote) );
	memset( &Ext, 0, sizeof(tExtension) );
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void flWiiDevice::DecodePacket()
{
#if __USE_RAWDATA
	unsigned int	extype= _GetExtensionType();
	switch( extype ){
	default:
	case EXTENSIONTYPE_NONE:
		break;
	case EXTENSIONTYPE_BALANCE:
		_CalibrationBalance( 0, 0, 0, 0 );
		break;
	case EXTENSIONTYPE_NUNCHUK:
		break;
	case EXTENSIONTYPE_CLASSIC:
		break;
	}
#endif
}



float flWiiDevice::Calibration( int ma, int mi, int ce, int v )
{
	float	xmax=  static_cast<float>( ma );
	float	xmin=  static_cast<float>( mi );
	float	xzero= static_cast<float>( ce );
	float	x= static_cast<float>( v );

	x-= xzero;
	if( x < 0.0f ){
		float	len= xzero - xmin;
		if( len >= 1.0f ){
			return	x / len;
		}
	}else{
		float	len= xmax - xzero;
		if( len >= 1.0f ){
			return	x / len;
		}
	}
	return	0.0f;
}



float flWiiDevice::CalibrationA( int g, int ce, int v )
{
	float	xg=  static_cast<float>( g );
	float	xzero= static_cast<float>( ce );
	float	x= static_cast<float>( v );

	x-= xzero;
	xg-= xzero;
	if( xg >= 1.0f ){
		return	x / xg;
	}
	return	0.0f;
}



void flWiiDevice::CalibrationCore( unsigned int x, unsigned int y, unsigned int z )
{
	WiiRemote.AccelF[0]= CalibrationA(
			static_cast<int>( WiiRemote.Calibration[0x0a] ),
			static_cast<int>( WiiRemote.Calibration[0x06] ),
			static_cast<int>( x )
				);
	WiiRemote.AccelF[1]= CalibrationA(
			static_cast<int>( WiiRemote.Calibration[0x0b] ),
			static_cast<int>( WiiRemote.Calibration[0x07] ),
			static_cast<int>( y )
				);
	WiiRemote.AccelF[2]= CalibrationA(
			static_cast<int>( WiiRemote.Calibration[0x0c] ),
			static_cast<int>( WiiRemote.Calibration[0x08] ),
			static_cast<int>( z )
				);
}



void flWiiDevice::CalibrationNunchuk( unsigned int x, unsigned int y, unsigned int ax, unsigned int ay, unsigned int az )
{
	Ext.WiiNunchuk.AnalogF[0]= Calibration(
				static_cast<int>( WiiRemote.ExtMemory[ 0x28 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x29 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x2a ] ),
				static_cast<int>( x )
				);
	Ext.WiiNunchuk.AnalogF[1]= Calibration(
				static_cast<int>( WiiRemote.ExtMemory[ 0x2b ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x2c ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x2d ] ),
				static_cast<int>( y )
				);

	Ext.WiiNunchuk.AccelF[0]= CalibrationA(
				static_cast<int>( WiiRemote.ExtMemory[ 0x24 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x20 ] ),
				static_cast<int>( ax )
				);
	Ext.WiiNunchuk.AccelF[1]= CalibrationA(
				static_cast<int>( WiiRemote.ExtMemory[ 0x25 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x21 ] ),
				static_cast<int>( ay )
				);
	Ext.WiiNunchuk.AccelF[2]= CalibrationA(
				static_cast<int>( WiiRemote.ExtMemory[ 0x26 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x22 ] ),
				static_cast<int>( az )
				);
}


void flWiiDevice::CalibrationClassic( unsigned int lx, unsigned int ly, unsigned int rx, unsigned int ry, unsigned int tl, unsigned int tr )
{
	Ext.WiiClassic.AnalogLF[0]= Calibration(
				static_cast<int>( WiiRemote.ExtMemory[ 0x20 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x21 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x22 ] ),
				static_cast<int>( lx )
				);
	Ext.WiiClassic.AnalogLF[1]= Calibration(
				static_cast<int>( WiiRemote.ExtMemory[ 0x23 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x24 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x25 ] ),
				static_cast<int>( ly )
				);
	Ext.WiiClassic.AnalogRF[0]= Calibration(
				static_cast<int>( WiiRemote.ExtMemory[ 0x26 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x27 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x28 ] ),
				static_cast<int>( rx )
				);
	Ext.WiiClassic.AnalogRF[1]= Calibration(
				static_cast<int>( WiiRemote.ExtMemory[ 0x29 ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x2a ] ),
				static_cast<int>( WiiRemote.ExtMemory[ 0x2b ] ),
				static_cast<int>( ry )
				);
	Ext.WiiClassic.AnalogTF[0]= Calibration(
				0xf8,
				0,
				0,//static_cast<int>( WiiRemote.ExtMemory[ 0x2c ] ),
				static_cast<int>( tl )
				);
	Ext.WiiClassic.AnalogTF[1]= Calibration(
				0xf8,
				0,
				0,//static_cast<int>( WiiRemote.ExtMemory[ 0x2d ] ),
				static_cast<int>( tr )
				);
}


void flWiiDevice::DoCalibrationBalance( const int* _a )
{
	if( _GetReadType() != READTYPE_NOP ){
		return;
	}
	unsigned char*	cab= &WiiRemote.ExtMemory[0x24];
	for( int i= 0 ; i< 12 ; i++ ){
		Ext.WiiBalance.Calibration[i]= static_cast<int>( GetShort( cab ) );
		cab+= 2;
	}
	for( int i= 0 ; i< 4 ; i++ ){
		Ext.WiiBalance.Calibration[i+0]+= Ext.WiiBalance.Adjust[i];
		Ext.WiiBalance.Calibration[i+4]+= Ext.WiiBalance.Adjust[i];
		Ext.WiiBalance.Calibration[i+8]+= Ext.WiiBalance.Adjust[i];

		Ext.WiiBalance.Adjust[i]+=
							(_a[i] - Ext.WiiBalance.Calibration[i]) >> 1;
	}
}


void flWiiDevice::_CalibrationBalance( unsigned int a, unsigned int b, unsigned int c, unsigned int d )
{
	int	_a[4];
#if __USE_RAWDATA
	_a[0]= static_cast<int>( GetShort( Ext.WiiBalance.RawData + 0 ) );
	_a[1]= static_cast<int>( GetShort( Ext.WiiBalance.RawData + 2 ) );
	_a[2]= static_cast<int>( GetShort( Ext.WiiBalance.RawData + 4 ) );
	_a[3]= static_cast<int>( GetShort( Ext.WiiBalance.RawData + 6 ) );
#else
	_a[0]= static_cast<int>( a );
	_a[1]= static_cast<int>( b );
	_a[2]= static_cast<int>( c );
	_a[3]= static_cast<int>( d );
#endif

	if( Ext.WiiBalance.IsCalibrationMode() ){
		DoCalibrationBalance( _a );
		return;
	}

	const int*	cap= Ext.WiiBalance.Calibration;
	for( int i= 0 ; i< 4 ; i++, cap++ ){
		int	a0= cap[0];
		int	a1= cap[4];
		int	a2= cap[8];
		if( a0 == a1 || a1 == a2 ){
			continue;
		}
		int	press= _a[i];
		if( press < a1 ){
			Ext.WiiBalance.PressureF[i]=
							68.0f * (static_cast<float>( press ) - a0)
							/ static_cast<float>( a1 - a0 );
		}else{
			Ext.WiiBalance.PressureF[i]=
							68.0f * (static_cast<float>( press ) - a1)
							/ static_cast<float>( a2 - a1 ) + 68.0f;
		}
	}
}



void flWiiDevice::DecodeX17( unsigned char* ptr, unsigned int len )
{
	for( unsigned int i= 0 ; i< len ; i++ ){
		*ptr++= ((*ptr ^ 0x17) + 0x17) & 0xff;
	}
}


void flWiiDevice::DecodeButton( const unsigned char* command )
{
	_SetButton( GetShort( command ) );
}


void flWiiDevice::DecodeAccel( const unsigned char* command )
{
	CalibrationCore(
			command[0],
			command[1],
			command[2]
		);
}


void flWiiDevice::DecodeIR10( const unsigned char* ptr, unsigned int len )
{
	for( int i= 0 ; i< 4 ; i+= 2, ptr+= 5 ){
		unsigned int	x1= ptr[0] | ((ptr[2]<<4) & 0x300);
		unsigned int	y1= ptr[1] | ((ptr[2]<<2) & 0x300);
		unsigned int	x2= ptr[3] | ((ptr[2]<<8) & 0x300);
		unsigned int	y2= ptr[4] | ((ptr[2]<<6) & 0x300);
		_SetIR( i+0, x1, y1 );
		_SetIR( i+1, x2, y2 );
	}
}


void flWiiDevice::DecodeIR12( const unsigned char* ptr, unsigned int len )
{
	for( int i= 0 ; i< 4 ; i++, ptr+= 3 ){
		unsigned int	x1= ptr[0] | ((ptr[2]<<4) & 0x300);
		unsigned int	y1= ptr[1] | ((ptr[2]<<2) & 0x300);
		_SetIR( i, x1, y1 );
	}
}







void flWiiDevice::DecodeExtension( unsigned char* ptr, unsigned int len )
{
	if( IsX17() ){
		DecodeX17( ptr, len );
	}
	unsigned int	extype= _GetExtensionType();
//flERROR( "\n*******\n%d %x\n**********\n", extype );
	switch( extype ){
	default:
	case EXTENSIONTYPE_NONE:
		return;
	case EXTENSIONTYPE_BALANCE:
		if( len >= 8 ){
#if __USE_RAWDATA
			memcpy( Ext.WiiBalance.RawData, ptr, 8 );
#else
			_CalibrationBalance(
						GetShort( ptr + 0 ),
						GetShort( ptr + 2 ),
						GetShort( ptr + 4 ),
						GetShort( ptr + 6 )
					);
#endif
		}
		return;
	case EXTENSIONTYPE_NUNCHUK:
		if( len >= 6 ){
			_SetNunchukButton( (~ptr[5]) & 0x03 );
			CalibrationNunchuk(
					ptr[0],	// x
					ptr[1],	// y
					ptr[2],	// ax
					ptr[3],	// ay
					ptr[4]	// az
						);
		}
		return;
	case EXTENSIONTYPE_CLASSIC:
		if( len >= 6 ){
			unsigned int	lx= ptr[0] & 0x3f;
			unsigned int	ly= ptr[1] & 0x3f;
			unsigned int	rx=   ((ptr[2]>>7) & 0x01)
								| ((ptr[1]>>5) & 0x06)
								| ((ptr[0]>>3) & 0x18);
			unsigned int	ry= ptr[2] & 0x1f;
			unsigned int	tl=  ((ptr[2]>>2) & 0x18)
								|((ptr[3]>>5) & 0x07);
			unsigned int	tr= ptr[3] & 0x1f;
			_SetClassicButton( (~((ptr[4]<<8) | ptr[5])) & 0xffff );
			CalibrationClassic(
						lx << 2,
						ly << 2,
						rx << 3,
						ry << 3,
						tl << 3,
						tr << 3
						);
		}
		return;
	}
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void flWiiDevice::_EnableIR( int flag, int mode )
{
	if( flag ){
		_Send( "\x13\x04", 2 );
		_Send( "\x1a\x04", 2 );
		_Send(
				"\x16\x04\xb0\x00\x30\x01"
				"\x08\x00\x00\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00\x00\x00\x00"
				,
					22
				);
		_Send(
				"\x16\x04\xb0\x00\x00\x09"
				"\x00\x00\x00\x00\x00\x00\x90\x00"
				"\xc0\x00\x00\x00\x00\x00\x00\x00"
				,
					22
				);
		_Send(
				"\x16\x04\xb0\x00\x1a\x02"
				"\x40\x00\x00\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00\x00\x00\x00"
				,
					22
				);
		char	buf[32];
		memcpy( buf,
				"\x16\x04\xb0\x00\x33\x01"
				"\x01\x00\x00\x00\x00\x00\x00\x00"
				"\x00\x00\x00\x00\x00\x00\x00\x00"
				,
					22
				);
		buf[6]= mode;
		_Send(	buf, 22 );
	}else{
		_Send( "\x13\x00", 2 );
		_Send( "\x1a\x00", 2 );
	}
}


void flWiiDevice::_SetPlayerLED( int ledid )
{
	char	_msgbuf[2];
	_msgbuf[0]= 0x11;
	_msgbuf[1]= 0x10 << (ledid & 0x03);
	_Send( _msgbuf, 2 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int flWiiDevice::ReadMemory( unsigned int address, char* buffer, unsigned int size )
{
	if( _GetReadType() != READTYPE_NOP ){
		return	FALSE;
	}
	_ReadMemory= buffer;
	ReadSize= size;
	char	command[7];
	command[0]= 0x17;
	command[1]= (address >>24) & 0xff;
	command[2]= (address >>16) & 0xff;
	command[3]= (address >> 8) & 0xff;
	command[4]= (address     ) & 0xff;
	command[5]= (size    >> 8) & 0xff;
	command[6]= (size        ) & 0xff;
	_SetReadType( READTYPE_MEMORY );
	_Send( command, 7 );
	return	TRUE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void flWiiDevice::SetupExtensionMode()
{
	int	extype= _GetExtensionType();
	switch( extype ){
	case EXTENSIONTYPE_CLASSIC:
		_EnableIR( TRUE, IRMODE_BASIC );
		_SetPlayerLED( PlayerID );
		//_Send( "\x12\x00\x32", 3 );	// ReqReport BAE8
		_Send( "\x12\x00\x37", 3 );	// ReqReport BAI10E6
		break;
	case EXTENSIONTYPE_NUNCHUK:
		_EnableIR( TRUE, IRMODE_BASIC );
		_SetPlayerLED( PlayerID );
		_Send( "\x12\x00\x37", 3 );	// ReqReport BAI10E6
		break;
	case EXTENSIONTYPE_BALANCE:
		//_EnableIR( FALSE, IRMODE_BASIC );
		_SetPlayerLED( 0 );			// LED-1 on
		_Send( "\x12\x00\x32", 3 );	// ReqReport BE8
		break;
	default: // single
		_EnableIR( TRUE, IRMODE_EXTENDED );
		_SetPlayerLED( PlayerID );
		_Send( "\x12\x00\x33", 3 );	// ReqReport BAI12
		break;
	}
}


void flWiiDevice::StopReport()
{
	int	extype= _GetExtensionType();
	switch( extype ){
	case EXTENSIONTYPE_BALANCE:
		break;
	default:
		_EnableIR( FALSE, IRMODE_BASIC );
		break;
	}
	_Send( "\x12\x00\x30", 3 );	// ReqReport B
}



void flWiiDevice::f_Nop( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// Nop
	flWiiLibCommon::DebugPrint( "unknown report %x\n", *command );
}


void flWiiDevice::f_RecvStatus( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	_This->DecodeButton( command + 1 );
flERROR( "extport=%02x %02x %02x", command[3], command[4], command[5] );
	if( command[3] & 0x02 ){	// extension port
		// use extension
		_This->_Send( // init extension
					"\x16\x04\xa4\x00\x40\x01"
					"\x00\x00\x00\x00\x00\x00\x00\x00"
					"\x00\x00\x00\x00\x00\x00\x00\x00"
					,
						22
					);
		_This->_SetReadType( READTYPE_DEVICETYPE );
		_This->_Send( // read read registers
					"\x17\x04\xa4\x00\xf0\x00\x10",
					7
					);
	}else{
		// core only
		//_This->_Send( "\x12\x00\x33", 3 );	// ReqReport BAI12
		_This->_SetExtensionType( EXTENSIONTYPE_NONE );
		_This->_SetReadType( READTYPE_CALIBRATIONDATA );
		_This->_Send( // read calibration data
					"\x17\x00\x00\x00\x10\x00\x10",
					7
					);
	}
	_This->_SetBatteryLevel( command[6] );
}



void flWiiDevice::f_RecvRead( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	_This->DecodeButton( command + 1 );
	unsigned int	dlen= ((command[3] >> 4) & 0x0f) + 1;
	unsigned int	derr= command[3] & 0x0f;
	unsigned int	doffset= GetShort( command + 4 );
	if( derr ){
		flWiiLibCommon::DebugPrint( "ERROR:RecvRead Error\n" );
		return;
	}
	switch( _This->ReadType ){
	case READTYPE_NOP:
		break;
	case READTYPE_MEMORY:
		if( _This->ReadSize >= dlen && _This->_ReadMemory ){
			if( _This->IsX17() ){
				_This->DecodeX17( command+6, dlen );
			}
			memcpy( &reinterpret_cast<char*>(_This->_ReadMemory)[ doffset ],
									command + 6, dlen );
			_This->ReadSize-= dlen;
			if( _This->ReadSize <= 0 ){
				_This->_SetReadType( READTYPE_NOP );
			}
		}
		break;
	case READTYPE_CALIBRATIONDATA:
		memcpy( &_This->WiiRemote.Calibration[ 0 ], command + 6, dlen );
#if 0
flMESSAGE(
	"%02x %02x %02x %02x %02x %02x %02x %02x",
	_This->WiiRemote.Calibration[0x06],
	_This->WiiRemote.Calibration[0x07],
	_This->WiiRemote.Calibration[0x08],
	_This->WiiRemote.Calibration[0x09],
	_This->WiiRemote.Calibration[0x0a],
	_This->WiiRemote.Calibration[0x0b],
	_This->WiiRemote.Calibration[0x0c],
	_This->WiiRemote.Calibration[0x0d]
		);
#endif
		_This->_SetReadType( READTYPE_NOP );
		_This->SetupExtensionMode();
		break;
	case READTYPE_EXTMEMORY:
		if( _This->IsX17() ){
			_This->DecodeX17( command+6, dlen );
		}
		memcpy( &_This->WiiRemote.ExtMemory[ doffset ], command + 6, dlen );
		if( doffset >= 0xf0 ){
			if( _This->_GetExtensionType() != EXTENSIONTYPE_BALANCE ){
				// それ以外は core のデータ取得
				_This->_SetReadType( READTYPE_CALIBRATIONDATA );
				_This->_Send( // read calibration data
							"\x17\x00\x00\x00\x10\x00\x10",
							7
							);
			}else{
				// balance なら終わり
				_This->SetupExtensionMode();
				_This->_SetReadType( READTYPE_NOP );
			}
		}
		break;
	case READTYPE_DEVICETYPE:
#if 1
flWiiLibCommon::DebugPrint( "EXT: %02x %02x %02x %02x\n",
command[6+0x0c],
command[6+0x0d],
command[6+0x0e],
command[6+0x0f] );
#endif
		if( command[ 6 + 0x0c ] == 0xa4 ){
			_This->SetEncodeType( ENCODETYPE_NOP );
		}else{
			_This->SetEncodeType( ENCODETYPE_X17 );
			_This->DecodeX17( command+6, dlen );
		}
		_This->_SetExtensionType(
					(command[6 + 0x0e]<<8)| command[6 + 0x0f] );
		//_This->SetupExtensionMode();
		_This->_SetReadType( READTYPE_EXTMEMORY );
		_This->_Send( // read all registers
					"\x17\x04\xa4\x00\x00\x01\x00",
					7
					);
		break;
	}
}


void flWiiDevice::f_RecvWrite( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
}



void flWiiDevice::f_RecvB( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 30 BB BB
	_This->DecodeButton( command + 1 );
}



void flWiiDevice::f_RecvBA( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 31 BB BB AA AA AA
	_This->DecodeButton( command + 1 );
	_This->DecodeAccel( command + 3 );
}



void flWiiDevice::f_RecvBE8( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 32 BB BB EE EE EE EE EE EE EE EE (x8)
	_This->DecodeButton( command + 1 );
	_This->DecodeExtension( command + 3, 8 );
}



void flWiiDevice::f_RecvBAI12( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 33 BB BB AA AA AA II II II II II II II II II II II II (x12)
	_This->DecodeButton( command + 1 );
	_This->DecodeAccel( command + 3 );
	_This->DecodeIR12( command + 6, 12 );
}



void flWiiDevice::f_RecvBE19( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 34 BB BB EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE (x19)
	_This->DecodeButton( command + 1 );
	_This->DecodeExtension( command + 3, 19 );
}



void flWiiDevice::f_RecvBAE16( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 35 BB BB AA AA AA EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE (x16)
	_This->DecodeButton( command + 1 );
	_This->DecodeAccel( command + 3 );
	_This->DecodeExtension( command + 6, 16 );
}


void flWiiDevice::f_RecvBI10E9( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 36 BB BB II II II II II II II II II II EE EE EE EE EE EE EE EE EE (x9)
	_This->DecodeButton( command + 1 );
	_This->DecodeIR10( command + 3, 10 );
	_This->DecodeExtension( command + 13, 9 );
}



void flWiiDevice::f_RecvBAI10E6( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 37 BB BB AA AA AA II II II II II II II II II II EE EE EE EE EE EE (x6)
	_This->DecodeButton( command + 1 );
	_This->DecodeAccel( command + 3 );
	_This->DecodeIR10( command + 6, 10 );
	_This->DecodeExtension( command + 16, 6 );
}



void flWiiDevice::f_RecvE21( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 3d EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE EE (x21)
	_This->DecodeExtension( command + 1, 21 );
}


void flWiiDevice::f_RecvBAI36A( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 3e BB BB AA II II II II II II II II II II II II II II II II II II (x18)
	_This->DecodeButton( command + 1 );
//	_This->DecodeIR18( command + 4, 18 );
}


void flWiiDevice::f_RecvBAI36B( flWiiDevice* _This, unsigned char* command, unsigned int len )
{
	// 3f BB BB AA II II II II II II II II II II II II II II II II II II (x18)
	_This->DecodeButton( command + 1 );
//	_This->DecodeIR18( command + 4, 18 );
}




flWiiDevice::DecodeFunc	flWiiDevice::_FuncTable[]= {
	flWiiDevice::f_RecvStatus,		// 0x20
	flWiiDevice::f_RecvRead,		// 0x21
	flWiiDevice::f_RecvWrite,		// 0x22
	flWiiDevice::f_Nop,	// 0x23
	flWiiDevice::f_Nop,	// 0x24
	flWiiDevice::f_Nop,	// 0x25
	flWiiDevice::f_Nop,	// 0x26
	flWiiDevice::f_Nop,	// 0x27
	flWiiDevice::f_Nop,	// 0x28
	flWiiDevice::f_Nop,	// 0x29
	flWiiDevice::f_Nop,	// 0x2a
	flWiiDevice::f_Nop,	// 0x2b
	flWiiDevice::f_Nop,	// 0x2c
	flWiiDevice::f_Nop,	// 0x2d
	flWiiDevice::f_Nop,	// 0x2e
	flWiiDevice::f_Nop,	// 0x2f
	flWiiDevice::f_RecvB,		// 0x30	BB BB
	flWiiDevice::f_RecvBA,		// 0x31	BB BB AA AA AA
	flWiiDevice::f_RecvBE8,		// 0x32	BB BB EE EE EE EE EE EE EE EE .. 8
	flWiiDevice::f_RecvBAI12,	// 0x33	BB BB AA AA AA II II II II II II .. 12
	flWiiDevice::f_RecvBE19,	// 0x34	BB BB EE EE EE EE EE EE EE EE .. 19
	flWiiDevice::f_RecvBAE16,	// 0x35	BB BB AA AA AA EE EE EE EE EE EE .. 16
	flWiiDevice::f_RecvBI10E9,	// 0x36	BB BB II II II x10 EE EE EE x9
	flWiiDevice::f_RecvBAI10E6,	// 0x37	BB BB AA AA AA II II x10 EE EE EE x6
	flWiiDevice::f_Nop,	// 0x38
	flWiiDevice::f_Nop,	// 0x39
	flWiiDevice::f_Nop,	// 0x3a
	flWiiDevice::f_Nop,	// 0x3b
	flWiiDevice::f_Nop,	// 0x3c
	flWiiDevice::f_RecvE21,		// 0x3d	EE EE EE EE EE .. x21
	flWiiDevice::f_RecvBAI36A,	// 0x3e	BB BB AA II II II .. x18
	flWiiDevice::f_RecvBAI36B,	// 0x3f	BB BB AA II II II .. x18
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


void flWiiDevice::SetPath( const char* path )
{
	if( Path ){
		flWiiLibCommon::dFree( Path );
		Path= NULL;
	}
	size_t	plen= strlen( path ) + 1;
	Path= reinterpret_cast<char*>( flWiiLibCommon::dAlloc( plen ) );
	strcpy( Path, path );
}




void CALLBACK flWiiDevice::_ReadFileCB_Nop( DWORD dwErrorCode, DWORD dwNumberOfByteTransfered, LPOVERLAPPED lpOverlapped )
{
	flMESSAGE("ReadCB Nop err=%d tr=%d", dwErrorCode, dwNumberOfByteTransfered );
}


#if __USE_APICHECKER
void flWiiDevice::CancelIO()
{
	CancelIo( hDev );
}

int flWiiDevice::APIChecker()
{

	const char*	command= "\x15\x00";
	const int	cmdlength= 2;

	if( !flHidLib::HidD_SetOutputReport(
					hDev,
					reinterpret_cast<void*>( const_cast<char*>( command ) ),
					cmdlength
				) ){
		SetAPIFlag( APIFLAG_WRITEMODE|APIFLAG_SELECTED );
		flMESSAGE( " --- [err] select API WriteFile" );
		return	FALSE;
	}
	memset( &Overlap, 0, sizeof(OVERLAPPED) );
	if( !ReadFileEx(
				hDev,
				ReadBuffer,
				InputByteLength,
				&Overlap,
				_ReadFileCB_Nop
				) ){
		flMESSAGE( "Error:CHECK ReadFileEx\n" );
		Close();
		return	FALSE;
	}
	return	TRUE;
}
#endif




int flWiiDevice::Open( const char* path, int id )
{
	Quit();
	SetPath( path );
	_SetID( id );

	HANDLE	hdev= CreateFile(
					path,
					GENERIC_READ|GENERIC_WRITE,
					FILE_SHARE_READ|FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_OVERLAPPED,
					NULL
				);
	if( hdev == INVALID_HANDLE_VALUE ){
		//Quit();
		Close();
		return	FALSE;
	}
	hDev= hdev;

	if( !flHidLib::HidD_GetPreparsedData( hdev, &Ppd ) ){
		//Quit();
		Close();
		return	FALSE;
	}


	flHidLib::HIDP_CAPS	caps;
	if( !flHidLib::HidP_GetCaps( Ppd, &caps ) ){
		//Quit();
		Close();
		return	FALSE;
	}
#if __USE_SENDDUMP
flWiiLibCommon::DebugPrint( "US=%d", sizeof(caps.UsagePage) );
// US=2
#endif

	InputByteLength= caps.InputReportByteLength;
	OutputByteLength= caps.OutputReportByteLength;

	ReadBuffer= flWiiLibCommon::dAlloc( InputByteLength );
	Queue.Init( MAX_SENDQUEUESIZE, OutputByteLength );

	_SetExtensionType( EXTENSIONTYPE_NONE );

#if 0 //__USE_SENDDUMP
flWiiLibCommon::DebugPrint( "Send Read OutLen=%d", OutputByteLength );
#endif
//	SetFlag( DEVICEFLAG_FIRSTREQ );

#if 0
	_Send( "\x11\x00", 2 );		// LED off
	_Send( "\x15\x00", 2 );		// ReqStatus
	Read();
#endif

	return	TRUE;
}


void flWiiDevice::Start()
{
	_Send( "\x11\x00", 2 );		// LED off
	_Send( "\x15\x00", 2 );		// ReqStatus
	Read();
}


void flWiiDevice::ReadExec( unsigned char* command, unsigned int len )
{
	int	type= command[0];

#if __USE_DUMPTEST2
	flWiiLibCommon::DebugPrint( "R %02x", type );
#endif
#if __USE_DUMPTEST
	{
#define	__hex(ch)	((((ch&15)>9?((ch)+7):(ch))&15)+'0')
		char	_debugbuf[128];
		char*	str= _debugbuf;
		for( DWORD i= 0 ; i< len ; i++ ){
			unsigned char	ch= command[i];
			*str++= __hex( ch >> 4 );
			*str++= __hex( ch & 15 );
			*str++= ' ';
			//flWiiLibCommon::DebugPrint( " %02x", command[i] );
		}
		*str++= '\0';
		flWiiLibCommon::DebugPrint( "%s", _debugbuf );
		//flWiiLibCommon::DebugPrint( "\n" );
	}
#endif

	if( type >= 0x20 && type <= 0x3f ){
		_FuncTable[ type - 0x20 ].Exec( this, command, len );
	}

#if __USE_BATTERYUPDATE__
	if( ++ReqCount >= STATEREQ_COUNT_MAX ){
		ReqCount= 0;
		unsigned int	nt= GetTickCount();
		if( nt > ReqTimer ){
			if( GetReadType() == READTYPE_NOP ){
				_Send( "\x15\x00", 2 );		// ReqStatus
				ReqTimer= nt + STATEREQ_TIMER;
			}
		}
	}
#endif

	Read();
}



void CALLBACK flWiiDevice::_ReadFileCB( DWORD dwErrorCode, DWORD dwNumberOfByteTransfered, LPOVERLAPPED lpOverlapped )
{
	flWiiDevice*	_This=
					reinterpret_cast<flWiiDevice*>( lpOverlapped->hEvent );
	unsigned char*	ptr= reinterpret_cast<unsigned char*>( _This->ReadBuffer );
	_This->ReadExec( ptr, dwNumberOfByteTransfered );
}


void flWiiDevice::Read()
{
	if( !_IsValid() ){
		return;
	}
	memset( &Overlap, 0, sizeof(OVERLAPPED) );
	Overlap.hEvent= reinterpret_cast<HANDLE>( this );
	if( !ReadFileEx(
				hDev,
				ReadBuffer,
				InputByteLength,
				&Overlap,
				_ReadFileCB
				) ){
		flWiiLibCommon::DebugPrint( "Error:ReadFileEx\n" );
		//Quit();
		Close();
	}
#if 0//__USE_SENDDUMP
	flWiiLibCommon::DebugPrint( "set read" );
#endif
}


void CALLBACK flWiiDevice::_WriteFileCB( DWORD dwErrorCode, DWORD dwNumberOfByteTransfered, LPOVERLAPPED lpOverlapped )
{
	flWiiDevice*	_This=
					reinterpret_cast<flWiiDevice*>( lpOverlapped->hEvent );
	_This->Queue.Read();
flMESSAGE("WriteFileCB err=%d wri=%d", dwErrorCode, dwNumberOfByteTransfered );
}


void flWiiDevice::_Send( const char* command, unsigned int length )
{
	if( !_IsValid() ){
		return;
	}


#if __OVERLAP_WRITE //-----------------------------
	if( TestAPIFlag( APIFLAG_WRITEMODE ) ){

		char*	bufp= Queue.WriteAlloc();
		assert( length <= OutputByteLength );

		if( !bufp ){
			flMESSAGE( "ERROR:SendQueue Overflow" );
			return;
		}
		memcpy( bufp, command, length );

		memset( &WriteOverlap, 0, sizeof(OVERLAPPED) );
		WriteOverlap.hEvent= reinterpret_cast<HANDLE>( this );
		if( !WriteFileEx(
						hDev,
						bufp,
						OutputByteLength,
						&WriteOverlap,
						_WriteFileCB ) ){
			int	err= GetLastError();
			flMESSAGE( "ERROR:WriteFileEx %x %d\n", err, err );
		}
		if( GetLastError() != ERROR_SUCCESS ){
			int	err= GetLastError();
			flMESSAGE( "ERROR:2:WriteFileEx %x %d\n", err, err );
		}
#if __USE_SENDDUMP
		flMESSAGE( "sendWRI %d %02x", length, command[0] );
#endif
	}else
#endif
	{

		if( !flHidLib::HidD_SetOutputReport(
						hDev,
						reinterpret_cast<void*>( const_cast<char*>( command ) ),
						length
					) ){
#if _flDEBUG
			int	err= GetLastError();
			flMESSAGE( "ERROR:SetOutputReport %d %x", err, err );
#endif
#if 0 // __OVERLAP_WRITE
			SetAPIFlag( APIFLAG_WRITEMODE );
			ResetFlag( DEVICEFLAG_FIRSTREQ|DEVICEFLAG_TIMEOUT );
			_Send( command, length );
#endif
		}
#if __USE_SENDDUMP
		flMESSAGE( "sendREP %d %02x", length, command[0] );
#endif
	}

}


#if 0
void flWiiDevice::Dump()
{
	flWiiLibCommon::DebugPrint( "%s\n", Path );
	flWiiLibCommon::DebugPrint( "%d %d\n", InputByteLength, OutputByteLength );
}
#endif



