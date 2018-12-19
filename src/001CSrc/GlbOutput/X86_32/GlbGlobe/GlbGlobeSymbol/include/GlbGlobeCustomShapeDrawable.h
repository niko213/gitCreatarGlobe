/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeCustomShapeDrawable.h
* @brief   自定义几何体绘制文件，文档定义自定义几何体绘制
* @version 1.0
* @author  ChenPeng
* @date    2014-5-14 15:00
*********************************************************************/
#pragma once
#include <osg/ShapeDrawable>
using namespace osg;

namespace GlbGlobe
{
	class GlobeShapeDrawable : public ShapeDrawable
	{
	public:
		GlobeShapeDrawable(Shape* shape, TessellationHints* hints=0);
		~GlobeShapeDrawable();
		virtual void drawImplementation(RenderInfo& renderInfo) const;
		virtual void accept(PrimitiveFunctor& pf) const;
		virtual osg::BoundingBox computeBound() const;
	};
}