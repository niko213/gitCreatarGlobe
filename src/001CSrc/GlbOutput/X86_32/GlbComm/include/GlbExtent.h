/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbExtent.h
  * @brief   �������ͷ�ļ�
  *
  * �����������CGlbExtent�����
  *
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
*********************************************************************/
#pragma once
#include "GlbCommExport.h"
#include "GlbReference.h"

/**
  * @brief CGlbExtent���������
  *
  * �̳����ü������������û��ȡ���ֵ
  * 					 
  * @version 1.0
  * @author  HJZ
  * @date    2014-3-10 14:40
  */
class GLBCOMM_API  CGlbExtent:public CGlbReference
{
public:
/**
  * @brief ���캯��
  *
  * ��ʼ����Ա����Ϊ0
  *
  * @return ��
  */
	CGlbExtent(void);
/**
  * @brief ���캯��
  *
  * ���ݴ���ֵ��ʼ����Ա����
  *
  * @param minX ��Сxֵ
  * @param maxX ���xֵ
  * @param minY ��Сyֵ
  * @param maxY ���yֵ
  * @param minZ ��Сzֵ
  * @param maxZ ���zֵ
  * @return ��
  */
	CGlbExtent(const glbDouble minX,
		       const glbDouble maxX,
               const glbDouble minY, 
			   const glbDouble maxY,
               const glbDouble minZ=0,
			   const glbDouble maxZ=0);
/**
  * @brief ��������
  *
  * @return ��
  */
	virtual ~CGlbExtent(void){}

/**
  * @brief �Ƿ���Ч
  *
  * �ж�����Ƿ���Ч
  *
  * @return ��Ч����true����Ч����false
  */
	glbBool   IsValid()const;
/**
  * @brief x������
  *
  * ��ȡx������
  *
  * @return ���ؿ��
  */
	glbDouble GetXWidth()const;
/**
  * @brief y����߶�
  *
  * ��ȡy����߶�
  *
  * @return ���ظ߶�
  */
	glbDouble GetYHeight()const;
/**
  * @brief z���򳤶�
  *
  * ��ȡz���򳤶�
  *
  * @return ���س���
  */
	glbDouble GetZLengh()const;
/**
  * @brief ��ȡ��С���ֵ
  *
  * ��ȡ��С���ֵ
  *
  * @param x ������� ��Сxֵ
  * @param y ������� ��Сyֵ
  * @param z ������� ��Сzֵ
  * @return ��
  */
	void      GetMin(glbDouble *x,
		             glbDouble *y,
				     glbDouble *z=0)const;
/**
  * @brief ������С���ֵ
  *
  * ������С���ֵ
  *
  * @param x ��Сxֵ
  * @param y ��Сyֵ
  * @param z ��Сzֵ
  * @return ��
  */
	void      SetMin(const glbDouble x,
		             const glbDouble y,
					 const glbDouble z=0);
/**
  * @brief ��ȡ������ֵ
  *
  * ��ȡ��С���ֵ
  *
  * @param x ������� ���xֵ
  * @param y ������� ���yֵ
  * @param z ������� ���zֵ
  * @return ��
  */
	void      GetMax(glbDouble* x, 
		             glbDouble* y,
					 glbDouble* z=0)const;
/**
  * @brief ����������ֵ
  *
  * ����������ֵ
  *
  * @param x ���xֵ
  * @param y ���yֵ
  * @param z ���zֵ
  * @return ��
  */
	void      SetMax(const glbDouble x, 
		             const glbDouble y,
					 const glbDouble z=0);
/**
  * @brief ��ȡ������ĵ�
  *
  * ��ȡ������ĵ�
  *
  * @param x ������� ����xֵ
  * @param y ������� ����yֵ
  * @param z ������� ����zֵ
  * @return ��
  */
	void      GetCenter(glbDouble* x, 
		                glbDouble* y,
						glbDouble* z=0)const;
/**
  * @brief ����
  *
  * �����Ƿ�����ڵ�ǰ���
  *
  * @param x xֵ
  * @param y yֵ
  * @param z zֵ
  * @return ��������true������������false
  */
	glbBool   Contains(const glbDouble x, 
		                const glbDouble y,
						const glbDouble z =0)const;
/**
  * @brief ����
  *
  * ����Ƿ�����ڵ�ǰ���
  *
  * @param other ���ֵ
  * @return ��������true������������false
  */
    glbBool   Contain(const CGlbExtent& other)const;
/**
  * @brief �ϲ�
  *
  * ������͵�ǰ����ϲ�
  *
  * @param other ���ֵ
  * @return ��
  */
	void      Merge(const CGlbExtent& other);
/**
  * @brief �ϲ�
  *
  * �ѵ�͵�ǰ����ϲ�
  *
  * @param x xֵ
  * @param y yֵ
  * @param z zֵ
  * @return ��
  */
    void      Merge(const glbDouble x,
		            const glbDouble y,
					const glbDouble z=0);
/**
  * @brief �ཻ
  *
  * ����Ƿ�͵�ǰ����ཻ
  *
  * @param other ���ֵ
  * @return ��������true������������false
  */
    CGlbExtent* Intersect(const CGlbExtent& other,bool useZ=true)const;
/**
  * @brief ��ȡ���ֵ
  *
  * ��ȡ���ֵ
  *
  * @param minX ������� ��Сxֵ
  * @param maxX ������� ���xֵ
  * @param minY ������� ��Сyֵ
  * @param maxY ������� ���yֵ
  * @param minZ ������� ��Сzֵ
  * @param maxZ ������� ���zֵ
  * @return ��
  */
    void      Get(glbDouble *minX, 
		          glbDouble *maxX,
                  glbDouble *minY,
				  glbDouble *maxY,
                  glbDouble *minZ=0,
				  glbDouble *maxZ=0)const;
/**
  * @brief �������ֵ
  *
  * �������ֵ
  *
  * @param minX ��Сxֵ
  * @param maxX ���xֵ
  * @param minY ��Сyֵ
  * @param maxY ���yֵ
  * @param minZ ��Сzֵ
  * @param maxZ ���zֵ
  * @return ��
  */
    void      Set(const glbDouble minX, 
		          const glbDouble maxX,
                  const glbDouble minY,
				  const glbDouble maxY,
                  const glbDouble minZ=0,
				  const glbDouble maxZ=0);

/**
  * @brief ƽ��
  *
  *@param dx X��ƽ����
  *@param dy Y��ƽ����
  *@param dz Z��ƽ����
  */
	void Translate(const glbDouble dx,
		           const glbDouble dy,
				   const glbDouble dz);
/**
  * @brief ����Ƿ���ͬ
  *
  * ������==���أ�����Ƿ���ͬ
  *
  * @param toExtent ���ֵ
  * @return ��ͬ����true����ͬ����false
  */
	glbBool operator==(const CGlbExtent& toExtent)const;
/**
  * @brief ����Ƿ���ͬ
  *
  * ������!=���أ�����Ƿ���ͬ
  *
  * @param toExtent ���ֵ
  * @return ����ͬ����true����ͬ����false
  */
	glbBool operator!=(const CGlbExtent& toExtent)const;

	glbDouble GetLeft()const{ return mpr_minVal[0]; }
	glbDouble GetTop()const{ return mpr_maxVal[1]; }
	glbDouble GetRight()const{ return mpr_maxVal[0]; }
	glbDouble GetBottom()const{ return mpr_minVal[1]; }
private:
	glbDouble mpr_minVal[3];///<�����Сֵ
	glbDouble mpr_maxVal[3];///<������ֵ
};
