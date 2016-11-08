// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#ifndef	FL_FLWIILIB_H_
#define	FL_FLWIILIB_H_


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*!
	@brief
		Wii コントローラの詳細情報を参照します。
*/
class flWiiLibDevice {
public:
	enum {
		EXTENSIONTYPE_NONE		= 0xffff,
		EXTENSIONTYPE_NUNCHUK	= 0x0000,
		EXTENSIONTYPE_CLASSIC	= 0x0101,
		EXTENSIONTYPE_BALANCE	= 0x0402,
		EXTENSIONTYPE_GUITAR	= 0x0103,
	};
	enum {
		BT_T2		= (1<<0),
		BT_T1		= (1<<1),
		BT_TB		= (1<<2),
		BT_TA		= (1<<3),
		BT_MINUS	= (1<<4),	// select
		BT_HOME		= (1<<7),
		BT_LEFT		= (1<<8),
		BT_RIGHT	= (1<<9),
		BT_DOWN		= (1<<10),
		BT_UP		= (1<<11),
		BT_PLUS		= (1<<12),	// start
		//--
		NBT_TZ		= (1<<0),
		NBT_TC		= (1<<1),
		//--
		CBT_UP		= (1<<0),
		CBT_LEFT	= (1<<1),
		CBT_TZR		= (1<<2),
		CBT_TX		= (1<<3),
		CBT_TA		= (1<<4),
		CBT_TY		= (1<<5),
		CBT_TB		= (1<<6),
		CBT_TZL		= (1<<7),
		CBT_TR		= (1<<9),
		CBT_PLUS	= (1<<10),	// start
		CBT_HOME	= (1<<11),
		CBT_MINUS	= (1<<12),	// select
		CBT_TL		= (1<<13),
		CBT_DOWN	= (1<<14),
		CBT_RIGHT	= (1<<15),
	};
protected:
	flWiiLibDevice()
	{
	}
public:
	virtual ~flWiiLibDevice()
	{
	}
	virtual void 	Quit()= 0;
	virtual void 	StopReport()= 0;
	virtual int 	_EqName( const char* name ) const= 0;
	virtual int 	Open( const char* path, int id )= 0;
	virtual void 	Start()= 0;
	virtual int 	TestAPIFlag( int f ) const= 0;
	virtual void 	SetAPIFlag( int f )= 0;
	virtual void 	ResetAPIFlag( int f )= 0;
	virtual void 	CancelIO()= 0;
	virtual int 	APIChecker()= 0;
public:
	/*!
		@retval	FALSE		無効
		@retval	FALSE以外	有効
		@brief
			デバイス情報が有効かどうか判定します。
	*/
	virtual int		IsValid() const= 0;
	//-------------------------------------------------------------------------
	/*!
		@brief
			コントローラの番号を参照します。

		このコントローラ番号は LED にも反映されます。
	*/
	virtual int		GetID() const= 0;
	//-------------------------------------------------------------------------
	/*!
		@param[in]	command	送信コマンド列
		@param[in]	len		コマンドの byte 数
		@brief
			デバイスへ直接コマンドを送信します。

		コマンドは byte 単位で指定します。
	*/
	virtual void			SendCommand( const char* command, unsigned int len )= 0;
	//-------------------------------------------------------------------------
	virtual int				ReadMemory( unsigned int address, char* buffer, unsigned int size )= 0;
	virtual unsigned int	GetReadType() const= 0;
	virtual void			ReadExtension( unsigned char* buf ) const= 0;
	virtual void			DecodePacket()= 0;
	//-------------------------------------------------------------------------
	/*!
		@retval	extension type を返します。
		@brief
			拡張アダプタポートに接続されているオプションの種類を判定します。

		@li EXTENSIONTYPE_NONE		未接続です
		@li EXTENSIONTYPE_NUNCHUK	ヌンチャク
		@li EXTENSIONTYPE_CLASSIC	クラシックコントローラ
		@li EXTENSIONTYPE_BALANCE	バランスWiiボード
		@li EXTENSIONTYPE_GUITAR	ギター
	*/
	virtual unsigned int	GetExtensionType() const= 0;
	/*!
		@retval	バッテリー容量 0～200
		@brief
			バッテリーの容量を返します。

		0～200 が 0%～100% に相当します。
	*/
	virtual unsigned int	GetBatteryLevel() const= 0;
	//-------------------------------------------------------------------------
	/*!
		@retval	button
		@brief
			ボタン情報を返します。

		bit pattern で返します。下記のシンボルが定義されています。
		押されているとき各 bit が 1 になります。

		@li BT_LEFT
		@li BT_RIGHT
		@li BT_UP
		@li BT_DOWN
		@li BT_TA
		@li BT_TB
		@li BT_T1
		@li BT_T2
		@li BT_PLUS
		@li BT_MINUS
		@li BT_HOME
	*/
	virtual unsigned int	GetButton() const= 0;
	/*!
		@param[in]	id	0～2
		@brief
			加速センサーのデータを返します。

		値は 1G が 1.0 になるよう正規化されます。
		@li id=0 X軸
		@li id=1 Y軸
		@li id=2 Z軸
	*/
	virtual float		GetAccel( int id ) const= 0;
	/*!
		@param[in]	id	0～7
		@brief
			赤外線センサーのポインティング情報を返します。

		4点分の情報を下記番号でアクセスします。

		@li	id=0 X0
		@li	id=1 Y0
		@li	id=2 X1
		@li	id=3 Y1
		@li	id=4 X2
		@li	id=5 Y2
		@li	id=6 X3
		@li	id=7 Y3
	*/
	virtual unsigned int	GetIR( int id ) const= 0;
	//-------------------------------------------------------------------------
	/*!
		@brief
			ヌンチャクコントローラのボタン情報を返します。

		押されているとき各 bit が 1 になります。

		@li	NBT_TC
		@li	NBT_TZ
	*/
	virtual unsigned int	GetNunchukButton() const= 0;
	/*!
		@param[in]	id	0～1
		@brief
			ヌンチャクコントローラのアナログスティック情報を返します。

		値は -1.0～1.0 に正規化されます。
		@li	id=0	X軸
		@li	id=1	Y軸
	*/
	virtual float	GetNunchukAnalog( int id ) const= 0;
	/*!
		@param[in]	id	0～2
		@brief
			ヌンチャクの加速センサーのデータを返します。

		@li id=0 X軸
		@li id=1 Y軸
		@li id=2 Z軸
	*/
	virtual float	GetNunchukAccel( int id ) const= 0;
	//-------------------------------------------------------------------------
	/*!
		@retval	button
		@brief
			クラシックコントローラのボタン情報を返します。

		bit pattern で返します。下記のシンボルが定義されています。
		押されているとき各 bit が 1 になります。

		@li CBT_LEFT
		@li CBT_RIGHT
		@li CBT_UP
		@li CBT_DOWN
		@li CBT_TA
		@li CBT_TB
		@li CBT_TX
		@li CBT_TY
		@li CBT_TZL
		@li CBT_TZR
		@li CBT_TL
		@li CBT_TR
		@li CBT_PLUS	// start
		@li CBT_MINUS	// select
		@li CBT_HOME
	*/
	virtual unsigned int	GetClassicButton() const= 0;
	/*!
		@param[in]	id	0～5
		@brief
			クラシックコントローラのアナログ情報を取得します。

		@li	id=0	左 X軸
		@li	id=1	左 Y軸
		@li	id=2	右 X軸
		@li	id=3	右 Y軸
		@li	id=4	左トリガ
		@li	id=5	右トリガ
	*/
	virtual float	GetClassicAnalog( int id ) const= 0;
	//-------------------------------------------------------------------------
	/*!
		@param[in]	id	0～3
		@brief
			バランスボードの測定値を得ます。

		値は 4点それぞれにかかっている重さの 1/4 の値 kg 単位です。
		平均を取ることで全体にかかる重量になります。
		ただし値を正確にするためにはキャリブレーションが必要です。
		@li	id=0	右上
		@li	id=1	左上
		@li	id=2	右下
		@li	id=3	左下
	*/
	virtual float	GetBalancePressure( int id ) const= 0;
	/*!
		@param[in]	mode	TRUE or FALSE
		@brief
			バランスボードのキャリブレーションを制御します。

		TRUE を指定している間キャリブレーションを行います。
		この間はバランスボードに何も乗せないでください。
		終了時は FALSE を指定します。
	*/
	virtual void	CalibrationBalance( int mode )= 0;
	/*!
		@brief
			測定した重量を返します。

		4点の平均値となります。kg 単位。
	*/
	virtual float	GetWeight() const= 0;
	/*!
		@param[in]	id	0～1
		@brief
			重量差分を返します。

		左右、前後のバランスに相当します。
	*/
	virtual float	GetWeightBalance( int id ) const= 0;
	//-------------------------------------------------------------------------
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/*!
	@brief
		コントローラの情報を管理します。
*/
class flWiiLib {
protected:
	flWiiLib()
	{
	}
	virtual	~flWiiLib()
	{
	}
public:
//	virtual void 	DumpAll()= 0;
	//-------------------------------------------------------------------------
	/*!
		@brief
			サービスを停止します

		デバイスもすべて開放されるため、この命令発行後は
		アクセスしないでください。
	*/
	virtual void 	Release()= 0;
	/*!
		@brief
			接続されているデバイスの個数を参照します。
	*/
	virtual int 	GetDeviceCount() const = 0;
	/*!
		@brief
			個別のデバイスへアクセスします。
	*/
	virtual flWiiLibDevice* 	GetDevice( unsigned int id )= 0;
	/*!
		@param[in]	vid		VendorID
		@param[in]	pid		ProductID
		@brief
			サービスの開始を行います。

		指定した ID からデバイスを取り出し管理下に置きます。
		Wii Remote (Wiimote, Wiiリモコン) を使う場合は。
		vid == 0x057e, pid == 0x0306 を指定してください。
	*/
	virtual void 	BeginLoop( unsigned int vid= 0x057e, unsigned int pid= 0x0306 )= 0;
	//-------------------------------------------------------------------------
	virtual void 	Update()= 0;
	//-------------------------------------------------------------------------
	/*!
		@brief
			flWiiLib のインスタンスを作成します。

		作成したデバイスのインスタンスを返します。
	*/
	static flWiiLib*	Create();
	//-------------------------------------------------------------------------
};





#endif

