#ifndef _GLBGLOBETYPES_H
#define _GLBGLOBETYPES_H

#include "GlbCommTypes.h"

/**
 * @brief 定义最精细等级 23 级
 * 23级 纹理【256*256】精度 赤道处0.0156米， 地形精度【17*17】 赤道处 0.0156*16= 0.2496米 
*/
#define MAX_LEVEL 23

namespace GlbGlobe
{
	/** 场景枚举类型GlbGlobeTypeEnum */
	enum GlbGlobeTypeEnum
	{
		GLB_GLOBETYPE_UNKNOWN = -1,	/**<  未知 */ 
		GLB_GLOBETYPE_GLOBE	= 0,	/**<  球[默认] */
		GLB_GLOBETYPE_FLAT	= 1		/**<  平面 */
	};

	/** 地形块子块位置枚举类型GlbGlobeChildLocationEnum */
	enum GlbGlobeChildLocationEnum
	{
		GLB_UNKONWN	  = 0,			///<未知位置
		GlB_SOUTHWEST = 1,			///<西南子块
		GLB_SOUTHEAST = 2,			///<东南子块
		GLB_NORTHWEST = 3,			///<西北子块
		GLB_NORTHEAST = 4			///<东北子块
	};

	/** 地形块边缘枚举类型GlbTileEdgeEnum */
	enum GlbTileEdgeEnum
	{
		GLB_WESTEDGE_NORTHSECTION = 0,		///<西部边缘的北半部
		GLB_WESTEDGE_SOUTHSECTION = 1,		///<西部边缘的南半部
		GLB_EASTEDGE_NORTHSECTION = 2,		///<东部边缘的北半部
		GLB_EASTEDGE_SOUTHSECTION = 3,		///<东部边缘的南半部
		GLB_NORTHEDGE_WESTSECTION = 4,		///<北部边缘的西半部
		GLB_NORTHEDGE_EASTSECTION = 5,		///<北部边缘的东半部
		GLB_SOUTHEDGE_WESTSECTION = 6,		///<南部边缘的西半部
		GLB_SOUTHEDGE_EASTSECTION = 7,		///<南部边缘的东半部
		GLB_CENTEREDGE_WESTSECTION = 8,		///<中间水平线的西半部
		GLB_CENTEREDGE_EASTSECTION = 9,		///<中间水平线的东半部
		GLB_CENTEREDGE_NORTHSECTION = 10,	///<中间垂直线的北半部
		GLB_CENTEREDGE_SOUTHSECTION = 11	///<中间垂直线的南半部
	};

	/** 场景对象枚举类型GlbGlobeObjectTypeEnum */
	enum GlbGlobeObjectTypeEnum
	{
		GLB_OBJECTTYPE_UNKNOW = 0,                 ///<未知
		GLB_OBJECTTYPE_POINT = 1,                  ///<点
		GLB_OBJECTTYPE_LINE = 2,                   ///<线
		GLB_OBJECTTYPE_POLYGON = 3,                ///<面
		GLB_OBJECTTYPE_TIN = 4,                    ///<地质体
		GLB_OBJECTTYPE_DYNAMIC = 5,		           ///<动态对象
		GLB_OBJECTTYPE_MODIFYTERRIN = 6,           ///<地形修改对象
		GLB_OBJECTTYPE_DIGHOLE = 7,		           ///<挖洞	
		GLB_OBJECTTYPE_COMPLEX = 8,	               ///<组合对象
		GLB_OBJECTTYPE_VISUALLINEANALYSIS = 9,	   ///<视线分析对象
		GLB_OBJECTTYPE_VIEWANALYSIS = 10,	       ///<视域分析对象
		GLB_OBJECTTYPE_VIDEO = 11,				   ///<投影视频对象
		GLB_OBJECTTYPE_DRILL = 12,				   ///<钻孔对象
		GLB_OBJECTTYPE_SECTION = 13,			   ///<剖面对象
		GLB_OBJECTTYPE_NETWORK = 14,			   ///<网络状对象-管网
		GLB_OBJECTTYPE_COORDINAT_AXIS = 15,		   ///<坐标轴对象
		GLB_OBJECTTYPE_CLIP = 16,				   ///<裁剪对象
		GLB_OBJECTTYPE_GRID = 17,				   ///<grid网格对象
		GLB_OBJECTTYPE_VDRILL = 18,				   ///<虚拟钻孔对象
		GLB_OBJECTTYPE_VSECTION = 19			   ///<虚拟剖面对象
		//......
	};

	/** 场景对象激活方式枚举类型GlbGlobeActiveActionEnum */
	enum GlbGlobeActiveActionEnum
	{
		GLB_GLOBE_FLYTO = 0,			 ///<飞向对象激活
		GLB_GLOBE_JUMPTO = 1			 ///<跳向对象激活
	};

	/** 图层类型	*/
	enum GlbGlobeLayerTypeEnum
	{
		GLB_GLOBELAYER_UNKNOWN = -1,		///<未知图层
		GLB_GLOBELAYER_DOM	= 0,			///<dom图层
		GLB_GLOBELAYER_DEM	= 1,			///<dem图层
		GLB_GLOBELAYER_FEATURE = 2,			///<要素图层
		GLB_GLOBELAYER_TILTPHOTO = 3,		///<倾斜摄影数据图层
		GLB_GLOBELAYER_IMAGE = 4,			///<image图层
		GLB_GLOBELAYER_WMTS = 5,			///<wmts图层
		GLB_GLOBELAYER_GRID = 6				///<grid图层
	};
	/**
	  * 高程模式枚举
	  */
	enum GlbAltitudeModeEnum
	{
		GLB_ALTITUDEMODE_ONTERRAIN       = 0, ///<贴地形
		GLB_ALTITUDEMODE_RELATIVETERRAIN = 1, ///<相对地形
		GLB_ALTITUDEMODE_ABSOLUTE        = 2  ///<相对海平面
	};
	/**
	  * 地形修改模式枚举
	  */
	enum GlbGlobeTerrainModifyModeEnum
	{
		GLB_TERRAINMODIFY_REPLACE	= 0,	///<替换
		GLB_TERRAINMODIFY_BELOW		= 1,	///<修剪下方
		GLB_TERRAINMODIFY_UP		= 2		///<修剪上方
	};
	/**
	  * 光源模式枚举
	  */
	enum GlbGlobeLightingModeEnum
	{
		GLB_HEAD_LIGHT	= 0,	///<头灯
		GLB_SKY_LIGHT	= 1,	///<环境光
		GLB_NO_LIGHT	=2		///<无灯
	};
	/**
	  * 格网显示模式枚举
	  */
	enum GlbGlobeGridRenderModeEnum
	{
		GLB_RENDERMODE_GRID	= 0,			///<网格模式
		GLB_RENDERMODE_LINECLIP	= 1,		///<线剖线模式
		GLB_RENDERMODE_AXISCLIP = 2,		///<轴剖线模式NI，NJ
		GLB_RENDERMODE_BARRIERCLIP = 3		///<栅栏剖线模式
	};
	/**
	* @brief 图例在3D窗口的停靠位置
	*/
	enum GlbDockStyleEnum
	{
		GLB_Position_LeftTop = 0,			//左上
		GLB_Position_LeftCenter = 1,		//左中
		GLB_Position_LeftBottom = 2,		//左下
		GLB_Position_CenterTop = 3,			//中上
		GLB_Position_CenterBottom = 4,		//中下
		GLB_Position_RightTop = 5,			//右上
		GLB_Position_RightCenter = 6,		//右中
		GLB_Position_RightBottom = 7		//右下
	};
	/**
	* @brief 2D坐标结构体（用以存储图例标注的起始位置）
	*/
	struct pointCoord
	{
		double x;
		double y;
	};
	/** 格网块所处位置枚举类型GlbTileLocationEnum */
	enum GlbTileLocationEnum
	{
		GLB_LOC_UNKONWN			 = 0,		///<未知
		GLB_WEST_EDGE		 = 1,		///<西部边缘的北半部
		GLB_EAST_EDGE		 = 2,		///<西部边缘的南半部
		GLB_SOUTH_EDGE		 = 3,		///<东部边缘的北半部
		GLB_NORTH_EDGE		 = 4,		///<东部边缘的南半部
		GLB_INSIDE			 = 5,		///<北部边缘的西半部
		GLB_SOUTHWEST_CORNER = 6,		///<北部边缘的东半部
		GLB_SOUTHEAST_CORNER = 7,		///<南部边缘的西半部
		GLB_NORTHWEST_CORNER = 8,		///<南部边缘的东半部
		GLB_NORTHEAST_CORNER = 9		///<中间水平线的西半部
	};
}

#endif