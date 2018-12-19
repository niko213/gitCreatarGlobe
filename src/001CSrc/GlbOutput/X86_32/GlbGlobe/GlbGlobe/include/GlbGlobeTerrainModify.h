/********************************************************************
* Copyright (c) 2014 北京超维创想信息技术有限公司
* All rights reserved.
*
* @file    GlbGlobeTerrainModify.h
* @brief   地形修改对象 头文件
*
* 这个档案定义GlbGlobeTerrainModify这个class
*
* @version 1.0
* @author  马林
* @date    2014-7-15 16:30
*********************************************************************/

#pragma once
//#include "GlbGlobeObjectExport.h"
#include "GlbGlobeExport.h"
#include "GlbGlobeREObject.h"
#include "GlbLine.h"
#include "CGlbGlobe.h"
#include "GlbGlobeTypes.h"
#include <osg/Geometry>

namespace GlbGlobe
{
	/**
	* @brief 地形修改方法提供者
	*
	* 该类继承于CGlbReference， 用于客户自定义地形修改
	* 
	*/
	class GLB_DLLCLASS_EXPORT IGlbGlobeTerrainProvider : public CGlbReference
	{
	public:
		IGlbGlobeTerrainProvider(){}
		virtual ~IGlbGlobeTerrainProvider(){}

		/**
		* @brief 设置对象范围
		* @param[in]  region 范围
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetRegion(CGlbLine* region)=0;

		/**
		* @brief 获取(xOrLon,yOrLat)处的高度值
		* @param[in]  xOrLon x坐标或经度(单位:度)
		* @param[in]  yOrLat y坐标或纬度(单位:度)
		* @param[in,out]  zOrAlt z坐标或高度
		* @return (xOrLon,yOrLat)在地形修改对象的范围内返回true，同时修改zOrAlt值
		不在范围内返回false.				
		*/
		virtual glbBool GetElevate(glbDouble xOrLon,glbDouble yOrLat,glbDouble &zOrAlt)=0;
	};
	/**
	* @brief 地形修改对象
	*
	* 该类继承于CGlbGlobeREObject， 替换对象区域范围内的地形起伏形态
	* 
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeTerrainModify :	public CGlbGlobeREObject
	{
	public:
		CGlbGlobeTerrainModify(void);
		~CGlbGlobeTerrainModify(void);
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
		// CGlbRObject	
		/**
		* @brief 获取相机与对象的距离
		* @param[in]  cameraPos 相机在场景中的位置
		* @param[in]  isCompute 距离是否需要重新计算，重新计算设置true，反之设置false
		* @return 相机与对象的距离
		*/
		virtual glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
		/**
		* @brief 装载数据，调度器会频繁的调用该方法
		* @param[in]  level 需要装载数据的等级
		*/
		virtual void LoadData(glbInt32 level);
		/**
		* @brief 挂osg节点或者交换osg节点到场景树
		*/
		virtual void AddToScene();
		/**
		* @brief 从场景树摘除osg节点
		* @param[in]  isClean true表示同时删除所有osg节点以节省内存
		* @return 返回节省的内存大小
		*/
		virtual glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief 获取对象节点包围盒
		* @param[in]  isWorld true表示获取世界坐标包围盒(单位：米)
		*  -                                    false表示获取地理坐标包围盒（单位：度）
		* @return 节点包围盒
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief 贴地形绘制对象的二维轮廓线 单位：地理单位
		* @return  对象的2D轮廓
		*/
		virtual IGlbGeometry *GetOutLine();
		/**
		* @brief 设置对象显隐
		* @param[in]  isShow 对象显示设置为true，隐藏设置为false
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief 设置对象的选中状态
		* @param[in]  isSelected 被拣选中设置为true，反之设置为false
		* @return 设置成功返回true，反之返回false
		*/
		virtual glbBool SetSelected(glbBool isSelected);
		/**
		* @brief 对象立即渲染	
		* @param[in]  level 对象渲染等级
		*/
		virtual void DirectDraw(glbInt32 level);
		/**
		* @brief 获取当前LOD节点的大小，单位“字节”
		* @return 当前LOD级的osg节点大小
		*/
		virtual glbInt32 GetOsgNodeSize();

		// CGlbGlobeTerrainModify
		/**
		* @brief 设置对象范围
		* @param[in]  region 范围
		* @return 设置成功返回true，反之返回false
		*/
		glbBool SetRegion(CGlbLine* region);
		/**
		* @brief 获取对象范围
		* @return 范围
		*/
		CGlbLine* GetRegion();
		/**
		* @brief 设置是否平坦
		* @param[in]  isFlat 是否平坦
		* @return 设置成功返回true，反之返回false
		*/
		glbBool SetFlat(glbBool isFlat);
		/**
		* @brief 获取是否平坦
		* @return 平坦返回true，否则返回false
		*/
		glbBool IsFlat();
		/**
		* @brief 设置地形修改方式
		* @param[in]  mode 地形修改方式
		*						GLB_TERRAINMODIFY_REPLACE	替换
		GLB_TERRAINMODIFY_BELOW		修剪下方
		GLB_TERRAINMODIFY_UP		修剪上方
		* @return 设置成功返回true，反之返回false
		*/
		glbBool SetModifyMode(GlbGlobeTerrainModifyModeEnum mode);
		/**
		* @brief 获取地形修改方式
		* @return 地形修改方式
		*/
		GlbGlobeTerrainModifyModeEnum GetModifyMode();
		/**
		* @brief 设置高度
		* @param[in]  hei 高度
		* @return 设置成功返回true，反之返回false
		*/
		glbBool SetHeight(glbDouble hei);
		/**
		* @brief 获取高度
		* @return 高度
		*/
		glbDouble GetHeight();
		/**
		* @brief 获取(xOrLon,yOrLat)处的高度值
		* @param[in]  xOrLon x坐标或经度(单位:度)
		* @param[in]  yOrLat y坐标或纬度(单位:度)
		* @param[in,out]  zOrAlt z坐标或高度
		* @return (xOrLon,yOrLat)在地形修改对象的范围内返回true，同时修改zOrAlt值
		不在范围内返回false.				
		*/
		glbBool GetElevate(glbDouble xOrLon,glbDouble yOrLat,glbDouble &zOrAlt);
		/**
		* @brief 设置地形修改器
		* @param[in]  op 地形修改器
		* @return 设置成功返回true，失败返回false
		*/
		glbBool SetElevateOp(IGlbGlobeTerrainProvider* op);

		/**
		* @brief 判断外轮廓线是否显示	
		* @return 显示返回true，隐藏返回false
		*/
		glbBool IsShowOutline();
		/**
		* @brief 设置外轮廓线显隐
		* @param[in]  isShow 是否显示外轮廓线 显示为true，隐藏为false
		* @return 成功返回true，失败返回false
		*/
		glbBool SetOutlineShow(glbBool isShow);

		/**
		* @brief 由地形修改对象修改dem数据
		* @param[in]  demdata dem数据
		* @param[in]  demsize dem尺寸
		* @param[in]  tileExtent dem数据的覆盖区域
		* @return 成功返回true，失败返回false
		*/
		glbBool ModifyDem(glbFloat* demdata, glbInt32 demsize, CGlbExtent* tileExtent);

		/**
		* @brief 设置对象进入或退出"编辑"状态
		* @param[in]  isEdit 进入编辑状态为true，退出为false
		* @return 成功返回true，失败返回false
		*/
		glbBool SetEdit(glbBool isEdit);
	private:
		// 计算节点占用的内存大小
		glbInt32 ComputeNodeSize( osg::Node *node );
		// 处理编辑状态
		void DealEdit();
		// 绘制范围轮廓线
		glbBool ReadData();
		// 绘制当前点
		glbBool DrawCurrPoint();
		/**
		* @brief 绘制范围线上的点和当前点
		* @param[in]  color 范围线上点的颜色
		* @return 成功返回true，失败返回false
		*/
		glbBool DrawPoints(glbInt32 color);
		/**
		* @brief 设置当前点
		* @param[in]  idx 当前点索引序号
		* @param[in]  color 当前点颜色 
		* @return 设置成功返回true，失败返回false
		*/
		glbBool SetCurrPoint(glbInt32 idx, glbInt32 color);
		/**
		* @brief 绘制临时点
		* @param[in]  xOrLon x坐标或经度(单位:度)
		* @param[in]  yOrLat y坐标或纬度(单位:度)
		* @param[in,out]  zOrAlt z坐标或高度
		* @param[in]  color 临时点的颜色
		* @return 成功返回true，失败返回false
		*/
		glbBool DrawTemp(glbDouble xOrLon, glbDouble yOrLat, glbDouble zOrAlt, glbInt32 color);
		/**
		* @brief 删除范围线上的某个点
		* @param[in]  idx 点索引序号
		* @return 成功返回true，失败返回false
		*/
		glbBool RemovePoint(glbInt32 idx);
		/**
		* @brief 清除所有绘制出的点	
		* @return 成功返回true，失败返回false
		*/
		glbBool ClearDraw();
	private:
		glbDouble							mpr_distance;				///<与相机距离
		glbref_ptr<IGlbGeometry>			mpr_outline;				///<贴地形绘制对象的二维轮廓线 单位：地理单位
		glbref_ptr<CGlbLine>				mpr_region;					///<地形修改范围
		glbBool								mpr_isFlat;					///<标识是否平坦
		GlbGlobeTerrainModifyModeEnum		mpr_modifymode;				///<高程表现方式
		glbDouble							mpr_height;					///<地形修改的高度
		glbref_ptr<IGlbGlobeTerrainProvider> mpr_terrainProvider;		///<地形算法提供对象

		osg::ref_ptr<osg::Geometry>			mpr_osgOutlineGeometry;		///<轮廓线drawable
		osg::ref_ptr<osg::Geometry>			mpr_lnPtsGeometry;			///<轮廓线上的点drawable
		osg::ref_ptr<osg::Geometry>			mpr_currPtGeometry;			///<当前点drawable
		osg::ref_ptr<osg::Geometry>			mpr_tempPtGeometry;			///<临时点drawable
		glbInt32							mpr_curPointIndex;			///<当前点索引号
		glbInt32							mpr_curPointColor;			///<当前点颜色

		glbInt32                            mpr_objSize;				///<节点大小
		GlbCriticalSection                  mpr_readData_critical;		///<创建节点临界区
		GlbCriticalSection                  mpr_addToScene_critical;	///<节点添加场景临界区
		glbBool								mpr_isShowOutline;
	};
}
