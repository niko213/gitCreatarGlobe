/********************************************************************
  * Copyright (c) 2014 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbGlobeDemLayer.h
  * @brief   Dem图层头文件
  *
  * 这个档案定义CGlbGlobeDemLayer这个class
  *
  * @version 1.0
  * @author  马林
  * @date    2014-5-7 17:40
*********************************************************************/
#pragma once

#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
namespace GlbGlobe
{

class GLB_DLLCLASS_EXPORT CGlbGlobeDemLayer : public CGlbGlobeLayer
{
//public:
//	enum GlbGlobeDemType
//	{
//		GLBDEMTYPE_OVERLAY = 0,		//替换模式
//		GLBDEMTYPE_ACCUMULATION = 1	//累加模式
//	};
public:
	 /**
	  * @brief dem图层构造函数
	  * @return 无
	  */
	CGlbGlobeDemLayer(void);
	 /**
	  * @brief 要素图层析构函数
	  * @return 无
	  */
	~CGlbGlobeDemLayer(void);

	/*基类接口*/
	 /**
	  * @brief 获取图层名称
	  * @return 图层名称 
	  */
	const glbWChar* GetName();
	 /**
	  * @brief 设置图层名称
	  * @param [in] name 图层名称
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

	/*类接口*/
	 /**
	  * @brief 获取是否有金字塔			
	  * @return  true表示有金字塔，false表示没有金字塔
	  */
	glbBool HasPyramid();
	 /**
	  * @brief 生成金字塔
	  * @param [in]	progress 进度信息
	  * @param [in] lzts 分块大小
	  * @param [in] isGlobe 是否是球模式 true表示是球模式，false表示平面模式
	  * @return  成功返回true，失败返回false.
	  */
	glbBool BuildPyramid(IGlbProgress * progress,glbDouble lzts , glbBool isGlobe);
	 /**
	  * @brief 获取金字塔最大和最小等级
	  * @param [out] minLevel 金子塔的最小等级
	  * @param [out] maxLevel 金子塔的最大等级	
	  * @return  成功返回true，
				 没有金字塔或失败返回false.
	  */
	glbBool GetPyramidLevels(glbInt32 *minLevel, glbInt32 *maxLevel);
	 /**
	  * @brief 创建数据块
	  * @param [in][out] pixelblock 数据块指针				
	  * @return   成功返回true，失败返回false.
	  */
	glbBool CreatePixelBlock(CGlbPixelBlock ** pixelblock );
	 /**
	  * @brief 读取数据块
	  * @param [in] level 等级
	  * @param [in] tilerow 块的行号
	  * @param [in] tilecolumn 块的列号
	  * @param [out] p_pixelblock 读取数据后的数据块
	  * @return   成功返回true，失败返回false.
	  */
	glbBool ReadData(glbInt32 level,glbInt32 tilerow,glbInt32 tilecolumn,CGlbPixelBlock* p_pixelblock);
	 /**
	  * @brief 读取数据到数据块
	  * @param [in] p_extent 数据范围
	  * @param [out] p_pixelblock 读取数据后的数据块
	  * @return   成功返回true，失败返回false.
	  */
	glbBool ReadDataEx(CGlbExtent* p_extent, CGlbPixelBlock* p_pixelblock);
	 /**
	  * @brief 获取是否是累加模式			
	  * @return  true表示是累加模式，false表示是替换模式
	  */	
	glbBool IsAccumulationMode();
	 /**
	  * @brief 设置是否是累加模式			
	  * @param  isAccumulation true表示是累加模式，false表示是替换模式
	  */	
	void	SetAccumulationMode(glbBool isAccumulation);
private:
	glbBool mpr_isAccumulationMode; // 是否是高程累加模式
};
}


