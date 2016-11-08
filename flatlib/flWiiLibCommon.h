// 2007-2008 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#ifndef	FL_FLWIILIBCOMMON_H_
#define	FL_FLWIILIBCOMMON_H_


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define	__USE_BATTERYUPDATE__	0
#define	__OVERLAP_WRITE			1
#define	__USE_WRITEFILE__		0

#define	__USE_RAWDATA			1
#define	__USE_APICHECKER		1

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// debug
#define	__USE_DUMPTEST			0
#define	__USE_DUMPTEST2			0
#define	__USE_SENDDUMP			1


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

enum {
	APIFLAG_SELECTED	= (1<<0),
	APIFLAG_WRITEMODE	= (1<<1),
};



class flWiiLibCommon {
private:
public:
	static void*	dAlloc( size_t size );
	static void		dFree( void* ptr );
	static void		DebugPrint( const char* format ... );
};


#endif

