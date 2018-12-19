/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbCullActiveVisitor.h
* @brief   ͳһ���ؽڵ��������ӽڵ���׶ɸѡѡ������ͷ�ļ����ĵ�����CGlbCullActiveVisitor��
* @version 1.0
* @author  Malin
* @date    2016-2-23 10:00
*********************************************************************/
#pragma once

#include <osg/NodeVisitor>
namespace GlbGlobe
{
	/**
	* @brief   ͳһ���ؽڵ��������ӽڵ���׶ɸѡѡ�����
	* @version 1.0
	* @author  Malin
	* @date    2016-2-23 10:00
	*/
	class GLB_DLLCLASS_EXPORT CGlbCullActiveVisitor :	public osg::NodeVisitor
	{
	public:
		CGlbCullActiveVisitor(bool isCullActive);
		~CGlbCullActiveVisitor(void);

		/**
		* @brief ʵ����ӿ�
		* @return  ��
		*/
		virtual void apply(osg::Node &node);		
	private:
		bool _isCullActive;
	};
}

