/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeSection.h
* @brief   剖面对象头文件，文档定义CGlbGlobeSection类
* @version 1.0
* @author  马林
* @date    2014-11-20 14:30
*********************************************************************/
#pragma once

#include "GlbGlobeObjectExport.h"
#include "GlbGlobeREObject.h"
#include "IGlbGlobeTask.h"
namespace GlbGlobe{
	/**
	* @brief CGlbGlobeDrill钻孔对象类				 
	* @version 1.0
	* @author  马林
	* @date    2014-11-18 16:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeSection : public CGlbGlobeREObject
	{
	private:
		class CGlbGlobeSectionTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeSectionTask(CGlbGlobeSection *obj,glbInt32 level);
			~CGlbGlobeSectionTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeSection> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeSection(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeSection(void);
		/**
		* @brief 获取对象类型
		* @return 对象枚举类型
		*/
		GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief 反序列化对象，解析xml获取对象属性值
		* @param[in]  node libXML2节点
		* @param[in]  prjPath 工程文件目录
		* @return 反序列化成功返回true、反之返回false
		*/
		virtual glbBool Load(xmlNodePtr* node,glbWChar* prjPath);
		virtual glbBool Load2(xmlNodePtr* node,glbWChar* prjPath);
		/**
		* @brief 序列化对象，序列化对象属性到xml工程文件
		* @param[in]  os 工程文件输出流
		* @param[in]  prjPath 工程文件目录
		* @return 序列化成功返回true、反之返回false
		*/
		virtual glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		/**
		* @brief 获取相机与对象的距离
		* @param[in]  cameraPos 相机在场景中的位置
		* @param[in]  isCompute 距离是否需要重新计算，重新计算设置true，反之设置false
		* @return 相机与对象的距离
		*/
		glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute=true);
		/**
		* @brief 设置对象海拔模式
		* @param[in]  mode 海拔模式枚举
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，设置失败返回false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);
		/**
		* @brief 装载数据，调度器会频繁的调用该方法
		* @param[in]  level 需要装载数据的等级
		*/
		 void LoadData(glbInt32 level);
		/**
		* @brief 挂osg节点或者交换osg节点到场景树
		*/
		 void AddToScene();
		/**
		* @brief 从场景树摘除osg节点
		* @param[in]  isClean true表示同时删除所有osg节点以节省内存
		* @return 返回节省的内存大小
		*/
		 glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief 获取对象节点包围盒
		* @param[in]  isWorld true表示获取世界坐标包围盒(单位：米)
		*  -                                    false表示获取地理坐标包围盒（单位：度）
		* @return 节点包围盒,有可能是NULL
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		 IGlbGeometry *GetOutLine();
		/**
		* @brief 设置对象的渲染信息
		* @param[in]  renderInfo 对象的渲染信息
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，反之返回false
		*/
		 glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief 获取对象的渲染信息
		* @return 对象的渲染信息
		*/
		 GlbRenderInfo *GetRenderInfo();
		/**
		* @brief 设置对象显隐
		* @param[in]  isShow 对象显示设置为true，隐藏设置为false
		* @return 设置成功返回true，反之返回false
		*/
		 glbBool SetShow(glbBool isShow,glbBool isOnState = false);
		/**
		* @brief 设置对象的选中状态
		* @param[in]  isSelected 被拣选中设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/
		 glbBool SetSelected(glbBool isSelected);
		/**
		* @brief 设置对象是否闪烁
		* @param[in]  isBlink 闪烁设置为true，不闪烁设置为false
		* @return 设置成功返回true，设置失败返回false
		*/
		 glbBool SetBlink(glbBool isBlink);
		/**
		* @brief 对象立即渲染
		* @param[in]  level 对象渲染等级
		*/
		 void DirectDraw(glbInt32 level);
		/**
		* @brief 获取当前LOD节点的大小，单位“字节”
		* @return 当前LOD级的osg节点大小
		*/
		glbInt32 GetOsgNodeSize();

		/**************		Section剖面类的专有方法		*************/
		/**
		* @brief 拾取弧段.
		* @param[in]  mx 鼠标X值
		* @param[in]  my 鼠标Y值
		* @param[out] arcId 输出拾取到的弧段Id
		* @return 成功返回true、反之返回false
		*/
		glbBool PickArc(int mx,int my,glbInt32& arcId);
		/**
		* @brief 设置弧段是否选中状态.
		* @param[in]  arcId ,弧段Id
		* @param[in]  isSelected,true代表选中.
		*/
		void SetArcSelected(glbInt32 arcId,glbBool isSelected);
		/**
		* @brief 设置弧段是否闪烁
		* @param[in]  arcId ,弧段Id
		* @param[in]  isBlink,true代表闪烁.
		*/
		void SetArcBlink(glbInt32 arcId,glbBool isBlink);
		/**
		* @brief 设置弧段是否显示
		* @param[in]  arcId ,弧段Id
		* @param[in]  isShow,true代表显示.
		*/
		void SetArcShow(glbInt32 arcId,glbBool isShow);
		/**
		* @brief 拾取多边形.
		* @param[in]  mx 鼠标X值
		* @param[in]  my 鼠标Y值
		* @param[out] polyId 输出拾取到的多边形Id
		* @return 成功返回true、反之返回false
		*/
		glbBool PickPoly(int mx,int my,glbInt32& polyId);
		/**
		* @brief 设置多边形是否选中状态.
		* @param[in]  polyId ,多边形Id
		* @param[in]  isSelected,true代表选中.
		*/
		void SetPolySelected(glbInt32 polyId,glbBool isSelected);
		/**
		* @brief 设置多边形是否闪烁
		* @param[in]  polyId ,多边形Id
		* @param[in]  isBlink,true代表闪烁.
		*/
		void SetPolyBlink(glbInt32 polyId,glbBool isBlink);
		/**
		* @brief 设置弧段是否显示
		* @param[in]  arcId ,弧段Id
		* @param[in]  isShow,true代表显示.
		*/
		void SetPolyShow(glbInt32 polyId,glbBool isShow);
	private:
		//装载数据
		void ReadData(glbInt32 level,glbBool isDirect = false);
		void DealSelected();
		void DealBlink();
		glbInt32 ComputeNodeSize(osg::Node *node);

		osg::Switch* FindSectionNode();
		osg::Switch* FindArcNode();
		osg::Switch* FindPolyNode();
		osg::Node* FindArcNodeById(glbInt32 arcId);
		osg::Node* FindPolyNodeById(glbInt32 polyId);

		glbBool DealSectionRenderInfo(GlbSectionRenderInfo* newSectionRenderInfo, glbBool &isNeedDirectDraw);
		
		bool DealSectionArcPolyONOFF(glbBool isSecShow, glbBool isArcShow, glbBool isPolyShow);

		void DealSectionColor(GlbRenderColor* clr);
		void DealSectionOpacity(GlbRenderInt32* opacity);

		void DealArcColor(GlbRenderColor* clr);
		void DealArcOpacity(GlbRenderInt32* opacity);
		void DealArcWidth(GlbRenderDouble* width);

		void DealPolyColor(GlbRenderColor* clr);
		void DealPolyOpacity(GlbRenderInt32* opacity);

		//解析对象的颜色，渐进的时候需要
		void ParseObjectFadeColor();
	private:
		glbDouble                 mpr_distance;  ///<相机到对象的距离
		glbref_ptr<IGlbGeometry>  mpr_outline;   ///<对象的2D轮廓		
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;///<对象渲染信息		
		glbInt32                  mpr_objSize;   ///<obj的size
		GlbCriticalSection        mpr_addToScene_critical;	///<节点添加场景临界区
		GlbCriticalSection        mpr_readData_critical;	///<ReadData创建节点临界区						
		glbBool                   mpr_needReReadData;		///<重新读取数据标志

		std::map<glbInt32, glbBool> mpr_arcIsSeclected;
		std::map<glbInt32, glbBool> mpr_polyIsSelected;


	};

}

