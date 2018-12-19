/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeMarkerModelSymbol.h
* @brief   模型符号头文件，文档定义CGlbGlobeMarkerModelSymbol类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-8 13:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief 模型符号类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-8 13:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeMarkerModelSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeMarkerModelSymbol(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeMarkerModelSymbol(void);
		/**
		* @brief 模型符号渲染
		* @param[in] obj 渲染对象
		* @return 对象节点
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo,glbInt32 level=0);
	private:
		// 统计osg节点中的顶点数量和面片数量
		void statusNode(osg::Node* node,int& verNum,int& faceNum);
	};
}