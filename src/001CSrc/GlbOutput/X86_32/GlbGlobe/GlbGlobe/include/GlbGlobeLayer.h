/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeLayer.h
* @brief   Globe图层基类头文件
*
* 这个档案定义CGlbGlobeLayer这个class,此类是Dom图层，Dem图层和要素图层的基类
*
* @version 1.0
* @author  马林
* @date    2014-5-6 15:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbReference.h"
#include "GlbWString.h"
#include "GlbGlobeTypes.h"
#include "IGlbDataEngine.h"
#include <osg/Switch>
#include "libxml/tree.h"
namespace GlbGlobe
{
	class CGlbGlobe;
	class GLB_DLLCLASS_EXPORT CGlbGlobeLayer : public CGlbReference
	{
	public:
		/**
		* @brief 图层构造函数
		*/
		CGlbGlobeLayer(void);

		/**
		* @brief 图层析构函数
		*/
		virtual ~CGlbGlobeLayer(void);

		/**
		* @brief 获取图层名称
		* @note 纯虚函数
		* @return 图层名称 
		*/
		virtual const glbWChar* GetName()=0;

		/**
		* @brief 设置图层名称
		* @param name 图层名称
		* @note 纯虚函数
		* @return  成功返回true
		-	 失败返回false
		*/
		virtual glbBool SetName(glbWChar* name)=0;

		/**
		* @brief 获取图层子名称
		* @return 图层名称 
		*/
		const glbWChar* GetSubName();
		/**
		* @brief 设置图层子名称
		* @param name 图层子名称
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetSubName(glbWChar* subname);

		/**
		* @brief 获取图层类型
		* 
		* @note 纯虚函数
		* @return  图层类型
		-	 GLB_GLOBELAYER_DOM		dom图层
		-	 GLB_GLOBELAYER_DEM		dem图层
		-	 GLB_GLOBELAYER_FEATURE	要素图层
		*/
		virtual GlbGlobeLayerTypeEnum GetType()=0;

		/**
		* @brief 获取图层id
		* @return  图层id
		*/
		glbInt32 GetId();
		/**
		* @brief 设置图层id
		* @param [in] id 图层唯一id
		* 
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetId(glbInt32 id);

		/**
		* @brief 获取图层所属分组的id
		* @return  分组id
		*/
		glbInt32 GetGroupId();
		/**
		* @brief 设置图层所属分组id
		* @param [in] id 图层所属分组id
		* 
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetGroupId(glbInt32 id);
		/**
		* @brief 设置图层激活动作
		* @param [in] action 激活动作
		*				
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetActiveAction(GlbGlobeActiveActionEnum action);
		/**
		* @brief 获取图层激活动作
		* @return  激活动作
		*/
		GlbGlobeActiveActionEnum GetActiveAction();
		/**
		* @brief 图层序列化（加载）
		* @param [in] node xml读取节点
		* @param [in] relativepath 相对路径
		* @note 纯虚函数
		* @return  成功返回true
		-	 失败返回false
		*/
		virtual glbBool   Load(xmlNodePtr node, glbWChar* relativepath )=0;

		virtual glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief 图层序列化（保存）
		* @param [in] node xml写入节点
		* @param [in] relativepath 相对路径
		* @note 纯虚函数
		* @return  成功返回true
		-	 失败返回false
		*/
		virtual glbBool   Save(xmlNodePtr node, glbWChar* relativepath )=0;

		/**
		* @brief 设置场景
		* @param [in] globe 场景
		*
		* @return  成功返回true
		-	 失败返回false
		*/
		virtual glbBool SetGlobe(CGlbGlobe* globe);

		/**
		* @brief 获取场景对象
		* @return 场景对象
		*/
		CGlbGlobe *GetGlobe();

		/**
		* @brief 设置图层显示或隐藏
		* @param [in] show 显隐  显示为true，隐藏为false 
		* @param [in] isOnState true表示在可见区域  false表示不在可见区域
		* @note 纯虚函数
		* @return  成功返回true
		-	 失败返回false
		*/
		virtual glbBool Show(glbBool show,glbBool isOnState=false)=0;
		/**
		* @brief 获取图层是否显示
		* @return  显示 返回true
		-  隐藏 返回false
		*/
		glbBool IsShow();
		/**
		* @brief 设置图层不透明度
		* @param [in] opacity 不透明度(0-100)
		-	0表示全透明
		-	100表示不透明
		* @note 虚函数
		* @return  成功返回true
		-	 失败返回false
		*/
		virtual glbBool SetOpacity(glbInt32 opacity)=0;
		/**
		* @brief 获取图层不透明度
		* @return  图层不透明度
		*/
		glbInt32 GetOpacity();
		/**
		* @brief 设置图层坐标系统
		* @param new_srs 坐标系统	  
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetSpatialRS(glbWChar* new_srs);
		/**
		* @brief 获取图层坐标系统
		* @return  图层坐标系
		*/
		const glbWChar* GetSpatialRS();

		/**
		* @brief 设置图层数据集
		* @param dataset 数据集	  
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetDataset(IGlbDataset* dataset);
		/**
		* @brief 获取图层数据集
		* @return  图层数据集
		*/
		IGlbDataset* GetDataset();
		/**
		* @brief 获取图层范围
		* @param [in] isWorld 是否是世界坐标，如果是世界坐标，以米为单位;如果不是世界坐标，则使用地理坐标，以度为单位
		* @note 纯虚函数					
		* @return  图层范围
		*/
		virtual glbref_ptr<CGlbExtent> GetBound(glbBool isWorld)=0;
		/**
		* @brief 设置图层渲染顺序
		* @param order 渲染顺序
		* @note order数值越大,越先渲染
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetRenderOrder(glbInt32 order);
		/**
		* @brief 获取图层渲染顺序
		* @return  渲染顺序
		*/
		glbInt32 GetRenderOrder();
		/**
		* @brief 设置图层加载顺序
		* @param order 加载顺序
		* @note order数值越大,越先加载
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetLoadOrder(glbInt32 order);
		/**
		* @brief 获取图层加载顺序
		* @return  加载顺序
		*/
		glbInt32 GetLoadOrder();
		/**
		* @brief 设置图层飞行，跳转时的偏航角
		* @param yaw 偏航角(单位：度)	 
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetActionYaw(glbDouble yaw);
		/**
		* @brief 获取图层飞行，跳转时的偏航角
		* @return 偏航角(单位：度)
		*/
		glbDouble  GetActionYaw();
		/**
		* @brief 设置图层飞行，跳转时的倾斜角
		* @param pitch 倾斜角(单位：度)	 
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetActionPitch(glbDouble pitch);
		/**
		* @brief 获取图层飞行，跳转时的倾斜角
		* @return 倾斜角(单位：度)
		*/
		glbDouble  GetActionPitch();
		/**
		* @brief 设置图层飞行，跳转时的旋转角
		* @param roll 旋转角(单位：度)	 
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetActionRoll(glbDouble roll);
		/**
		* @brief 获取图层飞行，跳转时的旋转角
		* @return 旋转角(单位：度)
		*/
		glbDouble GetActionRoll();
		/**
		* @brief 设置图层飞行，跳转时的定位点的经度或X值
		* @param lonOrX 经度(单位：度)或X值
		-  球模式下为 经度, 平面模式下为 X
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetActionLonOrX(glbDouble lonOrX);

		/**
		* @brief 获取图层飞行，跳转时的定位点的经度或X值
		* @return 经度或X值
		*/
		glbDouble GetActionLonOrX();
		/**
		* @brief 设置图层飞行，跳转时的定位点的纬度或Y值
		* @param latOrY 纬度(单位：度)或Y值
		-  球模式下为 纬度, 平面模式下为 Y
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetActionLatOrY(glbDouble latOrY);
		/**
		* @brief 获取图层飞行，跳转时的定位点的纬度或Y值
		* @return 纬度或Y值
		*/
		glbDouble GetActionLatOrY();
		/**
		* @brief 设置图层飞行，跳转时的定位点的高度或Z值
		* @param altOrZ 高度或Z
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetActionAltOrZ(glbDouble altOrZ);
		/**
		* @brief 获取图层飞行，跳转时的定位点的高度或Z
		* @return 高度或Z
		*/
		glbDouble GetActionAltOrZ();
		/**
		* @brief 设置图层显示的默认距离
		* @param distance 距离
		* @note 当相机距离图层距离<=默认距离时，显示图层; >默认距离时，隐藏图层
		* @return  成功返回true
		-	 失败返回false
		*/
		glbBool SetDefaultVisibleDistance(glbDouble distance);
		/**
		* @brief 获取图层显示的默认距离
		* @return 图层显示的默认距离
		*/
		glbDouble GetDefaultVisibleDistance();
		/**
		* @brief 获取图层对应的osg节点
		* @note 纯虚函数
		* @return osg节点
		*/
		virtual osg::Node* GetOsgNode();
		/**
		* @brief 设置图层父osg节点
		* @note 纯虚函数
		* @return 成功返回true， 失败返回false
		*/
		virtual glbBool SetParentNode(osg::Node* parentnode);
		/**
		* @brief 获取图层节点的父osg节点
		* @note 纯虚函数
		* @return osg节点
		*/
		virtual osg::Node* GetParentNode();
		/**
		* @brief 获取图层是否"脏"了
		* @note 纯虚函数
		* @return true表示"脏"了需要刷新,false则表示"不脏"不需要刷新				
		*/
		virtual glbBool IsDirty();
		/**
		* @brief 设置脏标志
		* @param [in][out] pixelblock 数据块指针				
		* @return   成功返回true，失败返回false.
		*/
		virtual glbBool SetDiry(glbBool dirty);

		void SetDestroy();

	protected:
		glbInt32	mpr_layer_id;						//图层id
		CGlbWString mpr_layer_name;						//图层名称
		CGlbWString	mpr_layer_subname;					//图层子名称
		glbInt32	mpr_layergroup_id;					//图层分组id
		GlbGlobeActiveActionEnum mpr_activeaction;		//图层激活动作类型
		GlbGlobeLayerTypeEnum	mpr_layer_type;			//图层类型
		CGlbGlobe*	mpr_globe;							//图层所属场景
		glbBool		mpr_show;							//图层显隐
		glbBool		mpr_stateShow;						//调度控制的显示隐藏
		glbBool		mpr_isOnState;						//图层是否在可见范围
		glbInt32	mpr_opacity;						//不透明度【0-100】
		CGlbWString	mpr_spatialRS;						//坐标系
		glbref_ptr<IGlbDataset> mpr_dataset;			//图层数据集
		glbInt32	mpr_renderorder;					//渲染顺序
		glbInt32	mpr_loadorder;						//加载顺序
		glbDouble	mpr_actionyaw;						//图层飞向跳转时的偏航角【绕y轴旋转】
		glbDouble	mpr_actionpitch;					//图层飞向跳转时的倾斜角【绕x轴旋转】
		glbDouble	mpr_actionroll;						//图层飞向跳转时的旋转角【绕z轴旋转】
		glbDouble	mpr_actionLonOrX;					//图层飞向跳转时的视点经度或X
		glbDouble	mpr_actionLatOrY;					//图层飞向跳转时的视点纬度或Y			
		glbDouble	mpr_actionAltOrZ;					//图层飞向跳转时的视点高度或Z
		glbDouble	mpr_defaultvisibledist;				//图层默认可见距离
		osg::ref_ptr<osg::Node>	mpr_layer_node;		    //图层生成的osg节点
		osg::ref_ptr<osg::Node> mpr_parent_node;		//图层父osg节点
		glbBool		mpr_dirty;							//“脏”标志
		glbBool     mpr_isDestory;           			//图层范围
	};

}


