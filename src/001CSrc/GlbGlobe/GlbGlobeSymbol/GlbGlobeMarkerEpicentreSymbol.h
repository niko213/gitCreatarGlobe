/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeMarkerEpicentreSymbol.h
* @brief   震源点符号头文件，文档定义CGlbGlobeMarkerEpicentreSymbol类
* @version 1.0
* @author  malin
* @date    2015-12-30 9:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief 震源点符号类				 
	* @version 1.0
	* @author  malin
	* @date   2015-12-30 9:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerEpicentreSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeMarkerEpicentreSymbol(void);
		~CGlbGlobeMarkerEpicentreSymbol(void);

		/**
		* @brief 符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);	
	private:
		osg::Node* createEpicentreNode(glbDouble radius, glbInt32 clr);
	};
}


