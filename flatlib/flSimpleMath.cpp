// 2005 Hiroyuki Ogasawara
// vim:ts=4 sw=4:

#include	"flCore.h"
#include	"flSimpleMath.h"


#define		_Sin(x)		(flMath::SinD(x))
#define		_Cos(x)		(flMath::CosD(x))



_flALIGN32 DMATF	flMatrix::_MatrixIdentity= {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
};



void flMatrix::RotationXD( float s )
{
	float	Cs= _Cos(s), Ss= _Sin(s), x[4];
	x[0]   = _21 * Cs + _31 * Ss;
	x[1]   = _22 * Cs + _32 * Ss;
	x[2]   = _23 * Cs + _33 * Ss;
	x[3]   = _24 * Cs + _34 * Ss;
	_31= _21 *-Ss + _31 * Cs;
	_32= _22 *-Ss + _32 * Cs;
	_33= _23 *-Ss + _33 * Cs;
	_34= _24 *-Ss + _34 * Cs;
	_21= x[0];
	_22= x[1];
	_23= x[2];
	_24= x[3];
}


void flMatrix::RotationYD( float s )
{
	float	Cs= _Cos(s), Ss= _Sin(s), x[4];
	x[0]   = _11 * Cs + _31 *-Ss;
	x[1]   = _12 * Cs + _32 *-Ss;
	x[2]   = _13 * Cs + _33 *-Ss;
	x[3]   = _14 * Cs + _34 *-Ss;
	_31= _11 * Ss + _31 * Cs;
	_32= _12 * Ss + _32 * Cs;
	_33= _13 * Ss + _33 * Cs;
	_34= _14 * Ss + _34 * Cs;
	_11= x[0];
	_12= x[1];
	_13= x[2];
	_14= x[3];
}


void flMatrix::RotationZD( float s )
{
	float	Cs= _Cos(s), Ss= _Sin(s), x[4];
	x[0]   = _11 * Cs + _21 * Ss;
	x[1]   = _12 * Cs + _22 * Ss;
	x[2]   = _13 * Cs + _23 * Ss;
	x[3]   = _14 * Cs + _24 * Ss;
	_21= _11 *-Ss + _21 * Cs;
	_22= _12 *-Ss + _22 * Cs;
	_23= _13 *-Ss + _23 * Cs;
	_24= _14 *-Ss + _24 * Cs;
	_11= x[0];
	_12= x[1];
	_13= x[2];
	_14= x[3];
}







