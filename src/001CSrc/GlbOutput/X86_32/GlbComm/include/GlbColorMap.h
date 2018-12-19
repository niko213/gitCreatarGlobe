/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbColorMap.h
  * @brief   色带映射表头文件
  *
  * 这个档案定义CGlbColorMap这个类
  *
  * @version 1.0
  * @author  ML
  * @date    2018-7-16 14:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbReference.h"

// 备注： 废弃，改用
class GLBCOMM_API CGlbColorMap:public CGlbReference
{
public:
	enum GlbColorMapStyle
	{
		Cool,
		Hsv,
		Gray,
		Hot,
		Undefined  
	};
	struct _GlbColor
	{
	public:
		_GlbColor()
		{
			red = green = blue = alpha = 0;
		}
		_GlbColor(float r, float g, float b, float a)
		{
			red = r;
			green = g;
			blue = b;
			alpha = a;				 
		}
		float red;
		float green;
		float blue;
		float alpha;
	};
	typedef _GlbColor GlbColor;

public:
	CGlbColorMap(void);
	~CGlbColorMap(void);

	void SetStyle(GlbColorMapStyle _style);
	GlbColorMapStyle GetStyle();
	bool GetRange(double& min, double& max);
	void SetRange(double min, double max);
	GlbColor GetColor(double v);  
	
	bool SetColors(GlbColor* clrs, int num);

private:
	void buildCoolStyle();
	void buildHsvStyle();
	void buildGrayStyle();
	void buildHotStyle();
private:    
	std::vector<GlbColor>  mpr_colors;
	GlbColorMapStyle mpr_style;
	double mpr_min;
	double mpr_max;
};

