/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeVirtualDrillSymbol.h
* @brief   ������׷���ͷ�ļ����ĵ�����CGlbGlobeVirtualDrillSymbol��
* @version 1.0
* @author  ����
* @date    2018-8-21 10:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeVirtualDrillSymbol������׷�����				 
	* @version 1.0
	* @author  ����
	* @date    2018-8-21 10:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeVirtualDrillSymbol : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeVirtualDrillSymbol(void);
		~CGlbGlobeVirtualDrillSymbol(void);

		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	};
}

