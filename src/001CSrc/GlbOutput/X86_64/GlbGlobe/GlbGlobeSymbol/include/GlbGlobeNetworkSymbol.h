/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeNetworkSymbol.h
* @brief   �������ͷ�ļ����ĵ�����CGlbGlobeNetworkSymbol��
* @version 1.0
* @author  ����
* @date    2016-3-1 17:40
*********************************************************************/
#pragma once

#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeNetworkSymbol���������				 
	* @version 1.0
	* @author  ����
	* @date    2016-3-1 17:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeNetworkSymbol  : public CGlbGlobeSymbol
	{
	public:
		CGlbGlobeNetworkSymbol(void);
		~CGlbGlobeNetworkSymbol(void);

		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);

	private:
		// ������ͷ�յ�Բ����
		osg::ref_ptr<osg::Drawable> buildCylinder(unsigned int numSegments, float radius, float height, osg::Vec4 clr);

		// ���������߻��Ƴ�Բ����ʽ(ʹ�ò��ʻ���)
		osg::ref_ptr<osg::Node> DrawEdgeByCylinderShape(osg::Vec3d v1, osg::Vec3d v2, float radius, osg::Vec4 clr);
		// ���������߻��Ƴ�Բ����ʽ(ʹ�ò��ʻ���)
		osg::ref_ptr<osg::Node> DrawEdgeByCylinderShape(IGlbGeometry* geo, float radius, osg::Vec4 clr);
		// �����߻��Ƴ�Բ����ʽ(ʹ���������)
		osg::ref_ptr<osg::Node> DrawEdgeByCylinderShape(osg::Vec3 v1, osg::Vec3 v2, float radius, const char* textureLocate);
	};
}

