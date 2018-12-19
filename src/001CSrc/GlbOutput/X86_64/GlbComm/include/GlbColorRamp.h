/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    colorRamp.h
  * @brief   色带类型 渐变/单值/多段渐变 头文件
  *
  * 这个档案定义CGlbColorRamp这个类
  *
  * @version 1.0
  * @author  lfc
  * @date    2018-8-7 14:40
*********************************************************************/
#pragma once
#include "GlbCommTypes.h"
#include "GlbReference.h"
#include <vector>
#include <map>
/**
* @brief 色带类型.
*/
enum GlbRampTypeEnum
{
	GLB_Ramp_Single = 0,			// 单值色带		S
	GLB_Ramp_Gradient = 1,			// 渐变色带 【起始颜色，终止颜色】 G
	GLB_Ramp_MultiGradient = 2		// 多段渐变色带 MG
};
/**
* @brief 颜色
*/
struct GlbColor
{
public:
	GlbColor(){}
	GlbColor(glbDouble r, glbDouble g, glbDouble b, glbDouble a)
	{
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}
	glbDouble m_r;
	glbDouble m_g;
	glbDouble m_b;
	glbDouble m_a;

};
/**
* @brief 预定义色带种类(GRamp)
*/
enum GlbGradientModeEnum
{
	GLB_GRamp_Unknown = 0,			//未知
	GLB_GRamp_BlueToRed = 1,		//蓝-红
	GLB_GRamp_RedToBlue = 2,		//红-蓝
	GLB_GRamp_BrownToYellow = 3,	//棕-黄
	GLB_GRamp_RedToGreen = 4,		//红-绿
	GLB_GRamp_GreenToRed = 5		//绿-红
};

/**
* @brief 预定义色带种类(MGRamp)
*/
enum GlbMGradientModeEnum
{
	GLB_MGRamp_Unknown = 0,		//未知
	GLB_MGRamp_BlueToRed = 1,	//蓝-青-黄-红
	GLB_MGRamp_Brown = 2,		//黑-棕-黄
	GLB_MGRamp_Rainbow1 = 3,	//红1-品1-蓝1-青1-黄0.3-绿0.7-黄1-红0.7-橙0.3
	GLB_MGRamp_Rainbow2 = 4,	//红橙黄绿青蓝紫
	GLB_MGRamp_Rainbow3 = 5,	//紫蓝青绿黄橙红
	GLB_MGRamp_GreenToRed = 6	//绿-黄-红	
};
/**
* @brief 色条： 渐变图例中的最小单元，包括起始点/终止点的颜色、位置信息，色条中间的颜色由起止点线性插值
*/
class GLBCOMM_API CGlbColorSeg : public CGlbReference
{
public:
	CGlbColorSeg();
	CGlbColorSeg(GlbColor fromClr, GlbColor toClr, glbDouble fromVal, glbDouble toVal);
	~CGlbColorSeg();

	/**
	* @brief 添加色条的颜色信息/位置信息.
	* @param[in]  fromClr,色条起始颜色
	* @param[in]  toClr,色条终止颜色
	* @param[in]  fromVal,色条起始位置
	* @param[in]  toVal,色条终止位置
	* return
	*/
	void SetParameters(GlbColor fromClr, GlbColor toClr, glbDouble fromVal, glbDouble toVal);

	/**
	* @brief 获取色条的颜色信息/位置信息.
	* @param[out]  fromClr,色条起始颜色
	* @param[out]  toClr,色条终止颜色
	* @param[out]  fromVal,色条起始位置
	* @param[out]  toVal,色条终止位置
	* return
	*/
	void GetParameters(GlbColor& fromClr, GlbColor& toClr, glbDouble& fromVal, glbDouble& toVal);

	/**
	* @brief 由位置信息获得颜色数据
	* @param[in]  val,位置信息，范围为[0,1]
	* @param[out]  color,返回的颜色信息，结果为val所对应的色条颜色（根据首尾位置与颜色的线性插值所得，val不在色条范围内时，返回色条中距离val最近的颜色）
	* return  val与返回的颜色所对应位置的距离长度（val在色带内部返回0），用以判断val在同一色带的所有色条中的最优颜色值。
	*/
	glbDouble GetColor(glbDouble val, GlbColor& color);

private:
	//起止点颜色信息
	GlbColor m_fromColor;
	GlbColor m_toColor;
	//起止点位置信息
	glbDouble  m_fromValue;
	glbDouble  m_toValue;
};
/**
* @brief 色带：定义色带的所有信息
*/
class GLBCOMM_API CGlbColorRamp : public CGlbReference
{
public:
	CGlbColorRamp(GlbRampTypeEnum rampType = GLB_Ramp_Gradient);
	~CGlbColorRamp();

	/**
	* @brief 设置色带的类型：单值/分段/渐变
	* @param[in]  rampType,色带类型
	* return
	*/
	void SetRampType(GlbRampTypeEnum rampType);

	/**
	* @brief 获取色带的类型：单值/分段/渐变
	* @param[out]  rampType,色带类型
	* return
	*/
	void GetRampType(GlbRampTypeEnum& rampType);


#pragma region 单值色带(SRamp)
	/**
	* @brief 添加单值图例的颜色信息.
	* @param[in]  clr ,单值图例颜色
	* @param[in]  name,颜色对应的描述.
	* return 成功返回true,失败返回false
	*/
	glbBool AddSRampItem(GlbColor clr,const glbWChar* name);

	/**
	* @brief 获取单值图例的颜色数量.
	* return 颜色数量
	*/
	glbInt32 GetSRampItemCount();

	/**
	* @brief 获取单值图例中的索引值为idx的颜色、名称信息
	* @param[in]  idx ,索引
	* @param[in]  name,颜色对应的描述.
	* @param[out]  clr,idx对应的颜色.
	* return 成功返回true,失败返回false（idx不在范围内）
	*/
	glbBool GetSRampItem(glbInt32 idx, glbWChar** name, GlbColor& clr);

	/**
	* @brief 获取单值图例中名称为name的颜色
	* @param[in]  name,颜色对应的描述.
	* @param[out] clr,颜色.
	* return 成功返回true,失败返回false（idx不在范围内）
	*/
	glbBool GetSRampItemByName(const glbWChar* name, GlbColor& clr);

	/**
	* @brief 删除单值图例中索引值为idx的颜色、名称信息
	* @param[in]  idx,索引.
	* return 成功返回true,失败返回false（idx不在范围内）
	*/
	glbBool RemoveSRampItem(glbInt32 idx);

	/**
	* @brief 删除单值图例中名称为name的颜色、名称信息
	* @param[in]  name,名称.
	* return 成功返回true,失败返回false
	*/
	glbBool RemoveSRampItemByName(const glbWChar* name);

	/**
	* @brief 删除单值图例中所有颜色、名称信息
	* return
	*/
	void RemoveAllSRampItems();
#pragma endregion

#pragma region 渐变色带 【起始颜色，终止颜色】(GRamp)
	/**
	* @brief 设置渐变图例中的颜色、属性、名称等信息
	* @param[in]  fromClr,起始颜色.
	* @param[in]  toClr,终止颜色.
	* @param[in]  fromVal,起始属性.
	* @param[in]  toVal,终止属性.
	* @param[in]  fromName,起始名称.
	* @param[in]  toName,终止名称.
	* return
	*/
	void SetGRampColor(GlbColor fromClr, GlbColor toClr, glbDouble fromVal, glbDouble toVal, const glbWChar* fromName, const glbWChar* toName);
	
	/**
	* @brief 用预定义色条RampType，设置渐变图例中的属性、名称等信息
	* @param[in]  RampType,色条.
	* @param[in]  fromVal,起始属性.
	* @param[in]  toVal,终止属性.
	* @param[in]  fromName,起始色条名称.
	* @param[in]  toName,终止色条名称.
	* return
	*/
	void SetGRampColorByMode(GlbGradientModeEnum RampType, glbDouble fromVal, glbDouble toVal, const glbWChar* fromName, const glbWChar* toName);
	
	/**
	* @brief 获取渐变图例的预定义色条
	* return 渐变图例的预定义色条样式
	*/
	GlbGradientModeEnum GetGrampMode();

	/**
	* @brief 获取渐变色带的色条、位置、属性名称信息
	* @param[out]  fromCol,起始颜色.
	* @param[out]  toCol,终止颜色.
	* @param[out]  fromVal,起始属性.
	* @param[out]  toVal,终止属性.
	* @param[out]  fromName,起始色条名称.
	* @param[out]  toName,终止色条名称.
	* return
	*/
	glbBool GetGRampItem(GlbColor& fromClr, GlbColor& toClr, glbDouble& fromVal, glbDouble& toVal, glbWChar** fromName, glbWChar** toName);
	
	/**
	* @brief 获取渐变图例中，属性为val所对应的颜色值
	* @param[in]  val,属性值.
	* @param[out]  clr,属性为val的颜色.
	* return 成功返回true, 失败返回false
	*/
	glbBool GetGRampColor(glbDouble val, GlbColor& clr);

#pragma endregion

#pragma region 多段渐变色带 (MGRamp)
	/**
	* @brief 向MGRamp中添加色条信息
	* @param[in]  fromClr,所添加色条的起始颜色信息.
	* @param[in]  toClr,所添加色条的终止颜色信息.
	* @param[in]  fromPos,所添加色条的起始位置信息.
	* @param[in]  toPos,所添加色条的终止颜色信息.
	* return
	*/
	void AddMGRampColorSeg(GlbColor fromClr, GlbColor toClr,glbDouble fromPos, glbDouble toPos); //fromPos,,toPos [0-1]

	/**
	* @brief 获取MGRamp中色条数量
	* return MGRamp中色条数量
	*/
	glbInt32 GetMGRampColorSegCount();

	/**
	* @brief 获取MGRamp中索引值为idx的色条信息
	* @param[in]  idx,索引.
	* @param[out]  fromClr,色条的起始颜色信息.
	* @param[out]  toClr,色条的终止颜色信息.
	* @param[out]  fromPos,色条的起始位置信息.
	* @param[out]  toPos,色条的终止颜色信息.
	* return 获取成功返回true， 获取失败返回false
	*/
	glbBool GetMGRampColorSeg(glbInt32 idx,GlbColor& fromClr, GlbColor& toClr,glbDouble& fromPos, glbDouble& toPos);

	/**
	* @brief 删除MGRamp中索引值为idx的色条信息
	* @param[in]  idx,索引.
	* return 删除成功返回true， 删除失败返回false
	*/
	glbBool RemoveMGRampColorSeg(glbInt32 idx);

	/**
	* @brief 设置MGRamp中索引值为idx的色条信息
	* @param[in]  idx,索引.
	* @param[in]  colSeg,色条信息.
	* return 设置成功返回true， 设置失败返回false
	*/
	glbBool SetMGRampColorSeg(glbInt32 idx, CGlbColorSeg colSeg);

	/**
	* @brief 获取MGRamp中索引值为idx的色条信息
	* @param[in]  idx,索引.
	* @param[out]  colSeg,色条信息.
	* return 获取成功返回true， 获取失败返回false
	*/
	glbBool GetMGRampColorSeg(glbInt32 segIndex, CGlbColorSeg& colSeg);

	/**
	* @brief 清空MGRamp中的色条信息
	* return 
	*/
	void RemoveAllMGRampColorSeg();

	/**
	* @brief 设置MGRamp的色条预定义信息
	* @param[in]  RampType,预定义色条类型.
	* return
	*/
	void SetMGRampMode(GlbMGradientModeEnum RampType);

	/**
	* @brief 获取MGRamp的色条预定义信息
	* return MGRamp的色条预定义类型
	*/
	GlbMGradientModeEnum GetMGRampMode();

	/**
	* @brief 添加MGRamp的位置、属性值、名称映射关系
	* @param[in]  colorPos,位置信息.
	* @param[in]  mapVal,属性信息.
	* @param[in]  mapName,名称信息.
	* return 添加成功返回true， 添加失败返回false
	*/
	glbBool AddMGRampMapping(glbDouble colorPos, glbDouble mapVal, const glbWChar* mapName/*名称*/);

	/**
	* @brief 获取MGRamp的位置、属性值、名称映射关系的数量
	* return 映射数量
	*/
	glbInt32 GetMGRampMappingCount();

	/**
	* @brief 获取MGRamp中索引值为idx的位置、属性值、名称映射关系
	* @param[in]  idx,索引.
	* @param[out]  colorPos,位置信息.
	* @param[out]  mapVal,属性信息.
	* @param[out]  name,名称信息.
	* return 获取成功返回true， 获取失败返回false
	*/
	glbBool GetMGRampMapping(glbInt32 idx, glbDouble& colorPos, glbDouble& mapVal, glbWChar** name /*名称*/);

	/**
	* @brief 删除MGRamp中索引值为idx的位置、属性值、名称映射关系
	* @param[in]  idx,索引.
	* return 删除成功返回true， 删除失败返回false
	*/
	glbBool RemoveMGRampMapping(glbInt32 idx);

	/**
	* @brief 删除MGRamp中所有的位置、属性值、名称映射关系
	* return 删除成功返回true， 删除失败返回false
	*/
	void RemoveAllMGRampMapping();

	/**
	* @brief 获取MGRamp中属性为val的颜色信息
	* @param[in]  val,属性信息.
	* @param[out]  clr,颜色信息.
	* return 获取成功返回true， 获取失败返回false
	*/
	glbBool GetMGRampColor(glbDouble val, GlbColor& clr);
#pragma endregion

private:
	/********************************
	*func:初始化色带（分别为单值/分段/渐变色带添加默认数据）
	********************************/
	void InitRampData();
	/********************************
	*func:预设色带
	********************************/
	void PreInstallRampData();

private:
	GlbRampTypeEnum				m_rampType;//色带类型：单值/渐变/多段渐变

	GlbGradientModeEnum			m_GradientMode; // 渐变色带模式
	GlbMGradientModeEnum		m_MGradientMode; // 多段渐变色带模式

	std::vector<std::pair<CGlbWString /*名称*/, GlbColor/*颜色*/>> m_SRamp_rampData;//单值图例名称-颜色 数据对
	std::pair<CGlbColorSeg/*色条*/, std::pair<CGlbWString /*起点名称*/, CGlbWString /*终点名称*/> >  m_GRamp_rampData;//渐变图例色条名称数据
	
	std::vector<CGlbColorSeg>		m_MGRamp_rampData;//多段渐变图例包含的色条
	std::vector<std::pair<glbDouble /*pos*/, std::pair<CGlbWString /*名称*/, glbDouble /*pro*/> > > m_Gradient_valueVec;//多段渐变图例的位置/属性映射关系
	
	std::vector<std::vector<CGlbColorSeg> >	m_MGradient_rampData_preInstall;//预设色带数据（MGRamp）
	std::vector<CGlbColorSeg>	m_Gradient_rampData_preInstall;//预设色带数据（GRamp）
};
//}

