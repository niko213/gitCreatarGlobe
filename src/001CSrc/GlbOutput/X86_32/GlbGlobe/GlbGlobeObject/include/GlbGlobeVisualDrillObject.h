/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobeVisualDrillObject.h
* @brief   ������׶���ͷ�ļ����ĵ�����CGlbGlobeVisualDrillObject��
* @version 1.0
* @author  Malin
* @date    2018-8-20 15:30
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "glbglobereobject.h"
#include "IGlbGlobeTask.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobeVisualDrillObject������׶�����				 
	* @version 1.0
	* @author  ����
	* @date    2018-8-21 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobeVisualDrillObject :	public CGlbGlobeREObject
	{
	public:
		class CGlbGlobeVDrillTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobeVDrillTask(CGlbGlobeVisualDrillObject *obj,glbInt32 level);
			~CGlbGlobeVDrillTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobeVisualDrillObject> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobeVisualDrillObject(void);
		/**
		* @brief ���캯��
		* @param[in] jsonString json�ַ���
		*/
		CGlbGlobeVisualDrillObject(glbChar* jsonString);
		/**
		* @brief ��������
		*/
		~CGlbGlobeVisualDrillObject(void);
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
		glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		glbBool Load2(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  os �����ļ������
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		glbBool Save(xmlNodePtr node,const glbWChar* prjPath);
		/**
		* @brief ��ȡ��������ľ���
		* @param[in]  cameraPos ����ڳ����е�λ��
		* @param[in]  isCompute �����Ƿ���Ҫ���¼��㣬���¼�������true����֮����false
		* @return ��������ľ���
		*/
		 glbDouble GetDistance(osg::Vec3d &cameraPos,glbBool isCompute);
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
		* @brief ��Ե��λ���ʱ�����¶����Ӧ��ĵ��θ߶�
		*/
		 void UpdateElevate();
		/**
		* @brief �����λ��ƶ���Ķ�ά������ ��λ��������λ
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
		* @brief ���ö��󺣰�ģʽ
		* @param[in]  mode ����ģʽö��
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		 glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);
		/**
		* @brief ��ȡ��ǰLOD�ڵ�Ĵ�С����λ���ֽڡ�
		* @return ��ǰLOD����osg�ڵ��С
		*/
		glbInt32 GetOsgNodeSize();

		/**************		VisualSection�����������ר�з���		*************/
		/**
		* @brief ����Json�ַ���
		* @param[in] jsonString json�ַ���
		*/
		void SetJsonString(char* jsonString);
		/**
		* @brief �������		
		*/
		void Update();
		/**
		* @brief ʰȡ�ز�.
		* @param[in]  mx ���Xֵ
		* @param[in]  my ���Yֵ
		* @param[out] stratumId ���ʰȡ���ĵز�Id
		* @return �ɹ�����true����֮����false
		*/
		glbBool Pick(glbInt32 mx,glbInt32 my,glbInt32& stratumId);
		/**
		* @brief ���õز��Ƿ�ѡ��״̬.
		* @param[in]  stratumId ,�ز�Id
		* @param[in]  isSelected,true����ѡ��.
		*/
		void SetStratumSelected(glbInt32 stratumId,glbBool isSelected);
		/**
		* @brief ���õز��Ƿ���˸
		* @param[in]  stratumId ,�ز�Id
		* @param[in]  isBlink,true������˸.
		*/
		void SetStratumBlink(glbInt32 stratumId,glbBool isBlink);
		/**
		* @brief ���õز��Ƿ���ʾ
		* @param[in]  stratumId ,�ز�Id
		* @param[in]  isShow,true������ʾ.
		*/
		void SetStratumShow(glbInt32 stratumId,glbBool isShow);
		/**
		* @brief ���õز��Ƿ�����Ӧ��С
		* @param[in]  isAuto ,����Ӧ��С��ʶ
		*/
		void SetStratumAuto(glbBool isAuto);
	private:
		/**
		* @brief ��ȡ����
		*/
		void ReadData(glbInt32 level,glbBool isDirect = false);
		void DealSelected();
		void DealBlink();
		glbInt32 ComputeNodeSize(osg::Node *node);

		glbBool DealDrillRenderInfo(GlbDrillRenderInfo* newDrillRenderInfo, glbBool &isNeedDirectDraw);

		void DealDrillBaselineColor(GlbRenderColor* clr);
		void DealDrillBaselineOpacity(GlbRenderInt32* opacity);		
		void DealDrillBaselineWidth(GlbRenderDouble* wid);

		void DealDrillStratumColor(GlbRenderColor* clr);
		void DealDrillStratumOpacity(GlbRenderInt32* opacity);
		void DealDrillStratumWidth(GlbRenderDouble* wid);

		// ���������߻�ز㿪��
		void DealDrillBLOrStRenderOrNot(bool renderBaseline, bool renderStratum);

	private:
		glbDouble                 mpr_distance;  ///<���������ľ���
		glbref_ptr<IGlbGeometry>  mpr_outline;   ///<�����2D����		
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;///<������Ⱦ��Ϣ		
		glbInt32                  mpr_objSize;   ///<obj��size
		GlbCriticalSection        mpr_addToScene_critical;	///<�ڵ����ӳ����ٽ���
		GlbCriticalSection        mpr_readData_critical;	///<ReadData�����ڵ��ٽ���						
		glbBool                   mpr_needReReadData;		///<���¶�ȡ���ݱ�־
		std::map<glbInt32,glbBool> mpr_stratumSelect;		///<�ز�id���Ƿ�ѡ�б�־����
		glbBool mpr_isAuto;									///<��ʶ�Ƿ�������Ӧ

		glbBool mpr_isFromConstructModel;					///<��ʶ��������Ƿ��Ǵӽṹģ��Tin�����ɵ�
		glbBool mpr_isFromGridModel;						///<��ʶ��������Ƿ��ǴӸ߾�����ģ��Grid�����ɵ�
	};
}