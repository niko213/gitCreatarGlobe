#pragma once
#include "IGlbGeometry.h"
#include "GlbCommTypes.h"
#include <string>
#include<fstream>
using namespace std;


class GLBDATAENGINE_API Square 
{
private:
	glbVector m_pts[4];
public:
	void SetFourPoints(glbVector pts[4]);
	void GetFourPoints(glbVector(&pts)[4]);
};

/**
  * @brief 单元格
  * 
  * 
  * @version 1.0
  * @date    2018-01-02 15:27
  * @author  ML
  */
class GLBDATAENGINE_API CGlbCell : public CGlbReference
{
public:
	CGlbCell();
	~CGlbCell();
	// 设置i,j,k位置
	void SetIndex(int i, int j, int k);
	// 设置xy平面上的两个角点对应的(x0,y0),(x1,y1)
	void SetXY(double x[2], double y[2]);
	// 沿ijk方向的8个点的z值
	void SetZ(double z[8]);
	// 设置有效值
	void SetValid(int i);

	void GetIndex(int & i, int &j, int & k);
	void GetXY(double(&x)[2], double(&y)[2]);
	void GetZ(double(&z)[8]);
	int GetValid();
private:
	int m_i;
	int m_j;
	int m_k;
	int m_isValid;
	//xy方向上的四个点分别是 m_x和m_y的组合，四个点分别是（m_x[0],m_y[0]）,（m_x[1],m_y[0]）,（m_x[0],m_y[1]）,（m_x[1],m_y[1]）
	double m_x[2];
	double m_y[2];
	double m_z[8];//顺序沿着ijk方向
};

/**
  * @brief 格网
  *
  * 该类继承于IGlbGeometry，该对象包含一系列点坐标和点对应的n个属性值组成
  * 
  * @version 1.0
  * @date     2018-01-02 15:27
  * @author  ML
  */
class GLBDATAENGINE_API CGlbGrid : public IGlbGeometry
{
public:
	CGlbGrid(void);
	CGlbGrid(int length, int width, int height);
	~CGlbGrid(void);

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
	void SetLength(int length) { m_length = length; }
	int GetLength() { return m_length; }

	void SetWidth(int width) { m_width = width; }
	int GetWidth() { return m_width; }

	void SetHeight(int height) { m_height = height; }
	int GetHeight() { return m_height; }

	void SetSize(int length, int width, int height);

	void PrintToFile(/*std::string*/const glbWChar* wpath);
	void ReadFromFile(/*std::string*/const glbWChar* wpath);
	CGlbCell GetCell(int x, int y, int z);
	void SetCell(CGlbCell c);	

	void GetSurface(std::vector<Square> & squares);
private:
	int m_length;
	int m_width;
	int m_height;

	double * m_x;
	double * m_y;
	double * m_z;

	int* m_isValid;
	glbref_ptr<CGlbExtent>			mpr_extent;			///<外包
};

