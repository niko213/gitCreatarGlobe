/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeFeatureLayer.h
* @brief   要素图层头文件
*
* 这个档案定义CGlbGlobeFeatureLayer这个class
*
* @version 1.0
* @author  马林
* @date    2014-5-8 17:40
*********************************************************************/

#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
#include "GlbQueryFilter.h"
#include "GlbGlobeRenderer.h"
#include "GlbFeature.h"
#include "GlbTin.h"
#include <osg\Polytope>
#include <osg\BoundingBox>
#include "libxml/tree.h"
#include "GlbClipObject.h"

namespace GlbGlobe
{
	/**
	* @brief 要素图层类
	* @note 描述和渲染几何数据集（如：点 ，线，面......）
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeFeatureLayer : public CGlbGlobeLayer
	{ 
	public:
		/**
		* @brief 要素图层构造函数
		* @return 无
		*/
		CGlbGlobeFeatureLayer(void);
		/**
		* @brief 要素图层析构函数
		* @return 无
		*/
		~CGlbGlobeFeatureLayer(void);

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
		glbBool Show(glbBool show,glbBool isOnState=false);
		/**
		* @brief 设置图层闪烁
		* @param [in] isBlink 是否闪烁,true闪烁
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Blink(glbBool isBlink);
		glbBool IsBlink();
		// 设置裁剪面对象
		void SetClipObject(CGlbClipObject *clipObject);
		glbBool Clip(glbBool isClip);
		glbBool IsClip();

		glbBool SetGlobe(CGlbGlobe* globe);
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

		/* 要素图层接口 */
		/**
		* @brief 图层更新
		*				 
		*/
		void Update();
		/*
		* @brief 设置图层中对象均为地面上对象或地面下对象
		* @param [in] isground 地面上对象或地面下对象
		-	true表示为地面上对象
		-	false表示为地面下对象
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetGround(glbBool isground);
		/**
		* @brief 获取图层中对象是地面上对象还是地面下对象
		* 				
		* @return  地面上对象或地面下对象
		-	true表示为地面上对象
		-	false表示为地面下对象
		*/
		glbBool IsGround();
		/**
		* @brief 设置图层的数据过滤器
		* @param [in] filter 过滤器
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetFilter(CGlbQueryFilter* filter);
		/**
		* @brief 获取图层的数据过滤器
		* 				
		* @return  数据过滤器
		*/
		CGlbQueryFilter* GetFilter();
		/**
		* @brief 设置当前地面的可见区域
		* @param [in] extent1 可见区域1
		* @param [in] extent2 可见区域2
		* @note 可见区域可以作为过滤条件缩小图层数据集的查询结果
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetVisibleTerrainExtent(CGlbExtent* extent1, CGlbExtent* extent2);
		/**
		* @brief 通知图层有新的要素对象被添加了
		* @param [in] feature 新的要素对象				
		* @return  无
		*/
		void NotifyFeatureAdd(CGlbFeature* feature); 
		/**
		* @brief 通知图层有要素对象被删除了
		* @param [in] feature_oid 删除的要素对象id				
		* @return  无
		*/
		void NotifyFeatureDelete(glbInt32 feature_oid);
		/**
		* @brief 通知图层有要素对象被修改了
		* @param [in] feature_oid 修改的要素对象id				
		* @return  无
		*/
		void NotifyFeatureModity(glbInt32 feature_oid);
		/**
		* @brief 添加一个图层渲染器
		* @param [in] renderer 渲染器
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool AddRenderer(CGlbGlobeRenderer* renderer); 
		/**
		* @brief 删除一个图层渲染器
		* @param [in] idx 渲染器序号
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool RemoveRenderer(glbInt32 idx); 
		/**
		* @brief 删除所有图层渲染器
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool RemoveAllRenderers();
		/**
		* @brief 获取图层渲染数量
		* 			
		* @return  渲染器数量
		*/
		glbInt32 GetRendererCount(); 
		/**
		* @brief 获取一个图层渲染器
		* @param [in] idx 渲染器序号
		* @return  成功返回渲染器指针
		-	 失败返回NULL
		*/
		CGlbGlobeRenderer* GetRenderer(glbInt32 idx); 
		/**
		* @brief 设置图层对象是否允许卸载
		* @param [in] isAllowUnload 是否允许被卸载 true-允许卸载  false-不可卸载
		* @return 无
		*/
		void SetIsAllowUnLoad(glbBool isAllowUnload=false);
		/**
		* @brief 获取图层对象是否允许被卸载		
		* @return 对象允许被卸载 true-允许卸载  false-不可卸载
		*/
		glbBool IsAllowUnLoad();
		/**
		* @brief  设置要素选中状态		
		* @param  [in] ftOID 要素对象的ID
		* @param  [in] isSelected 选中状态,true 选中,false 不选中
		* @return void
		*/
		void    SetFeatureSelected(glbInt32 ftOID,glbBool isSelected);
		glbBool IsFeatureSelected(glbInt32 ftOID);
		/**
		* @brief  设置要素闪烁状态		
		* @param  [in] ftOID 要素对象的ID
		* @param  [in] isBlink 闪烁状态,true 闪烁,false 不闪烁
		* @return void
		*/
		void    SetFeatureBlink(glbInt32 ftOID,glbBool isBlink);
		glbBool IsFeatureBlink(glbInt32 ftOID);
		/**
		* @brief  设置要素显隐状态		
		* @param  [in] ftOID  要素对象的ID
		* @param  [in] isShow 显隐状态,true 显示,false 不显示
		* @return void
		*/
		void    SetFeatureShow(glbInt32 ftOID,glbBool isShow);
		glbBool IsFeatureShow(glbInt32 ftOID);

		/**
		* @brief 设置是否使用相机海拔高作为相机到对象的距离	 -- 移到Renderer中
		* @param [in] isCameraAltitudeAsDistance true-使用相机海报高作为
		* @return void
		*/
		//void SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance);
		//glbBool IsCameraAltitudeAsDistance();

		/**
		* @brief  设置图层是否需要碰撞检测		
		* @param  [in] isCollisionDetect  true表示需要进行碰撞检测，false表示不需要碰撞检测		
		* @return void
		*/
		void	SetCollisionDetect(glbBool isCollisionDetect);
		glbBool IsCollisionDectect();

		/**
		* @brief  释放内存
		*/
		void ReleaseMemory();
	private:
		glbBool IsEqual(CGlbExtent* extent, CGlbExtent* extent_other);
		glbInt32 ComputeFitLoopdepth(GlbGlobeTypeEnum globe_type, osg::Camera* p_osgcamera, CGlbExtent* p_layer_extent);
		osg::BoundingBoxd ComputeVisibleExtent(osg::Polytope cv, CGlbExtent* extent, GlbGlobeTypeEnum globe_type, glbInt32 loopdepth);
		CGlbTin* ExtentToTin(CGlbExtent* p_extent);

		bool	BuildQueryfilterPostfixClause(CGlbQueryFilter* queryFilter);
	private:
		glbref_ptr<IGlbFeatureCursor> mpr_p_cursor;							//要素游标
		std::map<glbInt32/*feature oid*/,glbInt32/*feature oid*/> mpr_features;		//装载的要素
		//std::map<glbInt32/*feature oid*/,glbInt32/*feature oid*/> mpr_tempfoids;	//可见的要素oid集合
		std::map<glbInt32/*feature oid*/,glbByte/*state*/> mpr_ftstate;//要素状态
		glbBool	mpr_isground;										//是否地面对象					
		glbref_ptr<CGlbQueryFilter> mpr_query_filter;				//数据过滤器
		glbref_ptr<CGlbExtent> mpr_visible_extent;					//
		glbref_ptr<CGlbExtent> mpr_p_visible_extent_onterrain;		//贴地面可见区域【地理坐标】
		std::vector<glbref_ptr<CGlbGlobeRenderer>> mpr_renderers;	//渲染器集合
		glbref_ptr<CGlbQueryFilter> mpr_temp_query_filter;

		GlbCriticalSection	mpr_critical;							//修改操作 更新 临界区
		GlbCriticalSection	mpr_queryfilter_critical;				//数据过滤器 临界区
		glbBool				mpr_isBlink;							//图层闪烁
		glbBool				mpr_isClip;							//图层裁剪
		glbBool				mpr_isAllowUnLoad;						//图层要素装载后是否可卸载
		glbBool				mpr_isCameraAltitudeAsDistance;			//是否使用相机海拔高度做为相机到对象的距离

		glbBool				mpr_isCollisionDetected;				//是否需要碰撞检测
		glbInt32			mpr_featureCnt;							//数据集中的对象数量
		glbBool				mpr_isEOF;								//标志是否遍历完毕数据集中的所有数据
		glbref_ptr<CGlbClipObject>   mpr_clipObject;//保存裁剪对象，方面对象解除裁剪操作

		osg::Vec3d			mpr_eyePosition;						//上一次相机位置	
		glbDouble			mpr_pitch;
		glbDouble			mpr_yaw;

	};
}


