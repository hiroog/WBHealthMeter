// 2007 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#ifndef	FL_FLBLUETOOTHLIB__H__
#define	FL_FLBLUETOOTHLIB__H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


class flBluetoothLib {
protected:
	flBluetoothLib()
	{
	}
	virtual	~flBluetoothLib()
	{
	}
public:
	virtual void	Release()= 0;
	virtual int		AddDevice( int timeout )= 0;
	static flBluetoothLib*	Create();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



#endif

