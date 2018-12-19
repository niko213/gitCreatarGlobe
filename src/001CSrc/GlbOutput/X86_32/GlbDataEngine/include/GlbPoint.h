#pragma once
#pragma warning(disable:4251)
#include <vector>
#include "IGlbGeometry.h"
using namespace std;

/**
  * @brief 点
  *
  * 该类继承于IGlbGeometry
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbPoint :public IGlbGeometry
{
public:
/**
  * @brief 构造函数
  *
  * @param x 坐标值x
  * @param y 坐标值y
  */
	CGlbPoint(glbDouble x,glbDouble y,glbBool hasM=false);
/**
  * @brief 构造函数
  *
  * @param x 坐标值x
  * @param y 坐标值y
  * @param z 坐标值z
  */
	CGlbPoint(glbDouble x,glbDouble y,glbDouble z,glbBool hasM=false);
/**
  * @brief 析构函数
  *
  * 没有任何参数及传回值
  */
	virtual~CGlbPoint(void);
public:
/**
  * @brief 获取点x坐标值
  *
  *
  * @param x 坐标值x
  * @return 成功true，失败false
  */
	glbBool				GetX(glbDouble* x);
/**
  * @brief 获取点y坐标值
  *
  *
  * @param y 坐标值y
  * @return 成功true，失败false
  */
	glbBool				GetY(glbDouble* y);
  /**
  * @brief 获取点z坐标值
  *
  *
  * @param z 坐标值z
  * @return 成功true，失败false
  */
	glbBool				GetZ(glbDouble* z);
/**
  * @brief 获取度量值
  *
  *
  * @param m 度量值
  * @return 成功true，失败false
  */
	glbBool				GetM(glbDouble* m);
/**
  * @brief 设置点x坐标值
  *
  *
  * @param x 坐标值x
  * @return 成功true，失败false
  */
	glbBool				SetX(glbDouble x);
/**
  * @brief 设置点y坐标值
  *
  *
  * @param y 坐标值y
  * @return 成功true，失败false
  */
	glbBool				SetY(glbDouble y);
/**
  * @brief 设置点z坐标值
  *
  *
  * @param z 坐标值z
  * @return 成功true，失败false
  */
	glbBool				SetZ(glbDouble z);
/**
  * @brief 设置点度量值
  *
  *
  * @param m 度量值m
  * @return 成功true，失败false
  */
	glbBool				SetM(glbDouble m);
/**
  * @brief 获取点x,y坐标值
  *
  *
  * @param x 坐标值x
  * @param y 坐标值y
  * @return 成功true，失败false
  */
	glbBool				GetXY(glbDouble* x,glbDouble* y);
/**
  * @brief 获取点x,y,z坐标值
  *
  *
  * @param x 坐标值x
  * @param y 坐标值y
  * @param z 坐标值z
  * @return 成功true，失败false
  */
	glbBool				GetXYZ(glbDouble* x,glbDouble* y,glbDouble* z);
/**
  * @brief 设置点x,y,z坐标值
  *
  *
  * @param x 坐标值x
  * @param y 坐标值y
  * @param z 坐标值z
  * @return 成功true，失败false
  */
	glbBool				SetXYZ(glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief 设置点x,y坐标值
  *
  *
  * @param x 坐标值x
  * @param y 坐标值y
  * @return 成功true，失败false
  */
	glbBool				SetXY(glbDouble x,glbDouble y);
public:
/**
  * @brief 获取对象外包
  *
  * @return 外包
  */
	CGlbExtent*			GetExtent();
/**
  * @brief 获取对象几何类型
  *
  * @return 几何类型
  */
	GlbGeometryTypeEnum GetType();
/**
  * @brief 判断几何对象是否为空
  *
  * @return 空，返回true，不为空，返回false
  */
	glbBool				IsEmpty();
/**
  * @brief 获取所占用内存大小.
  */
	glbUInt32           GetSize();
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  */
	glbInt32            GetCoordDimension();
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
  */
	glbBool             HasM();
/**
  *  @brief 置空对象
  */
	void                Empty();
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
private:
	glbDouble				mpr_x;				///<点x坐标
	glbDouble				mpr_y;				///<点y坐标
	glbDouble               mpr_z;              ///<点z坐标
	glbDouble               mpr_m;              ///<度量值
	glbByte                 mpr_coordDimension; ///<坐标维度
	glbBool                 mpr_hasM;           ///<是否带度量
};
/*
   多点
*/
class GLBDATAENGINE_API CGlbMultiPoint:public IGlbGeometry
{
public:
	CGlbMultiPoint(glbByte coordDimension = 2,glbBool hasM = false);
	~CGlbMultiPoint();
/**
  * @brief 获取对象几何类型
  *
  * @return 几何类型
  */
	GlbGeometryTypeEnum GetType();
/**
  * @brief 判断几何对象是否为空
  *
  * @return 空，返回true，不为空，返回false
  */
	glbBool             IsEmpty();
/**
  * @brief 获取几何对象外包
  *
  * @return 外包
  */
	const CGlbExtent*   GetExtent();
/**
  * @brief 获取所占用内存大小.
  */
	glbUInt32           GetSize();
/**
  *  @brief 获取坐标纬度:
  *  @brief 2 代表二维，有X,Y
  *  @brief 3 代表三维，有X,Y,Z
  *  @brief 0 代表维数不定,只有CGlbGeometryCollect会返回0
  */
	glbInt32            GetCoordDimension();
/**
  *  @brief 是否带度量值
  *  @brief true 代表有,false代表无
  *  @brief CGlbGeometryCollect 不能用该返回值确定其子对象是否带度量.
  *                             调用子对象的HasM来判断其是否带度量.
  */
	glbBool             HasM();
/**
  *  @brief 置空对象
  */
	void                Empty();
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
/**
  * @brief 获取对象点个数
  *
  *
  * @return 对象包含的点个数
  */
	glbInt32			GetCount();
/**
  * @brief 获取对象指定索引位置的点x,y坐标
  *
  * @param idx 要获取点的索引下标
  * @param ptx x坐标
  * @param pty y坐标
  * @return 成功true，失败false
  */
	glbBool				GetPoint(glbInt32 idx,glbDouble* ptx,glbDouble* pty);
/**
  * @brief 获取对象指定索引位置的点x,y,z坐标
  * @brief 如果是二维多点,因为没有z坐标，会失败.
  *
  * @param idx 要获取点的索引下标
  * @param ptx x坐标
  * @param pty y坐标
  * @param pty z坐标
  * @return 成功true，失败false
  */	
	glbBool				GetPoint(glbInt32 idx,glbDouble* ptx,glbDouble* pty,glbDouble* ptz);
/**
  * @brief 给对象添加一个点
  * @brief 如果是三维多点,z赋值为 0
  * @brief 如果带度量值,度量值赋值为0
  *
  * @param x 添加点x坐标
  * @param y 添加点y坐标
  * @return 成功true，失败false
  */
	glbBool				AddPoint(glbDouble x,glbDouble y);
/**
  * @brief 添加一个点
  * @brief 如果是二维多点,z 会被忽略.
  * @brief 如果带度量值,度量值赋值为0
  * @param x 添加点x坐标
  * @param y 添加点y坐标
  * @param z 添加点z坐标
  * @return 成功true，失败false
  */
	glbBool				AddPoint(glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief 添加一批点
  * @brief 如果带度量值,度量值赋值为0
  *
  * @param coords 点的坐标列表
                  二维线,是XYXY...的序列,三维线是XYZXYZ...的序列
  * @param count  点的个数
  * @return 成功true，失败false
  */
	glbBool             AddPoints(glbDouble* coords,glbInt32 count);
/**
  * @brief 删除一个点
  *
  * @param idx 删除点的索引位置
  * @return 成功true，失败false
  */
	glbBool				DeletePoint(glbInt32 idx);
/**
  * @brief 删除一批点
  *
  * @param idx 删除点的索引位置
  * @param idx 点的个数
  * @return 成功true，失败false
  */
	glbBool				DeletePoints(glbInt32 idx,glbInt32 count);
/**
  * @brief 修改点坐标
  * @brief 如果是三维多点,z赋值为 0
  * @brief 如果带度量值,度量值赋值为0
  *
  * @param idx 要设置的索引位置
  * @param x 要设置的点的x坐标
  * @param y 要设置的点的y坐标
  * @return 成功true，失败false
  */
	glbBool				SetPoint(glbInt32 idx,glbDouble x,glbDouble y);
/**
  * @brief 修改点坐标
  * @brief 如果是二维多点,z 忽略
  * @brief 如果带度量值,度量值赋值为0

  * @param idx 要设置的索引位置
  * @param x 要设置的点的x坐标
  * @param y 要设置的点的y坐标
  * @param z 要设置的点的y坐标
  * @return 成功true，失败false
  */
	glbBool				SetPoint(glbInt32 idx,glbDouble x,glbDouble y,glbDouble z);	
/**
  * @brief 获取对象所有点坐标值列表
  * @brief 二维多点,是XYXY...的序列,三维多点是XYZXYZ...的序列
  *
  * @return 坐标值列表
  */
	const glbDouble*	GetPoints();
/**
  * @brief 获取点的度量值
  *
  * @param idx 要设置的索引位置
  * @param m  度量值
  * @return 成功true，失败false
  */
	glbBool             GetM(glbInt32 idx,glbDouble* m);
/**
  * @brief 设置点的度量值
  *
  * @param idx 要设置的索引位置
  * @param m   度量值
  * @return 成功true，失败false
  */
	glbBool             SetM(glbInt32 idx,glbDouble m);
/**
  * @brief 获取所有点的度量值
  *
  */
	const glbDouble*    GetMs();
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
  * @param fidx  字段索引
  * @param pidx  点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				GetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief  获取一个点字段值
  *        可能失败的原因:
  *            1.对象是Empty
  *            2.没有字段
  *            3.点索引<0 或 大于点数目
  *            4.字段索引 <0 或 >= 字段数目
  *            5.字段变化后,没有重新SetPointClass,造成:
                    5.1 字段数目变化
					5.2 字段类型变化
  *
  * @param fname  字段名字
  * @param pidx   点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				GetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
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
	glbBool				SetPointValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 设置一个点的几何字段值
  *
  * @param fname 要设置几何属性的字段名字
  * @param pidx 要设置的点索引位置
  * @param pvalue 要设置的值
  * @return 成功true，失败false
  */
	glbBool				SetPointValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 获取所有点的字段值
  *
  * @param fidx       字段索引
  * @param pvalues    字段值
  * @return 成功true，失败false
  */
	const glbArray*		GetPointsValue(glbInt32 fidx);
/**
  * @brief 获取所有点的字段值
  *
  * @param fname      字段名字
  * @param pvalues    字段值
  * @return 成功true，失败false
  */
	const glbArray*		GetPointsValueByName(glbWChar* fname);
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
  *            
  * @param fidx       几何字段索引
  * @param pvalues    值列表
  * @return 成功true，失败false
  */
	glbBool				SetPointsValue (glbInt32 fidx,glbArray& pvalues);
/**
  * @brief 设置字段值
  *
  * @param fname   字段名字
  * @param pvalues 值列表
  * @return 成功true，失败false
  */
	glbBool				SetPointsValueByName (glbWChar* fname,glbArray& pvalues);
private:
	void				UpdateExtent();
private:
	glbDouble*                    mpr_pointscoord;	  ///<点坐标列表
	glbInt32	                  mpr_pointscount;	  ///<点个数
	glbref_ptr<CGlbGeometryClass> mpr_geometryclass;  ///<属性字段集合
	vector<glbArray*>			  mpr_pointsvalue;	  ///<属性值向量列表
	glbref_ptr<CGlbExtent>        mpr_extent;		  ///<外包
	glbByte                       mpr_coordDimension; ///<坐标维度
	glbBool                       mpr_hasM;           ///<是否带度量
	glbDouble*                    mpr_m;
};
