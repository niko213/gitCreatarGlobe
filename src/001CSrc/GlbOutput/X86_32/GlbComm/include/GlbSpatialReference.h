/********************************************************************
  * Copyright (c) 2013 ������ά������Ϣ�������޹�˾
  * All rights reserved.
  *
  * @file    GlbSpatialReference.h
  * @brief   �ռ�ο�ϵͷ�ļ�
  *
  *
  * @version 1.0
  * @author  LJH
  * @date    2014-3-13 14:40
*********************************************************************/
#pragma once
#include "GlbReference.h"
#include "GlbCommTypes.h"
class GLBCOMM_API CGlbSpatialReference : public CGlbReference
{
	friend class CGlbCoordTrans;
protected:
	//explicit ������ʽת��
	explicit CGlbSpatialReference(void* handle);
public:
	
	~CGlbSpatialReference(void);
public:
	/**
		* @brief ����WKT��ʽ�Ŀռ�ο��ַ����������ռ�ο�ϵ
		*
		* @param wkt WKT��ʽ�Ŀռ�ο��ַ���.		
		* @return ʧ�ܷ���NULL
		*/
	static CGlbSpatialReference* CreateFromWKT(const glbWChar* wkt);
	/**
		* @brief ����Proj4��ʽ�Ŀռ�ο��ַ����������ռ�ο�ϵ
		*
		* @param proj4 Proj4��ʽ�Ŀռ�ο��ַ���.		
		* @return ʧ�ܷ���NULL
		*/
	static CGlbSpatialReference* CreateFromProj4(const glbWChar* proj4);
	/**
		* @brief ����Esri��ʽ�Ŀռ�ο��ַ����������ռ�ο�ϵ
		*
		* @param esri Esri��ʽ�Ŀռ�ο��ַ���.		
		* @return ʧ�ܷ���NULL
		*/
	static CGlbSpatialReference* CreateFromEsriFile(const glbWChar* esri);
	/**
		* @brief ���ݹ��ʱ�׼���룬�����ռ�ο�ϵ
		*
		* @param epsgCode �ռ�ο��Ĺ��ʱ���.		
		* @return ʧ�ܷ���NULL
		*/
	static CGlbSpatialReference* CreateFromEPSG (glbInt32 epsgCode);
	/**
		* @brief ���ݹ㷺ʹ��Լ���׳ɵ����ƣ������ռ�ο�ϵ
		*
		* @param wellknown �ռ�ο�������.		
		* @return ʧ�ܷ���NULL
		*/
	static CGlbSpatialReference* CreateFromWellKnown(const glbWChar* wellknown);
	/**
		* @brief ����Ozi�淶�������ռ�ο�ϵ
		*
		* @param pszDatum ��������
		* @param pszProj  ͶӰ����
		* @param pszProjParms ͶӰ����
		* @return ʧ�ܷ���NULL
		*/
	static CGlbSpatialReference* CreateFromOzi(const glbWChar *pszDatum,
		                                       const glbWChar *pszProj,
											   const glbWChar *pszProjParms
											   );
	/**
		* @brief �ռ�ο����ΪWKT��ʽ
		*		
		* @return ʧ�ܷ���NULL
		*/
	glbWChar* ExportToWKT();
	/**
	 * @brief �ж��Ƿ��ǵ�������ϵ:��������ϵ���ο�ƽ����� �����档���굥λ:��γ��
	 * @brief ������ͨ���õ�Ĵ������������ʼ���������֮��ļн�
	 * @brief γ����ͨ���õ�ķ���������ļн�
	 * @brief ���ǵ�����ط��ߵ��ο�������ľ���
	 * @return �Ƿ���true���񷵻�false
	 */
	glbBool IsGeographic();
	/**
	 * @brief �ж��Ƿ��ǵ�������ϵ:�Ե�������Ϊԭ�㽨���Ŀռ�ֱ������ϵ��
	 * @brief ������������������غϵĵ���������Ϊ��׼���������Ĵ������ϵ.
	 * @brief ��Ϊ�����Ŀռ�ֱ������ϵ�����Ĵ������ϵ
	 * @brief WGS84
	 *
	 * @return �Ƿ���true���񷵻�false
	 *
	 */
	glbBool IsGeocentric();
	/**
	 * @brief �ж��Ƿ���ͶӰ����ϵ
	 *
	 * @return �Ƿ���true���񷵻�false
	 *
	 */
	glbBool IsProjected();
	/**
	 * @brief �ж��Ƿ��Ǹ߳�����ϵ
	 *
	 * @return �Ƿ���true���񷵻�false
	 *
	 */
	glbBool IsVertical();
	/**
	  * @brief ��ȡ��ػ�׼������
	  *
	  * @return û�з���NULL
	  */
	glbWChar* GetDatum();
	/**
	  * @brief ��ȡ�ο�����������
	  *
	  * @return û�з���NULL
	  */
	glbWChar* GetSpheroid();
	/**
	  * @brief ��ȡ�ο������峤��뾶
	  *
	  * @return ����뾶,ʧ�ܷ���0.0;
	  */
	glbDouble GetSemiMajor();
	/**
	  * @brief ��ȡ�ο����������뾶
	  *
	  * @return ����뾶,ʧ�ܷ���0.0;
	  */
	glbDouble GetSemiMinor();
	/**
	  * @brief �ж��Ƿ�����ͬ�Ŀռ�ο�
	  *
	  * @return ��ͬ����true����ͬ����false;
	  */
	glbBool IsSame(CGlbSpatialReference* other);

private:
	void* mpr_handle;
};

class GLBCOMM_API CGlbCoordTrans : public CGlbReference
{
public:	
	CGlbCoordTrans();
	~CGlbCoordTrans();
	/**
	  * @brief ��ʼ��
	  *
	  * @param poSrc Դ�ռ�ο�
	  * @param poTarget Ŀ��ռ�ο�
	  *
	  * @return ����poSrc����poTarget��NULL������false
	  */
	glbBool Initialize(CGlbSpatialReference* poSrc,CGlbSpatialReference* poTarget);
	/**
	  * @brief �������Դ�ռ�ο� ת�� ΪĿ��ռ�ο��µ�ֵ
	  *
	  * @param x Դ�ռ�ο�X������
	  * @param y Դ�ռ�ο�Y������
	  * @param z Դ�ռ�ο�Z������
	  * @param outx ת����Ŀ��ռ�ο�X������
	  * @param outy ת����Ŀ��ռ�ο�Y������
	  * @param outz ת����Ŀ��ռ�ο�Z������
	  *
	  * @return �ɹ�����true��û�г�ʼ����ʧ�ܷ���false
	  */
	glbBool Transform(glbDouble x, glbDouble y, glbDouble z,glbDouble *outx, glbDouble *outy, glbDouble *outz);
private:
	void* mpr_handle;
};
const glbDouble WGS_84_RADIUS_EQUATOR = 6378137.0;
const glbDouble WGS_84_RADIUS_POLAR = 6356752.3142;
const glbDouble PI   = 3.14159265358979323846;
class GLBCOMM_API CGlbEllipsoidModel:public CGlbReference
{
public:
	/**
	  * @brief ���캯��
	  *
	  * @param radiusEquator �����峤�뾶(����뾶)
	  * @param radiusPolar   ������̰뾶
	  */
	CGlbEllipsoidModel(glbDouble radiusEquator=WGS_84_RADIUS_EQUATOR,
		               glbDouble radiusPolar=WGS_84_RADIUS_POLAR);
	/**
	  *  @brief �Ѿ�γ��ת��Ϊ�ѿ���
	  * 
	  *  @param longitude ����,��λ����
	  *  @param latitude γ��,��λ����	  
	  *  @param height �߶�,��λ����
	  *  @param X ת�����X������ֵ
	  *  @param Y ת�����Y������ֵ
	  *  @param Z ת�����Z������ֵ
	  */
	void ConvertLatLongHeightToXYZ(glbDouble latitude,glbDouble longitude, glbDouble height,
                                   glbDouble& X, glbDouble& Y, glbDouble& Z) const;
	/**
	  *  @brief �ѵѿ���ת��Ϊ��γ��
	  * 
	  *  @param X X������ֵ
	  *  @param Y Y������ֵ
	  *  @param Z Z������ֵ
	  *  @param longitude ת����ľ���,��λ����
	  *  @param latitude  ת�����γ��,��λ����	  
	  *  @param height    ת����ĸ߶�,��λ����	  
	  */
	void ConvertXYZToLatLongHeight(glbDouble X, glbDouble Y, glbDouble Z,
                                   glbDouble& longitude, glbDouble& latitude, glbDouble& height) const;
	/*
		@brief ��ת����
	*/
	glbDouble DegreesToRadians(glbDouble angle);
	/*
	    @brief ����ת��
	*/
	glbDouble RadiansToDegrees(glbDouble angle);
private:
	glbDouble mpr_radiusEquator;
    glbDouble mpr_radiusPolar;
	glbDouble mpr_eccentricitySquared;
};


