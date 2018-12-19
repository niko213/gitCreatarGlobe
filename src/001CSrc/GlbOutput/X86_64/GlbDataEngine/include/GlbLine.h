#pragma once
#include <vector>
#include "IGlbGeometry.h"
using namespace std;

/**
  * @brief 线
  *
  * 该类继承于IGlbGeometry，该对象包含一系列点坐标和点对应的n个属性值组成
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbLine:public IGlbGeometry
{
public:
/**
  * @brief 构造函数
  *
  * @param coordDimension 坐标维度,2代表有XY,3代表有XYZ
  * @param hasM           true 代表有度量值,false代表没有度量值
  */
	CGlbLine(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	~CGlbLine(void);
public:
/**
  * @brief 获取对象外包
  *
  * @return 外包
  */
	const CGlbExtent*				GetExtent();
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
	glbUInt32                       GetSize();
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  */
	glbInt32                        GetCoordDimension();
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
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
  * @brief 获取对象点个数
  *
  *
  * @return 对象包含的点个数
  */
	glbInt32						GetCount();
/**
  * @brief 判断线对象是否是闭环
  *
  *
  * @return 是：true，否：false
  */
	glbBool							IsClosed();
/**
  * @brief 添加一个点
  * @brief 如果是三维线,z赋值为 0
  * @brief 如果带度量值,度量值赋值为0
  * @param x 添加点x坐标
  * @param y 添加点y坐标
  * @return 成功true，失败false
  */
	glbBool							AddPoint(glbDouble x,glbDouble y) ;
/**
  * @brief 添加一个点
  * @brief 如果是二维线,z 会被忽略.
  * @brief 如果带度量值,度量值赋值为0
  * @param x 添加点x坐标
  * @param y 添加点y坐标
  * @param z 添加点z坐标
  * @return 成功true，失败false
  */
	glbBool                         AddPoint(glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief 插入一个点
  * @brief 如果是三维线,z赋值为 0
  * @brief 如果带度量值,度量值赋值为0
  *
  * @param idx 插入点的位置
  * @param x   插入点x坐标
  * @param y   插入点y坐标
  * @return 成功true，失败false
  */
	glbBool							InsertPoint(glbInt32 idx,glbDouble x,glbDouble y);
/**
  * @brief 插入一个点
  * @brief 如果是二维线,z 会被忽略
  * @brief 如果带度量值,度量值赋值为0
  *
  * @param idx 插入点的位置
  * @param x   插入点x坐标
  * @param y   插入点y坐标
  * @param z   插入点z坐标
  * @return 成功true，失败false
  */
	glbBool							InsertPoint(glbInt32 idx,glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief 插入一批点
  * @brief 如果带度量值,度量值赋值为0
  *
  * @param coords 点的坐标列表
                  二维线,是XYXY...的序列,三维线是XYZXYZ...的序列
  * @param count  点的个数
  * @return 成功true，失败false
  */
	glbBool							InsertPoints(glbInt32 idx,glbDouble* coords,glbInt32 count);
/**
  * @brief 删除一个点
  *
  * @param idx 删除点的索引位置
  * @return 成功true，失败false
  */
	glbBool							DeletePoint(glbInt32 idx);
/**
  * @brief 删除一批点
  *
  * @param idx 删除点的索引位置
  * @param idx 点的个数
  * @return 成功true，失败false
  */
	glbBool							DeletePoints(glbInt32 idx,glbInt32 count);
/**
  * @brief 获取对象指定索引位置的点x,y坐标
  *
  * @param idx 要获取点的索引下标
  * @param ptx x坐标
  * @param pty y坐标
  * @return 成功true，失败false
  */
	glbBool							GetPoint(glbInt32 idx,glbDouble*ptx,glbDouble* pty);
/**
  * @brief 获取对象指定索引位置的点x,y,z坐标
  * @brief 如果是二维线,因为没有z坐标，会失败.
  *
  * @param idx 要获取点的索引下标
  * @param ptx x坐标
  * @param pty y坐标
  * @param pty z坐标
  * @return 成功true，失败false
  */
	glbBool							GetPoint(glbInt32 idx,glbDouble*ptx,glbDouble* pty,glbDouble *ptz);
/**
  * @brief 设置对象指定索引位置点的坐标
  * @brief 如果是三维线,z赋值为 0
  * @brief 如果带度量值,度量值赋值为0
  *
  * @param idx 要设置的索引位置
  * @param x 要设置的点的x坐标
  * @param y 要设置的点的y坐标
  * @return 成功true，失败false
  */
	glbBool							SetPoint(glbInt32 idx,glbDouble x,glbDouble y);
/**
  * @brief 设置对象指定索引位置点的坐标
  * @brief 如果是二维维线,z 忽略
  * @brief 如果带度量值,度量值赋值为0

  * @param idx 要设置的索引位置
  * @param x 要设置的点的x坐标
  * @param y 要设置的点的y坐标
  * @param z 要设置的点的y坐标
  * @return 成功true，失败false
  */
	glbBool							SetPoint(glbInt32 idx,glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief 获取坐标值序列
  * @brief 二维线,是XYXY...的序列,三维线是XYZXYZ...的序列
  *
  * @return 坐标值列表
  */
	const glbDouble*				GetPoints();
/**
  * @brief 获取点的度量值
  *
  * @param idx 要设置的索引位置
  * @param m  度量值
  * @return 成功true，失败false
  */
	glbBool                         GetM(glbInt32 idx,double* m);
/**
  * @brief 设置点的度量值
  *
  * @param idx 要设置的索引位置
  * @param m   度量值
  * @return 成功true，失败false
  */
	glbBool                         SetM(glbInt32 idx,double m);
/**
  * @brief 获取度量序列
  */
	const glbDouble*                GetMs();
/**
  * @brief 获取一个点字段值
  *        可能失败的原因:
  *            1.对象是Empty
  *            2.没有字段
  *            3.点索引<0 或 大于点数目
  *            4.字段索引 <0 或 >= 字段数目
  *            5.字段变化后,没有重新SetPointClass,造成:
                    5.1 字段数目变化
					5.2 字段类型变化
  *
  * @param fidx   字段索引
  * @param pidx   点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool							GetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 设置一个点字段值
  *        可能失败的原因:
  *            1.对象是Empty
  *            2.没有字段
  *            3.点索引<0 或 大于点数目
  *            4.字段索引 <0 或 >= 字段数目
  *            5.字段变化后,没有重新SetPointClass,造成:
                    5.1 字段数目变化
					5.2 字段类型变化
  *            6.值类型与字段类型不一致
  *
  * @param fidx   字段索引
  * @param pidx   点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool							SetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 获取点的几何字段值
  *
  * @param fname  字段名字
  * @param pidx   点索引
  * @param pvalue 要获取的值
  * @return 成功true，失败false
  */
	glbBool							GetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 设置点的几何字段值
  *
  * @param fname  字段名字
  * @param pidx   点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool							SetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 获取所有点的字段值
  *
  * @param fidx       字段索引
  * @param pvalues    字段值
  * @return 成功true，失败false
  */
	const glbArray*					GetPointsValue(glbInt32 fidx);
/**
  * @brief 获取所有点的字段值
  *
  * @param fname      字段名字
  * @param pvalues    字段值
  * @return 成功true，失败false
  */
	const glbArray*					GetPointsValueByName(glbWChar* fname);
/**
  * @brief 设置字段值
  *        可能失败的原因:
  *            1.对象是Empty
  *            2.没有字段
  *            3.字段索引 <0 或 >= 字段数目
  *            4.字段变化后,没有重新SetPointClass,造成:
                    4.1 字段数目变化
					4.2 字段类型变化
  *            5.值类型与字段类型不一致
  *            6.值的dims != 1
  *            7.值的dimSize !=0 或 dimSize != 点数目
  * @param fidx    字段索引
  * @param pvalues 字段值
  * @return 成功true，失败false
  */
	glbBool							SetPointsValue(glbInt32 fidx,glbArray& pvalues);
/**
  * @brief 设置字段值
  *
  * @param fname   字段名字
  * @param pvalues 值列表
  * @return 成功true，失败false
  */
	glbBool							SetPointsValueByName(glbWChar* fname,glbArray& pvalues);
private:
	void							UpdateExtent();
private:	
	glbDouble*						mpr_pointscoord;	///<点坐标列表
	glbInt32						mpr_pointscount;	///<点个数
	glbref_ptr<CGlbGeometryClass>	mpr_geometryclass;	///<点的几何属性字段几何
	vector<glbArray*>				mpr_pointsvalue;	///<点的几何属性值列表
	glbref_ptr<CGlbExtent>			mpr_extent;			///<外包
	glbByte                         mpr_coordDimension; ///<坐标维度
	glbBool                         mpr_hasM;           ///<是否带度量
	glbDouble*                      mpr_m;
 };

 /**
  * @brief 多线
  *
  * 该类继承于IGlbGeometry
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbMultiLine:public IGlbGeometry
{
public:
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	CGlbMultiLine(glbByte coordDimension = 2,glbBool hasM = false);
/**
  * @brief 构造函数
  *
  * 没有任何参数及传回值
  */
	~CGlbMultiLine();
public:
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
  * @brief 获取对象外包
  *
  * @return 外包
  */
	const CGlbExtent*			GetExtent();
/**
  * @brief 获取所占用内存大小.
  */
	glbUInt32                   GetSize();
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  *  @brief 0 代表维数不定,只有CGlbGeometryCollect会返回0
  */
	glbInt32                    GetCoordDimension();
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
  *  @brief CGlbGeometryCollect 不能用该返回值确定其子对象是否带度量.
  *                             调用子对象的HasM来判断其是否带度量.
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
  * @brief 获取线数目
  *
  *
  * @return 线数目
  */
	glbInt32					GetCount();
/**
  * @brief 获取一条线
  *
  * @param idx 线索引
  * @return 线
  */
	const CGlbLine*				GetLine(glbInt32 idx);
/**
  * @brief 添加一条线
  *
  * @param curve 线
  * @return 成功返回true，失败返回false
  */
	glbBool						AddLine( CGlbLine* curve);
/**
  *@brief 添加一批线
*/
	glbBool                     AddLines(CGlbLine** curves,glbInt32 cnt);
/**
  * @brief 删除一条线
  *
  * @param idx 线索引
  * @return 成功返回true，失败返回false
  */
	glbBool						DeleteLine(glbInt32 idx);
/**
  * @brief 删除一条线
  *
  * @param line 线
  * @return 成功返回true，失败返回false
  */
	glbBool                     DeleteLine(CGlbLine* line);
/**
  * @brief 删除一批线
  *
  * @param idx  线索引
  * @param cnt  线数目
  * @return 成功返回true，失败返回false
  */
	glbBool                     DeleteLines(glbInt32 idx,glbInt32 cnt);
private:
	void						UpdateExtent();
private:	
	glbInt32                         mpr_count;
	glbref_ptr<CGlbLine>*            mpr_curves;			///<线对象列表
	glbref_ptr<CGlbExtent>		     mpr_extent;			///<外包
	glbref_ptr<CGlbGeometryClass>    mpr_geocls;
	glbByte                          mpr_coordDimension; ///<坐标维度
	glbBool                          mpr_hasM;  
};

