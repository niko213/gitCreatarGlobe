/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeAutoTransform.h
* @brief   根据需求重新实现AutoTransform
* @version 1.0
* @author  ChenPeng
* @date    2014-8-21 11:55
*********************************************************************/
#pragma once
#include "EasyGlbGlobeTypes.h"
#include <osg/AutoTransform>
#include "EasyGlbGlobe.h"

using namespace osg;
namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT GlbGlobeAutoTransform : public osg::AutoTransform
	{
	public:
		GlbGlobeAutoTransform(CEasyGlbGlobe *globe = NULL);
		~GlbGlobeAutoTransform(void);

		virtual void accept(NodeVisitor& nv);

	private:
		CEasyGlbGlobe *mpr_globe;
	};
}