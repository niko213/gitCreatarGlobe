/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbCollisionGemetry.h
* @brief   碰撞检测的Gemetry
* @version 1.0
* @author  xt
* @date    2016-7-27 10:35
*********************************************************************/

#pragma once

#include <osg/Vec3d>

#ifndef __GLB_COLLISION_GEMETRY_H__
#define __GLB_COLLISION_GEMETRY_H__ 1

namespace Glb_Collision
{
#if 0
struct VECTOR
{
	typedef double value_type ;

	VECTOR()
	{
		_x = _y = _z = 0.0;
	}

	VECTOR(const value_type x,const value_type y,const value_type z)
	{
		_x = x;
		_y = y;
		_z = z;
	}

	value_type x() const {return _x;}
	value_type y() const {return _y;}
	value_type z() const {return _z;}

	value_type& x()  {return _x;}
	value_type& y()  {return _y;}
	value_type& z()  {return _z;}

	VECTOR(const VECTOR&rhs)
	{
		_x = rhs.x();
		_y = rhs.y();
		_z = rhs.z();
	}

	inline void normalize() 
	{
		double len = sqrt( _x*_x + _y*_y + _z*_z );
		if (len>0.0)
		{
			double inv = 1.0f/len;
			_x *= inv;
			_y *= inv;
			_z *= inv;
		}
	}
	inline VECTOR normalizeV()
	{
		double len = sqrt( _x*_x + _y*_y + _z*_z );
		value_type x = _x;
		value_type y = _y;
		value_type z = _z;

		if (len>0.0)
		{
			double inv = 1.0f/len;
			x *= inv;
			y *= inv;
			z *= inv;
		}

		return VECTOR(x,y,z);
	}

	inline value_type dot(const VECTOR&rhs)const
	{
		return _x*rhs.x()+_y*rhs.y()+_z*rhs.z();
	}
	/** Cross product. */
	inline const VECTOR cross(const VECTOR& rhs) const
	{
		return VECTOR(_y*rhs.z()-_z*rhs.y(),
			_z*rhs.x()-_x*rhs.z() ,
			_x*rhs.y()-_y*rhs.x());
	}

	inline const VECTOR operator + (const VECTOR& rhs) const
	{
		return VECTOR(_x+rhs.x(), _y+rhs.y(), _z+rhs.z());
	}
	inline const VECTOR operator - (const VECTOR& rhs) const
	{
		return VECTOR(_x-rhs.x(), _y-rhs.y(), _z-rhs.z());
	}
	inline const VECTOR operator * (const VECTOR& rhs) const
	{
		return VECTOR(_x * rhs.x(),_y * rhs.y(),_z * rhs.z());
	}
	inline const VECTOR operator * (double S)const  
	{
		return VECTOR(_x * S,_y * S,_z * S);
	}
    inline const value_type operator [] (long i) const
    {
        if(i == 0) return _x;
        if(i == 1) return _y;
        if(i == 2) return _z;
        return 0.0;
    }
	inline const VECTOR operator -=(const VECTOR& rhs) 
	{
		_x -= rhs.x();
		_y -= rhs.y();
		_z -= rhs.z();
		return *this;
	}
	inline void operator = (const VECTOR& rhs) 
	{
		_x = rhs.x();
		_y = rhs.y();
		_z = rhs.z();
	}
	inline VECTOR operator / (const VECTOR& rhs) const
	{
		return VECTOR (_x / rhs.x(),_y / rhs.y(),_z / rhs.z());
	}
	
	double squaredLength()const
	{
		return _x*_x + _y*_y + _z*_z;
	}
	double length()const
	{
		return std::sqrt(squaredLength());
	}
	
private:
	value_type _x;
	value_type _y;
	value_type _z;
};
#endif 

	typedef osg::Vec3d  VECTOR ;
	//plane
	struct PLANE 
	{
	public:
		double equation[4];
		VECTOR origin;  //经过平面的点
		VECTOR normal;
		PLANE(const VECTOR& origin, const VECTOR& normal);
		PLANE(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3);
		bool isFrontFacingTo(const VECTOR& direction) const;
		double signedDistanceTo(const VECTOR& point) const;
	};
	//struct AABB
//{
// public:
//    VECTOR P;
//    VECTOR E;
//
//     AABB( const VECTOR& p,
//         const VECTOR& e ):	P(p), E(e)
//
//    {}
//
//    const bool overlaps( const AABB& b ) const
//    {
//        const VECTOR T = b.P - P;//vector from A to B
//
//        return	fabs(T.x()) <= (E.x() + b.E.x())
//                &&
//                fabs(T.y()) <= (E.y() + b.E.y())
//                &&
//                fabs(T.z()) <= (E.z() + b.E.z());
//    }
//
//    const double min( long i ) const
//    {
//        return ((AABB*)this)->P[i] - ((AABB*)this)->E[i];
//    }
//    
//        //max x, y, or z
//    const double max( long i ) const
//    {
//        return ((AABB*)this)->P[i] + ((AABB*)this)->E[i];
//    }
//};

	//AABB
	struct AABB
	{
	public:
		AABB( const VECTOR& i,
		   const VECTOR& a );
	//private:
		VECTOR _max;
		VECTOR _min;
	};
	//线段
	struct Segment
	{
		Segment();
		Segment(const VECTOR&_a,const VECTOR&_b);
		VECTOR a; //起始点
		VECTOR b; //终止点
	};
	//capsule
	struct Capsule
	{
		Capsule();
		Segment seg;
		float	r;
	};
	//sphere
	struct Sphere
	{
		Sphere(const VECTOR&cc,double rr);
		VECTOR c; //球心
		double r;
	};
	
}
#endif