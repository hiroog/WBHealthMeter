// 2005 Hiroyuki Ogasawara
// vim:ts=4 sw=4:
/*!
	@file
	@author	Hiroyuki Ogasawara
	@brief
*/

#ifndef FL_FLSIMPLEMATH_H_
#define FL_FLSIMPLEMATH_H_

#include	"flPlatformMacro.h"
#include	"flDataType.h"
#include	<math.h>


#define		FLMATH_PI_Float		(3.1415926536f)

class flMath {
public:
	static const float 	DEGtoRAD( float deg )
	{
		return	deg * (FLMATH_PI_Float/180.0f);
	}
	static const float 	RADtoDEG( float deg )
	{
		return	deg * (180.0f/FLMATH_PI_Float);
	}
	static float 	SinD( float s )
	{
		return	sinf( s );
	}
	static float 	CosD( float s )
	{
		return	cosf( s );
	}
	static float 	Abs( float n )
	{
		return	fabsf( n );
	}
	static float 	Exp( float n )
	{
		return	expf( n );
	}
	static float 	Sqrt( float n )
	{
		return	sqrtf( n );
	}
	static float 	InvSqrt( float n )
	{
		return	1.0f/sqrtf( n );
	}
};




class flVect3 {
public:
	float	x;
	float	y;
	float	z;
public:
	flVect3()
	{
	}
	flVect3( const flVect3 &vec ) : x(vec.x), y(vec.y), z(vec.z)
	{
	}
	flVect3( float _x, float _y, float _z ) : x(_x), y(_y), z(_z)
	{
	}
	~flVect3()
	{
	}
};




typedef struct {
	union {
		struct {
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};
		float	m[4][4];
	};
} DMATF;






_flALIGN16 class flMatrix {
private:
	_flALIGN32 static DMATF	_MatrixIdentity;
public:
	union {
		float	m[4][4];
		struct {
			float	_11, _12, _13, _14,
					_21, _22, _23, _24,
					_31, _32, _33, _34,
					_41, _42, _43, _44;
		};
		DMATF	Mat;
	};
public:
	flMatrix()
	{
	}
	flMatrix( const flMatrix &mat )
	{
		Mat= mat.Mat;
	}
	~flMatrix()
	{
	}
	void 	SetIdentity()
	{
		Mat= _MatrixIdentity;
	}
	void 	RotationXD( float t );
	void 	RotationYD( float t );
	void 	RotationZD( float t );
	void  	_Transformation_FPU( flVect3* s, const flVect3* p ) const;
	void  	Transformation( flVect3* s, const flVect3* p ) const;
};




inline void flMatrix::_Transformation_FPU( flVect3* s, const flVect3* p ) const
{
	s->x= _11*p->x + _21*p->y + _31*p->z + _41;
	s->y= _12*p->x + _22*p->y + _32*p->z + _42;
	s->z= _13*p->x + _23*p->y + _33*p->z + _43;
}


inline void flMatrix::Transformation( flVect3* s, const flVect3* p ) const
{
	flASSERT( s && p );
	flASSERT_ALIGN16( this );

	_Transformation_FPU( s, p );
}



#endif


