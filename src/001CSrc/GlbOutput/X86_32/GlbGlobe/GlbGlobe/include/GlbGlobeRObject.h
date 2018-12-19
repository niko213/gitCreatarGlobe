/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeRObject.h
* @brief   可绘制对象头文件，文档定义CGlbGlobeRObject类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-6 10:50
*********************************************************************/
#pragma once
#include "GlbGlobeObject.h"
#include "osg/Node"
#include "GlbWString.h"
#include "GlbExtent.h"
#include "IGlbGeometry.h"
#include "GlbFeature.h"
#include "GlbGlobeLayer.h"
#include "GlbGlobeObjectRenderInfos.h"
#include "GlbClipObject.h"

namespace GlbGlobe
{
	/**
	* @brief 可绘制对象类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-6 10:50
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeRObject : public CGlbGlobeObject
	{
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeRObject();
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeRObject();
		/**
		* @brief 修改对象状态：是否在可见区域
		* @param[in]  isInViewPort 在可见区域设置为true，反之设置为false
		*/
		void SetInViewPort(glbBool isInViewPort);
		/**
		* @brief 获取对象是否在可见区域
		* @return 在可见区域返回true，反之返回false
		*/
		glbBool IsInViewPort();
		/**
		* @brief 对象从场景中移除时，设置销毁标识
		* @param[in]  isInViewPort 在可见区域设置为true，反之设置为false
		*/
		void SetDestroy();
		/**
		* @brief 获取对象销毁标识
		* @return 销毁返回true，反之返回false
		*/
		glbBool IsDestroy();
		/**
		* @brief 获取相机与对象的距离
		* @param[in]  cameraPos 相机在场景中的位置
		* @param[in]  isCompute 距离是否需要重新计算，重新计算设置true，反之设置false
		* @return 相机与对象的距离
		*/
		virtual glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
		/**
		* @brief 设置是否使用相机海拔高作为相机到对象的距离	
		* @param [in] isCameraAltitudeAsDistance true-使用相机海报高作为
		* @return 无
		*/
		void SetCameraAltitudeAsDistance(glbBool isCameraAltitudeAsDistance);
		glbBool IsCameraAltitudeAsDistance();
		/**
		* @brief 装载数据，调度器会频繁的调用该方法
		* @param[in]  level 需要装载数据的等级
		*/
		virtual void LoadData(glbInt32 level);
		/**
		* @brief 挂osg节点或者交换osg节点到场景树
		* @return 返回挂到场景树上osg节点大小
		*/
		virtual void AddToScene();
		/**
		* @brief 从场景树摘除osg节点
		* @param[in]  isClean true表示同时删除所有osg节点以节省内存
		* @return 返回节省的内存大小
		*/
		virtual glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief 获取对象当前LOD级数
		* @return 对象当前LOD级数
		*/
		glbInt32 GetCurrLevel();
		/**
		* @brief 是否使用实例化
		* @param[in]  useInstance 使用实例化设置为true，反之设置为false
		* @note 只对点起作用，实例化对象，共同使用osg节点，可以节省内存、显存
		*/
		virtual void SetUseInstance(glbBool useInstance);
		/**
		* @brief 获取对象是否实例化标识
		* @return 实例化返回true，反之返回false
		*/
		glbBool IsUseInstance();
		/**
		* @brief 获取对象父节点
		* @return 父节点指针
		*/
		osg::Node *GetParentNode();
		/**
		* @brief 设置对象父节点
		* @param[in] 父节点指针
		*/
		virtual void SetParentNode(osg::Node *parent);
		/**
		* @brief 判断对象是否从来没有试图装载过数据
		* @return 从来没有返回true，反之返回false
		*/
		glbBool GetNeverLoadState();
		/**
		* @brief 重置数据装载状态，使得再次LoadData时可以产生数据装载任务
		*/
		void ResetLoadData();
		/**
		* @brief 获取对象是否正在装载数据.
		*/
		glbBool IsLoadData();		
		/**
		* @brief 获取对象最大可见距离
		* @return 对象最大可见距离
		*/
		glbDouble GetMaxVisibleDistance();
		/**
		* @brief 获取对象最小可见距离
		* @return 对象最小可见距离
		*/
		glbDouble GetMinVisibleDistance();
		/**
		* @brief 获取对象渲染优先级
		* @return 对象渲染优先级
		*/
		glbInt32 GetRenderOrder();
		/**
		* @brief 获取对象装载优先级
		* @return 对象装载优先级
		*/
		glbInt32 GetLoadOrder();
		/**
		* @brief 设置元素
		* @param[in]  feature 对象对应的元素
		* @param[in]  geoField 几何字段
		*/
		void SetFeature(CGlbFeature *feature,const glbWChar* geoField=L"");
		/**
		* @brief 获取元素
		* @return 对象对应的元素
		*/
		CGlbFeature *GetFeature();
		/**
		* @brief 设置元素图层
		* @param[in]  featureLayer 元素所在的图层
		*/
		void SetFeatureLayer(CGlbGlobeLayer *featureLayer);
		/**
		* @brief 获取元素图层
		* @return 对象对应的元素图层
		*/
		CGlbGlobeLayer* GetFeatureLayer();
		/**
		* @brief 获取对象节点包围盒
		* @param[in]  isWorld true表示获取世界坐标包围盒(单位：米)
		*  -                                    false表示获取地理坐标包围盒（单位：度）
		* @return 节点包围盒
		*/
		virtual glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief 设置对象所在八叉树节点 
		* @param[in]  octree 对象所在八叉树节点
		*/
		void SetOctree(void *octree);		
		/**
		* @brief 获取对象所在八叉树节点
		* @return 对象所在八叉树节点
		*/
		void *GetOctree();		
		/**
		* @brief 设置对象所在四叉树节点 
		* @param[in]  qtree 对象所在四叉树节点
		*/
		void SetQtree(void *qtree);

		/**
		* @brief 获取对象所在四叉树节点
		* @return  对象所在四叉树节点
		*/
		void *GetQtree();

		/**
		* @brief 获取对象的海拔模式
		* @return 海拔模式枚举
		*/
		GlbAltitudeModeEnum GetAltitudeMode();
		/**
		* @brief 相对地形绘制时，更新对象对应点的地形高度
		*/
		virtual void UpdateElevate();
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		virtual IGlbGeometry *GetOutLine();
		/**
		* @brief 设置对象的渲染信息
		* @param[in]  renderInfo 对象的渲染信息
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief 获取对象的渲染信息
		* @return 对象的渲染信息
		*/
		virtual GlbRenderInfo *GetRenderInfo();
		/**
		* @brief 设置对象显隐
		* @param[in]  isShow 对象显示设置为true，隐藏设置为false
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief 获取对象的显隐标识
		* @return 显示返回true，隐藏返回false
		*/
		glbBool IsShow();	

		/**
		* @brief 设置对象是否开启渐进渐出标识
		* @param[in]  isFaded 开启渐进渐出设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/
		glbBool EnableFade(glbBool isFaded);

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
		* @brief 设置对象的选中状态
		* @param[in]  isSelected 被拣选中设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetSelected(glbBool isSelected);
		/**
		* @brief 获取对象的选中状态
		* @return 选中返回true，反之返回false
		*/
		glbBool IsSelected();
		/**
		* @brief 设置对象的裁剪状态
		* @param[in]  isSelected 被裁剪设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetCliped(glbBool isCliped);
		/**
		* @brief 获取对象的裁剪状态
		* @return 选中返回true，反之返回false
		*/
		glbBool IsCliped();
		void SetClipObject(CGlbClipObject *clipObject);
		/**
		* @brief 设置对象是否闪烁
		* @param[in]  isBlink 闪烁设置为true，不闪烁设置为false
		* @return 设置成功返回true，设置失败返回false
		*/
		virtual glbBool SetBlink(glbBool isBlink);
		/**
		* @brief 获取对象的闪烁标识
		* @return 闪烁返回true，不闪烁返回false
		*/
		glbBool IsBlink();
		/**
		* @brief 对象立即渲染	
		* @param[in]  level 对象渲染等级
		*/
		virtual void DirectDraw(glbInt32 level);
		/**
		* @brief 获取对象提示信息
		* @return 对象提示显示
		* @note 当鼠标放到对象上时会显示出来
		*/		
		const glbWChar* GetTooltip();
		/**
		* @brief 获取对象信息
		* @return 对象信息
		* @note 当点击对象时会显示出来
		*/		
		const glbWChar* GetRMessage();
		/**
		* @brief 设置对象是否是地面对象
		* @param[in]  ground 是地面对象设置为true，反之设置为false
		* @return 设置成功返回true，设置失败返回false
		*/
		glbBool SetGround(glbBool ground);
		/**
		* @brief 获取对象是否是地面对象
		* @return 是返回true，反之返回false
		*/
		glbBool IsGround();
		/**
		* @brief AGG二维绘制到像素
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		virtual void DrawToImage(glbByte *image/*RGBA*/,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief 获取当前LOD级的osg节点
		* @return 当前LOD级的osg节点
		*/
		osg::Node *GetOsgNode();

		/**
		* @brief 获取Precompile的osg节点
		* @return 当前Precompile osg节点
		*/
		osg::Node *GetPreCompileOsgNode();

		void CompileCompleted();
		/**
		* @brief 获取当前LOD节点的大小，单位“字节”
		* @return 当前LOD级的osg节点大小
		*/
		virtual glbInt32 GetOsgNodeSize();
		/**
		* @brief 设置在挂队列中
		* @return  
		*/
		void DecreaseHangTaskNum(void);

		/**
		* @brief 反序列化对象，解析xml获取对象属性值
		* @param[in]  node libXML2节点
		* @param[in]  prjPath 工程文件目录
		* @return 反序列化成功返回true、反之返回false
		*/
		virtual glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		virtual glbBool Load2(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief 序列化对象，序列化对象属性到xml工程文件
		* @param[in]  os 工程文件输出流
		* @param[in]  prjPath 工程文件目录
		* @return 序列化成功返回true、反之返回false
		*/
		virtual glbBool Save(xmlNodePtr node,const glbWChar* prjPath);

		/**
		* @brief 设置对象的父对象
		* @param[in]  parentObj 父对象
		*/
		void SetParentObject(CGlbGlobeObject *parentObj);

		/**
		* @brief 获取对象的父对象
		*/
		CGlbGlobeObject *GetParentObject()const;
		/**
		*  @brief 获取子对象数目.
		*/
		virtual glbInt32 GetCount();
		/**
		*  @brief 获取子对象.
		*/
		virtual CGlbGlobeRObject *GetRObject(glbInt32 idx);
		/**
		* @brief 设置对象进入编辑状态
		* @return 成功返回true,失败返回false
		*/
		virtual glbBool SetEdit(glbBool isEdit);
		/**
		* @brief 获取对象是否在编辑状态
		* @return 在编辑状态返回true,否则返回false
		*/
		glbBool IsEdit();
		/**
		* @brief 设置对象编辑模式
		* @return 成功返回true,失败返回false
		*/
		glbBool SetEditMode(GlbGlobeRObEditModeEnum editMode);
		/**
		* @brief 获取对象编辑模式
		* @return 对象当前编辑模式
		*/
		GlbGlobeRObEditModeEnum GetEditMode();
		/**
		* @brief 获取对象编辑节点
		* @return 对象当前编辑节点
		*/
		osg::Group *GetEditNode();
		/**
		* @brief 设置对象通用数据
		* @param[in] data 通用数据，便于symbol与object之间的数据传输
		*/
		void SetCustomData(osg::Object *data);
		/**
		* @brief 获取对象通用数据
		* @return 对象通用数据
		*/
		osg::Object *GetCustomData();
		/**
		* @brief 获取当前视频对象的索引
		* @return 当前视频对象的索引
		*/
		glbInt32 GetVideoIndex();
		/**
		* @brief 设置对象通用数据
		* @param[in] str 对象通用数据
		*/
		void SetCustomDouble(glbDouble customDouble);
		/**
		* @brief 获取对象通用数据
		* @return 对象通用数据
		*/
		glbDouble GetCustomDouble();
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
		/**
		* @brief 设置对象预编译显示列表是否完成
		* @param isPreCompiled true表示显示列表已经预编译完成 false 表示显示列表编译还没有完成
		*/
		void SetIsPreCompiled(glbBool isPreCompiled);
		/**
		* @brief 获取对象预编译显示列表是否完成
		* @return 完成返回true,否则返回false
		*/
		glbBool IsPreCompiled();

		// 成员变量
		typedef std::pair<glbDouble,glbDouble>  MinMaxPair;
		typedef std::vector<MinMaxPair> RangeList;

		/**
		* @brief 开启或关闭lod
		* @param enable true为开启，false 为关闭
		* @return 无
		*/
		void EnableLOD(glbBool enable);

		/**
		* @brief lod是否开启
		* @return true为开启，false 为关闭
		*/
		glbBool IsLODEnable();

		/**
		* @brief 计算对象在distance距离时的显示等级
		* @param distance 相机到object的距离
		* @return level等级
		*/
		virtual glbInt32 ComputeRefLevel(glbDouble distance);

		/**
		* @brief添加lod信息
		* @param level  等级
		* @param min 最近距离
		* @param max 最远距离
		* @return 无
		*/
		void AddLODRange(glbInt32 level, glbDouble min, glbDouble max);

		/**
		* @brief 获取对象rangeList列表		
		* @return rangeList列表
		*/
		inline const RangeList& getLODRangeList() const { return mpt_rangeList; }

		/**
		* @brief 设置对象rangeList列表		
		* @param [in] rangeList lod列表
		* @return 无
		*/
		inline void setRangeList(const RangeList& rangeList) { mpt_rangeList=rangeList; }
		/**
		* @brief 设置对象是否允许卸载
		* @param [in] isAllowUnload 是否允许被卸载 true-允许卸载  false-不可卸载
		* @return 无
		*/
		void SetIsAllowUnLoad(glbBool isAllowUnload=false);
		/**
		* @brief 获取对象是否允许被卸载		
		* @return 对象允许被卸载 true-允许卸载  false-不可卸载
		*/
		glbBool IsAllowUnLoad();

		/**
		* @brief 设置对象是否允许被选中
		* @param [in] isEnableSelect 是否允许被选中 true-允许被选中  false-不可被选中
		* @return 无
		*/
		void EnableSelect(glbBool isEnableSelect);
		/**
		* @brief 获取对象是否允许被选中	
		* @return 对象允许被选中 true-允许选中  false-不可选中
		*/
		glbBool IsSelectEnable();
		/**
		* @brief 线与对象的交点
		* @param vtStart  起点
		* @param vtEnd  终点
		* @param interPoint 交点
		* @return 
		*/
		virtual glbBool interset(osg::Vec3d vtStart, osg::Vec3d vtEnd, osg::Vec3d& interPoint);
		/**
		* @brief 设置数据集
		* @param[in]  dset 对象对应的数据集
		*/
		void SetDataset(IGlbDataset *dset);
		/**
		* @brief 获取元素
		* @return 对象对应的元素
		*/
		IGlbDataset *GetDataset();
	protected:
		osg::ref_ptr<osg::Node> mpt_node;///<OSG节点
		osg::ref_ptr<osg::Node> mpt_precompileNode;///<OSG节点
		glbBool                 mpt_isUseInstance;///<是否实例化对象
		glbInt32                mpt_currLevel;///<当前LOD级数
		glbInt32                mpt_preLevel;///<上一次LOD级数
		osg::ref_ptr<osg::Node> mpt_parent;///<父节点

		osg::ref_ptr<osg::Node> mpt_preNode;///<上一级OSG节点
		glbBool                 mpt_loadState;///<是否正在装载数据		
		glbBool                 mpt_compileState;///<是否正在预编译数据		
		glbInt32                mpt_HangTaskNum;  ///<挂任务计数		
		glbDouble               mpt_maxVisibleDistance;///<最大可见距离
		glbDouble               mpt_minVisibleDistance;///<最小可见距离
		glbInt32                mpt_renderOrder;///<渲染优先级 注:数值小的先绘制，数值大的后绘制 
		glbInt32                mpt_loadOrder;///<装载优先级
		GlbAltitudeModeEnum     mpt_altitudeMode;///<高程模式
		glbBool                 mpt_isSelected;///<是否选中
		glbBool                 mpt_isCliped;//是否裁剪
		glbBool                 mpt_isShow;///<用户控制是否显示
		glbBool                 mpt_isDispShow;///<调度控制是否显示
		glbBool                 mpt_isBlink;///<是否闪烁
		glbBool                 mpt_isGround;///<是否是地面对象
		glbref_ptr<CGlbFeature> mpt_feature;///<附加元素
		glbref_ptr<IGlbDataset> mpt_dataset;///<附加的数据集【grid对象用】
		CGlbWString             mpt_geoField;///<几何字段
		CGlbWString             mpt_tooltip;///<提示信息
		CGlbWString             mpt_message;///<信息
		glbBool                      mpt_isFaded;//是否开启渐进渐出
		glbFloat                     mpt_fadeStartAlpha;//渐变开始alpha值
		glbFloat                     mpt_fadeEndAlpha;//渐变结束alpha值
		glbDouble                 mpt_fadeDurationTime;//渐变周期时间
		osg::Vec4					mpt_fadeColor;//渐变颜色，一般都是对象的FillColor
		/*
		对象不能智能引用图层.
		如果这样,删除图层时,图层的引用计数会>0,图层不能析构.
		*/
		/*glbref_ptr<CGlbGlobeLayer>*/ CGlbGlobeLayer* mpt_featureLayer;///<要素图层

		CGlbGlobeObject *mpt_parentObj;///<父对象[组合对象用]

		glbBool mpt_isEdit;				///<是否在编辑状态
		GlbGlobeRObEditModeEnum mpt_editMode;///<编辑模式
		osg::ref_ptr<osg::Group>  mpr_editNode;				///<编辑对象.

		glbBool					mpt_isNeedToPreCompile;		///<是否需要预编译显示列表
		glbBool					mpt_isPreCompiled;			///<预编译是否完成

		glbBool					mpt_isLODEnable;			///<对象是否使用LOD
		RangeList               mpt_rangeList;				///<对象lod列表
		glbBool					mpt_isAllowUnLoad;			///<对象是否卸载
		glbBool					mpt_isCameraAltitudeAsDistance; ///<是否使用相机海拔高作为相机到对象的距离
		glbBool					mpt_isEnableSelect;			///<是否允许被选中
		glbInt32                mpt_videoIndex;//记录当前视频的索引
	private:
		glbBool mpr_isInViewPort;///<是否在可见区域
		glbBool mpr_isDestroy;///<是否销毁
		void *mpr_octree;///<八叉树节点
		void *mpr_qtree;///<四叉树节点
		osg::ref_ptr<osg::Object> mpr_customData;
		glbDouble                             mpr_customDouble;
		glbref_ptr<CGlbClipObject> mpr_clipObject;
	};
}