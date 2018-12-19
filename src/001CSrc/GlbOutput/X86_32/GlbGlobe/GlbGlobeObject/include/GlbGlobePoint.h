/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbGlobePoint.h
* @brief   �����ͷ�ļ����ĵ�����CGlbGlobePoint��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-6 15:30
*********************************************************************/
#pragma once
#include "GlbGlobeObjectExport.h"
#include "GlbGlobeREObject.h"
#include "CGlbGlobe.h"

namespace GlbGlobe
{
	/**
	* @brief CGlbGlobePoint�������				 
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-6 15:30
	*/
	class GLB_OBJECTDLL_CLASSEXPORT CGlbGlobePoint : public CGlbGlobeREObject
	{
	public:
		class CGlbGlobePointTask : public IGlbGlobeTask
		{
		public:
			CGlbGlobePointTask(CGlbGlobePoint *obj,glbInt32 level);
			~CGlbGlobePointTask();
			glbBool doRequest();
			CGlbGlobeObject *getObject();
		private:
			glbref_ptr<CGlbGlobePoint> mpr_obj;
			glbInt32 mpr_level;
		};
	public:
		/**
		* @brief ���캯��
		*/
		CGlbGlobePoint(void);
		/**
		* @brief ��������
		*/
		~CGlbGlobePoint(void);
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
		* @brief ���ö�����Ⱦ���ȼ�
		* @param[in]  order ������Ⱦ���ȼ�
		*/
		 void SetRenderOrder(glbInt32 order);
		 /**
		 * @brief ���ö���λ��
		 * @param[in]  xOrLon ���ȣ���λ���ȣ���x����
		 * @param[in]  yOrLat γ�ȣ���λ���ȣ���y����
		 * @param[in]  zOrAlt �߳�����
		 * @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		 */
		void SetPosition(glbDouble xOrLon,glbDouble yOrLat,glbDouble zOrAlt,glbBool &isNeedDirectDraw);
		/**
		* @brief ��ȡ����λ��
		* @param[out]  xOrLon ���ȣ���λ���ȣ���x����
		* @param[out]  yOrLat γ�ȣ���λ���ȣ���y����
		* @param[out]  zOrAlt �߳�����
		*/
		void GetPosition(glbDouble *xOrLon,glbDouble *yOrLat,glbDouble *zOrAlt);
		 /**
		 * @brief ���ü���
		 * @param[in]  geo ����
		 * @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		 */
		//void SetGeo(IGlbGeometry* geo,glbBool &isNeedDirectDraw);
		/**
		* @brief ��ȡ���󼸺�
		* @return ����
		*/
		//IGlbGeometry* GetGeo();
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		void DrawToImage(glbByte *image/*RGBA*/,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief ��ȡ��ǰLOD�ڵ�Ĵ�С����λ���ֽڡ�
		* @return ��ǰLOD����osg�ڵ��С
		*/
		glbInt32 GetOsgNodeSize();

		/**
		* @brief ���ö��������˳�"�༭"״̬
		* @param[in]  isEdit ����༭״̬Ϊtrue���˳�Ϊfalse
		* @return �ɹ�����true��ʧ�ܷ���false
		*/
		glbBool SetEdit(glbBool isEdit);

		/**
		 * @brief ���������distance����ʱ����ʾ�ȼ�
		 * @param distance �����object�ľ���
		 * @return level�ȼ�
		*/
		virtual glbInt32 ComputeRefLevel(glbDouble distance);
		/**
		 * @brief �Ƿ������˶�̬�����ע
		 * @param ��
		 * @return true ����; false �ر� 
		*/
		glbBool IsFadingDynamicLabel();
		/**
		 * @brief ����/�ر� ��̬�����ע
		 * @param true ��; false ��
		 * @return 
		*/
		void SetFadingDynamicLabel(glbBool isFading);
	private:
		//װ������
		void ReadData(glbInt32 level,glbBool isDirect = false);
		//SetRenderInfo
		//isNeedDirectDraw��ʶ�ڵ��Ƿ���Ҫ���»���
		glbBool DealModelSymbol(GlbMarkerModelSymbolInfo *modelInfo,glbBool &isNeedDirectDraw);
		glbBool Deal2DShapeSymbol(GlbMarker2DShapeSymbolInfo *marker2DInfo,glbBool &isNeedDirectDraw);
		glbBool Deal3DShapeSymbol(GlbMarker3DShapeSymbolInfo *marker3DInfo,glbBool &isNeedDirectDraw);
		glbBool DealPixelSymbol(GlbMarkerPixelSymbolInfo *pixelInfo,glbBool &isNeedDirectDraw);
		glbBool DealLabelSymbol(GlbMarkerLabelSymbolInfo *labelInfo,glbBool &isNeedDirectDraw);
		glbBool DealDynamicLabelSymbol(GlbMarkerDynamicLabelSymbolInfo *dynamicLabelInfo,glbBool &isNeedDirectDraw);
		glbBool DealImageSymbol(GlbMarkerImageSymbolInfo *imageInfo,glbBool &isNeedDirectDraw);
		//ReadData
		void DealModelOpacity(osg::Node *node,GlbMarkerModelSymbolInfo *modelInfo);
		void ComputePosByAltitudeAndGloleType(osg::Vec3d &position);
		void DealModePosByChangeAltitudeOrChangePos();		
		void DealSelected();
		void DealBlink();
		void DealEdit();
		
		glbInt32 ComputeNodeSize(osg::Node *node);
		
		//�������ʱ�ṩ��ȡ����㷽��
		glbBool GetFeaturePoint(glbInt32 idx,glbDouble *ptx,glbDouble *pty,glbDouble *ptz,glbDouble *ptm);

		void DirtyOnTerrainObject();
		//�ӳ������Ƴ��ڵ㣬���ڷ�������ģʽת��Ϊ������ģʽʱ���������еķ������ζ���ڵ��Ƴ�
		void RemoveNodeFromScene(glbBool isClean=true);

		//�����������ɫ[���]��������ʱ����Ҫ
		void ParseObjectFadeColor();

		osg::Matrixd ComputePosOffsetMatrix();
	private:
		glbDouble                 mpr_distance;  ///<���������ľ���
		glbref_ptr<IGlbGeometry>  mpr_outline;   ///<�����2D����		
		glbDouble                 mpr_elevation; ///<��ǰ���θ�
		glbref_ptr<GlbRenderInfo> mpr_renderInfo;///<������Ⱦ��Ϣ
		glbDouble                 mpr_xOrLon;    ///<���ȣ���λ���ȣ���x����
		glbDouble                 mpr_yOrLat;    ///<γ�ȣ���λ���ȣ���y����
		glbDouble                 mpr_zOrAlt;    ///<�߳�����
		//glbref_ptr<IGlbGeometry>  mpr_geo;		 ///<����
		glbInt32                  mpr_objSize;   ///<obj��size
		GlbCriticalSection        mpr_addToScene_critical;	///<�ڵ����ӳ����ٽ���
		GlbCriticalSection        mpr_readData_critical;	///<ReadData�����ڵ��ٽ���		
		glbBool                   mpr_isCanDrawImage;		///<���������������			
		glbBool                   mpr_needReReadData;		///<���¶�ȡ���ݱ�־
		glbref_ptr<IGlbGeometry> mpr_noRotOutline;
		glbBool mpr_isDirectDraw;///<��ʶ���ض�����ػ�
		glbInt32				  mpr_zerolevelObjNodeSize;		///<0���ڵ�Ľڵ��С
		glbInt32				  mpr_preNodeSize;

		GlbCriticalSection		  mpr_outline_critical;		///<outline��д�ٽ���

		glbBool                   mpr_isFadingDynamicLabel;
		glbDouble				  mpr_preDistance; //<����ͼƬ�����Ż����ټ����������

	};
}