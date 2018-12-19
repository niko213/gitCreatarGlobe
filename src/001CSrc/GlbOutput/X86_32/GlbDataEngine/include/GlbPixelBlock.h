/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbPixelBlock.h
  * @brief   像素块类头文件
  *
  * 这个档案定义CGlbPixelBlock这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-10 16:40
*********************************************************************/
#pragma once
#include "GlbExtent.h"
#include "GlbSpatialReference.h"
#include "GlbDataEngineExport.h"
#include "GlbDataEngineType.h"
#include "glbref_ptr.h"

/**
  * @brief 像素块的管理类
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-5-11 14:40
  */
class GLBDATAENGINE_API CGlbPixelBlock:public CGlbReference
{
public:
	/**
	* @brief 构造函数
	*
	* @param pixelType   像素类型
	* @param pixelLayout 像素布局方式
	* @return void
	*/
	CGlbPixelBlock(GlbPixelTypeEnum pixelType, GlbPixelLayoutTypeEnum pixelLayout);
	/**
	* @brief 初始化函数
	*
	* @param nBands  波段数
	* @param columns 列数
	* @param rows    行数
	* @return 成功:返回true，失败:false
	*/
	glbBool Initialize(glbInt32 nBands, glbInt32 columns, glbInt32 rows,glbDouble nodata=0);
	/**
	* @brief 析构函数
	*
	* @return void
	*/
	~CGlbPixelBlock();
	/**
	* @brief 判断该像素块是否有效
	*
	* @return 有效:返回true，无效:false
	*/
	glbBool               IsValid();
	/**
	* @brief 获取数据buffer指针
	*
	* @param ppData 数据buffer的地址
	* @return 成功:返回像素数据的字节数，失败:0
	*/
	glbInt32              GetPixelData(glbByte** ppData);	
	/**
	* @brief 获取掩码buffer指针
	*        掩码取值范围[0,255],0代表无效.        
	*
	* @param ppMask 掩码buffer的地址
	* @return 成功:返回像素掩码字节数，失败:0
	*/
	glbInt32              GetMaskData(glbByte** ppMask);
	/**
	* @brief 得到波段数
	*
	* @return 成功:返回波段数，失败:0
	*/
	glbInt32               GetBandCount();
	/**
	* @brief 获取像素类型
	*
	* @return 成功:返回像素类型，失败:无效类型
	*/
	GlbPixelTypeEnum       GetPixelType();
	/**
	* @brief 获取像素块的列数和行数
	*
	* @param colums 返回列数
	* @param rows   返回行数
	* @return void
	*/
	void                   GetSize(glbInt32& colums, glbInt32& rows);
	/**
	* @brief 获取像素布局
	*
	* @return 成功:返回像素布局，失败:无效布局类型
	*/
	GlbPixelLayoutTypeEnum GetPixelLayout();	
	/**
	* @brief 导出到文本文件
	*
	* @param fileName 输出文件的文件名(带路径)
	* @return 成功:返回true，失败:fasle
	*/
	glbBool                ExportToText(const glbWChar* fileName);	
	/**
	* @brief 导出到tiff文件
	*
	* @param fileName 输出文件的文件名(带路径)
	* @param extent 空间范围
	* @param srs 坐标信息
	* @return 成功:返回true，失败:fasle
	*/
	glbBool                ExportToTiff(const glbWChar*   fileName,
		                                const CGlbExtent* extent=NULL,
										const glbWChar*   srs   =NULL);
	/**
	* @brief 从当前块构造出rgba数据
	*        像素类型必须是byte
	*        像素布局必须是像素交叉
	*        如果通道数是4个，返回mpr_data
	*        如果通道数是3个，认为是rgb，把mask作为a通道合并成rgba，修改波段数为4，修改mpr_data
	*        其他通道格式，不支持
	*
	* @return 成功:返回true，失败:false
	*/
	glbBool                ConvertToRGBA();

	/**
	* @brief 获取像素的值
	*
	* @param iband 波段序号
	* @param col 列 
	* @param row 行
	* @param ppVal 返回像素的的指针地址,类型不固定
	* @return 成功:返回true，失败:fasle
	*/
	glbBool GetPixelVal( glbInt32 iband,  glbInt32 col,  glbInt32 row,  void** ppVal);
	
	/**
	* @brief 设置像素的值
	*
	* @param iband 波段序号
	* @param col 列 
	* @param row 行
	* @param ppVal 像素的的指针地址,类型不固定
	* @return 成功:返回true，失败:fasle
	*/
	glbBool SetPixelVal( glbInt32 iband,  glbInt32 col,  glbInt32 row,  void* ppVal);
	
	/**
	* @brief 获取掩码值
	*        掩码取值范围[0,255],0代表无效.
	*
	* @param col 列 
	* @param row 行
	* @param ppVal 返回地址
	* @return 成功:返回true，失败:fasle
	*/
	glbBool GetMaskVal(glbInt32 col,  glbInt32 row,  glbByte** ppVal);
	/**
	* @brief 设置掩码值
	*        掩码取值范围[0,255],0代表无效.
	*
	* @param col  列 
	* @param row  行
	* @param mVal 掩码值
	* @return 成功:返回true，失败:fasle
	*/
	glbBool SetMaskVal(glbInt32 col,  glbInt32 row,  glbByte mVal);
	/**
	* @brief 数据强制清0
	*
	* @return 成功:返回true，失败:fasle
	*/
	glbBool Clear(double nodata=0);
	
	/**
	* @brief 将pixelVal等于maskVal的像素掩码设置为零,以iUseBand为基准
	*
	* @param iUseBand 基准波段 
	* @param maskVal maks的像素值
	* @return 成功:返回true，失败:fasle
	*/
	glbBool Mask(glbInt32 iUseBand,void*maskVal);

	/**
	* @brief 将对应的mask为0的所有波段像素值,填充为无效值。如果没有无效值,不填充
	*
	* @param pNoDataVal 如果该参数为NULL,不操作;否则,无效值全部填充为该值 
	* @return 成功:返回true，失败:fasle
	*/
	glbBool ReplaceNoData(glbDouble NoData,glbDouble rpValue);

	/**
	* @brief 源像素块和当前像素块混合,输出到当前像素块
	*        临近的数据去平均值
	*        如果波段数不一致,不能处理.
	*        如果src和size和这个的size不一致,不能处理.
	*        如果类型不一致,不处理
	*
	* @param src 源像素块
	* @param sStartX 源像素X开始位置，上下其为-1
	* @param sStartY 源像素Y开始位置，左右其为-1
	* @param dStartX 目标像素X开始位置，上下其为-1
	* @param dStartY 目标像素Y开始位置，左右其为-1
	* @param cx 混合的像素宽度 上下不考虑该值
	* @param cy 混合的像素高度 左右不考虑该值
	* @return 成功:返回true，失败:fasle
	*/
	glbBool Blend(CGlbPixelBlock*src,glbInt32 sStartX,glbInt32 sStartY, 
		glbInt32 dStartX,glbInt32 dStartY,glbInt32 cx=4,glbInt32 cy=4);
	/**
	* @brief 获取像素块的范围
	*
	* @return 成功:返回范围，失败:NULL
	*/
	const CGlbExtent* GetExtent() {return mpr_extent.get();}
	/**
	* @brief 设置像素块的范围
	*
	* @param ext 范围
	* @return void
	*/
	void SetExtent(CGlbExtent* ext) {mpr_extent=ext;}
private:
	GlbPixelTypeEnum       mpr_pixelType;  ///<像素类型
	GlbPixelLayoutTypeEnum mpr_pixelLayout;///<像素布局
	glbInt32               mpr_nBands;     ///<波段数
	glbInt32               mpr_rows;       ///<行数
	glbInt32               mpr_colums;     ///<列数	
	glbByte*               mpr_data;       ///<像素数据
	glbByte*               mpr_maskdata;   ///<mask数据，0表示是无效数据；其他是有效数据
	glbref_ptr<CGlbExtent> mpr_extent;     ///<范围
};
