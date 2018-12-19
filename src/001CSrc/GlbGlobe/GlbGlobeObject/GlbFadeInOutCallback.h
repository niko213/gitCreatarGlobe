/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    CGlbFadeInOutCallback.h
* @brief   对象渐进渐出头文件，文档定义CGlbFadeInOutCallback类
* @version 1.0
* @author  ChenPeng
* @date    2016-3-4 9:00
*********************************************************************/
#pragma once

#include <osg/Geode>
#include <osg/Geometry>

namespace GlbGlobe
{
	class CGlbFadeInOutCallback : public osg::NodeCallback
	{
	public:
		CGlbFadeInOutCallback(glbFloat startAlpha,glbFloat endAlpha,
			CGlbGlobeRObject *rObject,glbDouble durationTime,osg::Vec4 fadeColor);
		~CGlbFadeInOutCallback(void);

		virtual void operator()(osg::Node *node,osg::NodeVisitor *nv);
		private:
			void changeMaterialAlpha(osg::StateSet* ss);
			//判断对象是否有三维立体效果
			glbBool isThreeDimensionalObject();
	private: 
		glbFloat mpr_startAlpha;
		glbFloat mpr_endAlpha;
		glbFloat mpr_currentAlpha;
		double mpr_simulationTime;//记录上次修改alpha的时间
		double mpr_durationTime;//修改alpha的时间周期
		glbBool mpr_isFadeIn;//标识是渐进还是渐出
		glbref_ptr<CGlbGlobeRObject> mpr_rObject;
		osg::Vec4 mpr_fadeColor;
		glbBool mpr_isThreeDimensional;
	};
}