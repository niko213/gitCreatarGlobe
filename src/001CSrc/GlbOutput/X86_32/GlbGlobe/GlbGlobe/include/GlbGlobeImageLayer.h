/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeImageLayer.h
* @brief   ͼ��ͼ��ͷ�ļ�
*
* �����������CGlbGlobeImageLayer���class
*
* @version 1.0
* @author  ����
* @date    2017-5-11 10:40
*********************************************************************/
#pragma once
#include "GlbGlobeExport.h"
#include "GlbGlobeLayer.h"
#include "libxml/tree.h"

namespace GlbGlobe
{
	/**
	* @brief ͼ��ͼ����
	* @note ��������Ⱦ�ɴ��߳���Ϣ��ͼ�������Ǵ�Χ��ͼ��---Ŀǰֻ֧�ֵ��Ų������Կ�֧�ֳߴ��ͼƬ 2017.6.28 ml
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeImageLayer :	public CGlbGlobeLayer
	{
	public:
		/**
		* @brief ͼ��ͼ�㹹�캯��
		* @return ��
		*/
		CGlbGlobeImageLayer(void);
		/**
		* @brief ͼ��ͼ����������
		* @return ��
		*/
		~CGlbGlobeImageLayer(void);

		/* ����ӿ� */
		/**
		* @brief ��ȡͼ������
		* @return ͼ������ 
		*/
		const glbWChar* GetName();
		/**
		* @brief ����ͼ������
		* @param name ͼ������
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief ��ȡͼ������
		* @return  ͼ������
		*/
		GlbGlobeLayerTypeEnum GetType();
		/**
		* @brief ͼ�����л������أ�
		* @param [in] node xml��ȡ�ڵ�
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool   Load(xmlNodePtr node, glbWChar* relativepath );
		glbBool   Load2(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ͼ�����л������棩
		* @param [in] node xmlд��ڵ�
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool   Save(xmlNodePtr node, glbWChar* relativepath );
		/**
		* @brief ����ͼ����ʾ������
		* @param [in] show ����  ��ʾΪtrue������Ϊfalse
		* @param [in] isOnState true��ʾ�ڿɼ�����  false��ʾ���ڿɼ�����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Show(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief ����ͼ�㲻͸����
		* @param [in] opacity ��͸����(0-100)
		-	0��ʾȫ͸��
		-	100��ʾ��͸��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetOpacity(glbInt32 opacity);
		/**
		* @brief ��ȡͼ�㷶Χ
		* @param [in] isWorld �Ƿ����������꣬������������꣬����Ϊ��λ;��������������꣬��ʹ�õ������꣬�Զ�Ϊ��λ				
		* @return  ͼ�㷶Χ
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld);

		/*
		* @brief ����ͼ����˸
		* @param [in] isBlink �Ƿ���˸,true��˸
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool Blink(glbBool isBlink);
		glbBool IsBlink();

		/* ImageLayer �ӿ� */
		// �Զ��и��ֱ���ͼƬ--->texture
		/* 
		* @brief ����ͼ�������ļ�
		* @param [in] strImageFile Ӱ���ļ�
					  ext		   ��Χ	
		* @return �ɹ�����true
		-	 ʧ�ܷ���false
		*/	
		glbBool SetImageDataFile(glbWChar* strImageFile, CGlbExtent* ext);
		/**
		* @brief ��ȡͼ�������ļ�				
		* @return  ͼ�������ļ�
		*/
		const glbWChar* GetImageDataFile();

		/* 
		* @brief ���ø����ܶ�
		* @param [in] mode �߳�ģʽ
		* @return �ɹ�����true
		-	 ʧ�ܷ���false
		*/				
		void 	SetMeshDensity(glbInt32 meshDensity);
		/**
		* @brief ��ȡ�����ܶ� Ĭ��Ϊ 1.0				
		* @return  �����ܶ�
		*/
		glbInt32 GetMeshDensity();

		/* 
		* @brief ���ø߳�ģʽ
		* @param [in] mode �߳�ģʽ
		* @return �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode); 
		/**
		* @brief ��ȡ�߳�ģʽ					
		* @return  �߳�ģʽ
		*/
		GlbAltitudeModeEnum GetAltitudeMode();

		/* 
		* @brief ���þ���ģʽ�µĸ߳�Zֵ
		* @param [in] altitude �߳�Zֵ
		* @return �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetAltitudeOrZ(glbDouble altitude);
		/**
		* @brief ��ȡ�߳�Zֵ					
		* @return  �߳�Zֵ
		*/
		glbDouble GetAltitudeOrZ();

		/**
		* @brief ͼ�����		 
		*/
		void Update();

		//glbBool SetHeightDataFile(glbWChar* strHeightMapFile);
	private:
		void ComputePosByAltitudeAndGloleType( osg::Vec3d &position );
		glbDouble GetDistance( osg::Vec3d &cameraPos);
		osg::ref_ptr<osg::Group> build();
		void	DirtyOnTerrainObject();
		osg::ref_ptr<osg::Drawable> buildMeshGeometry(osg::Vec3f origPos, CGlbExtent*extent, osg::Image*image);
	private:
		glbref_ptr<CGlbExtent>	mpr_extent;				///<ͼ�����
		GlbAltitudeModeEnum     mpr_altitudeMode;		///<�߳�ģʽ

		CGlbWString				mpr_imageDataFile;		///<Ӱ���ļ���ַ
		glbInt32				mpr_meshDensity;		///<�����ܶ�
		glbDouble				mpr_altitudeOrZ;		///<����ģʽʱ�ĸ߳�
		glbBool					mpr_isCanDrawImage;
		glbBool					mpr_isBlink;

		glbBool					mpr_isInit;				///<�Ƿ��ʼ��

		glbInt32				mpr_imageWidth;			///<Ӱ����
		glbInt32				mpr_imageHeight;		///<Ӱ��߶�
		glbBool					mpr_isImageLoaded;		///<Ӱ���Ƿ��Ѿ�����

	};
}

