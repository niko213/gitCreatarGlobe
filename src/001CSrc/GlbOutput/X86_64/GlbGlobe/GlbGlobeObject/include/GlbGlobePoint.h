/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
* @file    GlbGlobePoint.h
* @brief   点对象头文件，文档定义CGlbGlobePoint类
* @version 1.0
* @author  ChenPeng
* @date    2014-5-6 15:30
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeREObject.h"
#include "CGlbGlobe.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobePoint点对象类				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-6 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobePoint : public CGlbGlobeREObject
	{
	public:
		class CGlbGlobePointTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobePointTask(CGlbGlobePoint *obj,glbInt32 level);
			~CGlbGlobePointTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobePoint> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief 构造函数
		*/
		CGlbGlobePoint(void);
		/**
		* @brief 析构函数
		*/
		~CGlbGlobePoint(void);
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
		glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief 反序列化对象，解析xml获取对象属性值
		* @param[in]  node libXML2节点
		* @param[in]  prjPath 工程文件目录
		* @return 反序列化成功返回true、反之返回false
		*/
		glbBool Load2(xmlNodePtr* node,const glbWChar* prjPath);
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
		* @brief 相对地形绘制时，更新对象对应点的地形高度
		*/
		 void UpdateElevate();
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
		* @brief 设置对象海拔模式
		* @param[in]  mode 海拔模式枚举
		* @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		* @return 设置成功返回true，设置失败返回false
		*/
		 glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);
		/**
		* @brief 设置对象渲染优先级
		* @param[in]  order 对象渲染优先级
		*/
		 void SetRenderOrder(glbInt32 order);
		 /**
		 * @brief 设置对象位置
		 * @param[in]  xOrLon 经度（单位：度）或x坐标
		 * @param[in]  yOrLat 纬度（单位：度）或y坐标
		 * @param[in]  zOrAlt 高程坐标
		 * @param[out] isNeedDirectDraw 是否需要调用DirectDraw才能立即看到修改效果：true需要,false不需要.
		 */
		void SetPosition(glbDouble xOrLon,glbDouble yOrLat,glbDouble zOrAlt,glbBool &isNeedDirectDraw);
		/**
		* @brief 设置对象位置
		* @param[out]  xOrLon 经度（单位：度）或x坐标
		* @param[out]  yOrLat 纬度（单位：度）或y坐标
		* @param[out]  zOrAlt 高程坐标
		*/
		void GetPosition(glbDouble *xOrLon,glbDouble *yOrLat,glbDouble *zOrAlt);
		/**
		* @brief AGG二维绘制到像素
		* @param[in] image 底图缓冲区
		* @param[in] imageW 底图的宽
		* @param[in] imageH 底图的高
		* @param[in] ext 地形块对应的范围
		*/
		void DrawToImage(glbByte *image/*RGBA*/,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief 获取当前LOD节点的大小，单位“字节”
		* @return 当前LOD级的osg节点大小
		*/
		glbInt32 GetOsgNodeSize();

		/**
		* @brief 设置对象进入或退出"编辑"状态
		* @param[in]  isEdit 进入编辑状态为true，退出为false
		* @return 成功返回true，失败返回false
		*/
		glbBool SetEdit(glbBool isEdit);

		/**
		 * @brief 计算对象在distance距离时的显示等级
		 * @param distance 相机到object的距离
		 * @return level等级
		*/
		virtual glbInt32 ComputeRefLevel(glbDouble distance);
		/**
		 * @brief 是否启动了动态渐变标注
		 * @param 无
		 * @return true 启动; false 关闭 
		*/
		glbBool IsFadingDynamicLabel();
		/**
		 * @brief 开启/关闭 动态渐变标注
		 * @param true 开; false 关
		 * @return 
		*/
		void SetFadingDynamicLabel(glbBool isFading);
	private:
		//装载数据
		void ReadData(glbInt32 level,glbBool isDirect = false);
		//SetRenderInfo
		//isNeedDirectDraw标识节点是否需要重新绘制
		glbBool DealModelSymbol(GlbMarkerModelSymbolInfo *modelInfo,glbBool &isNeedDirectDraw);
		glbBool Deal2DShapeSymbol(GlbMarker2DShapeSymbolInfo *marker2DInfo,glbBool &isNeedDirectDraw);
		glbBool Deal3DShapeSymbol(GlbMarker3DShapeSymbolInfo *marker3DInfo,glbBool &isNeedDirectDraw);
		glbBool DealPixelSymbol(GlbMarkerPixelSymbolInfo *pixelInfo,glbBool &isNeedDirectDraw);
		glbBool DealLabelSymbol(GlbMarkerLabelSymbolInfo *labelInfo,glbBool &isNeedDirectDraw);
		glbBool DealDynamicLabelSymbol(GlbMarkerDynamicLabelSymbolInfo *dynamicLabelInfo,glbBool &isNeedDirectDraw);
		glbBool DealImageSymbol(GlbMarkerImageSymbolInfo *imageInfo,glbBool &isNeedDirectDraw);
		//ReadData
		void DealModelOpacity(osg::Node *node,GlbMarkerModelSymbolInfo *modelInfo);
		void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
		void DealModePosByChangeAltitudeOrChangePos();		
		void DealSelected();
		void DealBlink();
		void DealEdit();
		
		glbInt32 ComputeNodeSize(osg::Node *node);
		
		//处理多点时提供获取坐标点方法
		glbBool GetFeaturePoint(glbInt32 idx,glbDouble *ptx,glbDouble *pty,glbDouble *ptz,glbDouble *ptm);

		void DirtyOnTerrainObject();
		//从场景中移除节点，用于非贴地形模式转换为贴地形模式时，将场景中的非贴地形对象节点移除
		void RemoveNodeFromScene(glbBool isClean=true);

		//解析对象的颜色[填充]，渐进的时候需要
		void ParseObjectFadeColor();

		osg::Matrixd ComputePosOffsetMatrix();
	private:
		glbDouble                 mpr_distance;  ///<相机到对象的距离
		glbref_ptr<IGlbGeometry>  mpr_outline;   ///<对象的2D轮廓		
		glbDouble                 mpr_elevation; ///<当前地形高
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;///<对象渲染信息
		glbDouble                 mpr_xOrLon;    ///<经度（单位：度）或x坐标
		glbDouble                 mpr_yOrLat;    ///<纬度（单位：度）或y坐标
		glbDouble                 mpr_zOrAlt;    ///<高程坐标
		glbInt32                  mpr_objSize;   ///<obj的size
		GlbCriticalSection        mpr_addToScene_critical;	///<节点添加场景临界区
		GlbCriticalSection        mpr_readData_critical;	///<ReadData创建节点临界区		
		glbBool                   mpr_isCanDrawImage;		///<控制贴地面的显隐			
		glbBool                   mpr_needReReadData;		///<重新读取数据标志
		glbref_ptr<IGlbGeometry> mpr_noRotOutline;
		glbBool mpr_isDirectDraw;///<标识贴地对象的重绘
		glbInt32				  mpr_zerolevelObjNodeSize;		///<0级节点的节点大小
		glbInt32				  mpr_preNodeSize;

		GlbCriticalSection		  mpr_outline_critical;		///<outline读写临界区

		glbBool                            mpr_isFadingDynamicLabel;
		glbDouble				  mpr_preDistance; //<用于图片对象优化减少计算外包次数
	};
}