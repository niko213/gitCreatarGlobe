#pragma once
#include <vector>
#include "IGlbGeometry.h"
#include "GlbLine.h"
using namespace std;

/**
  * @brief 多边形
  *
  * 该类继承于IGlbGeometry，该对象由一个外环，多个内环组成，内环在外环范围以内
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class  GLBDATAENGINE_API CGlbPolygon :public IGlbGeometry
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
 CGlbPolygon(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
 ~CGlbPolygon(void);
public:
/**
  * @brief 获取对象外包
  *
  * @return 外包
  */
	const CGlbExtent*			GetExtent();
/**
  * @brief 获取对象几何类型
  *
  * @return 几何类型
  */
	GlbGeometryTypeEnum			GetType();
/**
  * @brief 判断几何对象是否为空
  *
  * @return 空，返回true，不为空，返回false
  */
	glbBool						IsEmpty();
/**
  * @brief 获取所占用内存大小.
  */
	glbUInt32                   GetSize();
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  */
	glbInt32                    GetCoordDimension();
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
  */
	glbBool                     HasM();
/**
  *  @brief 置空对象
  */
	void                        Empty();
/**
  * @brief 设置几何类
  *        设置对象几何类有两种情况：
  *             1. 对象没有几何类
  *             2. 对象已有几何类
  *                    2.1 字段类型 与对象对应的字段值类型不同
  *                            对象的字段值会被清空
  *                    2.2 字段类型 与对象对应的字段值类型相同
  *                            对象的字段值保留.
  *                            这时的字段值也许是没意义的,调用者需要自行设置!!!
  *             3. ctype 只能是GLB_GEOCLASS_POINT
  *
  * @param gcls 几何类
  * @return 成功true，失败false
  */
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
/**
  * @brief 获取几何类
  *
  * @return 几何类
  */
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
/**
  * @brief 获取对象内环个数
  *
  *
  * @return 对象包含的内环的个数
  */
	glbInt32					GetInRingCount();
/**
  * @brief 获取对象的外环
  *
  *
  * @return 对象的外环
  */
	const CGlbLine*				GetExtRing();
/**
  * @brief 设置对象的外环
  *
  * @param ring 环对象
  * @return 成功true，失败false
  */
	glbBool						SetExtRing(CGlbLine* ring);
/**
  * @brief 给对象添加一个内环
  *
  * @param ring 环对象
  * @return 成功true，失败false
  */
	glbBool						AddInRing(CGlbLine* ring);
/**
  * @brief 添加一批内环
  *
  * @param count 数目
  * @param rings 内环
  * @return 成功true，失败false
  */
	glbBool                     AddInRings(CGlbLine** rings,glbInt32 count);
/**
  * @brief 从对象中获取索引位置的内环
  *
  * @param idx 要获取的索引位置
  * @return 获取的环对象
  */
	const CGlbLine*				GetInRing(glbInt32 idx);
/**
  * @brief 删除内环
  *
  * @param idx 索引位置
  * @return 成功true，失败false
  */
	glbBool						DeleteInRing(glbInt32 idx);
/**
  * @brief 删除内环
  *
  * @param ring 内环
  * @return 成功true，失败false
  */
    glbBool                     DeleteInRing(CGlbLine* ring);
/**
  * @brief 删除一批内环
  *
  * @param idx 要删除的索引位置
  * @param count 数目
  * @return 成功true，失败false
  */
	glbBool                     DeleteInRings(glbInt32 idx,glbInt32 count);
private:
	void						UpdateExtent();
private:
	glbref_ptr<CGlbLine>		mpr_extRing;	///< 外环
	glbref_ptr<CGlbLine>*		mpr_inRings;	///< 内环集合
	glbInt32					mpr_inCount;    ///< 内环个数
	glbref_ptr<CGlbExtent>		mpr_extent;		///< 外包	
	glbref_ptr<CGlbGeometryClass>	mpr_geometryclass;	///<点的几何属性字段几何
	glbByte                     mpr_coordDimension;  ///<坐标维度
	glbBool                     mpr_hasM; 
};

/**
  * @brief 多多边形
  *
  * 该类继承于IGlbGeometry
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbMultiPolygon:public IGlbGeometry
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbMultiPolygon(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	~CGlbMultiPolygon();
public:
/**
  * @brief 获取对象几何类型
  *
  * @return 几何类型
  */
	GlbGeometryTypeEnum				GetType();
/**
  * @brief 判断几何对象是否为空
  *
  * @return 空，返回true，不为空，返回false
  */
	glbBool							IsEmpty();
/**
  * @brief 获取对象外包
  *
  * @return 外包
  */
	const CGlbExtent*	     		GetExtent();
/**
  * @brief 获取所占用内存大小.
  */
	glbUInt32                       GetSize();
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  *  @brief 0 代表维数不定,只有CGlbGeometryCollect会返回0
  */
	glbInt32                        GetCoordDimension();
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
  *  @brief CGlbGeometryCollect 不能用该返回值确定其子对象是否带度量.
  *                             调用子对象的HasM来判断其是否带度量.
  */
	glbBool                         HasM();
/**
  *  @brief 置空对象
  */
	void                            Empty();
/**
  * @brief 设置几何类
  *        设置对象几何类有两种情况：
  *             1. 对象没有几何类
  *             2. 对象已有几何类
  *                    2.1 字段类型 与对象对应的字段值类型不同
  *                            对象的字段值会被清空
  *                    2.2 字段类型 与对象对应的字段值类型相同
  *                            对象的字段值保留.
  *                            这时的字段值也许是没意义的,调用者需要自行设置!!!
  *             3. ctype 只能是GLB_GEOCLASS_POINT
  *
  * @param gcls 几何类
  * @return 成功true，失败false
  */
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
/**
  * @brief 获取几何类
  *
  * @return 几何类
  */
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
/**
  * @brief 获取对象多边形个数
  *
  *
  * @return 对象包含的多边形个数
  */
	glbInt32						GetCount();
/**
  * @brief 添加一个多边形
  *
  * @param polygon 要添加的多边形
  * @return 成功true，失败false
  */
	glbBool							AddPolygon( CGlbPolygon*  polygon);
/**
  * @brief 添加一批多边形
  *
 */
	glbBool                         AddPolygons(CGlbPolygon** polygons,glbInt32 cnt);
/**
  * @brief 删除一个多边形
  *
  * @param idx 索引位置
  * @return 成功true，失败false
  */
	glbBool							DeletePolygon(glbInt32 idx);
/**
  * @brief 删除一个多边形
  *
  * @param polygon 多变形
  * @return 成功true，失败false
  */
	glbBool                         DeletePolygon(CGlbPolygon* polygon);
/**
  * @brief 删除一批多边形
  *
  */
	glbBool                         DeletePolygons(glbInt32 idx,glbInt32 cnt);
/**
  * @brief 获取一个多边形
  *
  * @param idx 要获取的索引位置
  * @return 获取的多边形对象
  */
	const CGlbPolygon*	  		    GetPolygon(glbInt32 idx);
private:
	void							UpdateExtent();
private:
	glbInt32						mpr_count;			///< 多边形个数
	glbref_ptr<CGlbPolygon>*	    mpr_polygons;		///< 多边形集合
	glbref_ptr<CGlbExtent>			mpr_extent;			///< 外包
	glbref_ptr<CGlbGeometryClass>   mpr_geocls;
	glbByte                         mpr_coordDimension; ///<坐标维度
	glbBool                         mpr_hasM;  
};
