/********************************************************************
 * Copyright (c) 2013 超维创想
 * All rights reserved.
 *
 * @file    
 * @brief   
 * @version 1.0
 * @author  GWB
 * @date    2014-03-07 11:28
 ********************************************************************
 */
#pragma once

/**
  * @brief 数据集类型枚举
  *
  * 定义不同的地理数据集类型
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbDatasetTypeEnum
{
	GLB_DATASET_UNKNOWN                       =-1,///<未定义类型
	GLB_DATASET_OBJECTCLASS                   = 0,///<对象数据集
	GLB_DATASET_FEATURECLASS_POINT            = 1,///<单点数据集
	GLB_DATASET_FEATURECLASS_MULTIPOINT       = 2,///<多点数据集
	GLB_DATASET_FEATURECLASS_LINE             = 3,///<线数据集
	GLB_DATASET_FEATURECLASS_MULTILINE        = 4,///<多线数据集
	GLB_DATASET_FEATURECLASS_POLYGON          = 5,///<多边形数据集
	GLB_DATASET_FEATURECLASS_MULTIPOLYGON     = 6,///<多多边形数据集
	GLB_DATASET_FEATURECLASS_TIN              = 7,///<三角网数据集
	GLB_DATASET_FEATURECLASS_TEXT             = 8,///<标注数据集
	GLB_DATASET_NETWORK                       = 9,///<网络数据集
	GLB_DATASET_RASTER                        = 10,///<栅格数据集
	GLB_DATASET_TERRAIN                       = 11,///<地形数据集
	GLB_DATASET_DRILL                         = 12,///<钻孔数据集
	GLB_DATASET_SECTION                       = 13,///<剖面数据集
	GLB_DATASET_FEATURECLASS_MULTITIN         = 14,///<三角网数据集
	GLB_DATASET_TERRAIN_WMTS_TIANDITU		  = 15,///<天地图wmts数据集
	GLB_DATASET_TERRAIN_WMTS_ARCGIS			  = 16,///<Arcgis的wmts数据集
	GLB_DATASET_GRID						  = 17 ///<Grid数据集

	//GLB_TRISOLID_FEATURECLASS     = 6,///<三角网格体数据集
	//GLB_MODEL_FEATURECLASS        = 7,///<模型数据集 
	//GLB_ROUTE_FEATURECLASS        = 9,///<路由数据集
};
/**
  * @brief 基本数据类型枚举
  *
  * 定义不同的基本数据类型
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbDataTypeEnum
{
  GLB_DATATYPE_UNKNOWN  = 0,///<未知
  GLB_DATATYPE_BYTE     = 1,///<字节,无符号 8位
  //GLB_DATATYPE_CHAR     = 2,///<有符号 8位
  GLB_DATATYPE_BOOLEAN  = 3,///<布尔
  GLB_DATATYPE_INT16    = 4,///<整数，16位
  GLB_DATATYPE_INT32    = 5,///<长整数，32位
  GLB_DATATYPE_INT64    = 6,///<64位整数
  GLB_DATATYPE_FLOAT    = 7,///<单精度浮点数，32字节
  GLB_DATATYPE_DOUBLE   = 8,///<浮点数，64位
  GLB_DATATYPE_STRING   = 9,///<字符串
  GLB_DATATYPE_DATETIME = 10,///<日期+时间
  GLB_DATATYPE_GEOMETRY = 11,///<几何
  GLB_DATATYPE_VECTOR   = 12,///<向量【3个double,xyz】
  GLB_DATATYPE_BLOB     = 13///<二进制大数据
};
/**
  * @brief 子数据集类型枚举
  *
  * 定义不同的子数据集类型
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbSubDatasetRoleEnum
{
 GLB_SUBDATASETROLE_UNKNOWN     = 0,///<未定义
 //GLB_SUBDATASETROLE_NETWORKEDGE = 1,///<网络数据集边
 GLB_SUBDATASETROLE_NETWORKNODE = 2,///<网络数据集节点
 GLB_SUBDATASETROLE_NETWORKTURN = 3,///<网路数据集转弯
 GLB_SUBDATASETROLE_DRILLSKEW   = 4,///<钻孔数据集测斜
 GLB_SUBDATASETROLE_DRILLSTRA   = 5,///<钻孔数据集分层
 GLB_SUBDATASETROLE_DRILLSAMP   = 6,///<钻孔数据集样品
 GLB_SUBDATASETROLE_SECCTRL		= 7,///<剖面数据集：控制点
 GLB_SUBDATASETROLE_SECNODE     = 8,///<剖面数据集：节点
 GLB_SUBDATASETROLE_SECARC      = 9,///<剖面数据集：弧
 GLB_SUBDATASETROLE_SECPOLY     = 10///<剖面数据集：多边形
};
/**
  * @brief 设施网络 节点类型
*/
enum GlbNetworkNodeTypeEnum
{
	GLB_NETWORK_NORMAL_NODE = 0,///普通节点
	GLB_NETWORK_SOURCE_NODE = 1,///源
	GLB_NETWORK_SINK_NODE   = 2 ///汇
};
/**
  * @brief 设施网络 流向类型
*/
enum GlbNetworkDirectionTypeEnum
{
	GLB_NETWORK_DIRECTION_SAME     = 0,///流向与边的数字化方向(F->T)相同
	GLB_NETWORK_DIRECTION_OPPOSITE = 1,///流向与边的数字化方向(F->T)相反
	GLB_NETWORK_DIRECTION_UNCERTAIN= 2,///流向不确定
	GLB_NETWORK_DIRECTION_DISCONNECT=3 ///不连通
};
/**
  * @brief 空间索引类型枚举
  *
  * 定义不同的空间索引类型
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbSpatialIndexTypeEnum
{
	GLB_SPINDEX_UNKNOWN = 0,///<未知类型
	GLB_SPINDEX_MGRID   = 1///<多级格网索引
};
/**
  * @brief 几何类型枚举
  *
  * 定义不同的几何类型
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbGeometryTypeEnum
{
 GLB_GEO_UNKNOWN         = 0,///<未知
 GLB_GEO_POINT           = 1,///<点
 GLB_GEO_MULTIPOINT      = 2,///<多点
 GLB_GEO_LINE            = 3,///<线
 GLB_GEO_MULTILINE       = 4,///<多线
 GLB_GEO_POLYGON         = 5,///<多边形
 GLB_GEO_MULTIPOLYGON    = 6,///<多多边形
 GLB_GEO_TIN             = 7,///<三角网
 GLB_GEO_MULTITIN        = 8,///<多三角网
 GLB_GEO_TEXT            = 10,///<文字
 GLB_GEO_COLLECT         = 11,///<几何集合
 GLB_GEO_GRID			 = 12 ///<格网
};
/**
  * @brief 几何类
  *
  */
enum GlbGeometryClassEnum
{
 GLB_GEOCLASS_POINT     = 0,///<点几何类
 GLB_GEOCLASS_TRIANGLE  = 1 ///<三角形几何类
};

/**
  * @brief 像素类型枚举
  *
  * 定义不同的像素类型
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbPixelTypeEnum
{
	GLB_PIXELTYPE_UNKNOWN   = 0,	///<"未知类型"
	GLB_PIXELTYPE_BYTE      = 1,	///<"字节"			size=1字节
	GLB_PIXELTYPE_USHORT16  = 2,	///<"无符号位整数"  size=2字节
	GLB_PIXELTYPE_SHORT16   = 3,	///<"16位整数"      size=2字节
	GLB_PIXELTYPE_UINT32    = 4,	///<"无符号位整数"  size=4字节
	GLB_PIXELTYPE_INT32     = 5,	///<"32位整数"      size=4字节
	GLB_PIXELTYPE_FLOAT32   = 6, 	///<"32位浮点数"    size=4字节
	GLB_PIXELTYPE_DOUBLE64  = 7 	///<"64位浮点数"    size=8字节
};
/**
  * @brief 像素排序类型枚举
  *
  * 定义不同的像素排序格式
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbPixelLayoutTypeEnum
{
 GLB_PIXELLAYOUT_UNKNOWN=-1,///<未知
 GLB_PIXELLAYOUT_BSQ = 0,  ///< 波段顺序格式
 GLB_PIXELLAYOUT_BIP = 1,  ///< 像元波段交叉式
 GLB_PIXELLAYOUT_BIL = 2   ///< 波段按行交叉格式
};

/**
  * @brief 栅格重采样类型枚举
  *
  * 定义不同的栅格重采样方式
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
enum GlbRasterResampleTypeEnum
{
	GLB_RASTER_NEAREST = 0,///<就近采样
	GLB_RASTER_CUBIC   = 1, ///<二次样条
	GLB_RASTER_AVER    = 2///<平均值
};



