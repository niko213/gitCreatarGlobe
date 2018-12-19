/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbCollisionIntersector.h
* @brief   碰撞检测Intersector
*	
*	在遍历OSG节点：
		1.首先OSG节点的BoundingSphere与人物(镜头 使用椭圆表示)相交
		2.如果第一步有焦点，则继续判断人物与OSG节点下DrawAble的boundbox相交
		3.如果前两步都有焦点，则遍历该drawable的三角形面片，保存三角形定点
		4.使用swept sphere与所有三角形相交求出交点,
		5.生成新的人物运动向量
*
* @version 1.0
* @author  xt
* @date    2016-7-28 10:35
*********************************************************************/
#pragma once

#ifndef _GLB_COLLISION_INTERSECTOR_H
#define _GLB_COLLISION_INTERSECTOR_H 

#include <osgUtil/IntersectionVisitor>
#include "GlbSweptSphereCollision.h"

namespace Glb_Collision
{
	class  Glb_CollisionIntersector : public osgUtil::Intersector
	{

		public:
			Glb_CollisionIntersector(const osg::Vec3d er,const osg::Vec3d ve,const osg::Vec3d pos);

		public:
			virtual osgUtil::Intersector* clone(osgUtil::IntersectionVisitor& iv);

			virtual bool enter(const osg::Node& node);

			virtual void leave();

			virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable);

			virtual void reset();

			virtual bool containsIntersections();

	public:
			void setABase(const osg::Vec3d*b);
		private:
			bool intersects(const osg::BoundingSphere& bs);

			//球体交集 ,有交集返回true
			bool SphereIntersectSphere(osg::Vec3d&center,osg::Vec3d&v,double r)const;
			//swept sphere
			bool SweptSphere(osg::Vec3d&center,osg::Vec3d&v,double r)const;
			//moving sphere and AABB
			bool MovingSphereAABB(const osg::BoundingBox&box) ;
			//swept AABB
			bool SweptAABB(const osg::BoundingBox&box);


			//OBB与AABB
			bool OBBIntersectAABB(const osg::BoundingBox&box);
			const bool OBBOverlap
				(//A
				const osg::Vec3d&	a,	//
				const osg::Vec3d&	Pa,	//位置
				const osg::Vec3d*	A,	//归一化的局部坐标
				//B
				const osg::Vec3d&	b,	//
				const osg::Vec3d&	Pb,	//位置
				const osg::Vec3d*	B	//归一化的局部坐标
				);

			bool IntersectSegmentCapsule(const Glb_Collision::Segment seg, const Glb_Collision::Capsule cap, double &t);
			//线段和球体相交
			bool IntersectSegmentSphere(const Glb_Collision::Segment seg, const Glb_Collision::Sphere sp, double& t);
			//射线和AABB
			bool IntersectRayAABB(const Glb_Collision::VECTOR& p, const Glb_Collision::VECTOR&d, const Glb_Collision::AABB a, double &tmin,
				Glb_Collision::VECTOR& q);
			//射线 Sphere
			bool IntersectRaySphere(const Glb_Collision::VECTOR& p, Glb_Collision::VECTOR dir, Glb_Collision::Sphere sp,
				double&t, Glb_Collision::VECTOR &colP);

	private:
			
		osg::Vec3d eRadius;
		osg::Vec3d velocity;
		osg::Vec3d position;
		osg::Vec3d ABaseX;
		osg::Vec3d ABaseY;
		osg::Vec3d ABaseZ;

		osg::Matrixd localToWorld;
		Glb_CollisionIntersector * _parent;
	};
}

#endif