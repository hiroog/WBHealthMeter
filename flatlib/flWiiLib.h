// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#ifndef	FL_FLWIILIB_H_
#define	FL_FLWIILIB_H_


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*!
	@brief
		Wii �R���g���[���̏ڍ׏����Q�Ƃ��܂��B
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
		@retval	FALSE		����
		@retval	FALSE�ȊO	�L��
		@brief
			�f�o�C�X��񂪗L�����ǂ������肵�܂��B
	*/
	virtual int		IsValid() const= 0;
	//-------------------------------------------------------------------------
	/*!
		@brief
			�R���g���[���̔ԍ����Q�Ƃ��܂��B

		���̃R���g���[���ԍ��� LED �ɂ����f����܂��B
	*/
	virtual int		GetID() const= 0;
	//-------------------------------------------------------------------------
	/*!
		@param[in]	command	���M�R�}���h��
		@param[in]	len		�R�}���h�� byte ��
		@brief
			�f�o�C�X�֒��ڃR�}���h�𑗐M���܂��B

		�R�}���h�� byte �P�ʂŎw�肵�܂��B
	*/
	virtual void			SendCommand( const char* command, unsigned int len )= 0;
	//-------------------------------------------------------------------------
	virtual int				ReadMemory( unsigned int address, char* buffer, unsigned int size )= 0;
	virtual unsigned int	GetReadType() const= 0;
	virtual void			ReadExtension( unsigned char* buf ) const= 0;
	virtual void			DecodePacket()= 0;
	//-------------------------------------------------------------------------
	/*!
		@retval	extension type ��Ԃ��܂��B
		@brief
			�g���A�_�v�^�|�[�g�ɐڑ�����Ă���I�v�V�����̎�ނ𔻒肵�܂��B

		@li EXTENSIONTYPE_NONE		���ڑ��ł�
		@li EXTENSIONTYPE_NUNCHUK	�k���`���N
		@li EXTENSIONTYPE_CLASSIC	�N���V�b�N�R���g���[��
		@li EXTENSIONTYPE_BALANCE	�o�����XWii�{�[�h
		@li EXTENSIONTYPE_GUITAR	�M�^�[
	*/
	virtual unsigned int	GetExtensionType() const= 0;
	/*!
		@retval	�o�b�e���[�e�� 0�`200
		@brief
			�o�b�e���[�̗e�ʂ�Ԃ��܂��B

		0�`200 �� 0%�`100% �ɑ������܂��B
	*/
	virtual unsigned int	GetBatteryLevel() const= 0;
	//-------------------------------------------------------------------------
	/*!
		@retval	button
		@brief
			�{�^������Ԃ��܂��B

		bit pattern �ŕԂ��܂��B���L�̃V���{������`����Ă��܂��B
		������Ă���Ƃ��e bit �� 1 �ɂȂ�܂��B

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
		@param[in]	id	0�`2
		@brief
			�����Z���T�[�̃f�[�^��Ԃ��܂��B

		�l�� 1G �� 1.0 �ɂȂ�悤���K������܂��B
		@li id=0 X��
		@li id=1 Y��
		@li id=2 Z��
	*/
	virtual float		GetAccel( int id ) const= 0;
	/*!
		@param[in]	id	0�`7
		@brief
			�ԊO���Z���T�[�̃|�C���e�B���O����Ԃ��܂��B

		4�_���̏������L�ԍ��ŃA�N�Z�X���܂��B

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
			�k���`���N�R���g���[���̃{�^������Ԃ��܂��B

		������Ă���Ƃ��e bit �� 1 �ɂȂ�܂��B

		@li	NBT_TC
		@li	NBT_TZ
	*/
	virtual unsigned int	GetNunchukButton() const= 0;
	/*!
		@param[in]	id	0�`1
		@brief
			�k���`���N�R���g���[���̃A�i���O�X�e�B�b�N����Ԃ��܂��B

		�l�� -1.0�`1.0 �ɐ��K������܂��B
		@li	id=0	X��
		@li	id=1	Y��
	*/
	virtual float	GetNunchukAnalog( int id ) const= 0;
	/*!
		@param[in]	id	0�`2
		@brief
			�k���`���N�̉����Z���T�[�̃f�[�^��Ԃ��܂��B

		@li id=0 X��
		@li id=1 Y��
		@li id=2 Z��
	*/
	virtual float	GetNunchukAccel( int id ) const= 0;
	//-------------------------------------------------------------------------
	/*!
		@retval	button
		@brief
			�N���V�b�N�R���g���[���̃{�^������Ԃ��܂��B

		bit pattern �ŕԂ��܂��B���L�̃V���{������`����Ă��܂��B
		������Ă���Ƃ��e bit �� 1 �ɂȂ�܂��B

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
		@param[in]	id	0�`5
		@brief
			�N���V�b�N�R���g���[���̃A�i���O�����擾���܂��B

		@li	id=0	�� X��
		@li	id=1	�� Y��
		@li	id=2	�E X��
		@li	id=3	�E Y��
		@li	id=4	���g���K
		@li	id=5	�E�g���K
	*/
	virtual float	GetClassicAnalog( int id ) const= 0;
	//-------------------------------------------------------------------------
	/*!
		@param[in]	id	0�`3
		@brief
			�o�����X�{�[�h�̑���l�𓾂܂��B

		�l�� 4�_���ꂼ��ɂ������Ă���d���� 1/4 �̒l kg �P�ʂł��B
		���ς���邱�ƂőS�̂ɂ�����d�ʂɂȂ�܂��B
		�������l�𐳊m�ɂ��邽�߂ɂ̓L�����u���[�V�������K�v�ł��B
		@li	id=0	�E��
		@li	id=1	����
		@li	id=2	�E��
		@li	id=3	����
	*/
	virtual float	GetBalancePressure( int id ) const= 0;
	/*!
		@param[in]	mode	TRUE or FALSE
		@brief
			�o�����X�{�[�h�̃L�����u���[�V�����𐧌䂵�܂��B

		TRUE ���w�肵�Ă���ԃL�����u���[�V�������s���܂��B
		���̊Ԃ̓o�����X�{�[�h�ɉ����悹�Ȃ��ł��������B
		�I������ FALSE ���w�肵�܂��B
	*/
	virtual void	CalibrationBalance( int mode )= 0;
	/*!
		@brief
			���肵���d�ʂ�Ԃ��܂��B

		4�_�̕��ϒl�ƂȂ�܂��Bkg �P�ʁB
	*/
	virtual float	GetWeight() const= 0;
	/*!
		@param[in]	id	0�`1
		@brief
			�d�ʍ�����Ԃ��܂��B

		���E�A�O��̃o�����X�ɑ������܂��B
	*/
	virtual float	GetWeightBalance( int id ) const= 0;
	//-------------------------------------------------------------------------
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/*!
	@brief
		�R���g���[���̏����Ǘ����܂��B
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
			�T�[�r�X���~���܂�

		�f�o�C�X�����ׂĊJ������邽�߁A���̖��ߔ��s���
		�A�N�Z�X���Ȃ��ł��������B
	*/
	virtual void 	Release()= 0;
	/*!
		@brief
			�ڑ�����Ă���f�o�C�X�̌����Q�Ƃ��܂��B
	*/
	virtual int 	GetDeviceCount() const = 0;
	/*!
		@brief
			�ʂ̃f�o�C�X�փA�N�Z�X���܂��B
	*/
	virtual flWiiLibDevice* 	GetDevice( unsigned int id )= 0;
	/*!
		@param[in]	vid		VendorID
		@param[in]	pid		ProductID
		@brief
			�T�[�r�X�̊J�n���s���܂��B

		�w�肵�� ID ����f�o�C�X�����o���Ǘ����ɒu���܂��B
		Wii Remote (Wiimote, Wii�����R��) ���g���ꍇ�́B
		vid == 0x057e, pid == 0x0306 ���w�肵�Ă��������B
	*/
	virtual void 	BeginLoop( unsigned int vid= 0x057e, unsigned int pid= 0x0306 )= 0;
	//-------------------------------------------------------------------------
	virtual void 	Update()= 0;
	//-------------------------------------------------------------------------
	/*!
		@brief
			flWiiLib �̃C���X�^���X���쐬���܂��B

		�쐬�����f�o�C�X�̃C���X�^���X��Ԃ��܂��B
	*/
	static flWiiLib*	Create();
	//-------------------------------------------------------------------------
};





#endif

