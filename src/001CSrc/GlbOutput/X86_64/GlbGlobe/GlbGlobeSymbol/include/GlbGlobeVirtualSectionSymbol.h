/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeVirtualSectionSymbol.h
* @brief   �����������ͷ�ļ����ĵ�����CGlbGlobeVirtualSectionSymbol��
* @version 1.0
* @author  ����
* @date    2018-8-21 10:40
*********************************************************************/
#pragma once
#include "glbglobesymbol.h"
namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeVirtualSectionSymbol�������������				 
	* @version 1.0
	* @author  ����
	* @date    2018-8-21 10:40
	*/
	class GLB_SYMBOLDLL_CLASSEXPORT CGlbGlobeVirtualSectionSymbol : public CGlbGlobeSymbol
	{
	private:
		enum GlbProjPlaneEnum
		{// ͶӰ��ö�ٱ���  x��(����) y��(γ��) z��(���θ߶�)
			GLB_PLANE_UNKNOWN	= 0,	// δ֪ͶӰƽ��
			GLB_PLANE_XY		= 1,	// XYƽ��
			GLB_PLANE_XZ		= 2,	// XZƽ��
			GLB_PLANE_YZ		= 3		// YZƽ��		
		};
	public:
		CGlbGlobeVirtualSectionSymbol(void);
		~CGlbGlobeVirtualSectionSymbol(void);

		/**
		* @brief ģ�ͷ�����Ⱦ
		* @param[in] obj ��Ⱦ����
		* @return ����ڵ�
		*/
		virtual osg::Node *Draw(CGlbGlobeRObject *obj,IGlbGeometry *geo);
	private:
		// ����ͶӰ��  x��(����) y��(γ��) z��(���θ߶�)
		GlbProjPlaneEnum mpr_prjPlaneEnum;		
	};
}
