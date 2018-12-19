/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobeDigHole.h
* @brief   挖洞对象头文件，文档定义CGlbGlobeDigHole类
* @version 1.0
* @author  敖建
* @date    2014-8-22 15:34
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeReobject.h"
#include "CGlbGlobe.h"
#include "GlbPolygon.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeDigHole线对象类				 
	* @version 1.0
	* @author  敖建
	* @date    2014-8-22 15:34
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeDigHole : public CGlbGlobeREObject 
	{
	public:
		class CGlbGlobeDigHoleTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeDigHoleTask(CGlbGlobeDigHole *obj,glbInt32 level);
			~CGlbGlobeDigHoleTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeDigHole> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobeDigHole(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobeDigHole(void);
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
		glbBool Load(xmlNodePtr *node,const glbWChar* prjPath);
		glbBool Load2(xmlNodePtr *node,const glbWChar* prjPath);
		/**
		* @brief 序列化对象，序列化对象属性到xml工程文件
		* @param[in]  os 工程文件输出流
		* @param[in]  prjPath 工程文件目录
		* @return 序列化成功返回true、反之返回false
		*/
		glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		/**
		* @brief 获取相机与对象的距离
		* @param[in]  cameraPos 相机在场景中的位置
		* @param[in]  isCompute 距离是否需要重新计算，重新计算设置true，反之设置false
		* @return 相机与对象的距离
		*/
		glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
		/**
		* @brief 装载数据，调度器会频繁的调用该方法
		* @param[in]  level 需要装载数据的等级
		**/
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
		* @return 节点包围盒
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief 相对地形绘制时，更新对象对应点的地形高度
		**/
		//void UpdateElevate();
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
		glbBool SetRenderInfo(GlbRenderInfo *renderInfo ,glbBool &isNeedDirectDraw);
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
		/*
		* @brief 获取当前LOD节点的大小，单位“字节”
		* @return 当前LOD级的osg节点大小
		*/
		glbInt32 GetOsgNodeSize();
		/**
		* @brief 设置挖洞对象多边形几何体
		* @param[in] region 多边形几何体
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		*/
		void SetRegion(CGlbPolygon *region ,glbBool &isNeedDirectDraw);

		/**
		* @brief 获取挖洞对象多边形几何体
		* @return 挖洞对象多边形几何体
		*/
		CGlbPolygon* GetRegion();
		/**
		* @brief 设置挖洞对象深度
		* @param [in] depth 补底面海拔高度
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		*/
		void SetDepth(double depth ,glbBool &isNeedDirectDraw);

		/**
		* @brief 获取挖洞对象深度
		* @return 挖洞对象深度
		*/
		glbDouble GetDepth();
		/**
		* @brief 设置是否显示补的壁面
		*/
		void ShowWall(glbBool isshow);
		/**
		* @brief 获取是否显示补的壁面
		*/
		glbBool IsShowWall();
		/**
		* @brief AGG二维绘制到像素
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		void DrawToImage(glbByte *image,glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);

		/**
		* @brief 获取参与挖洞的对象数目
		*/
		glbInt32 GetDiggedCount();

		/**
		* @brief 获取参与挖洞的对象
		*/
		CGlbGlobeObject* GetDiggedObject(glbInt32 objId);

		/**
		* @brief 添加对象，参与挖洞
		*/
		glbBool AddDiggedObject(CGlbGlobeRObject* obj);

		/**
		* @brief 使对象不参与挖洞
		*/
		glbBool RemoveDiggedObject(glbInt32 objId);

		void testDigHoleObj();
	private:
		/**
		* @brief 读取数据
		*/
		void ReadData(glbInt32 level ,glbBool isDirect=false);
		/**
		* @brief 绘制挖洞对象
		*/
		osg::Node* DrawDigHole(CGlbPolygon* polygon3d,glbDouble depth);
		osg::Node* DrawDigHole2(CGlbPolygon* polygon3d,glbDouble depth);//采用阴影绘制的方法绘制挖洞对象
		osg::Node* DrawDigHole3(CGlbPolygon* polygon3d,glbDouble depth);
		// 解析多边形对象中几何体 - 返回CGlbMultiPolygon对象
		IGlbGeometry* AnalysisPolyGeomertry(CGlbFeature* feature, CGlbPolygon* polyGeo);
		glbInt32 ComputeNodeSize(osg::Node *node);
		void DealSelected();
		void DealBlink();

		void DirtyOnTerrainObject();
	private:
		glbDouble mpr_distance;///<相机到对象的距离
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;///<对象渲染信息
		glbref_ptr<CGlbPolygon> mpr_holeRegion;	//挖洞范围
		glbDouble mpr_holeDepth;	//挖洞深度
		glbBool mpr_isShowWall;		//是否显示补的壁面
		glbref_ptr<IGlbGeometry> mpr_outline;///<对象的2D轮廓
		glbDouble mpr_holeUpHei;	//蒙版往地上延伸高度
		glbDouble mpr_holeDownHei;	//蒙版往地下延伸高度
		std::vector<CGlbGlobeRObject*> mpr_diggedobjs;

		glbBool                   mpr_isCanDrawImage;		///<控制贴地面的显隐	
		GlbCriticalSection        mpr_addToScene_critical;	///<节点添加场景临界区
		GlbCriticalSection        mpr_readData_critical;	///<ReadData创建节点临界区	
		glbBool                   mpr_needReReadData;		///<重新读取数据标志
		glbInt32                  mpr_objSize;				///<obj的size
	};
}


