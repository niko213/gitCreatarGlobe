#pragma once

#include "EasyGlbGlobeLayer.h"
#include "GlbFeature.h"
#include "GlbTin.h"
#include <osg\Polytope>
#include <osg\BoundingBox>
#include "EasyGlbGlobeRenderer.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeFeatureLayer : public CEasyGlbGlobeLayer
	{
	public:
		CEasyGlbGlobeFeatureLayer(void);
		~CEasyGlbGlobeFeatureLayer(void);

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

		glbBool SetGlobe(CEasyGlbGlobe* globe);

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
		* @brief 添加一个图层渲染器
		* @param [in] renderer 渲染器
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool AddRenderer(CEasyGlbGlobeRenderer* renderer); 
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
		CEasyGlbGlobeRenderer* GetRenderer(glbInt32 idx); 
	private:
		glbBool IsEqual(CGlbExtent* extent, CGlbExtent* extent_other);
		glbInt32 ComputeFitLoopdepth(GlbGlobeTypeEnum globe_type, osg::Camera* p_osgcamera, CGlbExtent* p_layer_extent);
		osg::BoundingBoxd ComputeVisibleExtent(osg::Polytope cv, CGlbExtent* extent, GlbGlobeTypeEnum globe_type, glbInt32 loopdepth);
		CGlbTin* ExtentToTin(CGlbExtent* p_extent);
	private:
		glbref_ptr<IGlbFeatureCursor> mpr_p_cursor;									//要素游标
		std::map<glbInt32/*feature oid*/,glbInt32/*feature oid*/> mpr_features;		//装载的要素		
		std::map<glbInt32/*feature oid*/,glbByte/*state*/> mpr_ftstate;				//要素状态			
		glbref_ptr<CGlbQueryFilter> mpr_query_filter;								//数据过滤器
		glbref_ptr<CGlbExtent> mpr_visible_extent;									//
		glbref_ptr<CGlbExtent> mpr_p_visible_extent_onterrain;						//贴地面可见区域【地理坐标】
		std::vector<glbref_ptr<CEasyGlbGlobeRenderer>> mpr_renderers;				//渲染器集合
		glbref_ptr<CGlbQueryFilter> mpr_temp_query_filter;

		GlbCriticalSection	mpr_critical;							//修改操作 更新 临界区
		GlbCriticalSection	mpr_queryfilter_critical;				//数据过滤器 临界区
	};
}

