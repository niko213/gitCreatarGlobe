#pragma once

#include "EasyGlbGlobeObject.h"
#include "GlbWString.h"
#include "GlbExtent.h"
#include "IGlbGeometry.h"
#include "GlbFeature.h"
#include "osg/Node"
#include "EasyGlbGlobeObjectRenderInfos.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeRObject : public CEasyGlbGlobeObject
	{
	public:
		CEasyGlbGlobeRObject(void);
		~CEasyGlbGlobeRObject(void);
		/**
		* @brief ����Ԫ��
		* @param[in]  feature �����Ӧ��Ԫ��
		* @param[in]  geoField �����ֶ�
		*/
		void SetFeature(CGlbFeature *feature,const glbWChar* geoField=L"");
		/**
		* @brief ��ȡԪ��
		* @return �����Ӧ��Ԫ��
		*/
		CGlbFeature *GetFeature();
		/**
		* @brief ��ȡ����ڵ��Χ��
		* @param[in]  isWorld true��ʾ��ȡ���������Χ��(��λ����)
		*  -                                    false��ʾ��ȡ���������Χ�У���λ���ȣ�
		* @return �ڵ��Χ��
		*/
		virtual glbref_ptr<CGlbExtent> GetBound(glbBool isWorld = true);
		/**
		* @brief ���ö��󺣰�ģʽ
		* @param[in]  mode ����ģʽö��
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		virtual glbBool SetAltitudeMode(GlbAltitudeModeEnum mode,glbBool &isNeedDirectDraw);

		/**
		* @brief ��ȡ����ĺ���ģʽ
		* @return ����ģʽö��
		*/
		GlbAltitudeModeEnum GetAltitudeMode();

		/**
		* @brief ���ö������Ⱦ��Ϣ
		* @param[in]  renderInfo �������Ⱦ��Ϣ
		* @param[out] isNeedDirectDraw �Ƿ���Ҫ����DirectDraw�������������޸�Ч����true��Ҫ,false����Ҫ.
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetRenderInfo(GlbRenderInfo *renderInfo,glbBool &isNeedDirectDraw);
		/**
		* @brief ��ȡ�������Ⱦ��Ϣ
		* @return �������Ⱦ��Ϣ
		*/
		virtual GlbRenderInfo *GetRenderInfo();

		/**
		* @brief ���ö�������
		* @param[in]  isShow ������ʾ����Ϊtrue����������Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetShow(glbBool isShow,glbBool isOnState=false);
		/**
		* @brief ��ȡ�����������ʶ
		* @return ��ʾ����true�����ط���false
		*/
		glbBool IsShow();

		/**
		* @brief ���ö����ѡ��״̬
		* @param[in]  isSelected ����ѡ������Ϊtrue����֮����Ϊfalse
		* @return ���óɹ�����true����֮����false
		*/
		virtual glbBool SetSelected(glbBool isSelected);
		/**
		* @brief ��ȡ�����ѡ��״̬
		* @return ѡ�з���true����֮����false
		*/
		glbBool IsSelected();
		/**
		* @brief ����������Ⱦ	
		* @param[in]  level ������Ⱦ�ȼ�
		*/
		virtual void DirectDraw(glbInt32 level);
		/**
		* @brief AGG��ά���Ƶ�����
		* @param[in] image ��ͼ������
		* @param[in] imageW ��ͼ�Ŀ�
		* @param[in] imageH ��ͼ�ĸ�
		* @param[in] ext ���ο��Ӧ�ķ�Χ
		*/
		virtual void DrawToImage(glbByte *image/*RGBA*/,
			glbInt32 imageW,glbInt32 imageH,CGlbExtent &ext);
		/**
		* @brief ��ȡ��ǰLOD����osg�ڵ�
		* @return ��ǰLOD����osg�ڵ�
		*/
		osg::Node *GetOsgNode();
		/**
		* @brief �����л����󣬽���xml��ȡ��������ֵ
		* @param[in]  node libXML2�ڵ�
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return �����л��ɹ�����true����֮����false
		*/
		virtual glbBool Load(xmlNodePtr* node,const glbWChar* prjPath);
		/**
		* @brief ���л��������л��������Ե�xml�����ļ�
		* @param[in]  os �����ļ������
		* @param[in]  prjPath �����ļ�Ŀ¼
		* @return ���л��ɹ�����true����֮����false
		*/
		virtual glbBool Save(xmlNodePtr node,const glbWChar* prjPath);

		/**
		* @brief ��ȡ���󸸽ڵ�
		* @return ���ڵ�ָ��
		*/
		osg::Node *GetParentNode();
		/**
		* @brief ���ö��󸸽ڵ�
		* @param[in] ���ڵ�ָ��
		*/
		virtual void SetParentNode(osg::Node *parent);

		/**
		* @brief ���ö���װ�����ȼ�
		* @param[in]  order ����װ�����ȼ�
		*/
		virtual void SetLoadOrder(glbInt32 order);
		/**
		* @brief ��ȡ����װ�����ȼ�
		* @return ����װ�����ȼ�
		*/
		glbInt32 GetLoadOrder();
		/**
		* @brief ���ö�����Ⱦ���ȼ�
		* @param[in]  order ������Ⱦ���ȼ�
		*/
		virtual void SetRenderOrder(glbInt32 order);
		/**
		* @brief ��ȡ������Ⱦ���ȼ�
		* @return ������Ⱦ���ȼ�
		*/
		glbInt32 GetRenderOrder();
		/**
		* @brief ���ö������ɼ�����
		* @param[in]  distance �������ɼ�����
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		virtual glbBool SetMaxVisibleDistance(glbDouble distance);
		/**
		* @brief ��ȡ�������ɼ�����
		* @return �������ɼ�����
		*/
		glbDouble GetMaxVisibleDistance();
		/**
		* @brief ���ö�����С�ɼ�����
		* @param[in]  distance ������С�ɼ�����
		* @return ���óɹ�����true������ʧ�ܷ���false
		*/
		virtual glbBool SetMinVisibleDistance(glbDouble distance);
		/**
		* @brief ��ȡ������С�ɼ�����
		* @return ������С�ɼ�����
		*/
		glbDouble GetMinVisibleDistance();
		/**
		* @brief ���ö�����ʾ��Ϣ
		* @param[in]  tooltip ������ʾ��Ϣ
		* @return ���óɹ�����true������ʧ�ܷ���false
		* @note �����ŵ�������ʱ����ʾ����
		*/
		virtual glbBool SetToolTip(/*CGlbWString*/ glbWChar* tooltip);
		/**
		* @brief ��ȡ������ʾ��Ϣ
		* @return ������ʾ��ʾ
		* @note �����ŵ�������ʱ����ʾ����
		*/		
		const glbWChar* GetTooltip();
		/**
		* @brief ���ö�����Ϣ
		* @param[in]  msg ������Ϣ
		* @return ���óɹ�����true������ʧ�ܷ���false
		* @note ���������ʱ����ʾ����
		*/
		virtual glbBool SetRMessage(/*CGlbWString*/ glbWChar* msg);
		/**
		* @brief ��ȡ������Ϣ
		* @return ������Ϣ
		* @note ���������ʱ����ʾ����
		*/		
		const glbWChar* GetRMessage();
		/**
		* @brief ��ȡ����ǰLOD����
		* @return ����ǰLOD����
		*/
		glbInt32 GetCurrLevel();

		void SetCustomData(osg::Object *data);

		osg::Object *GetCustomData();
	protected:
		glbInt32                mpt_currLevel;///<��ǰLOD����
		osg::ref_ptr<osg::Node> mpt_node;///<OSG�ڵ�		
		osg::ref_ptr<osg::Node> mpt_parent;///<���ڵ�		
		glbDouble               mpt_maxVisibleDistance;///<���ɼ�����
		glbDouble               mpt_minVisibleDistance;///<��С�ɼ�����
		glbInt32                mpt_renderOrder;///<��Ⱦ���ȼ� ע:��ֵС���Ȼ��ƣ���ֵ��ĺ���� 
		glbInt32                mpt_loadOrder;///<װ�����ȼ�
		GlbAltitudeModeEnum     mpt_altitudeMode;///<�߳�ģʽ
		glbBool                 mpt_isSelected;///<�Ƿ�ѡ��
		glbBool                 mpt_isShow;///<�û������Ƿ���ʾ		
		glbref_ptr<CGlbFeature> mpt_feature;///<����Ԫ��
		CGlbWString             mpt_geoField;///<�����ֶ�
		CGlbWString             mpt_tooltip;///<��ʾ��Ϣ
		CGlbWString             mpt_message;///<��Ϣ	
		CEasyGlbGlobeObject *mpt_parentObj;///<������[��϶�����]		

		osg::ref_ptr<osg::Object> mpr_customData;
	};
}

