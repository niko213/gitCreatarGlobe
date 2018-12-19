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
  * @brief ��Ԫ��
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
	// ����i,j,kλ��
	void SetIndex(int i, int j, int k);
	// ����xyƽ���ϵ������ǵ��Ӧ��(x0,y0),(x1,y1)
	void SetXY(double x[2], double y[2]);
	// ��ijk�����8�����zֵ
	void SetZ(double z[8]);
	// ������Чֵ
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
	//xy�����ϵ��ĸ���ֱ��� m_x��m_y����ϣ��ĸ���ֱ��ǣ�m_x[0],m_y[0]��,��m_x[1],m_y[0]��,��m_x[0],m_y[1]��,��m_x[1],m_y[1]��
	double m_x[2];
	double m_y[2];
	double m_z[8];//˳������ijk����
};

/**
  * @brief ����
  *
  * ����̳���IGlbGeometry���ö������һϵ�е�����͵��Ӧ��n������ֵ���
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
	  * @brief ��ȡ�������
	  *
	  * @return ���
	  */
	const CGlbExtent*				GetExtent();
	/**
	  * @brief ��ȡ���󼸺�����
	  *
	  * @return ��������
	  */
	GlbGeometryTypeEnum				GetType();
	/**
	  * @brief �жϼ��ζ����Ƿ�Ϊ��
	  *
	  * @return �գ�����true����Ϊ�գ�����false
	  */
	glbBool							IsEmpty();
	glbUInt32                       GetSize();
	/**
	  *  @brief ��ȡ����γ��:
	  *  @brief 2 �����ά����X,Y
	  *  @brief 3 ������ά����X,Y,Z
	  */
	glbInt32                        GetCoordDimension();
	/**
	  *  @brief �Ƿ������ֵ
	  *  @brief true ������,false������
	  */
	glbBool                         HasM();
	/**
	  *  @brief �ÿն���
	  */
	void                            Empty();
	/**
	  * @brief ���ü�����
	  *        ���ö��󼸺��������������
	  *             1. ����û�м�����
	  *             2. �������м�����
	  *                    2.1 �ֶ����� ������Ӧ���ֶ�ֵ���Ͳ�ͬ
	  *                            ������ֶ�ֵ�ᱻ���
	  *                    2.2 �ֶ����� ������Ӧ���ֶ�ֵ������ͬ
	  *                            ������ֶ�ֵ����.
	  *                            ��ʱ���ֶ�ֵҲ����û�����,��������Ҫ��������!!!
	  *             3. ctype ֻ����GLB_GEOCLASS_POINT
	  *
	  * @param gcls ������
	  * @return �ɹ�true��ʧ��false
	  */
	glbBool             SetGeoClass(GlbGeometryClassEnum ctype,CGlbGeometryClass* gcls);
	/**
	  * @brief ��ȡ������
	  *
	  * @return ������
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
	glbref_ptr<CGlbExtent>			mpr_extent;			///<���
};

