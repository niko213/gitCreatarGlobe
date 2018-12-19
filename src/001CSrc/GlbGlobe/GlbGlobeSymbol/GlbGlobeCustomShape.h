/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeCustomShape.h
* @brief   自定义绘制几何体文件，文档定义自定义的绘制几何体
* @version 1.0
* @author  ChenPeng
* @date    2014-5-14 15:00
*********************************************************************/
#pragma once
#include <osg/Shape>
#include "GlbCommTypes.h"
using namespace osg;

namespace GlbGlobe
{
	class CustomCircle : public Shape
	{
	public:
		CustomCircle():
		mpr_center(Vec3(0.0f,0.0f,0.0f)),
		mpr_radius(1.0f),
		mpr_edges(24){}

		CustomCircle(const CustomCircle& circle,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		Shape(circle,copyop),
							mpr_center(circle.mpr_center),
							mpr_radius(circle.mpr_radius),
							mpr_edges(circle.mpr_edges){}

		META_Shape(osg, CustomCircle);

		void set(const Vec3 &center,const glbDouble &radius, const glbInt32 &edges)
		{
			mpr_center = center;
			mpr_radius = radius;
			mpr_edges = edges;
		}

		const Vec3 getCenter() const {return mpr_center;}
		const glbDouble getRadius() const {return mpr_radius;}
		const glbInt32 getEdges() const {return mpr_edges;}
	protected:
		virtual ~CustomCircle(){}

		Vec3    mpr_center;
		glbDouble mpr_radius;
		glbInt32 mpr_edges;
	};

	class CustomRectangle : public Shape
	{
	public:
		CustomRectangle():
		mpr_center(Vec3(0.0f,0.0f,0.0f)),
		mpr_width(1.0f),
		mpr_height(1.0f){}
		CustomRectangle(const CustomRectangle& rectangle,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		Shape(rectangle,copyop),
							mpr_center(rectangle.mpr_center),
							mpr_width(rectangle.mpr_width),
							mpr_height(rectangle.mpr_height) {}
		META_Shape(osg, CustomRectangle);

		void set(const Vec3 &center,const glbDouble &width, const glbDouble &height)
		{
			mpr_center = center;
			mpr_width = width;
			mpr_height = height;
		}

		const Vec3 getCenter() const {return mpr_center;}
		const glbDouble getWidth() const {return mpr_width;}
		const glbDouble getHeight() const {return mpr_height;}
	protected:
		virtual ~CustomRectangle(){}

		Vec3    mpr_center;
		glbDouble mpr_width;
		glbDouble mpr_height;
	};

	class CustomEllipse : public Shape
	{
	public:
		CustomEllipse():
		mpr_center(Vec3(0.0f,0.0f,0.0f)),
		mpr_xRadius(1.0f),
		mpr_yRadius(1.0f),
		mpr_edges(24){}
		CustomEllipse(const CustomEllipse& ellipse,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		Shape(ellipse,copyop),
							mpr_center(ellipse.mpr_center),
							mpr_xRadius(ellipse.mpr_xRadius),
							mpr_yRadius(ellipse.mpr_yRadius),
		                    mpr_edges(ellipse.mpr_edges){}
		META_Shape(osg, CustomEllipse);

		void set(const Vec3 &center,const glbDouble &xRadius,const glbDouble &yRadius,const glbInt32 &edges)
		{
			mpr_center = center;
			mpr_xRadius = xRadius;
			mpr_yRadius = yRadius;
			mpr_edges = edges;
		}

		const Vec3 getCenter() const {return mpr_center;}
		const glbDouble getXRadius() const {return mpr_xRadius;}
		const glbDouble getYRadius() const {return mpr_yRadius;}
		const glbInt32 getEdges() const {return mpr_edges;}
	protected:
		virtual ~CustomEllipse(){}

		Vec3 mpr_center;
		glbDouble mpr_xRadius;
		glbDouble mpr_yRadius;
		glbInt32 mpr_edges;
	};

	class CustomArc : public Shape
	{
	public:
		CustomArc():
		mpr_center(Vec3(0.0f,0.0f,0.0f)),
		mpr_xRadius(1.0f),
		mpr_yRadius(1.0f),
		mpr_sAngle(0.0),
		mpr_eAngle(osg::PI),
		mpr_edges(18),
		mpr_isFan(true){}
		CustomArc(const CustomArc& arc,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		Shape(arc,copyop),
							mpr_center(arc.mpr_center),
							mpr_xRadius(arc.mpr_xRadius),
							mpr_yRadius(arc.mpr_yRadius),
		                    mpr_sAngle(arc.mpr_sAngle),
		                    mpr_eAngle(arc.mpr_eAngle),
		                    mpr_edges(arc.mpr_edges),
		                    mpr_isFan(arc.mpr_isFan){}
		META_Shape(osg, CustomArc);

		void set(const Vec3 &center,const glbBool &isFan,const glbDouble &xRadius,const glbDouble &yRadius,
			            const glbDouble &sAngle,const glbDouble &eAngle,const glbInt32 &edges)
		{
			mpr_center = center;
			mpr_isFan = isFan;
			mpr_xRadius = xRadius;
			mpr_yRadius = yRadius;
			mpr_sAngle = sAngle;
			mpr_eAngle = eAngle;
			mpr_edges = edges;
		}

		const Vec3 getCenter() const {return mpr_center;}
		const glbBool getIsFan() const {return mpr_isFan;}
		const glbDouble getXRadius() const {return mpr_xRadius;}
		const glbDouble getYRadius() const {return mpr_yRadius;}
		const glbDouble getSAngle() const {return mpr_sAngle;}
		const glbDouble getEAngle() const {return mpr_eAngle;}
		const glbInt32 getEdges() const {return mpr_edges;}
	protected:
		virtual ~CustomArc(){}

		Vec3 mpr_center;
		glbBool mpr_isFan;
		glbDouble mpr_xRadius;
		glbDouble mpr_yRadius;
		glbDouble mpr_sAngle;
		glbDouble mpr_eAngle;
		glbInt32 mpr_edges;
	};

	class CustomCone : public Shape
	{
	public:

		CustomCone():
		  _center(0.0f,0.0f,0.0f),
			  _radius(1.0f),
			  _height(1.0f),
		      _edges(40){}

		  CustomCone(const osg::Vec3& center,glbDouble radius,glbDouble height,int edges):
		  _center(center),
			  _radius(radius),
			  _height(height),
		      _edges(edges){}

		  CustomCone(const CustomCone& cone,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		  Shape(cone,copyop),
			  _center(cone._center),
			  _radius(cone._radius),
			  _height(cone._height),
			  _rotation(cone._rotation),
			  _edges(cone._edges){}

		  META_Shape(osg, CustomCone);

		  inline bool valid() const { return _radius>=0.0f; }

		  inline void set(const Vec3& center,glbDouble radius, glbDouble height,int edges)
		  {
			  _center = center;
			  _radius = radius;
			  _height = height;
			  _edges = edges;
		  }

		  inline void setCenter(const Vec3& center) { _center = center; }
		  inline const Vec3& getCenter() const { return _center; }

		  inline void setRadius(glbDouble radius) { _radius = radius; }
		  inline glbDouble getRadius() const { return _radius; }

		  inline void setHeight(glbDouble height) { _height = height; }
		  inline glbDouble getHeight() const { return _height; }

		  inline void setEdges(int edges) { _edges = edges; }
		  inline glbDouble getEdges() const { return _edges; }

		  inline void setRotation(const Quat& quat) { _rotation = quat; }
		  inline const Quat& getRotation() const { return _rotation; }
		  inline Matrix computeRotationMatrix() const { return Matrix(_rotation); }
		  inline bool zeroRotation() const { return _rotation.zeroRotation(); } 

		  inline glbDouble getBaseOffsetFactor() const { return 0.25f; } 
		  inline glbDouble getBaseOffset() const { return -getBaseOffsetFactor()*getHeight(); } 

	protected:

		virtual ~CustomCone(){}

		Vec3    _center;
		glbDouble   _radius;
		glbDouble   _height;
		int _edges;

		Quat    _rotation;
	};

	class CustomCylinder : public Shape
	{
	public:

		CustomCylinder():
		  _center(0.0f,0.0f,0.0f),
			  _radius(1.0f),
			  _height(1.0f),
			  _edges(40){}

		  CustomCylinder(const osg::Vec3& center,glbDouble radius,glbDouble height,int edges):
		  _center(center),
			  _radius(radius),
			  _height(height),
			  _edges(edges){}

		  CustomCylinder(const CustomCylinder& cylinder,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		  Shape(cylinder,copyop),
			  _center(cylinder._center),
			  _radius(cylinder._radius),
			  _height(cylinder._height),
			  _rotation(cylinder._rotation),
			  _edges(cylinder._edges){}

		  META_Shape(osg, CustomCylinder);

		  inline bool valid() const { return _radius>=0.0f; }

		  inline void set(const Vec3& center,glbDouble radius, glbDouble height,int edges)
		  {
			  _center = center;
			  _radius = radius;
			  _height = height;
			  _edges = edges;
		  }

		  inline void setCenter(const Vec3& center) { _center = center; }
		  inline const Vec3& getCenter() const { return _center; }

		  inline void setRadius(glbDouble radius) { _radius = radius; }
		  inline glbDouble getRadius() const { return _radius; }

		  inline void setHeight(glbDouble height) { _height = height; }
		  inline glbDouble getHeight() const { return _height; }

		  inline void setEdges(int edges) { _edges = edges; }
		  inline glbDouble getEdges() const { return _edges; }

		  inline void setRotation(const Quat& quat) { _rotation = quat; }
		  inline const Quat& getRotation() const { return _rotation; }
		  inline Matrix computeRotationMatrix() const { return Matrix(_rotation); }
		  bool zeroRotation() const { return _rotation.zeroRotation(); } 

	protected:

		virtual ~CustomCylinder(){}

		Vec3    _center;
		glbDouble   _radius;
		glbDouble   _height;
		Quat    _rotation;
		int _edges;
	};

	class CustomPyramid : public Shape
	{
	public:

		CustomPyramid():
		  _center(0.0f,0.0f,0.0f),
			  _length(1.0f),
			  _width(1.0f),
			  _height(1.0f) {}

		  CustomPyramid(const osg::Vec3& center,glbDouble length,glbDouble width,glbDouble height):
		  _center(center),
			  _length(length),
			  _width(width),
			  _height(height) {}

		  CustomPyramid(const CustomPyramid& pyramid,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		  Shape(pyramid,copyop),
			  _center(pyramid._center),
			  _length(pyramid._length),
			  _width(pyramid._width),
			  _height(pyramid._height),
			  _rotation(pyramid._rotation) {}

		  META_Shape(osg, CustomPyramid);

		  inline void set(const Vec3& center,glbDouble length,glbDouble width, glbDouble height)
		  {
			  _center = center;
			  _length = length;
			  _width = width;
			  _height = height;
		  }

		  inline void setCenter(const Vec3& center) { _center = center; }
		  inline const Vec3& getCenter() const { return _center; }

		  inline void setLength(glbDouble length) { _length = length; }
		  inline glbDouble getLength() const { return _length; }

		  inline void setWidth(glbDouble width) { _width = width; }
		  inline glbDouble getWidth() const { return _width; }

		  inline void setHeight(glbDouble height) { _height = height; }
		  inline glbDouble getHeight() const { return _height; }

		  inline void setRotation(const Quat& quat) { _rotation = quat; }
		  inline const Quat& getRotation() const { return _rotation; }
		  inline Matrix computeRotationMatrix() const { return Matrix(_rotation); }
		  inline bool zeroRotation() const { return _rotation.zeroRotation(); } 

		  inline glbDouble getBaseOffsetFactor() const { return 0.25f; } 
		  inline glbDouble getBaseOffset() const { return -getBaseOffsetFactor()*getHeight(); } 

	protected:

		virtual ~CustomPyramid(){}

		Vec3    _center;
		glbDouble     _length;
		glbDouble     _width;
		glbDouble     _height;

		Quat    _rotation;
	};

	class CustomPie : public Shape
	{
	public:

		CustomPie():
		  _center(0.0f,0.0f,0.0f),
			  _radius(1.0f),
			  _height(1.0f),
			  _edges(40),
			  _sAngle(30.0),
			  _eAngle(90.0){}

		  CustomPie(const osg::Vec3& center,glbDouble radius,glbDouble height,int edges,glbDouble sAngle,glbDouble eAngle):
		  _center(center),
			  _radius(radius),
			  _height(height),
			  _edges(edges),
			  _sAngle(sAngle),
			  _eAngle(eAngle){}

		  CustomPie(const CustomPie& pie,const CopyOp& copyop=CopyOp::SHALLOW_COPY): 
		  Shape(pie,copyop),
			  _center(pie._center),
			  _radius(pie._radius),
			  _height(pie._height),
			  _rotation(pie._rotation),
			  _edges(pie._edges),
			  _sAngle(pie._sAngle),
			  _eAngle(pie._eAngle){}

		  META_Shape(osg, CustomPie);

		  inline bool valid() const { return _radius>=0.0f; }

		  inline void set(const Vec3& center,glbDouble radius, glbDouble height,int edges,glbDouble sAngle,glbDouble eAngle)
		  {
			  _center = center;
			  _radius = radius;
			  _height = height;
			  _edges = edges;
			  _sAngle = sAngle;
			  _eAngle = eAngle;
		  }

		  inline void setCenter(const Vec3& center) { _center = center; }
		  inline const Vec3& getCenter() const { return _center; }

		  inline void setRadius(glbDouble radius) { _radius = radius; }
		  inline glbDouble getRadius() const { return _radius; }

		  inline void setHeight(glbDouble height) { _height = height; }
		  inline glbDouble getHeight() const { return _height; }

		  inline void setEdges(int edges) { _edges = edges; }
		  inline glbDouble getEdges() const { return _edges; }

		  inline void setSAngle(glbDouble sAngle) { _sAngle = sAngle; }
		  inline glbDouble getSAngle() const { return _sAngle; }

		  inline void setEAngle(glbDouble eAngle) { _eAngle = eAngle; }
		  inline glbDouble getEAngle() const { return _eAngle; }

		  inline void setRotation(const Quat& quat) { _rotation = quat; }
		  inline const Quat& getRotation() const { return _rotation; }
		  inline Matrix computeRotationMatrix() const { return Matrix(_rotation); }
		  bool zeroRotation() const { return _rotation.zeroRotation(); } 

	protected:

		virtual ~CustomPie(){}

		Vec3    _center;
		glbDouble   _radius;
		glbDouble   _height;
		Quat    _rotation;
		int _edges;
		glbDouble _sAngle;
		glbDouble _eAngle;
	};

	class CustomBox : public Shape
	{
	public:

		CustomBox():
		  _center(0.0f,0.0f,0.0f),
			  _halfLengths(0.5f,0.5f,0.5f) {}

		  CustomBox(const osg::Vec3& center,float width):
		  _center(center),
			  _halfLengths(width*0.5f,width*0.5f,width*0.5f) {}

		  CustomBox(const osg::Vec3& center,float lengthX,float lengthY, float lengthZ):
		  _center(center),
			  _halfLengths(lengthX*0.5f,lengthY*0.5f,lengthZ*0.5f) {}

		  CustomBox(const CustomBox& box,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
		  Shape(box,copyop),
			  _center(box._center),
			  _halfLengths(box._halfLengths),
			  _rotation(box._rotation) {}

		  META_Shape(osg, CustomBox);

		  inline bool valid() const { return _halfLengths.x()>=0.0f; }

		  inline void set(const Vec3& center,const Vec3& halfLengths)
		  {
			  _center = center;
			  _halfLengths = halfLengths;
		  }

		  inline void setCenter(const Vec3& center) { _center = center; }
		  inline const Vec3& getCenter() const { return _center; }

		  inline void setHalfLengths(const Vec3& halfLengths) { _halfLengths = halfLengths; }
		  inline const Vec3& getHalfLengths() const { return _halfLengths; }

		  inline void setRotation(const Quat& quat) { _rotation = quat; }
		  inline const Quat&  getRotation() const { return _rotation; }
		  inline Matrix computeRotationMatrix() const { return Matrix(_rotation); }
		  inline bool zeroRotation() const { return _rotation.zeroRotation(); }

	protected:

		virtual ~CustomBox(){}

		Vec3    _center;
		Vec3    _halfLengths;
		Quat    _rotation;

	};

	class CustomSphere : public Shape
	{
	public:

		CustomSphere():
		  _center(0.0f,0.0f,0.0f),
			  _radius(1.0f) {}

		  CustomSphere(const osg::Vec3& center,float radius):
		  _center(center),
			  _radius(radius) {}

		  CustomSphere(const CustomSphere& sphere,const CopyOp& copyop=CopyOp::SHALLOW_COPY):
		  Shape(sphere,copyop),
			  _center(sphere._center),
			  _radius(sphere._radius) {}

		  META_Shape(osg, CustomSphere);

		  inline bool valid() const { return _radius>=0.0f; }

		  inline void set(const Vec3& center,float radius)
		  {
			  _center = center;
			  _radius = radius;
		  }

		  inline void setCenter(const Vec3& center) { _center = center; }
		  inline const Vec3& getCenter() const { return _center; }

		  inline void setRadius(float radius) { _radius = radius; }
		  inline float getRadius() const { return _radius; }

	protected:

		virtual ~CustomSphere(){}

		Vec3    _center;
		float   _radius;

	};
}