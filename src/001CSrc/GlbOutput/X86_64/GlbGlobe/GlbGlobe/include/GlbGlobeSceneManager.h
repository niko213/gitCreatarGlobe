/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeViewSymbol.h
* @brief   多边形符号头文件，文档定义CGlbGlobeViewSymbol类
* @version 1.0
* @author  龙庭友
* @date    2014-9-22 15:30
*********************************************************************/
#pragma once


#include "osgUtil/CullVisitor"
#include "osg/Group"

namespace GlbGlobe
{
	osg::Program* createShaderProgramEx( const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc, bool loadFromFiles );

	/* 
	* 场景管理器*
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeSceneManager : public osg::Group
	{
	public:
		enum EFunType
		{
			FT_Default = 0,
			FT_ViewAnalysis = 1	
		};
		CGlbGlobeSceneManager(void);
		~CGlbGlobeSceneManager(void);

		bool setFunType(EFunType funType);

		inline osg::Matrixd const &getCvModelViewMat()const{return mpr_ModelViewMat;}
		osg::Texture2D *getViewDepthTexturn()const{return mpr_viewDepthTexture.get();}

	private:
		void traverse(osg::NodeVisitor& nv);
		void cull(osgUtil::CullVisitor& cv);
		bool initViewSceneStateSet();
	private:
		int                               mpr_funType;//0,1(视域)
		osg::ref_ptr<osg::StateSet>       mpr_defaultSS;
		osg::ref_ptr<osg::StateSet>       mpr_viewSceneSS;
		unsigned int                      mpr_viewDepthTextureUnit;
		osg::ref_ptr<osg::Texture2D>      mpr_viewDepthTexture;

		osg::Matrixd                      mpr_ModelViewMat;    
	};

}
