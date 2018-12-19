#pragma once
#include <vector>
#include "GlbPoint.h"
#include "IGlbGeometry.h"
using namespace std;
/**
  * @brief 三角网对象
  *
  * 该类继承于IGlbGeometry，该对象包含多个三角形
  * 
  * @version 1.0
  * @date    2014-03-07 15:27
  * @author  GWB
  */
class GLBDATAENGINE_API CGlbTin :public IGlbGeometry
{
public:
	CGlbTin(glbBool hasM = false);
	~CGlbTin();
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
	GlbGeometryTypeEnum	GetType();
/**
  * @brief 判断几何对象是否为空
  *
  * @return 空，返回true，不为空，返回false
  */
	glbBool				IsEmpty();
/**
  * @brief 获取所占用内存大小.
  */
   glbUInt32            GetSize();
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
                                调用子对象的HasM来判断其是否带度量.
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
  *             3. ctype 只能是GLB_GEOCLASS_POINT,GLB_GEOCLASS_TRIANGLE
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
 //CGlbTin 方法
 public:
/**
  * @brief  获取点个数
  *
  *
  * @return 点个数
  */
	glbInt32			GetVertexCount();
/**
  * @brief     获取顶点的x,y,z坐标
  *
  * @param idx 点的索引下标
  * @param ptx x坐标
  * @param pty y坐标
  * @param ptz z坐标
  * @return 成功true，失败false
  */
	glbBool				GetVertex(glbInt32 idx,glbDouble* ptx,glbDouble* pty,glbDouble* ptz);
/**
  * @brief 添加一个顶点
  *
  * @param x 点的x坐标
  * @param y 点的y坐标
  * @param z 点的z坐标
  * @return 成功true，失败false
  */
	glbBool				AddVertex(glbDouble x,glbDouble y,glbDouble z);
/**
  * @brief 添加一批顶点
  *
  * @param coords 顶点坐标序列XYZXYZ...
  * @param count  数目
  * @return 成功true，失败false
*/
	glbBool             AddVertexes(glbDouble* coords,glbInt32 count);
/**
  * @brief 修改顶点坐标
  *
  * @param idx 点的索引
  * @param x   点的x坐标
  * @param y   点的y坐标
  * @param z   点的z坐标
  * @return 成功true，失败false
  */
	glbBool				SetVertex(glbInt32 idx,glbDouble x,glbDouble y, glbDouble z);
/**
  * @brief 删除一个顶点
  *
  * @param idx 点的索引
  * @return 成功true，失败false
  */
	glbBool				DeleteVertex(glbInt32 idx);
/**
  * @brief 删除一批顶点
  *
  * @param idx   点的索引
  * @param count 数目
  * @return 成功true，失败false
  */
	glbBool             DeleteVertexes(glbInt32 idx,glbInt32 count);
/**
  * @brief 获取对象顶点坐标列表
  *
  * @return 顶点坐标列表
  */
	const glbDouble*	GetVertexes();
/**
  * @brief 获取点的度量值
  *
  * @param idx 索引位置
  * @param m   度量值
  * @return 成功true，失败false
  */
	glbBool             GetM(glbInt32 idx,glbDouble* m);
/**
  * @brief 修改点的度量值
  *
  * @param idx 索引位置
  * @param m   度量值
  * @return 成功true，失败false
  */
	glbBool             SetM(glbInt32 idx,glbDouble m);
/**
  * @brief 获取度量序列
  */
	const glbDouble*    GetMs();
/**
  * @brief 获取三角形个数
  *
  * @return 三角形个数
  */
	glbInt32			GetTriangleCount();
/**
  * @brief 获取三角形
  *
  * @param idx          三角形的索引
  * @param first_ptidx  第一顶点索引
  * @param second_ptidx 第二顶点索引
  * @param third_ptidx  第三顶点索引
  * @return 成功true，失败false
  */
	glbBool				GetTriangle(glbInt32 idx,glbInt32* first_ptidx,glbInt32* second_ptidx,glbInt32* third_ptidx);
/**
  * @brief 添加一个三角形
  *
  * @param first_ptidx  第一顶点索引
  * @param second_ptidx 第二顶点索引
  * @param third_ptidx  第三顶点索引
  * @return 成功true，失败false
  */
	glbBool				AddTriangle(glbInt32 first_ptidx,glbInt32 second_ptidx,glbInt32 third_ptidx);
/**
  * @brief 添加一批三角形
  *
  * @param ptidxes  三角形顶点索引序列
  * @param count    数目
  * @return 成功true，失败false
  */
	glbBool				AddTriangles(glbInt32* ptidxes,glbInt32 count);
/**
  * @brief 修改三角形
  *
  * @param idx          三角形的索引
  * @param first_ptidx  第一顶点索引
  * @param second_ptidx 第二顶点索引
  * @param third_ptidx  第三顶点索引
  * @return 成功true，失败false
  */
	glbBool				SetTriangle(glbInt32 idx,glbInt32 first_ptidx,glbInt32 second_ptidx,glbInt32 third_ptidx);
/**
  * @brief 删除一个三角形
  *
  * @param idx  三角形的索引
  * @return 成功true，失败false
  */
	glbBool				DeleteTriangle(glbInt32 idx);
/**
  * @brief 删除一批三角形
  *
  * @param idx    三角形的索引
  * @param count  数目
  * @return 成功true，失败false
  */
	glbBool				DeleteTriangles(glbInt32 idx,glbInt32 count);
/**
  * @brief 获取三角形
  *
  * @return 三角形
  */
	const glbInt32*		GetTriangles();
/**
  * @brief 获取一个顶点的字段值
  *
  * @param fidx   字段索引
  * @param pidx   顶点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				GetVertexValue(glbInt32 fidx, glbInt32 pidx, GLBVARIANT& pvalue);
/**
  * @brief 获取一个顶点的字段值
  *
  * @param fname  字段名
  * @param pidx   顶点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				GetVertexValueByName(glbWChar* fname,glbInt32 pidx, GLBVARIANT& pvalue);
/**
  * @brief 修改一个顶点的字段值
  *
  * @param fidx   字段索引
  * @param pidx   顶点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				SetVertexValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 修改一个顶点的字段值
  *
  * @param name   字段名
  * @param pidx   顶点索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				SetVertexValueByName(glbWChar* name ,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 获取所有顶点字段值
  *
  * @param  fidx    字段索引
  * @return 字段值
  */
	const glbArray*		GetVertexesValue(glbInt32 fidx);
/**
  * @brief 获取所有顶点字段值
  *
  * @param fname   字段名
  * @return 字段值
  */
	const glbArray*		GetVertexesValueByName(glbWChar* fname);
/**
  * @brief 修改顶点字段值
  *
  * @param fidx    字段索引
  * @param pvalues 字段值
  * @return 成功true，失败false
  */
	glbBool				SetVertexesValue(glbInt32 fidx,glbArray& pvalues);
/**
  * @brief 修改顶点字段值
  *
  * @param fname   字段名
  * @param pvalues 字段值
  * @return 成功true，失败false
  */
	glbBool				SetVertexesValueByName(glbWChar* fname ,glbArray& pvalues);
/**
  * @brief 获取角形的字段值
  *
  * @param fidx   字段索引
  * @param pidx   三角形索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool 			GetTriangleValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 获取角形的字段值
  *
  * @param fname  字段名
  * @param pidx   三角形索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				GetTriangleValueByName(glbWChar* fname,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 修改三角形字段值
  *
  * @param fidx   字段索引
  * @param pidx   三角形索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				SetTriangleValue(glbInt32 fidx,glbInt32 pidx,GLBVARIANT& pvalue);
/**
  * @brief 修改三角形字段值
  *
  * @param fname  字段名
  * @param pidx   三角形索引
  * @param pvalue 值
  * @return 成功true，失败false
  */
	glbBool				SetTriangleValueByName(glbWChar* fname,glbInt32 pidx, GLBVARIANT& pvalue);
/**
  * @brief 获取三角形字段值
  *
  * @param fidx    字段索引
  * @return 字段值
  */
	const glbArray*		GetTrianglesValue(glbInt32 fidx);
/**
  * @brief 根据字段名获取对象三角形的指定索引字段的属性值列表
  *
  * @param fname 指定的字段名
  * @param pvalues 要获取的值
  * @return 成功true，失败false
  */
	const glbArray*		GetTrianglesValueByName(glbWChar* fname);
/**
  * @brief 修改三角形字段值
  *
  * @param fidx    字段索引
  * @param pvalues 值
  * @return 成功true，失败false
  */
	glbBool				SetTrianglesValue(glbInt32 fidx,glbArray& pvalues);
/**
  * @brief 修改三角形字段值
  *
  * @param fname   字段名
  * @param pvalues 值
  * @return 成功true，失败false
  */
	glbBool				SetTrianglesValueByName(glbWChar* fname, glbArray& pvalues);	
private:
	void				UpdateExtent();
	void                EmptyVertex();
	void                EmptyTriangle();
	glbBool				SetVertexClass(CGlbGeometryClass* gcls);
    glbBool				SetTriangleClass(CGlbGeometryClass* gcls);
private:
	glbInt32						mpr_vertexescount;			///< 顶点个数
	glbInt32						mpr_trianglecount;			///< 三角形个数
	glbref_ptr<CGlbGeometryClass>	mpr_vergeometryclass;		///< 顶点类
	glbref_ptr<CGlbGeometryClass>	mpr_trigeometryclass;		///< 三角形类
	glbDouble*						mpr_vertexcoord;			///< 顶点坐标列表
	glbInt32*						mpr_trivertex;				///< 三角形顶点索引，每三个点索引表示一个三角形
	vector<glbArray*>				mpr_verpvalue;				///< 顶点属性值列表
	vector<glbArray*>				mpr_tripvalue;				///< 三角形属性值列表
	glbref_ptr<CGlbExtent>			mpr_extent;					///< 外包	
	glbBool                         mpr_hasM;                   ///<是否带度量
	glbDouble*                      mpr_m;
	glbBool							mpr_isBody;
};

class GLBDATAENGINE_API CGlbMultiTin :public IGlbGeometry
{
public:
	CGlbMultiTin(glbBool hasM = false);
	~CGlbMultiTin();
	GlbGeometryTypeEnum GetType();
    glbBool             IsEmpty();
    const CGlbExtent*   GetExtent();
    glbUInt32           GetSize();
    glbInt32            GetCoordDimension();
    glbBool             HasM();
    void                Empty();
	glbBool             SetGeoClass
		                (GlbGeometryClassEnum ctype,
		                 CGlbGeometryClass* gcls);
	const CGlbGeometryClass* GetGeoClass(GlbGeometryClassEnum ctype);
public:
	/**
	* @brief 获取对象Tin个数
	*
	*
	* @return Tin个数
	*/
	glbInt32			GetCount();
	/**
	* @brief 添加一个Tin
	*
	* @param tin 要添加的Tin
	* @return 成功true，失败false
	*/
	glbBool				AddTin( CGlbTin* tin);
	/**
	* @brief 添加一批Tin
	*
	*/
	glbBool             AddTins(CGlbTin** tins,glbInt32 cnt);
	/**
	* @brief 删除一个Tin
	*
	* @param idx 索引位置
	* @return 成功true，失败false
	*/
	glbBool				DeleteTin(glbInt32 idx);
	/**
	* @brief 删除一批Tin
	*
	*/
	glbBool             DeleteTins(glbInt32 idx,glbInt32 cnt);
	/**
	* @brief 获取一个Tin
	*
	* @param  idx 索引位置
	* @return tin，失败返回NULL
	*/
	const CGlbTin*	    GetTin(glbInt32 idx);
private:
	void UpdateExtent();
private:
	glbInt32						mpr_count;			///< 多边形个数
	glbref_ptr<CGlbTin>*	        mpr_tins;		    ///< 多边形集合
	glbref_ptr<CGlbExtent>			mpr_extent;			///< 外包
	glbref_ptr<CGlbGeometryClass>	mpr_vertexcls;		///< 顶点类
	glbref_ptr<CGlbGeometryClass>	mpr_tricls;		    ///< 三角形类
	glbByte                         mpr_coordDimension; ///<坐标维度
	glbBool                         mpr_hasM; 
};