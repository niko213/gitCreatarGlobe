/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeLineModelSymbol.h
* @brief   ģ���߷���ͷ�ļ����ĵ�����CGlbGlobeLineModelSymbol��
* @version 1.0
* @author  ����
* @date    2014-11-23 10:30
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"

namespace GlbGlobe
{
	/**
	* @brief ģ���߷�����				 
	* @version 1.0
	* @author  ����
	* @date    2014-11-23 10:30
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeLineModelSymbol : public CGlbGlobeSymbol
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeLineModelSymbol(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeLineModelSymbol(void);
		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		// ����׺��Ϊ.ini��ͼ���б��ļ�ʱ
		osg::Node *DrawLageImage(IGlbGeometry *geo, CGlbWString dataPath, bool isGlobe);
		
		osg::Vec3 CGlbGlobeLineModelSymbol::GetCoordByLinePercent(IGlbGeometry *geo, double per, std::vector<double>segLen, double totallength,int& prePointIdx, bool isGlobe);
	};
}

