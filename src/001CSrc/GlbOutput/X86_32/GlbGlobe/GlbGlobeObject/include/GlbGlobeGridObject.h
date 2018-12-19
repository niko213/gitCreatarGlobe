/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeGridObject.h
* @brief   ��������ͷ�ļ����ĵ�����CGlbGlobeGridObject��
* @version 1.0
* @author  ML
* @date    2018-2-6 9:30
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeREObject.h"
#include "CGlbGlobe.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeGridObject����������				 
	* @version 1.0
	* @author  ML
	* @date    2018-2-6 9:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeGridObject : public CGlbGlobeREObject
	{
	private:
		class CGlbGlobeGridTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeGridTask(CGlbGlobeGridObject *obj,glbInt32 level);
			~CGlbGlobeGridTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeGridObject> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeGridObject(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobeGridObject(void);
		/**
		* @brief ��ȡ��������
		* @return ����ö������
		*/
		GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		virtual glbBool Load(xmlNodePtr* node,glbWChar* prjPath);
		virtual glbBool Load2(xmlNodePtr* node,glbWChar* prjPath);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  os �����ļ������
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		virtual glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		/**
		* @brief ��ȡ��������ľ���
		* @param[in]  cameraPos ����ڳ����е�λ��
		* @param[in]  isCompute �����Ƿ���Ҫ���¼��㣬���¼�������true����֮����false
		* @return ��������ľ���
		*/
		glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute=true);
		/**
		* @brief ���ö��󺣰�ģʽ
		* @param[in]  mode ����ģʽö��
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);
		/**
		* @brief װ�����ݣ���������Ƶ���ĵ��ø÷���
		* @param[in]  level ��Ҫװ�����ݵĵȼ�
		*/
		 void LoadData(glbInt32 level);
		/**
		* @brief ��osg�ڵ���߽���osg�ڵ㵽������
		*/
		 void AddToScene();
		/**
		* @brief �ӳ�����ժ��osg�ڵ�
		* @param[in]  isClean true��ʾͬʱɾ������osg�ڵ��Խ�ʡ�ڴ�
		* @return ���ؽ�ʡ���ڴ��С
		*/
		 glbInt32 RemoveFromScene(glbBool isClean);
		/**
		* @brief ��ȡ����ڵ��Χ��
		* @param[in]  isWorld true��ʾ��ȡ���������Χ��(��λ����)
		*  -                                    false��ʾ��ȡ���������Χ�У���λ���ȣ�
		* @return �ڵ��Χ��,�п�����NULL
		*/
		glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ������λ
		* @return  �����2D����
		*/
		 IGlbGeometry *GetOutLine();
		/**
		* @brief ���ö������Ⱦ��Ϣ
		* @param[in]  renderInfo �������Ⱦ��Ϣ
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true����֮����false
		*/
		 glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief ��ȡ�������Ⱦ��Ϣ
		* @return �������Ⱦ��Ϣ
		*/
		 GlbRenderInfo *GetRenderInfo();
		/**
		* @brief ���ö�������
		* @param[in]  isShow ������ʾ����Ϊtrue����������Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		 glbBool SetShow(glbBool isShow,glbBool isOnState = false);
		/**
		* @brief ���ö����ѡ��״̬
		* @param[in]  isSelected ����ѡ������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		 glbBool SetSelected(glbBool isSelected);
		/**
		* @brief ���ö����Ƿ���˸
		* @param[in]  isBlink ��˸����Ϊtrue������˸����Ϊfalse
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		 glbBool SetBlink(glbBool isBlink);
		/**
		* @brief ����������Ⱦ
		* @param[in]  level ������Ⱦ�ȼ�
		*/
		 void DirectDraw(glbInt32 level);
		/**
		* @brief ��ȡ��ǰLOD�ڵ�Ĵ�С����λ���ֽڡ�
		* @return ��ǰLOD����osg�ڵ��С
		*/
		glbInt32 GetOsgNodeSize();

		/**************		CGlbGlobeGridObject�������ר�з���		*************/
	private:
		//װ������
		void ReadData(glbInt32 level,glbBool isDirect = false);
		void DealSelected();
		void DealBlink();
		glbInt32 ComputeNodeSize(osg::Node *node);
		glbBool DealGridRenderInfo(GlbRenderInfo* newSectionRenderInfo, glbBool &isNeedDirectDraw);
		glbBool DealGridColorOpacity(CGlbWString colorLocate, glbInt32 opacity);

		//����
		osg::Node* Draw();

		osg::Node* buildGrid();
		// ������
		osg::Node* buildLineClipMap(IGlbGeometry* geo);
		// NI,NJ��������
		osg::Node* buildNINJClipMap(glbInt32 I,glbInt32 J);	
		// m*n դ��ͼ ����
		osg::Node* buildBarrierClipMap(glbInt32 INum, glbInt32 JNum);

		//�����Ƿ���ƽ���ཻ
		bool cellCrossPlane( const cwSGridCell& cell, const GlbPoint3D& planep1, const GlbPoint3D& planep2);
		//�߶��ཻ
		bool segmentsIntr(const GlbPoint3D& a, const GlbPoint3D& b, const GlbPoint3D& c, const GlbPoint3D& d, GlbPoint3D& intrP);

		//��ʱ--> ���Ƽ���sgrid�ı�
		osg::Node* buildTempLargeCopyGrid();
	private:
		glbDouble                 mpr_distance;  ///<���������ľ���
		glbref_ptr<IGlbGeometry>  mpr_outline;   ///<�����2D����		
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;///<������Ⱦ��Ϣ		
		glbInt32                  mpr_objSize;   ///<obj��size
		GlbCriticalSection        mpr_addToScene_critical;	///<�ڵ���ӳ����ٽ���
		GlbCriticalSection        mpr_readData_critical;	///<ReadData�����ڵ��ٽ���						
		glbBool                   mpr_needReReadData;		///<���¶�ȡ���ݱ�־

		osg::ref_ptr<osg::Geode>  mpr_faceGeode;
		osg::ref_ptr<osg::Geode>  mpr_edgeGeode;
	};
}

