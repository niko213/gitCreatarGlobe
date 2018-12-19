#pragma once

#include "GlbReference.h"
#include "EasyGlbGlobeTypes.h"
#include "GlbWString.h"
#include "libxml/tree.h"

namespace GlbGlobe
{
	class CEasyGlbGlobe;
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeObject : public CGlbReference
	{
	public:
		CEasyGlbGlobeObject(void);
		~CEasyGlbGlobeObject();
		/**
		* @brief ��ȡ��������
		* @return ��������
		*/
		const glbWChar *GetName();
		/**
		* @brief ���ö�������
		* @param[in]  name ��������
		*/
		void SetName(const glbWChar *name);
		/**
		* @brief ��ȡ��������
		* @return ����ö������
		*/
		virtual GlbGlobeObjectTypeEnum GetType();
		/**
		* @brief ��ȡ����ID
		* @return ����ID
		*/
		glbInt32 GetId();
		/**
		* @brief ���ö���ID
		* @param[in]  id ����ID
		*/
		void SetId(glbInt32 id);
		/**
		* @brief ��ȡ�����������ID
		* @return �����������ID
		*/
		glbInt32 GetGroupId();
		/**
		* @brief ���ö����������ID
		* @param[in]  id �����������ID
		*/
		void SetGroupId(glbInt32 id);
		/**
		* @brief ��ȡ��������
		* @return ��������
		*/
		CEasyGlbGlobe *GetGlobe();
		/**
		* @brief ���ó�������
		* @param[in]  globe ��������
		*/
		virtual void SetGlobe(CEasyGlbGlobe *globe);	
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
		* @brief ��ȡ���󼤻ʽ
		* @return ���󼤻ʽö��
		*/
		GlbGlobeActiveActionEnum GetActiveAction();
		/**
		* @brief ���ö��󼤻ʽ
		* @param[in]  action ���ʽö��
		*/
		void SetActiveAction(GlbGlobeActiveActionEnum action);	
	protected:
		glbInt32 mpt_id;///<����Id
		glbInt32 mpt_grpId;///����������Id
		CGlbWString mpt_name;///<��������
		CEasyGlbGlobe *mpt_globe;///<��������
		GlbGlobeActiveActionEnum mpt_action;///<�����
	};
}


