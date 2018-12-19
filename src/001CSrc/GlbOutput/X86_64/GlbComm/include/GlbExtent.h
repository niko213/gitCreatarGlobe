/********************************************************************
  * Copyright (c) 2013 北京超维创想信息技术有限公司
  * All rights reserved.
  *
  * @file    GlbExtent.h
  * @brief   几何外包头文件
  *
  * 这个档案定义CGlbExtent这个类
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbReference.h"

/**
  * @brief CGlbExtent几何外包类
  *
  * 继承引用计数，可以设置或获取外包值
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API  CGlbExtent:public CGlbReference
{
public:
/**
  * @brief 构造函数
  *
  * 初始化成员变量为0
  *
  * @return 无
  */
	CGlbExtent(void);
/**
  * @brief 构造函数
  *
  * 根据传入值初始化成员变量
  *
  * @param minX 最小x值
  * @param maxX 最大x值
  * @param minY 最小y值
  * @param maxY 最大y值
  * @param minZ 最小z值
  * @param maxZ 最大z值
  * @return 无
  */
	CGlbExtent(const glbDouble minX,
		       const glbDouble maxX,
               const glbDouble minY, 
			   const glbDouble maxY,
               const glbDouble minZ=0,
			   const glbDouble maxZ=0);
/**
  * @brief 析构函数
  *
  * @return 无
  */
	virtual ~CGlbExtent(void){}

/**
  * @brief 是否有效
  *
  * 判断外包是否有效
  *
  * @return 有效返回true，无效返回false
  */
	glbBool   IsValid()const;
/**
  * @brief x方向宽度
  *
  * 获取x方向宽度
  *
  * @return 返回宽度
  */
	glbDouble GetXWidth()const;
/**
  * @brief y方向高度
  *
  * 获取y方向高度
  *
  * @return 返回高度
  */
	glbDouble GetYHeight()const;
/**
  * @brief z方向长度
  *
  * 获取z方向长度
  *
  * @return 返回长度
  */
	glbDouble GetZLengh()const;
/**
  * @brief 获取最小外包值
  *
  * 获取最小外包值
  *
  * @param x 输出参数 最小x值
  * @param y 输出参数 最小y值
  * @param z 输出参数 最小z值
  * @return 无
  */
	void      GetMin(glbDouble *x,
		             glbDouble *y,
				     glbDouble *z=0)const;
/**
  * @brief 设置最小外包值
  *
  * 设置最小外包值
  *
  * @param x 最小x值
  * @param y 最小y值
  * @param z 最小z值
  * @return 无
  */
	void      SetMin(const glbDouble x,
		             const glbDouble y,
					 const glbDouble z=0);
/**
  * @brief 获取最大外包值
  *
  * 获取最小外包值
  *
  * @param x 输出参数 最大x值
  * @param y 输出参数 最大y值
  * @param z 输出参数 最大z值
  * @return 无
  */
	void      GetMax(glbDouble* x, 
		             glbDouble* y,
					 glbDouble* z=0)const;
/**
  * @brief 设置最大外包值
  *
  * 设置最大外包值
  *
  * @param x 最大x值
  * @param y 最大y值
  * @param z 最大z值
  * @return 无
  */
	void      SetMax(const glbDouble x, 
		             const glbDouble y,
					 const glbDouble z=0);
/**
  * @brief 获取外包中心点
  *
  * 获取外包中心点
  *
  * @param x 输出参数 中心x值
  * @param y 输出参数 中心y值
  * @param z 输出参数 中心z值
  * @return 无
  */
	void      GetCenter(glbDouble* x, 
		                glbDouble* y,
						glbDouble* z=0)const;
/**
  * @brief 包含
  *
  * 坐标是否包含于当前外包
  *
  * @param x x值
  * @param y y值
  * @param z z值
  * @return 包含返回true，不包含返回false
  */
	glbBool   Contains(const glbDouble x, 
		                const glbDouble y,
						const glbDouble z =0)const;
/**
  * @brief 包含
  *
  * 外包是否包含于当前外包
  *
  * @param other 外包值
  * @return 包含返回true，不包含返回false
  */
    glbBool   Contain(const CGlbExtent& other)const;
/**
  * @brief 合并
  *
  * 把外包和当前外包合并
  *
  * @param other 外包值
  * @return 无
  */
	void      Merge(const CGlbExtent& other);
/**
  * @brief 合并
  *
  * 把点和当前外包合并
  *
  * @param x x值
  * @param y y值
  * @param z z值
  * @return 无
  */
    void      Merge(const glbDouble x,
		            const glbDouble y,
					const glbDouble z=0);
/**
  * @brief 相交
  *
  * 外包是否和当前外包相交
  *
  * @param other 外包值
  * @return 包含返回true，不包含返回false
  */
    CGlbExtent* Intersect(const CGlbExtent& other,bool useZ=true)const;
/**
  * @brief 获取外包值
  *
  * 获取外包值
  *
  * @param minX 输出参数 最小x值
  * @param maxX 输出参数 最大x值
  * @param minY 输出参数 最小y值
  * @param maxY 输出参数 最大y值
  * @param minZ 输出参数 最小z值
  * @param maxZ 输出参数 最大z值
  * @return 无
  */
    void      Get(glbDouble *minX, 
		          glbDouble *maxX,
                  glbDouble *minY,
				  glbDouble *maxY,
                  glbDouble *minZ=0,
				  glbDouble *maxZ=0)const;
/**
  * @brief 设置外包值
  *
  * 设置外包值
  *
  * @param minX 最小x值
  * @param maxX 最大x值
  * @param minY 最小y值
  * @param maxY 最大y值
  * @param minZ 最小z值
  * @param maxZ 最大z值
  * @return 无
  */
    void      Set(const glbDouble minX, 
		          const glbDouble maxX,
                  const glbDouble minY,
				  const glbDouble maxY,
                  const glbDouble minZ=0,
				  const glbDouble maxZ=0);

/**
  * @brief 平移
  *
  *@param dx X轴平移量
  *@param dy Y轴平移量
  *@param dz Z轴平移量
  */
	void Translate(const glbDouble dx,
		           const glbDouble dy,
				   const glbDouble dz);
/**
  * @brief 外包是否相同
  *
  * 操作符==重载，外包是否相同
  *
  * @param toExtent 外包值
  * @return 相同返回true，不同返回false
  */
	glbBool operator==(const CGlbExtent& toExtent)const;
/**
  * @brief 外包是否不相同
  *
  * 操作符!=重载，外包是否不相同
  *
  * @param toExtent 外包值
  * @return 不相同返回true，相同返回false
  */
	glbBool operator!=(const CGlbExtent& toExtent)const;

	glbDouble GetLeft()const{ return mpr_minVal[0]; }
	glbDouble GetTop()const{ return mpr_maxVal[1]; }
	glbDouble GetRight()const{ return mpr_maxVal[0]; }
	glbDouble GetBottom()const{ return mpr_minVal[1]; }
private:
	glbDouble mpr_minVal[3];///<外包最小值
	glbDouble mpr_maxVal[3];///<外包最大值
};
