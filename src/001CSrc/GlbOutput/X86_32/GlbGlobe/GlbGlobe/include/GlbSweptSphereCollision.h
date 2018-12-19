/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbCollision.h
* @brief   碰撞检测
*          该swept sphere参考:Kasper Fauerby 
			Improved Collision detection and Response
* @version 1.0
* @author  xt
* @date    2016-7-28 10:35
*********************************************************************/

#pragma once

#include "GlbCollisionGemetry.h"
#include <vector>
#include <osg/Vec3>

#ifndef __GLB_COLLISION_H__
#define __GLB_COLLISION_H__

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
	//用来保存GlbCollisionIntersector遍历的drawable三角形点
	struct TraingVec
	{
		TraingVec()
		{
			//point = NULL;
			next = NULL;
		}
		osg::Vec3  point;
		TraingVec * next;
	};
	//封装碰撞参数信息
	struct CollisionPacket 

	{
	public:
		VECTOR eRadius; // ellipsoid 椭圆的三个半径
		//世界坐标系下信息
		VECTOR R3Velocity;
		VECTOR R3Position;
		//(eSpace)	椭圆坐标系信息
		VECTOR velocity;		
		VECTOR normalizedVelocity;
		VECTOR basePoint;
		// 碰撞信息
		bool foundCollision; //是否有碰撞
		double nearestDistance;//到碰撞点距离
		VECTOR intersectionPoint; //碰撞点
		int collisionRecursionDepth; //多次碰撞
	};

	class SweptSphereCollision
	{
	public:
	
		static SweptSphereCollision&Instance()
		{
			static SweptSphereCollision singleton;
			return singleton;
		}

		~SweptSphereCollision(void);

		typedef osg::Vec3d V3;

		void init();

		VECTOR CollisionSlide(CollisionPacket& cP);	

		VECTOR CollideWithWorld(CollisionPacket& cP);

		bool SphereCollidingWithTriangle(
			CollisionPacket& cP,	// Pointer to a CollisionPacket object	
			VECTOR &p0,										 
			VECTOR &p1,										 
			VECTOR &p2);
		 TraingVec * getCurrentPtr(){return currentPtr;}
		 //设置当前列表指针
		 void setCurrentPtr(TraingVec *ptr) {currentPtr = ptr;}
		 //情况点列表点信息
		 void clearTraingVec();

	private:
		bool checkPointInTriangle(const VECTOR& point, const VECTOR& pa,const VECTOR& pb, const VECTOR& pc);
		bool getLowestRoot(double a, double b, double c, double maxR, double* root);

	private:
		SweptSphereCollision(void);
		SweptSphereCollision(const SweptSphereCollision&);                  

	private:

		 VECTOR gravity;  //重力影响，目前暂时屏蔽重力影响
		 double unitsPerMeter; //单位比例

		 TraingVec* headPtr; //drawable所有点队列的首指针
		 TraingVec * currentPtr;//drawable所有点队列的目前指针指向位置
	};

}
#endif