#ifndef _GLBGLOBEOBJECTRENDERINFOS_H
#define _GLBGLOBEOBJECTRENDERINFOS_H

#include "GlbCommTypes.h"
#include "GlbString.h"
#include "GlbWString.h"
#include "GlbFeature.h"
#include "GlbPath.h"
#include "GlbGlobeTypes.h"
#include "GlbGlobeExport.h"
#include "libxml/tree.h"

#pragma once
namespace GlbGlobe
{
	struct GLB_DLLCLASS_EXPORT GlbRenderInt32
	{
		GlbRenderInt32()
		{
			bUseField = false;
			value = 0;
			field = L"";
		}
		glbBool bUseField;
		glbInt32 value;
		CGlbWString field;
		bool operator != (const GlbRenderInt32& r) const;
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderDouble
	{
		GlbRenderDouble()
		{
			bUseField = false;
			value = 0.0;
			field = L"";
		}
		glbBool bUseField;
		glbDouble value;
		CGlbWString field;
		bool operator != (const GlbRenderDouble& r) const; 		
		glbDouble GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);	
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderBool
	{
		GlbRenderBool()
		{
			bUseField = false;
			value = false;
			field = L"";
		}
		glbBool bUseField;
		glbBool value;
		CGlbWString field;
		bool operator != (const GlbRenderBool& r) const; 
		glbBool GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);		
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderString
	{
		GlbRenderString()
		{
			bUseField = false;
			value = L"";
			field = L"";
		}
		glbBool bUseField;
		CGlbWString value;
		CGlbWString field;
		bool operator != (const GlbRenderString& r) const;
		glbWChar* GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderColor
	{
		glbBool bUseField;
		glbInt32 value;
		CGlbWString field;
		bool operator != (const GlbRenderColor& r) const; 		
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);		
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	enum GlbGlobeMaterialEnum
	{

	};

	struct GLB_DLLCLASS_EXPORT GlbRenderMaterial
	{
		glbBool bUseField;
		GlbGlobeMaterialEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderMaterial& r) const; 
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);		
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderInfo  :  public CGlbReference
	{
		virtual ~GlbRenderInfo(){}
		GlbGlobeObjectTypeEnum type;

		virtual void Save(xmlNodePtr node,const glbWChar* relativepath=NULL)=0;
		virtual void Load(xmlNodePtr node,const glbWChar* relativepath=NULL)=0;
	};

	enum GlbMarkerSymbolTypeEnum
	{
		GLB_MARKERSYMBOL_MODEL = 0,
		GLB_MARKERSYMBOL_2DSHAPE = 1,
		GLB_MARKERSYMBOL_3DSHAPE = 2,
		GLB_MARKERSYMBOL_PIXEL = 3,
		GLB_MARKERSYMBOL_LABEL = 4,
		GLB_MARKERSYMBOL_IMAGE = 5,		// 图片点符号
		GLB_MARKERSYMBOL_FIRE = 6,		// 火焰点符号
		GLB_MARKERSYMBOL_SMOKE = 7,		// 烟雾点符号
		GLB_MARKERSYMBOL_EPICENTRE = 8,		// 震源点符号
		GLB_MARKERSYMBOL_DYNAMICLABEL
	};
	enum GlbGlobeBillboardModeEnum
	{
		GLB_BILLBOARD_SCREEN = 0,//屏幕
		GLB_BILLBOARD_AXIS = 1,//轴向
		GLB_BILLBOARD_AUTOSLOPE = 2//自动倾斜
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderBillboard
	{
		GlbRenderBillboard()
		{
			bUseField = false;
			value = GLB_BILLBOARD_SCREEN;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeBillboardModeEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderBillboard& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);	
	};
	///<标注对准方式
	enum GlbGlobeLabelAlignTypeEnum
	{
		GLB_LABELALG_LEFTTOP = 0,///<顶，左
		GLB_LABELALG_LEFTCENTER = 1,///<中心，左
		GLB_LABELALG_LEFTBOTTOM = 2,///<底，左
		GLB_LABELALG_CENTERTOP = 3,///<顶，中心
		GLB_LABELALG_CENTERCENTER = 4,///<中心，中心
		GLB_LABELALG_CENTERBOTTOM= 5,///<底，中心
		GLB_LABELALG_RIGHTTOP = 6,///<顶，右
		GLB_LABELALG_RIGHTCENTER = 7,///<中心，右
		GLB_LABELALG_RIGHTBOTTOM = 8///<底，右
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderLabelAlign
	{
		GlbRenderLabelAlign()
		{
			bUseField = false;
			value = GLB_LABELALG_CENTERCENTER;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeLabelAlignTypeEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderLabelAlign& r) const ;		
		glbInt32 GetValue(CGlbFeature *feature);	
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	enum GlbGlobeMultilineAlignTypeEnum
	{
		GLB_MULTILINEALG_LEFT = 0,
		GLB_MULTILINEALG_CENTER = 1,
		GLB_MULTILINEALG_RIGHT =2
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderMultilineAlign
	{
		GlbRenderMultilineAlign()
		{
			bUseField = false;
			value = GLB_MULTILINEALG_LEFT;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeMultilineAlignTypeEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderMultilineAlign& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};


	struct GLB_DLLCLASS_EXPORT GlbRenderTextInfo
	{
		GlbRenderTextInfo()
		{
			color = NULL;
			size = NULL;
			font = NULL;
			content = NULL;
			isBold = NULL;
			isUnderline = NULL;
			isItalic = NULL;
			backColor = NULL;
			backOpacity = NULL;
			multilineAlign = NULL;
		}

		~GlbRenderTextInfo()
		{
			if(color)
				delete color;
			if(size)
				delete size;
			if(font)
				delete font;
			if(content)
				delete content;
			if(isBold)
				delete isBold;
			if(isUnderline)
				delete isUnderline;
			if(isItalic)
				delete isItalic;
			if(backColor)
				delete backColor;
			if(backOpacity)
				delete backOpacity;
			if(multilineAlign)
				delete multilineAlign;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);

		GlbRenderColor *color;///<文本颜色
		GlbRenderInt32 *size;///<文本大小
		GlbRenderString *font;///<文本字体
		GlbRenderString *content;///<文本内容
		GlbRenderBool *isBold;///<粗体
		GlbRenderBool *isUnderline;///<下划线
		GlbRenderBool *isItalic;///<斜体
		GlbRenderColor *backColor;///<背景颜色
		GlbRenderInt32 *backOpacity;///<背景透明度
		GlbRenderMultilineAlign *multilineAlign;///<文本对齐方式
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderGeoInfo
	{
		GlbRenderGeoInfo()
		{
			isLimit = NULL;
			ratio = NULL;
		}

		~GlbRenderGeoInfo()
		{
			if(isLimit)
				delete isLimit;
			if(ratio)
				delete ratio;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);

		GlbRenderBool *isLimit;///<限制增长
		GlbRenderDouble *ratio;///<比例
	};

		/*
		自定义的内部使用的简单标注信息,用于绘制点，线，面对象的附属标注 2015.11.7 
	*/

	struct GLB_DLLCLASS_EXPORT GlbRenderSimpleLabel : public CGlbReference
	{		
		GlbRenderSimpleLabel()
		{
			bShow = NULL;
			xOffset = yOffset = zOffset = NULL;
			mode = NULL;
			labelAlign = NULL;
			textInfo = NULL;
			geoInfo = NULL;			
		}
		~GlbRenderSimpleLabel()
		{
			if (bShow)
				delete bShow;
			if(xOffset)delete xOffset;
			if(yOffset)delete yOffset;
			if(zOffset)delete zOffset;
			if(mode)
				delete mode;
			if(labelAlign)
				delete labelAlign;
			if(textInfo)
				delete textInfo;
			if(geoInfo)
				delete geoInfo;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);	

		GlbRenderBool		*bShow;
		GlbRenderDouble     *xOffset;
		GlbRenderDouble     *yOffset;
		GlbRenderDouble     *zOffset;		
		GlbRenderBillboard  *mode;///<布告板锁定模式
		GlbRenderLabelAlign *labelAlign;///<文本轴对准方式
		GlbRenderTextInfo   *textInfo;///<文本信息
		GlbRenderGeoInfo    *geoInfo;///<几何信息
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerSymbolInfo : public GlbRenderInfo
	{
		GlbMarkerSymbolInfo()
		{
			type = GLB_OBJECTTYPE_POINT;
		}
		GlbMarkerSymbolTypeEnum symType;
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerModelSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarkerModelSymbolInfo()
		{
			symType= GLB_MARKERSYMBOL_MODEL;
			locate = NULL;
			xScale = NULL;
			yScale = NULL;
			zScale = NULL;
			pitch = NULL;
			yaw = NULL;
			roll = NULL;
			opacity = NULL;
			label = NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;
		}

		~GlbMarkerModelSymbolInfo()
		{
			if(locate)
				delete locate;
			if(xScale)
				delete xScale;
			if(yScale)
				delete yScale;
			if(zScale)
				delete zScale;
			if(pitch)
				delete pitch;
			if(yaw)
				delete yaw;
			if(roll)
				delete roll;
			if(opacity)
				delete opacity;
			if(label)
				delete label;
			if(xOffset)
				delete xOffset;
			if(yOffset)
				delete yOffset;
			if(zOffset)
				delete zOffset;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderString *locate;
		GlbRenderDouble *xScale;
		GlbRenderDouble *yScale;
		GlbRenderDouble *zScale;
		GlbRenderDouble *pitch;
		GlbRenderDouble *yaw;
		GlbRenderDouble *roll;
		GlbRenderInt32 *opacity;
		GlbRenderDouble *xOffset;
		GlbRenderDouble *yOffset;
		GlbRenderDouble *zOffset;
		GlbRenderSimpleLabel *label;
	};

	enum GlbGlobeLinePatternEnum
	{
		GLB_LINE_SOLID = 0,//实线
		GLB_LINE_DOTTED = 1//点划线
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderLinePattern
	{
		GlbRenderLinePattern()
		{
			bUseField = false;
			value = GLB_LINE_SOLID;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeLinePatternEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderLinePattern& r) const; 
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);	
	};

	enum GlbGlobeLineSymbolTypeEnum
	{
		GLB_LINESYMBOL_PIXEL = 0,//像素线符号
		GLB_LINESYMBOL_2D = 1,//二维线符号
		GLB_LINESYMBOL_3D = 2,//三维线符号
		GLB_LINESYMBOL_ARROW = 3,//箭头线符号
		GLB_LINESYMBOL_MODEL =4, //线模型符号
		GLB_LINESYMBOL_DYNAMICARROW = 5//动态箭头符号
	};

	struct GLB_DLLCLASS_EXPORT GlbLineSymbolInfo : public GlbRenderInfo
	{
		GlbLineSymbolInfo()
		{
			type = GLB_OBJECTTYPE_LINE;
		}
		GlbGlobeLineSymbolTypeEnum symType;//线符号类型
	};

	struct GLB_DLLCLASS_EXPORT GlbLinePixelSymbolInfo : public GlbLineSymbolInfo
	{
		GlbLinePixelSymbolInfo()
		{
			symType = GLB_LINESYMBOL_PIXEL;
			lineWidth = NULL;
			lineColor = NULL;
			lineOpacity = NULL;
			linePattern = NULL;
		}

		~GlbLinePixelSymbolInfo()
		{
			if(lineWidth)
				delete lineWidth;
			if(lineColor)
				delete lineColor;
			if(lineOpacity)
				delete lineOpacity;
			if(linePattern)
				delete linePattern;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderDouble* lineWidth;
		GlbRenderColor *lineColor;
		GlbRenderInt32 *lineOpacity;
		GlbRenderLinePattern *linePattern;//线样式
	};

	enum GlbGlobeTexRepeatModeEnum
	{
		GLB_TEXTUREREPEAT_TIMES = 0,// 按次数重复模式
		GLB_TEXTUREREPEAT_SIZE = 1 //按尺寸重复模式
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderTexRepeatMode
	{
		GlbRenderTexRepeatMode()
		{
			bUseField = false;
			value = GLB_TEXTUREREPEAT_TIMES;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeTexRepeatModeEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderTexRepeatMode& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	struct GLB_DLLCLASS_EXPORT GlbLine2DSymbolInfo : public GlbLineSymbolInfo
	{
		GlbLine2DSymbolInfo()
		{
			symType = GLB_LINESYMBOL_2D;
			linePixelInfo = NULL;
			//lineWidth = NULL;
			//lineColor = NULL;
			//lineOpacity = NULL;
			//linePattern = NULL;
			lineBackColor = NULL;
			lineBackOpacity = NULL;
			linefillColor = NULL;
			linefillOpacity = NULL;
			lineWidth = NULL;
			textureData = NULL;
			texRepeatMode = NULL;
			tilingU = NULL;
			tilingV = NULL;
			textureRotation = NULL;
		}
		//GlbRenderDouble *lineWidth;
		//GlbRenderColor *lineColor;
		//GlbRenderInt32 *lineOpacity;
		//GlbRenderLinePattern *linePattern;
		GlbLinePixelSymbolInfo *linePixelInfo;	//轮廓线
		
		GlbRenderColor *lineBackColor;
		GlbRenderInt32 *lineBackOpacity;

		GlbRenderColor *linefillColor;	//填充颜色
		GlbRenderInt32 *linefillOpacity;//填充透明度
		GlbRenderDouble *lineWidth;		//宽度

		GlbRenderString *textureData;
		GlbRenderTexRepeatMode *texRepeatMode;
		GlbRenderDouble *tilingU;
		GlbRenderDouble *tilingV;
		GlbRenderDouble *textureRotation;

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);
	};

	struct GLB_DLLCLASS_EXPORT GlbLine3DSymbolInfo : public GlbLineSymbolInfo
	{
		GlbLine3DSymbolInfo()
		{
			symType = GLB_LINESYMBOL_3D;
			radius = NULL;
			edges = NULL;
			color = NULL;
			opacity = NULL;
			material = NULL;
		}
		GlbRenderDouble *radius;
		GlbRenderInt32 *edges;
		GlbRenderColor *color;
		GlbRenderInt32 *opacity;
		GlbRenderMaterial *material;

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);
	};

	enum GlbArrowPatternEnum
	{
		GLB_ARROWPATTERN_D1 = 0,
		GLB_ARROWPATTERN_D2 = 1,
		GLB_ARROWPATTERN_D3 = 2,
		GLB_ARROWPATTERN_D4 = 3,
		GLB_ARROWPATTERN_D5 = 4
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderArrowPattern
	{
		GlbRenderArrowPattern()
		{
			bUseField = false;
			value = GLB_ARROWPATTERN_D1;
			field = L"";
		}
		glbBool bUseField;
		GlbArrowPatternEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderArrowPattern& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);	
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	struct GLB_DLLCLASS_EXPORT GlbLineArrowSymbolInfo : public GlbLineSymbolInfo
	{
		GlbLineArrowSymbolInfo()
		{
			symType = GLB_LINESYMBOL_ARROW;
			roll=NULL;
			pattern=NULL;
			height=NULL;
			linePixelInfo = NULL;
			width = NULL;
			fillColor = NULL;
			fillOpacity=NULL;
			textureData=NULL;
			texRepeatMode=NULL;
			tilingU=NULL;
			tilingV=NULL;
			textureRotation=NULL;
		}
		GlbRenderDouble *roll;					//箭头翻滚角

		GlbRenderArrowPattern *pattern;			//箭头样式
		GlbRenderDouble *height;				//厚度
		//轮廓线
		GlbLinePixelSymbolInfo *linePixelInfo;
		//填充
		GlbRenderDouble *width;					//宽度
		GlbRenderColor *fillColor;				//填充颜色
		GlbRenderInt32 *fillOpacity;            //填充透明度
		GlbRenderString *textureData;			//纹理路径
		GlbRenderTexRepeatMode *texRepeatMode;	//纹理平铺方式
		GlbRenderDouble *tilingU;				//平铺次数，U向
		GlbRenderDouble *tilingV;				//平铺次数，V向
		GlbRenderDouble *textureRotation;		//纹理环绕旋转角度

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);
	};

	struct GLB_DLLCLASS_EXPORT GlbLineDynamicArrowSymbolInfo : public GlbLineSymbolInfo
	{
		GlbLineDynamicArrowSymbolInfo()
		{
			symType = GLB_LINESYMBOL_DYNAMICARROW;
			isForwardDirection = NULL;
			isShowLine = NULL;
			number = NULL;
			color = NULL;
			diameter = NULL;
			time = NULL;
			modelPath = NULL;
			yaw = NULL;
			pitch = NULL;
			roll = NULL;
			xScale = NULL;
			yScale = NULL;
			zScale = NULL;
			xOffset = NULL;
			yOffset = NULL;
			zOffset = NULL;
		}

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderBool        *isForwardDirection;
		GlbRenderBool        *isShowLine;
		GlbRenderInt32       *number;
		GlbRenderColor      *color;
		GlbRenderDouble   *diameter;
		GlbRenderDouble   *time;
		GlbRenderString     *modelPath;
		GlbRenderDouble   *yaw;
		GlbRenderDouble   *pitch;
		GlbRenderDouble   *roll;
		GlbRenderDouble   *xScale;
		GlbRenderDouble   *yScale;
		GlbRenderDouble   *zScale;
		GlbRenderDouble   *xOffset;
		GlbRenderDouble   *yOffset;
		GlbRenderDouble   *zOffset;
	};


	struct GLB_DLLCLASS_EXPORT GlbLineModelSymbolInfo : public GlbLineSymbolInfo
	{
		GlbLineModelSymbolInfo()
		{
			symType = GLB_LINESYMBOL_MODEL;
			isRenderLine = false;
			color	= NULL;
			opacity = NULL;
			width	= NULL;
			modelLocate = NULL;
		}
		~GlbLineModelSymbolInfo()
		{
			if (color)			delete color;
			if (opacity)		delete opacity;
			if (width)			delete width;
			if (modelLocate)	delete modelLocate;
		}
		glbBool isRenderLine;    //绘制线.
		GlbRenderColor  *color;  //线颜色
		GlbRenderInt32  *opacity;//不透明度
		GlbRenderDouble *width;  //线宽
		GlbRenderString *modelLocate;//模型宽.

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
	};

	enum GlbGlobePolygonSymbolTypeEnum
	{
		GLB_POLYGONSYMBOL_2D = 0,
		GLB_POLYGONSYMBOL_POLYHEDRON = 1,
		GLB_POLYGONSYMBOL_BUILDING = 2,
		GLB_POLYGONSYMBOL_WATER = 3,
		GLB_POLYGONSYMBOL_LAKE = 4
	};

	struct GLB_DLLCLASS_EXPORT GlbPolygonSymbolInfo : public GlbRenderInfo
	{
		GlbPolygonSymbolInfo()
		{
			type = GLB_OBJECTTYPE_POLYGON;
		}
		GlbGlobePolygonSymbolTypeEnum symType;
	};

	struct GLB_DLLCLASS_EXPORT GlbPolygon2DSymbolInfo : public GlbPolygonSymbolInfo
	{
		GlbPolygon2DSymbolInfo()
		{
			symType = GLB_POLYGONSYMBOL_2D;
			outlineInfo = NULL;
			color = NULL;
			opacity = NULL;
			textureData = NULL;
			texRepeatMode = NULL;
			tilingU = NULL;
			tilingV = NULL;
			textureRotation = NULL;
			zOrAltitude = NULL;
			isEdgeVerticalExtend = NULL;
			isExtendDirReserve = NULL;
			extendDepth = NULL;
		}
		~GlbPolygon2DSymbolInfo()
		{
			if(outlineInfo)
				delete outlineInfo;
			if(color)
				delete color;
			if(opacity)
				delete opacity;
			if(textureData)
				delete textureData;
			if(texRepeatMode)
				delete texRepeatMode;
			if(tilingU)
				delete tilingU;
			if(tilingV)
				delete tilingV;
			if(textureRotation)
				delete textureRotation;
			if(zOrAltitude)
				delete zOrAltitude;
			if(isEdgeVerticalExtend)
				delete isEdgeVerticalExtend;
			if(isExtendDirReserve)
				delete isExtendDirReserve;
			if(extendDepth)
				delete extendDepth;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbLinePixelSymbolInfo     *outlineInfo;
		GlbRenderColor             *color;
		GlbRenderInt32             *opacity;
		GlbRenderString            *textureData;
		GlbRenderTexRepeatMode     *texRepeatMode;
		GlbRenderInt32             *tilingU;//平铺次数
		GlbRenderInt32             *tilingV;
		GlbRenderDouble            *textureRotation;
		//2018.5.23 增加Z高度和边沿垂直延展
		GlbRenderDouble			   *zOrAltitude;
		GlbRenderBool			   *isEdgeVerticalExtend;	///<是否边缘垂直向下延展 isEdgeWall
		GlbRenderBool			   *isExtendDirReserve;		///<反向延展 isWallDirectionReserve
		GlbRenderDouble			   *extendDepth;			///<延展深度 wallHeight	
	};

	struct GLB_DLLCLASS_EXPORT GlbPolyhedronSymbolInfo :  public GlbPolygonSymbolInfo
	{
		GlbPolyhedronSymbolInfo()
		{
			symType = GLB_POLYGONSYMBOL_POLYHEDRON;
			outlineInfo = NULL;
			color = NULL;
			opacity = NULL;
			textureData = NULL;
			texRepeatMode = NULL;
			tilingU = NULL;
			tilingV = NULL;
			textureRotation = NULL;
			height = NULL;
			yaw = NULL;
			pitch = NULL;
			roll = NULL;
		}		
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbLinePixelSymbolInfo *outlineInfo;
		GlbRenderColor *color;
		GlbRenderInt32 *opacity;
		GlbRenderString *textureData;
		GlbRenderTexRepeatMode *texRepeatMode;
		GlbRenderInt32 *tilingU;
		GlbRenderInt32 *tilingV;
		GlbRenderDouble *textureRotation;		
		GlbRenderDouble *height;
		GlbRenderDouble *yaw;
		GlbRenderDouble *pitch;
		GlbRenderDouble *roll;
	};

	struct GLB_DLLCLASS_EXPORT GlbPolygonBuildingSymbolInfo : public GlbPolygonSymbolInfo
	{
		GlbPolygonBuildingSymbolInfo()
		{
			symType = GLB_POLYGONSYMBOL_BUILDING;
		}

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL){}
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL){}
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL){}
	};

	struct GLB_DLLCLASS_EXPORT GlbPolygonWaterSymbolInfo : public GlbPolygonSymbolInfo
	{
		GlbPolygonWaterSymbolInfo()
		{
			symType = GLB_POLYGONSYMBOL_WATER;
			color=NULL;
			opacity=NULL;
			isUseEnv=NULL;
			windStrong=NULL;
			windDirection=NULL;
			enableReflections=NULL;
			enableRefractions=NULL;
			refractionImageData=NULL;
			refractTexRepeatMode = NULL;
			refractTextilingU=NULL;
			refractTextilingV=NULL;
			refractTexRotation=NULL;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderColor             *color;
		GlbRenderInt32             *opacity;
		GlbRenderBool              *isUseEnv;
		GlbRenderDouble            *windStrong;
		GlbRenderDouble            *windDirection;
		GlbRenderBool              *enableReflections;
		GlbRenderBool              *enableRefractions;
		GlbRenderString            *refractionImageData;
		GlbRenderTexRepeatMode     *refractTexRepeatMode;  //折射纹理重复模式
		GlbRenderInt32             *refractTextilingU;     //折射纹理平铺次数
		GlbRenderInt32             *refractTextilingV;
		GlbRenderDouble            *refractTexRotation;
	};

	struct GLB_DLLCLASS_EXPORT GlbPolygonLakeSymbolInfo : public GlbPolygonSymbolInfo
	{
		GlbPolygonLakeSymbolInfo()
		{
			symType = GLB_POLYGONSYMBOL_LAKE;
			windStrong=NULL;
			windDirection=NULL;
			waterLevel = NULL;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderDouble            *windStrong;
		GlbRenderDouble            *windDirection;
		GlbRenderDouble            *waterLevel;
	};

	enum GlbGlobeTinSymbolTypeEnum
	{
		GLB_TINSYMBOL_SIMPLE = 0,
		GLB_TINSYMBOL_WATER = 1
	};
	struct GLB_DLLCLASS_EXPORT GlbTinSymbolInfo : public GlbRenderInfo
	{
		GlbTinSymbolInfo()
		{
			type = GLB_OBJECTTYPE_TIN;
			symType = GLB_TINSYMBOL_SIMPLE;
			color=NULL;
			opacity=NULL;
			locate=NULL;
			xScale=NULL;
			yScale=NULL;
			zScale=NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;
			bSmoothing = NULL;
			bIsBody = NULL;
		}
		~GlbTinSymbolInfo()
		{
			if (color) delete color;
			if (opacity) delete opacity;
			if (locate) delete locate;
			if (xScale) delete xScale;
			if (yScale) delete yScale;
			if (zScale) delete zScale;
			if (xOffset) delete xOffset;
			if (yOffset) delete yOffset;
			if (zOffset) delete zOffset;
			if (bSmoothing) delete bSmoothing;
			if (bIsBody) delete bIsBody;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbGlobeTinSymbolTypeEnum	symType;
		GlbRenderColor             *color;
		GlbRenderInt32             *opacity;
		GlbRenderString            *locate;
		GlbRenderDouble            *xScale;
		GlbRenderDouble            *yScale;
		GlbRenderDouble            *zScale;
		/*GlbRenderDouble          *pitch;
		GlbRenderDouble            *yaw;
		GlbRenderDouble            *roll;*/

		GlbRenderDouble				*xOffset;
		GlbRenderDouble				*yOffset;
		GlbRenderDouble				*zOffset;

		GlbRenderBool				*bSmoothing; ///<是否平滑处理
		GlbRenderBool				*bIsBody;	 ///<是否是闭合体
	};

	struct GLB_DLLCLASS_EXPORT GlbTinWaterSymbolInfo : public GlbTinSymbolInfo
	{// 水面tin符号
		GlbTinWaterSymbolInfo()
		{
			type = GLB_OBJECTTYPE_TIN;
			symType = GLB_TINSYMBOL_WATER;
			color=NULL;
			opacity=NULL;

			locate=NULL;
			xScale=NULL;
			yScale=NULL;
			zScale=NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;

			isUseEnv=NULL;
			windStrong=NULL;
			windDirection=NULL;
			enableReflections=NULL;
			enableRefractions=NULL;
			refractionImageData=NULL;
			refractTexRepeatMode = NULL;
			refractTextilingU=NULL;
			refractTextilingV=NULL;
			refractTexRotation=NULL;
		}
		~GlbTinWaterSymbolInfo()
		{			
			if (isUseEnv) delete isUseEnv;
			if (windStrong) delete windStrong;
			if (windDirection) delete windDirection;
			if (enableReflections) delete enableReflections;
			if (enableRefractions) delete enableRefractions;
			if (refractionImageData) delete refractionImageData;
			if (refractTexRepeatMode) delete refractTexRepeatMode;
			if (refractTextilingU) delete refractTextilingU;
			if (refractTextilingV) delete refractTextilingV;
			if (refractTexRotation) delete refractTexRotation;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderBool              *isUseEnv;
		GlbRenderDouble            *windStrong;
		GlbRenderDouble            *windDirection;
		GlbRenderBool              *enableReflections;
		GlbRenderBool              *enableRefractions;
		GlbRenderString            *refractionImageData;
		GlbRenderTexRepeatMode     *refractTexRepeatMode;  //折射纹理重复模式
		GlbRenderInt32             *refractTextilingU;     //折射纹理平铺次数
		GlbRenderInt32             *refractTextilingV;
		GlbRenderDouble            *refractTexRotation;
	};

	struct GLB_DLLCLASS_EXPORT GlbVisualLineSymbolInfo : public GlbRenderInfo
	{
		GlbVisualLineSymbolInfo()
		{
			type = GLB_OBJECTTYPE_VISUALLINEANALYSIS;
			visibleLineColor = NULL;
			invisibleLineColor=NULL;
			opacity = NULL;
		}
		~GlbVisualLineSymbolInfo()
		{
			if(visibleLineColor!=NULL)
				delete visibleLineColor;
			if(invisibleLineColor!=NULL)
				delete invisibleLineColor;
			if(opacity!=NULL)
				delete opacity;
		}
		void Save(xmlNodePtr node,glbWChar* relativepath=NULL){}
		void Load(xmlNodePtr node,glbWChar* relativepath=NULL){}

		GlbRenderColor             *visibleLineColor;    // 可见视线
		GlbRenderColor             *invisibleLineColor;  // 不可见视线
		GlbRenderInt32             *opacity;
	};

	struct GLB_DLLCLASS_EXPORT GlbViewSymbolInfo : public GlbRenderInfo
	{
		GlbViewSymbolInfo()
		{
			type = GLB_OBJECTTYPE_VIEWANALYSIS;
			fovIndegree=NULL;
			visibleViewColor = NULL;
			invisibleViewColor=NULL;
			opacity = NULL;
		}
		~GlbViewSymbolInfo()
		{
			if (fovIndegree!=NULL)
				delete fovIndegree;
			if(visibleViewColor!=NULL)
				delete visibleViewColor;
			if(invisibleViewColor!=NULL)
				delete invisibleViewColor;
			if(opacity!=NULL)
				delete opacity;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL){}
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL){}

		GlbRenderDouble            *fovIndegree;         // 视域范围（0-90度，默认30度）
		GlbRenderColor             *visibleViewColor;    // 可见区域
		GlbRenderColor             *invisibleViewColor;  // 不可见区域
		GlbRenderInt32             *opacity;
	};

	enum GlbMarkerShapeTypeEnum
	{
		GLB_MARKERSHAPE_CIRCLE = 0,
		GLB_MARKERSHAPE_RECTANGLE = 1,
		GLB_MARKERSHAPE_ELLIPSE = 2,
		GLB_MARKERSHAPE_ARC = 3,
		GLB_MARKERSHAPE_SPHERE = 4,
		GLB_MARKERSHAPE_BOX = 5,
		GLB_MARKERSHAPE_PYRAMID = 6,
		GLB_MARKERSHAPE_CONE = 7,
		GLB_MARKERSHAPE_PIE = 8,
		GLB_MARKERSHAPE_CYLINDER = 9
	};

	struct GLB_DLLCLASS_EXPORT GlbShapeInfo
	{
		GlbMarkerShapeTypeEnum shapeType;
		virtual void Save(xmlNodePtr node);
		virtual void Load(xmlNodePtr* node);
		virtual void Load2(xmlNodePtr* node);
	};

	struct GLB_DLLCLASS_EXPORT GlbMarker2DShapeSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarker2DShapeSymbolInfo()
		{
			symType= GLB_MARKERSYMBOL_2DSHAPE;
			fillInfo = NULL;
			shapeInfo = NULL;
			yaw = NULL;
			xScale = NULL;
			yScale = NULL;
			zScale = NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;
		}

		~GlbMarker2DShapeSymbolInfo()
		{
			if(fillInfo)
				delete fillInfo;
			if(shapeInfo)
				delete shapeInfo;
			if(yaw)
				delete yaw;
			if(xScale)
				delete xScale;
			if(yScale)
				delete yScale;
			if(zScale)
				delete zScale;
			if(xOffset)
				delete xOffset;
			if(yOffset)
				delete yOffset;
			if(zOffset)
				delete zOffset;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbPolygon2DSymbolInfo *fillInfo;
		GlbShapeInfo *shapeInfo;
		GlbRenderDouble *yaw;
		GlbRenderDouble *xScale;
		GlbRenderDouble *yScale;
		GlbRenderDouble *zScale;
		GlbRenderDouble *xOffset;
		GlbRenderDouble *yOffset;
		GlbRenderDouble *zOffset;
	};

	struct GLB_DLLCLASS_EXPORT GlbMarker3DShapeSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarker3DShapeSymbolInfo()
		{
			symType= GLB_MARKERSYMBOL_3DSHAPE;				
			fillInfo = NULL;
			shapeInfo = NULL;
			pitch = NULL;
			yaw = NULL;
			roll = NULL;
			xScale = NULL;
			yScale = NULL;
			zScale = NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;
		}

		~GlbMarker3DShapeSymbolInfo()
		{
			if(fillInfo)
				delete fillInfo;
			if(shapeInfo)
				delete shapeInfo;
			if(pitch)
				delete pitch;
			if(yaw)
				delete yaw;
			if(roll)
				delete roll;
			if(xScale)
				delete xScale;
			if(yScale)
				delete yScale;
			if(zScale)
				delete zScale;
			if(xOffset)
				delete xOffset;
			if(yOffset)
				delete yOffset;
			if(zOffset)
				delete zOffset;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbPolygon2DSymbolInfo *fillInfo;
		GlbShapeInfo *shapeInfo;
		GlbRenderDouble *pitch;
		GlbRenderDouble *yaw;
		GlbRenderDouble *roll;
		GlbRenderDouble *xScale;
		GlbRenderDouble *yScale;
		GlbRenderDouble *zScale;
		GlbRenderDouble *xOffset;
		GlbRenderDouble *yOffset;
		GlbRenderDouble *zOffset;
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerPixelSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarkerPixelSymbolInfo()
		{
			symType= GLB_MARKERSYMBOL_PIXEL;
			color = NULL;
			opacity = NULL;
			size = NULL;
			label = NULL;
		}

		~GlbMarkerPixelSymbolInfo()
		{
			if(color)
				delete color;
			if(opacity)
				delete opacity;
			if(size)
				delete size;
			if (label)
				delete label;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderColor *color;
		GlbRenderInt32 *opacity;
		GlbRenderInt32 *size;

		// 增加附属标注信息		
		GlbRenderSimpleLabel *label;
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderImageAlign
	{
		GlbRenderImageAlign()
		{
			bUseField = false;
			value = GLB_LABELALG_CENTERCENTER;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeLabelAlignTypeEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderLabelAlign& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);	
		void Save(xmlNodePtr node);		
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderImageInfo
	{
		GlbRenderImageInfo()
		{
			dataSource = NULL;
			maxSize = NULL;
			opacity = NULL;
			color = NULL;
		}

		~GlbRenderImageInfo()
		{
			if(dataSource)
				delete dataSource;
			if(maxSize)
				delete maxSize;
			if(opacity)
				delete opacity;
			if(color)
				delete color;
		}
		void Save(xmlNodePtr node,const glbWChar* prjPath=NULL);
		void Load(xmlNodePtr node,const glbWChar* prjPath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* prjPath=NULL);

		GlbRenderString *dataSource;///<图片资源
		GlbRenderInt32 *maxSize;///<最大大小
		GlbRenderInt32 *opacity;///<透明度
		GlbRenderColor *color;///<颜色
	};

	enum GlbGlobeLabelRelationEnum
	{
		GLB_LABEL_ON_IMAGE = 0,///<文本在图片上
		GLB_LABEL_SURROUND_IMAGE = 1///<文本环绕图片
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderLabelRelation
	{
		GlbRenderLabelRelation()
		{
			bUseField = false;
			value = GLB_LABEL_ON_IMAGE;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeLabelRelationEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderLabelRelation& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderLabelRelationInfo
	{
		GlbRenderLabelRelationInfo()
		{
			relation = NULL;
			align = NULL;
		}

		~GlbRenderLabelRelationInfo()
		{
			if(relation)
				delete relation;
			if(align)
				delete align;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);
		void Load2(xmlNodePtr node);

		GlbRenderLabelRelation *relation;///<文本相对于图片
		GlbRenderLabelAlign *align;///<文本对准
	};



	struct GLB_DLLCLASS_EXPORT GlbMarkerLabelSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarkerLabelSymbolInfo()
		{
			symType = GLB_MARKERSYMBOL_LABEL;		    
			mode = NULL;
			labelAlign = NULL;
			textInfo = NULL;
			imageInfo = NULL;
			labelImageRelationInfo = NULL;
			geoInfo = NULL;
			pitch = NULL;
			yaw = NULL;
			roll = NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;
		}

		~GlbMarkerLabelSymbolInfo()
		{
			if(mode)
				delete mode;
			if(labelAlign)
				delete labelAlign;
			if(textInfo)
				delete textInfo;
			if(imageInfo)
				delete imageInfo;
			if(labelImageRelationInfo)
				delete labelImageRelationInfo;
			if(geoInfo)
				delete geoInfo;
			if(pitch)
				delete pitch;
			if(yaw)
				delete yaw;
			if(roll)
				delete roll;
			if(xOffset)delete xOffset;
			if(yOffset)delete yOffset;
			if(zOffset)delete zOffset;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderBillboard *mode;///<布告板锁定模式
		GlbRenderLabelAlign *labelAlign;///<文本轴对准方式
		GlbRenderTextInfo *textInfo;///<文本信息
		GlbRenderImageInfo *imageInfo;///<图片信息
		GlbRenderLabelRelationInfo *labelImageRelationInfo;///<文本和图像对准
		GlbRenderGeoInfo *geoInfo;///<几何信息
		GlbRenderDouble *pitch;///<俯仰角
		GlbRenderDouble *yaw;///<偏航角
		GlbRenderDouble *roll;///<旋转角
		GlbRenderDouble *xOffset;///X偏移
		GlbRenderDouble* yOffset;///Y偏移
		GlbRenderDouble* zOffset;///Z偏移
	};

	//动态文字渐显文字方式
	enum GlbGlobeDynamicLabelFadeTextEnum
	{
		GLB_FADETEXT_TOPTOBOTTOM = 0,//从上到下
		GLB_FADETEXT_LEFTTORIGHT        //从左到右
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderDynamicLabelFadeMode
	{
		GlbRenderDynamicLabelFadeMode()
		{
			bUseField = false;
			value = GLB_FADETEXT_LEFTTORIGHT;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeDynamicLabelFadeTextEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderDynamicLabelFadeMode& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);	
	};

	//动态文字渐显文字方式
	enum GlbGlobeDynamicLabelFadeSpeedEnum
	{
		GLB_FADETEXT_FAST = 0,       //快
		GLB_FADETEXT_MEDIUM = 1,//中
		GLB_FADETEXT_SLOW             //慢
	};

	struct GLB_DLLCLASS_EXPORT GlbRenderDynamicLabelFadeSpeed
	{
		GlbRenderDynamicLabelFadeSpeed()
		{
			bUseField = false;
			value = GLB_FADETEXT_MEDIUM;
			field = L"";
		}
		glbBool bUseField;
		GlbGlobeDynamicLabelFadeSpeedEnum value;
		CGlbWString field;
		bool operator != (const GlbRenderDynamicLabelFadeSpeed& r) const ;
		glbInt32 GetValue(CGlbFeature *feature);		
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr node);	
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerDynamicLabelSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarkerDynamicLabelSymbolInfo()
		{
			symType = GLB_MARKERSYMBOL_DYNAMICLABEL;		    
			mode = NULL;
			labelAlign = NULL;
			textInfo = NULL;
			imageInfo = NULL;
			geoInfo = NULL;
			pitch = NULL;
			yaw = NULL;
			roll = NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;
			fadeMode = NULL;
			fadeSpeed = NULL;
		}

		~GlbMarkerDynamicLabelSymbolInfo()
		{
			if(mode)
				delete mode;
			if(labelAlign)
				delete labelAlign;
			if(textInfo)
				delete textInfo;
			if(imageInfo)
				delete imageInfo;
			if(geoInfo)
				delete geoInfo;
			if(pitch)
				delete pitch;
			if(yaw)
				delete yaw;
			if(roll)
				delete roll;
			if(xOffset)delete xOffset;
			if(yOffset)delete yOffset;
			if(zOffset)delete zOffset;
			if(fadeMode)delete fadeMode;
			if(fadeSpeed) delete fadeSpeed;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderBillboard *mode;///<布告板锁定模式
		GlbRenderLabelAlign *labelAlign;///<文本轴对准方式
		GlbRenderTextInfo *textInfo;///<文本信息
		GlbRenderImageInfo *imageInfo;///<图片信息
		GlbRenderGeoInfo *geoInfo;///<几何信息
		GlbRenderDouble *pitch;///<俯仰角
		GlbRenderDouble *yaw;///<偏航角
		GlbRenderDouble *roll;///<旋转角
		GlbRenderDouble *xOffset;///X偏移
		GlbRenderDouble* yOffset;///Y偏移
		GlbRenderDouble* zOffset;///Z偏移
		GlbRenderDynamicLabelFadeMode *fadeMode;
		GlbRenderDynamicLabelFadeSpeed* fadeSpeed;///渐进速度
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerImageSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarkerImageSymbolInfo()
		{
			symType = GLB_MARKERSYMBOL_IMAGE;
			imgInfo = NULL;
			imgBillboard = NULL;
			imageAlign = NULL;
			geoInfo = NULL;
			pitch = NULL;
			yaw = NULL;
			roll = NULL;
			xOffset=NULL;
			yOffset=NULL;
			zOffset=NULL;
			is360DegreePanorama = NULL;
		}

		~GlbMarkerImageSymbolInfo()
		{
			if(imgInfo)
				delete imgInfo;
			if(imgBillboard)
				delete imgBillboard;
			if(imageAlign)
				delete imageAlign;
			if(geoInfo)
				delete geoInfo;
			if(pitch)
				delete pitch;
			if(yaw)
				delete yaw;
			if(roll)
				delete roll;
			if(xOffset)delete xOffset;
			if(yOffset)delete yOffset;
			if(zOffset)delete zOffset;
			if(is360DegreePanorama)delete is360DegreePanorama;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderImageInfo *imgInfo;
		GlbRenderBillboard *imgBillboard;
		GlbRenderImageAlign *imageAlign;
		GlbRenderGeoInfo *geoInfo;
		GlbRenderDouble *pitch;
		GlbRenderDouble *yaw;
		GlbRenderDouble *roll;
		GlbRenderDouble *xOffset;///X偏移
		GlbRenderDouble	*yOffset;///Y偏移
		GlbRenderDouble	*zOffset;///Z偏移
		GlbRenderBool	*is360DegreePanorama;///<360度全景图
	};

	struct GLB_DLLCLASS_EXPORT GlbCircleInfo : public GlbShapeInfo
	{
		GlbCircleInfo()
		{
			shapeType = GLB_MARKERSHAPE_CIRCLE;
			radius = NULL;
			edges = NULL;
		}

		~GlbCircleInfo()
		{
			if (radius)
				delete radius;
			if(edges)
				delete edges;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *radius;
		GlbRenderInt32 *edges;
	};

	struct GLB_DLLCLASS_EXPORT GlbRectangleInfo : public GlbShapeInfo
	{
		GlbRectangleInfo()
		{
			shapeType = GLB_MARKERSHAPE_RECTANGLE;
			width = NULL;
			height = NULL;
		}

		~GlbRectangleInfo()
		{
			if(width)
				delete width;
			if(height)
				delete height;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *width;
		GlbRenderDouble *height;
	};

	struct GLB_DLLCLASS_EXPORT GlbEllipseInfo : public GlbShapeInfo
	{
		GlbEllipseInfo()
		{
			shapeType = GLB_MARKERSHAPE_ELLIPSE;
			xRadius = NULL;
			yRadius = NULL;
			edges = NULL;
		}

		~GlbEllipseInfo()
		{
			if(xRadius)
				delete xRadius;
			if(yRadius)
				delete yRadius;
			if(edges)
				delete edges;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *xRadius;
		GlbRenderDouble *yRadius;
		GlbRenderInt32  *edges;
	};

	struct GLB_DLLCLASS_EXPORT GlbArcInfo : public GlbShapeInfo
	{
		GlbArcInfo()
		{
			shapeType = GLB_MARKERSHAPE_ARC;
			xRadius = NULL;
			yRadius = NULL;
			edges = NULL;
			sAngle = NULL;
			eAngle = NULL;
			isFan = NULL;
		}

		~GlbArcInfo()
		{
			if(xRadius)
				delete xRadius;
			if(yRadius)
				delete yRadius;
			if(edges)
				delete edges;
			if(sAngle)
				delete sAngle;
			if(eAngle)
				delete eAngle;
			if (isFan)
				delete isFan;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *xRadius;
		GlbRenderDouble *yRadius;
		GlbRenderInt32 *edges;
		GlbRenderDouble *sAngle;
		GlbRenderDouble *eAngle;
		GlbRenderBool *isFan;
	};

	struct GLB_DLLCLASS_EXPORT GlbSphereInfo : public GlbShapeInfo
	{
		GlbSphereInfo()
		{
			shapeType = GLB_MARKERSHAPE_SPHERE;
			radius = NULL;
		}

		~GlbSphereInfo()
		{
			if(radius)
				delete radius;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *radius;
	};

	struct GLB_DLLCLASS_EXPORT GlbBoxInfo : public GlbShapeInfo
	{
		GlbBoxInfo()
		{
			shapeType = GLB_MARKERSHAPE_BOX;
			length = NULL;
			width = NULL;
			height = NULL;
		}

		~GlbBoxInfo()
		{
			if(length)
				delete length;
			if(width)
				delete width;
			if(height)
				delete height;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *length;
		GlbRenderDouble *width;
		GlbRenderDouble *height;
	};

	struct GLB_DLLCLASS_EXPORT GlbPyramidInfo : public GlbShapeInfo
	{
		GlbPyramidInfo()
		{
			shapeType = GLB_MARKERSHAPE_PYRAMID;
			length = NULL;
			width = NULL;
			height = NULL;
		}

		~GlbPyramidInfo()
		{
			if(length)
				delete length;
			if(width)
				delete width;
			if(height)
				delete height;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *length;
		GlbRenderDouble *width;
		GlbRenderDouble *height;
	};

	struct GLB_DLLCLASS_EXPORT GlbConeInfo : public GlbShapeInfo
	{
		GlbConeInfo()
		{
			shapeType = GLB_MARKERSHAPE_CONE;
			radius = NULL;
			edges = NULL;
			height = NULL;
		}

		~GlbConeInfo()
		{
			if(radius)
				delete radius;
			if(edges)
				delete edges;
			if(height)
				delete height;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *radius;
		GlbRenderInt32 *edges;
		GlbRenderDouble *height;
	};

	struct GLB_DLLCLASS_EXPORT GlbPieInfo : public GlbShapeInfo
	{
		GlbPieInfo()
		{
			shapeType = GLB_MARKERSHAPE_PIE;
			radius = NULL;
			edges = NULL;
			sAngle = NULL;
			eAngle = NULL;
			height = NULL;
		}

		~GlbPieInfo()
		{
			if(radius)
				delete radius;
			if(edges)
				delete edges;
			if(sAngle)
				delete sAngle;
			if(eAngle)
				delete eAngle;
			if(height)
				delete height;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *radius;
		GlbRenderInt32 *edges;
		GlbRenderDouble *sAngle;
		GlbRenderDouble *eAngle;
		GlbRenderDouble *height;
	};

	struct GLB_DLLCLASS_EXPORT GlbCylinderInfo : public GlbShapeInfo
	{
		GlbCylinderInfo()
		{
			shapeType = GLB_MARKERSHAPE_CYLINDER;
			radius = NULL;
			edges = NULL;
			height = NULL;
		}

		~GlbCylinderInfo()
		{
			if(radius)
				delete radius;
			if(edges)
				delete edges;
			if(height)
				delete height;
		}
		void Save(xmlNodePtr node);
		void Load(xmlNodePtr* node);
		void Load2(xmlNodePtr* node);

		GlbRenderDouble *radius;
		GlbRenderInt32 *edges;
		GlbRenderDouble *height;
	};	

	struct GLB_DLLCLASS_EXPORT GlbDigHoleSymbolInfo : public GlbRenderInfo
	{
		GlbDigHoleSymbolInfo()
		{
			type = GLB_OBJECTTYPE_DIGHOLE;
			lineInfo = NULL;
			sideTextureData = NULL;
			belowTextureData = NULL;
		}

		~GlbDigHoleSymbolInfo()
		{
			if (lineInfo)
				delete lineInfo;
			if (sideTextureData)
				delete sideTextureData;
			if (belowTextureData)
				delete belowTextureData;
		}

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbLinePixelSymbolInfo *lineInfo;
		GlbRenderString *sideTextureData;
		GlbRenderString *belowTextureData;
	};

	enum GlbVideoSymbolTypeEnum
	{
		GLB_VIDEOSYMBOL_TERRAIN = 0,	//投影到地形
		GLB_VIDEOSYMBOL_BILLBOARD = 1,	//投影到平面（广告板）
		GLB_VIDEOSYMBOL_FREE = 2              //自由模式
	};

	struct GLB_DLLCLASS_EXPORT GlbVideoSymbolInfo : public GlbRenderInfo
	{
		GlbVideoSymbolInfo()
		{
			type = GLB_OBJECTTYPE_VIDEO;
			symType = GLB_VIDEOSYMBOL_FREE;
			videofile = NULL;
			opacity = NULL;
			bvoice = NULL;
			volume = NULL;
			bPause = NULL;
			referenceTime = NULL;
			bLoop = NULL;
			bRewind = NULL;
			xOffset = NULL;
			yOffset = NULL;
			zOffset = NULL;
			xScale = NULL;
			yScale = NULL;
		}

		~GlbVideoSymbolInfo()
		{
			if (videofile)
				delete videofile;
			if (opacity)
				delete opacity;
			if (bvoice)
				delete bvoice;
			if (volume)
				delete volume;
			if(bPause)
				delete bPause;
			if(referenceTime)
				delete referenceTime;
			if(bLoop)
				delete bLoop;
			if(bRewind)
				delete bRewind;
			if(xOffset)
				delete xOffset;
			if(yOffset)
				delete yOffset;
			if(zOffset)
				delete zOffset;
			if(xScale)
				delete xScale;
			if(yScale)
				delete yScale;
		}

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);
		
		GlbVideoSymbolTypeEnum symType;	//投影到目标对象枚举类型（地形or广告板）
		//视频相关
		GlbRenderString *videofile;		//视频文件
		GlbRenderDouble *opacity;		//不透明度
		GlbRenderBool *bvoice;			    //是否启用声音
		GlbRenderDouble *volume;		     //音量
		GlbRenderBool *bPause;            //是否暂停
		GlbRenderDouble *referenceTime;  //快进时间
		GlbRenderBool *bLoop;             //是否循环
		GlbRenderBool  *bRewind;       //是否重播
		GlbRenderDouble *xOffset;      //x方向偏移
		GlbRenderDouble *yOffset;      //y方向偏移
		GlbRenderDouble *zOffset;      //z方向偏移
		GlbRenderDouble *xScale;       //x方向缩放
		GlbRenderDouble *yScale;       //y方向缩放
	};

	enum GlbGlobeRObEditModeEnum
	{
		GLB_ROBJECTEDIT_DEFAULT    = 0, ///<缺省编辑模式
		GLB_ROBJECTEDIT_SHAPE      = 1, ///<编辑几何形态
		GLB_ROBJECTEDIT_HORIZONTAL = 2, ///<XY平移对象
		GLB_ROBJECTEDIT_VERTICAL   = 3  ///<Z 移动对象
	};

	struct GLB_DLLCLASS_EXPORT GlbDrillRenderInfo:GlbRenderInfo
	{
		GlbDrillRenderInfo()
		{
			type = GLB_OBJECTTYPE_DRILL;
			isRenderBaseLine = true;
			baselineColor = NULL;
			baselineOpacity = NULL;
			baselineWidth = NULL;
			baselineModelLocate = NULL;

			isRenderStratum = false;
			stratumColor = NULL;
			stratumOpacity = NULL;
			stratumWidth = NULL;
			stratumModelLocate = NULL;
			xOffset = NULL;
			yOffset = NULL;
			zOffset = NULL;
			xScale  = NULL;
			yScale  = NULL;
			zScale  = NULL;
		}
		~GlbDrillRenderInfo()
		{
			if (baselineColor)			delete baselineColor;
			if (baselineOpacity)		delete baselineOpacity;
			if (baselineWidth)			delete baselineWidth;
			if (baselineModelLocate)	delete baselineModelLocate;

			if (stratumColor)			delete stratumColor;
			if (stratumOpacity)			delete  stratumOpacity;
			if (stratumWidth)			delete stratumWidth;
			if (stratumModelLocate)		delete stratumModelLocate;

			if (xOffset)				delete xOffset;
			if (yOffset)				delete yOffset;
			if (zOffset)				delete zOffset;

			if (xScale)					delete xScale;
			if (yScale)					delete yScale;
			if (zScale)					delete zScale;
		}

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		/***********中心线***************************************************/
		glbBool         isRenderBaseLine;     //是否绘制钻孔中心线
		GlbRenderColor*  baselineColor;              //中心线颜色
		GlbRenderInt32*  baselineOpacity;            //中心线不透明度
		GlbRenderDouble* baselineWidth;              //中心线宽.
		GlbRenderString* baselineModelLocate;        //中心线模型
		/***********地  层***************************************************/
		glbBool         isRenderStratum;      //是否绘制地层
		GlbRenderColor*  stratumColor;              //地层颜色
		GlbRenderInt32*  stratumOpacity;            //地层不透明度
		GlbRenderDouble* stratumWidth;              //地层线宽.
		GlbRenderString* stratumModelLocate;        //地层模型

		//偏移
		GlbRenderDouble* xOffset; 
		GlbRenderDouble* yOffset;
		GlbRenderDouble* zOffset;

		//缩放
		GlbRenderDouble* xScale;
		GlbRenderDouble* yScale;
		GlbRenderDouble* zScale;
	};

	struct GLB_DLLCLASS_EXPORT GlbSectionRenderInfo:GlbRenderInfo
	{
		GlbSectionRenderInfo()
		{
			type				= GLB_OBJECTTYPE_SECTION;
			isRenderSec			= false;
			secColor			= NULL;
			secOpacity			= NULL;
			secTextureData		= NULL;
			secTexRepeatMode	= NULL;
			secTilingU			= NULL;
			secTilingV			= NULL;
			secTextureRotation	= NULL;
			secModelLocate		= NULL;
			isRenderArc			= false;
			arcWidth			= NULL;
			arcColor			= NULL;
			arcOpacity			= NULL;
			isRenderPoly		= false;
			polyColor			= NULL;
			polyOpacity			= NULL;
			polyTextureData		= NULL;
			polyTexRepeatMode	= NULL;
			polyTilingU			= NULL;
			polyTilingV			= NULL;
			polyTextureRotation = NULL;
			polyModelLocate		= NULL;
			xOffset				= NULL;
			yOffset				= NULL;
			zOffset				= NULL;
			xScale				= NULL;
			yScale				= NULL;
			zScale				= NULL;
		}
		~GlbSectionRenderInfo()
		{
			if (secColor)				delete secColor;
			if (secOpacity)				delete secOpacity;
			if (secTextureData)			delete secTextureData;
			if (secTexRepeatMode)		delete secTexRepeatMode;
			if (secTilingU)				delete secTilingU;
			if (secTilingV)				delete secTilingV;
			if (secTextureRotation)		delete secTextureRotation;
			if (secModelLocate)			delete secModelLocate;
			if (arcWidth)				delete arcWidth;
			if (arcColor)				delete arcColor;
			if (arcOpacity)				delete arcOpacity;
			if (polyColor)				delete polyColor;
			if (polyOpacity)			delete polyOpacity;
			if (polyTextureData)		delete polyTextureData;
			if (polyTexRepeatMode)		delete polyTexRepeatMode;
			if (polyTilingU)			delete polyTilingU;
			if (polyTilingV)			delete polyTilingV;
			if (polyTextureRotation)	delete polyTextureRotation;
			if (polyModelLocate)		delete polyModelLocate;
			if (xOffset)				delete xOffset;
			if (yOffset)				delete yOffset;
			if (zOffset)				delete zOffset;
			if (xScale)					delete xScale;
			if (yScale)					delete yScale;
			if (zScale)					delete zScale;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);
		/******剖面*****************************************************/
		glbBool                    isRenderSec;
		GlbRenderColor             *secColor;
		GlbRenderInt32             *secOpacity;
		GlbRenderString            *secTextureData;
		GlbRenderTexRepeatMode     *secTexRepeatMode;
		GlbRenderInt32             *secTilingU;
		GlbRenderInt32             *secTilingV;
		GlbRenderDouble            *secTextureRotation;
		GlbRenderString            *secModelLocate;
		/******弧段*****************************************************/
		glbBool                    isRenderArc;
		GlbRenderColor *           arcColor;
		GlbRenderInt32 *           arcOpacity;
		GlbRenderDouble*           arcWidth;			
		/******多边形*****************************************************/
		glbBool                    isRenderPoly;
		GlbRenderColor             *polyColor;
		GlbRenderInt32             *polyOpacity;
		GlbRenderString            *polyTextureData;
		GlbRenderTexRepeatMode     *polyTexRepeatMode;
		GlbRenderInt32             *polyTilingU;
		GlbRenderInt32             *polyTilingV;
		GlbRenderDouble            *polyTextureRotation;
		GlbRenderString            *polyModelLocate;

		//偏移
		GlbRenderDouble				*xOffset; 
		GlbRenderDouble				*yOffset;
		GlbRenderDouble				*zOffset;

		//缩放
		GlbRenderDouble*			xScale;
		GlbRenderDouble*			yScale;
		GlbRenderDouble*			zScale;
	};

	// 网络数据
	struct GLB_DLLCLASS_EXPORT GlbNetworkRenderInfo : public GlbRenderInfo
	{
		GlbNetworkRenderInfo()
		{
			type				= GLB_OBJECTTYPE_NETWORK;
			edgeColor			= NULL;
			edgeOpacity			= NULL;
			edgeLineWidth		= NULL;
			edgeModelLocate		= NULL;
			nodeColor			= NULL;
			nodeOpacity			= NULL;
			nodeSize			= NULL;
			isRenderFromNode	= NULL;
			isRenderToNode		= NULL;
			fromNodeModelLocate	= NULL;
			toNodeModelLocate	= NULL;
		}
		~GlbNetworkRenderInfo()
		{
			if (edgeColor)			delete edgeColor;
			if (edgeOpacity)		delete edgeOpacity;
			if (edgeLineWidth)		delete edgeLineWidth;
			if (edgeModelLocate)	delete edgeModelLocate;
			if (nodeColor)			delete nodeColor;
			if (nodeOpacity)		delete nodeOpacity;
			if (nodeSize)			delete nodeSize;
			if (isRenderFromNode)	delete isRenderFromNode;
			if (isRenderToNode)		delete isRenderToNode;
			if (fromNodeModelLocate)delete fromNodeModelLocate;
			if (toNodeModelLocate)	delete toNodeModelLocate;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		// 网络线
		GlbRenderColor *edgeColor;					///<网络线颜色
		GlbRenderInt32 *edgeOpacity;				///<网络线不透明度
		GlbRenderDouble*edgeLineWidth;				///<网络线线宽
		GlbRenderString*edgeModelLocate;			///<网络线文件路径
		//GlbRenderString*edgeTextureLocate;			///<网络线纹理路径	

		// 节点
		GlbRenderColor *nodeColor;					///<网络节点颜色
		GlbRenderInt32 *nodeOpacity;				///<网络节点不透明度
		GlbRenderDouble*nodeSize;					///<网络节点大小
		GlbRenderBool *isRenderFromNode;			///<是否绘制网络线的起始节点
		GlbRenderBool *isRenderToNode;				///<是否绘制网络线的终止节点
		GlbRenderString *fromNodeModelLocate;		///<网络线的起始节点文件路径
		GlbRenderString *toNodeModelLocate;			///<网络线的终止节点文件路径
	};
	//网格数据
	struct GLB_DLLCLASS_EXPORT GlbGridRenderInfo : public GlbRenderInfo
	{
		GlbGridRenderInfo()
		{
			type				= GLB_OBJECTTYPE_GRID;	
			isShowEdge			= true;
			gridOpacity			= 100;
			mode				= GLB_RENDERMODE_GRID;
			geo					= NULL;
			I					= -1;
			J					= -1;
			INum				= 0;
			JNum				= 0;
		}
		~GlbGridRenderInfo()
		{
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		// 是否显示edge
		glbBool		isShowEdge;
		// 格网不透明度
		glbInt32	gridOpacity;
		// 格网颜色文件路径
		CGlbWString colorLocate;

		// 模式 -- grid格网模式/lineClip线切片/AxisClip轴切片/BarrierClip栅栏切片
		GlbGlobeGridRenderModeEnum mode;

		// 线切片
		glbref_ptr<IGlbGeometry> geo;
		// AxisClip轴切片
		glbInt32	I;
		glbInt32	J;
		// 栅栏切片
		glbInt32	INum;
		glbInt32	JNum;
	};

	/**
	  * 光源结构
	  */
	struct GLB_DLLCLASS_EXPORT GlbGlobeLight  :  public CGlbReference
	{
		GlbGlobeLight()
		{
			Id = 1;
			bParallel = true;
			Position[0] = 0; Position[1] = 0; Position[2] = 1;
			Ambient[0] = 0; Ambient[1] = 0; Ambient[2] = 0;
			Diffuse[0] = 0; Diffuse[1] = 0; Diffuse[2] = 0;
			Specular[0] = 0; Specular[1] = 0; Specular[2] = 0;
			SpotDir[0] = 0; SpotDir[1] = 0; SpotDir[2] = -1;
			SpotCutoff = 180;
			SpotExponent = 0.0;
			ConstantAttenuation = 1.0;
			LinearAttenuation = 0.0;
			QuadRaticAttenuation = 0.0;
			onOrOFF = true;
		}
		GlbGlobeLight& operator=(GlbGlobeLight& lt)
		{
			Id = lt.Id;
			bParallel = lt.bParallel;
			Position[0] = lt.Position[0];	Position[1] = lt.Position[1];	Position[2] = lt.Position[2];
			Ambient[0] = lt.Ambient[0];		Ambient[1] = lt.Ambient[1];		Ambient[2] = lt.Ambient[2];
			Diffuse[0] =  lt.Diffuse[0];	Diffuse[1] = lt.Diffuse[1];		Diffuse[2] = lt.Diffuse[2];
			Specular[0] = lt.Specular[0];	Specular[1] = lt.Specular[1];	Specular[2] = lt.Specular[2];
			SpotDir[0] = lt.SpotDir[0];		SpotDir[1] = lt.SpotDir[1];		SpotDir[2] = lt.SpotDir[2];
			SpotCutoff = lt.SpotCutoff;
			SpotExponent = lt.SpotExponent;
			ConstantAttenuation = lt.ConstantAttenuation;
			LinearAttenuation = lt.LinearAttenuation;
			QuadRaticAttenuation = lt.QuadRaticAttenuation;
			onOrOFF = lt.onOrOFF;
			return *this;
		}
		glbInt32 Id;				///<光源编号1,2.....  0编号分配给场景默认头灯
		glbBool  onOrOFF;			///<光源开关 true-开 ， false-关
		glbBool  bParallel;			///<是否是平行光源
		glbFloat Position[3];		///<光源位置，如果是平行光源，则表示方向	默认值(0,0,1)
		glbFloat Ambient[3];		///<环境光颜色			默认值(0,0,0)
		glbFloat Diffuse[3];		///<散射光颜色			默认值(1,1,1)LIGHT0 或(0,0,0)LIGHT1,LIGHT2.....
		glbFloat Specular[3];		///<镜面反射光颜色		默认值(1,1,1)LIGHT0 或(0,0,0)LIGHT1,LIGHT2.....
		glbFloat SpotDir[3];		///<聚光灯方向			默认值(0,0,-1)
		glbFloat SpotCutoff;		///<聚光灯截止角		默认值180（表示聚光灯特性被禁用），取值范围[0-90]
		glbFloat SpotExponent;		///<聚光指数			默认值0.0， 取值范围[0-128]
		glbFloat ConstantAttenuation;	///<固定衰减因子	默认值1.0
		glbFloat LinearAttenuation;		///<线性衰减因子	默认值0.0
		glbFloat QuadRaticAttenuation;	///<二次衰减因子	默认值0.0
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerFireSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarkerFireSymbolInfo ()
		{
			symType = GLB_MARKERSYMBOL_FIRE;
			scale = NULL;
			intensity = NULL;
		}
		~GlbMarkerFireSymbolInfo ()
		{
			if (scale) delete scale;
			if (intensity) delete intensity;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderDouble *scale;
		GlbRenderDouble *intensity;
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerSmokeSymbolInfo : public GlbMarkerSymbolInfo
	{
		GlbMarkerSmokeSymbolInfo ()
		{
			symType = GLB_MARKERSYMBOL_SMOKE;
			startColor = NULL;
			endColor = NULL;
			scale = NULL;
		}
		~GlbMarkerSmokeSymbolInfo ()
		{
			if (startColor) delete startColor;
			if (endColor) delete endColor;
			if (scale) delete scale;
		}

		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderColor *startColor;
		GlbRenderColor *endColor;
		GlbRenderDouble *scale;
		GlbRenderDouble *intensity;
	};

	struct GLB_DLLCLASS_EXPORT GlbMarkerEpicentreSymbolInfo : public GlbMarkerSymbolInfo
	{// 震源点符号
		GlbMarkerEpicentreSymbolInfo ()
		{
			symType = GLB_MARKERSYMBOL_EPICENTRE;
			radius = NULL;
			color = NULL;
		}
		~GlbMarkerEpicentreSymbolInfo ()
		{
			if (radius) delete radius;
			if (color) delete color;
		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load2(xmlNodePtr node,const glbWChar* relativepath=NULL);

		GlbRenderDouble *radius;
		GlbRenderColor *color;
	};

	struct GLB_DLLCLASS_EXPORT GlbCoordinateAxisSymbolInfo : public GlbRenderInfo
	{// 坐标轴符号
		GlbCoordinateAxisSymbolInfo ()
		{
			type = GLB_OBJECTTYPE_COORDINAT_AXIS;
			originX = originY = originZ = 0;
			axisLengthX = axisLengthY = axisLengthZ = 1000;
			axisStepX = axisStepY = axisStepZ = 200;
			bShowGrid = true;
			bShowLabel = true;
			bInvertZ = false;
			labelSize = 40.0f;
			xOffset = 0;
			yOffset = 0;
			zOffset = 0;
			xScale = 1;
			yScale = 1;
			zScale = 1;
		}
		~GlbCoordinateAxisSymbolInfo ()
		{

		}
		void Save(xmlNodePtr node,const glbWChar* relativepath=NULL);
		void Load(xmlNodePtr node,const glbWChar* relativepath=NULL);

		glbBool bShowGrid;		///<显示格网
		glbBool	bShowLabel;		///<显示刻度

		glbBool bInvertZ;		///<Z坐标轴反向显示--指向负方向

		glbDouble originX;		///<原点位置
		glbDouble originY;
		glbDouble originZ;
		glbDouble axisLengthX;	///<轴长
		glbDouble axisLengthY;
		glbDouble axisLengthZ;
		glbDouble axisStepX;	///<轴刻度步距
		glbDouble axisStepY;
		glbDouble axisStepZ;

		glbDouble labelSize;	///<字体大小

		glbDouble xOffset;		///<偏移
		glbDouble yOffset;
		glbDouble zOffset;

		glbDouble xScale;		///<放缩
		glbDouble yScale;
		glbDouble zScale;
	};
}

class GLB_DLLCLASS_EXPORT CGlbGlobeDataExchange 
{
public:
	CGlbGlobeDataExchange();

	// tin文件转为osg文件
	static bool tinToOsgFile(glbWChar* tinfile, glbWChar* osgFile, glbInt32 tinColor, glbBool isGlobe);
	// tin文件转为osg文件 ,带参考偏移基准点(refPointX,refPointY,refPointZ) 
	// 说明：大坐标数据时一个图层或相关图层对象做转换时使用，避免浮点误差导致的对象连接处错位问题
	static bool tinToOsgFileByRefPoint(glbWChar* tinfile, glbWChar* osgFile, glbInt32 tinColor, glbBool isGlobe, glbDouble refPointX,  glbDouble refPointY,  glbDouble refPointZ);
};

GlbGlobe::GlbRenderInfo* glbLoadRenderInfo(xmlNodePtr node,glbWChar* relativepath);
GlbGlobe::GlbRenderInfo* glbLoadRenderInfo2(xmlNodePtr node,glbWChar* relativepath);
#endif