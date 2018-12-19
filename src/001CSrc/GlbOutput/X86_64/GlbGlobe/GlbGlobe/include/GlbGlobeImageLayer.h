/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeImageLayer.h
* @brief   图像图层头文件
*
* 这个档案定义CGlbGlobeImageLayer这个class
*
* @version 1.0
* @author  马林
* @date    2017-5-11 10:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
#include "libxml/tree.h"

namespace GlbGlobe
{
	/**
	* @brief 图像图层类
	* @note 描述和渲染可带高程信息的图像（尤其是大范围的图像）---目前只支持单张不超过显卡支持尺寸的图片 2017.6.28 ml
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeImageLayer :	public CGlbGlobeLayer
	{
	public:
		/**
		* @brief 图像图层构造函数
		* @return 无
		*/
		CGlbGlobeImageLayer(void);
		/**
		* @brief 图像图层析构函数
		* @return 无
		*/
		~CGlbGlobeImageLayer(void);

		/* 基类接口 */
		/**
		* @brief 获取图层名称
		* @return 图层名称 
		*/
		const glbWChar* GetName();
		/**
		* @brief 设置图层名称
		* @param name 图层名称
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief 获取图层类型
		* @return  图层类型
		*/
		GlbGlobeLayerTypeEnum GetType();
		/**
		* @brief 图层序列化（加载）
		* @param [in] node xml读取节点
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief 图层序列化（保存）
		* @param [in] node xml写入节点
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief 设置图层显示或隐藏
		* @param [in] show 显隐  显示为true，隐藏为false
		* @param [in] isOnState true表示在可见区域  false表示不在可见区域
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Show(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief 设置图层不透明度
		* @param [in] opacity 不透明度(0-100)
		-	0表示全透明
		-	100表示不透明
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetOpacity(glbInt32 opacity);
		/**
		* @brief 获取图层范围
		* @param [in] isWorld 是否是世界坐标，如果是世界坐标，以米为单位;如果不是世界坐标，则使用地理坐标，以度为单位				
		* @return  图层范围
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		/*
		* @brief 设置图层闪烁
		* @param [in] isBlink 是否闪烁,true闪烁
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Blink(glbBool isBlink);
		glbBool IsBlink();

		/* ImageLayer 接口 */
		// 自动切割大分辨率图片--->texture
		/* 
		* @brief 设置图像数据文件
		* @param [in] strImageFile 影像文件
					  ext		   范围	
		* @return 成功返回true
		-	 失败返回false
		*/	
		glbBool SetImageDataFile(glbWChar* strImageFile, CGlbExtent* ext);
		/**
		* @brief 获取图像数据文件				
		* @return  图像数据文件
		*/
		const glbWChar* GetImageDataFile();

		/* 
		* @brief 设置格网密度
		* @param [in] mode 高程模式
		* @return 成功返回true
		-	 失败返回false
		*/				
		void 	SetMeshDensity(glbInt32 meshDensity);
		/**
		* @brief 获取格网密度 默认为 1.0				
		* @return  格网密度
		*/
		glbInt32 GetMeshDensity();

		/* 
		* @brief 设置高程模式
		* @param [in] mode 高程模式
		* @return 成功返回true
		-	 失败返回false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode); 
		/**
		* @brief 获取高程模式					
		* @return  高程模式
		*/
		GlbAltitudeModeEnum GetAltitudeMode();

		/* 
		* @brief 设置绝对模式下的高程Z值
		* @param [in] altitude 高程Z值
		* @return 成功返回true
		-	 失败返回false
		*/
		glbBool SetAltitudeOrZ(glbDouble altitude);
		/**
		* @brief 获取高程Z值					
		* @return  高程Z值
		*/
		glbDouble GetAltitudeOrZ();

		/**
		* @brief 图层更新		 
		*/
		void Update();

		//glbBool SetHeightDataFile(glbWChar* strHeightMapFile);
	private:
		void ComputePosByAltitudeAndGloleType( osg::Vec3d &position );
		glbDouble GetDistance( osg::Vec3d &cameraPos);
		osg::ref_ptr<osg::Group> build();
		void	DirtyOnTerrainObject();
		osg::ref_ptr<osg::Drawable> buildMeshGeometry(osg::Vec3f origPos, CGlbExtent*extent, osg::Image*image);
	private:
		glbref_ptr<CGlbExtent>	mpr_extent;				///<图层外包
		GlbAltitudeModeEnum     mpr_altitudeMode;		///<高程模式

		CGlbWString				mpr_imageDataFile;		///<影像文件地址
		glbInt32				mpr_meshDensity;		///<网格密度
		glbDouble				mpr_altitudeOrZ;		///<绝对模式时的高程
		glbBool					mpr_isCanDrawImage;
		glbBool					mpr_isBlink;

		glbBool					mpr_isInit;				///<是否初始化

		glbInt32				mpr_imageWidth;			///<影像宽度
		glbInt32				mpr_imageHeight;		///<影像高度
		glbBool					mpr_isImageLoaded;		///<影像是否已经加载

	};
}

