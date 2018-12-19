/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeRenderer.h
* @brief   要素图层渲染器头文件
*
* 这个档案定义CGlbGlobeRenderer这个class
*
* @version 1.0
* @author  马林
* @date    2014-5-9 10:10
*********************************************************************/
#pragma once

#include "GlbReference.h"
#include "GlbGlobeObject.h"
#include "GlbWString.h"
#include "GlbGlobeObjectRenderInfos.h"
#include <osg\Node>

namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeFeatureLayer;
	/**
	* @brief 要素图层渲染器类				 
	* @version 1.0
	* @author  马林
	* @date    2014-5-9 10:10
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeRenderer : public CGlbReference
	{
		friend class CGlbGlobeFeatureLayer;
	public:
		glbUInt32 UnRef()
		{
			return CGlbReference::UnRef();
		}
		/**
		* @brief 渲染器构造函数
		* @return 无
		*/
		CGlbGlobeRenderer(void);
		/**
		* @brief 渲染器析构函数
		* @return 无
		*/
		~CGlbGlobeRenderer(void);
		/**
		* @brief 获取渲染器名称
		* @return  渲染器名称
		*/
		const glbWChar* GetName();
		/**
		* @brief 设置渲染器名称
		* @param [in] name 渲染器名称
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief 设置几何字段
		* @param [in] field 几何字段名
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetGeoField(glbWChar* field);
		/**
		* @brief 获取几何字段名称
		* @return 几何字段名称
		*/
		const glbWChar* GetGeoField();
		/**
		* @brief 设置标注字段
		* @param [in] field 标注字段名
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetLabelField(glbWChar* field);
		/**
		* @brief 获取标注字段名称
		* @return 标注字段名称
		*/
		const glbWChar* GetLabelField();
		/**
		* @brief 设置渲染描述信息
		* @param [in] rinfo 渲染描述信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRenderInfo(GlbRenderInfo* rinfo);
		/**
		* @brief 获取渲染描述信息
		* @return 渲染描述信息
		*/
		GlbRenderInfo* GetRenderInfo();
		/**
		* @brief 获取最小可见距离信息
		* @return 最小可见距离信息
		*/
		GlbRenderDouble* GetMinDistance();
		/**
		* @brief 设置最小可见距离信息
		* @param [in] dis 最小可见距离信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetMinDistance(GlbRenderDouble* dis);
		/**
		* @brief 获取最大可见距离信息
		* @return 最大可见距离信息
		*/	
		GlbRenderDouble* GetMaxDistance();
		/**
		* @brief 设置最大可见距离信息
		* @param [in] dis 最大可见距离信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetMaxDistance(GlbRenderDouble* dis);
		/**
		* @brief 获取渲染顺序
		* @return 渲染顺序
		*/	
		GlbRenderInt32* GetRenderOrder();
		/**
		* @brief 设置渲染顺序
		* @param [in] order 渲染顺序
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRenderOrder(GlbRenderInt32* order);
		/**
		* @brief 获取加载顺序
		* @return 加载顺序
		*/
		GlbRenderInt32* GetLoadOrder();
		/**
		* @brief 设置加载顺序
		* @param [in] order 加载顺序
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetLoadOrder(GlbRenderInt32* order);
		/**
		* @brief 获取弹出信息
		* @return 弹出信息
		*/
		GlbRenderString* GetRMessage();
		/**
		* @brief 设置弹出信息
		* @param [in] msg 弹出信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRMessage(GlbRenderString* msg);
		/**
		* @brief 获取提示信息
		* @return 提示信息
		*/
		GlbRenderString* GetToolTip();
		/**
		* @brief 设置提示信息
		* @param [in] tips 提示信息
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetToolTip(GlbRenderString* tips);
		/**
		* @brief 设置高程模式
		* @param [in] mode 高程模式
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode);
		/**
		* @brief 获取高程模式
		* @return 高程模式
		*/
		GlbAltitudeModeEnum GetAltitudeMode();
		/**
		* @brief 设置是否是地面对象
		* @param [in] isground 地面对象为true，地下对象为false
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetGround(glbBool isground);
		/**
		* @brief 获取是否是地面对象
		* @return 地面对象返回true，地下对象返回false
		*/
		glbBool IsGround();
		/**
		* @brief 设置渲染器中对象显隐
		* @param [in] isShow true表示显示，false表示隐藏
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetShow(glbBool isShow);
		/**
		* @brief 序列化(加载)
		* @param [in] node xml加载类
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Load(xmlNodePtr node,glbWChar* relativepath );
		glbBool Load2(xmlNodePtr node,glbWChar* relativepath );
		/**
		* @brief 序列化(保存)
		* @param [in] node xml加载类
		* @param [in] relativepath 相对路径
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool Save(xmlNodePtr node, glbWChar* relativepath);
		/**
		* @brief 根据对象id获取对象
		* @param [in] objId 对象id	
		* @return  成功返回对象指针
		-	 失败返回NULL
		*/
		CGlbGlobeObject* GetObjectById(glbInt32 objId);
		/**
		* @brief 根据index获取对象
		* @param [in] objIndex 索引
		* @return  成功返回对象指针
		-	 失败返回NULL
		*/
		CGlbGlobeObject* GetObjectByIndex(glbInt32 objIndex);
		/**
		* @brief 添加对象
		* @param [in] obj 对象指针	
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool AddObject(CGlbGlobeObject* obj);
		/**
		* @brief 根据对象id删除对象
		* @param [in] objId 对象id	
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool RemoveObject(glbInt32 objId);
		/**
		* @brief 删除所有对象		
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool RemoveAllObjects();

		/**
		* @brief 获取渲染器中对象数量	  
		* @return  对象数量
		*/
		glbInt32 GetObjectCount();
		/**
		* @brief 根据要素feature创建对象
		* @param [in] feature 要素	
		* @return  成功返回新建的对象指针
		-	 失败返回NULL
		*/
		CGlbGlobeObject* CreateObject(CGlbFeature* feature);
		/**
		* @brief 获取渲染器的osg节点 
		* @return osg节点
		*/
		osg::Node* GetNode();
		/**
		* @brief 设置Renderer中的对象是否允许卸载
		* @param [in] isAllowUnload 是否允许被卸载 true-允许卸载  false-不可卸载
		* @return 无
		*/
		void SetIsAllowUnLoad(glbBool isAllowUnload);
		/**
		* @brief 设置是否使用相机海拔高作为相机到对象的距离	
		* @param [in] isCameraAltitudeAsDistance true-使用相机海报高作为
		* @return 成功返回true,失败返回false
		*/
		void SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance);
		glbBool IsCameraAltitudeAsDistance();
		/**
		* @brief 设置对象是否开启渐进渐出标识
		* @param[in]  isFaded 开启渐进渐出设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/		
		void SetIsFade(glbBool isFaded);
		/**
		* @brief 获取对象的是否开启渐进渐出标识
		* @return 渐进渐出返回true，反之返回false
		*/
		glbBool IsFaded();
		/**
		* @brief 设置对象渐进渐出关于透明度参数
		* @param[in]  startAlpha 渐进渐出起始透明度
		* @param[in]  endAlpha 渐进渐出结束透明度
		*/
		void SetFadeParameter(glbFloat startAlpha,glbFloat endAlpha);

		/**
		* @brief 获取对象渐进渐出关于透明度参数
		* @param[out]  startAlpha 渐进渐出起始透明度
		* @param[out]  endAlpha 渐进渐出结束透明度
		*/
		void GetFadeParameter(glbFloat *startAlpha,glbFloat *endAlpha);

		/**
		* @brief 设置对象渐进渐出的过渡周期时间
		* @param[in]  durationTime 渐进渐出过渡周期时间
		*/
		void SetFadeDurationTime(glbDouble durationTime);

		/**
		* @brief 获取对象渐进渐出的过渡周期时间
		* @return 渐进渐出的过渡周期时间
		*/
		glbDouble GetFadeDurationTime();

		/**
		* @brief  设置图层是否需要碰撞检测		
		* @param  [in] isCollisionDetect  true表示需要进行碰撞检测，false表示不需要碰撞检测		
		*		  [in] layerId 图层ID
		* @return void
		*/
		void	SetCollisionDetect(glbBool isCollisionDetect, glbInt32 layerId);
		/**
		* @brief 设置对象是否需要预编译显示列表
		* @param isNeedToPreCompile true表示需要预编译 false 表示不需要预编译
		*/
		void SetIsNeedToPreCompile(glbBool isNeedToPreCompile);
		/**
		* @brief 获取对象是否需要预编译显示列表
		* @return 需要返回true,不需要返回false
		*/
		glbBool IsNeedToPreCompile();
	private:
		// 拷贝Renderer中的相关属性到obj中,如renderorder,loadorder,minDistance.....
		void UpdateAttribute(CGlbGlobeObject* obj);		
	private:
		CGlbWString               mpr_name;							//渲染器名称
		CGlbWString               mpr_geofield;						//几何字段
		CGlbWString               mpr_labelfield;					//注记字段
		glbBool		              mpr_isground;	   				    //是否地面对象
		glbref_ptr<GlbRenderInfo> mpr_renderinfo;					//渲染描述信息
		GlbRenderDouble*          mpr_mindistance;					//最小可见距离
		GlbRenderDouble*          mpr_maxdistance;					//最大可见距离
		GlbRenderInt32*           mpr_loadorder;					//加载顺序
		GlbRenderInt32*           mpr_renderorder;					//渲染顺序
		GlbRenderString*          mpr_message;						//弹窗信息
		GlbRenderString*          mpr_tooltips;						//提示信息
		GlbAltitudeModeEnum       mpr_altitudemode;					//高程模式
		CGlbGlobe*                mpr_globe;
		osg::ref_ptr<osg::Node>   mpr_node;							//osg节点
		std::map<glbInt32,glbref_ptr<CGlbGlobeObject>> mpr_objects;			//对象数组
		GlbCriticalSection	                           mpr_critical;		//修改操作 更新 临界区
		glbBool					  mpr_isShow;						//渲染器对象显隐
		glbBool					  mpr_isCameraAltitudeAsDistance;	//是否将相机高度视为距离

		glbBool					mpr_isFaded;						//图层中对象是否渐变显示
		glbFloat				mpr_fadeStartAlpha;					//渐变开始alpha值
		glbFloat				mpr_fadeEndAlpha;					//渐变结束alpha值
		glbDouble				mpr_fadeDurationTime;				//渐变周期时间

		glbBool					mpr_isNeedToPreCompile;		///<是否需要预编译显示列表
	};

}

