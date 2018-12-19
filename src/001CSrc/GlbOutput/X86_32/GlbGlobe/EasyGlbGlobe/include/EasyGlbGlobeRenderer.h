#pragma once
#include "GlbReference.h"
#include "EasyGlbGlobeObject.h"
#include "GlbWString.h"
#include "EasyGlbGlobeObjectRenderInfos.h"
#include <osg\Node>

namespace GlbGlobe
{
	class CEasyGlbGlobe;
	//class CEasyGlbGlobeFeatureLayer;
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeRenderer : public CGlbReference
	{
		//friend class CGlbGlobeFeatureLayer;
	public:
		CEasyGlbGlobeRenderer(void);
		~CEasyGlbGlobeRenderer(void);
		/**
		* @brief ��ȡ��Ⱦ������
		* @return  ��Ⱦ������
		*/
		const glbWChar* GetName();
		/**
		* @brief ������Ⱦ������
		* @param [in] name ��Ⱦ������
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetName(glbWChar* name);
		/**
		* @brief ���ü����ֶ�
		* @param [in] field �����ֶ���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetGeoField(glbWChar* field);
		/**
		* @brief ��ȡ�����ֶ�����
		* @return �����ֶ�����
		*/
		const glbWChar* GetGeoField();
		/**
		* @brief ���ñ�ע�ֶ�
		* @param [in] field ��ע�ֶ���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetLabelField(glbWChar* field);
		/**
		* @brief ��ȡ��ע�ֶ�����
		* @return ��ע�ֶ�����
		*/
		const glbWChar* GetLabelField();
		/**
		* @brief ������Ⱦ������Ϣ
		* @param [in] rinfo ��Ⱦ������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRenderInfo(GlbRenderInfo* rinfo);
		/**
		* @brief ��ȡ��Ⱦ������Ϣ
		* @return ��Ⱦ������Ϣ
		*/
		GlbRenderInfo* GetRenderInfo();
		/**
		* @brief ��ȡ��С�ɼ�������Ϣ
		* @return ��С�ɼ�������Ϣ
		*/
		GlbRenderDouble* GetMinDistance();
		/**
		* @brief ������С�ɼ�������Ϣ
		* @param [in] dis ��С�ɼ�������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetMinDistance(GlbRenderDouble* dis);
		/**
		* @brief ��ȡ���ɼ�������Ϣ
		* @return ���ɼ�������Ϣ
		*/	
		GlbRenderDouble* GetMaxDistance();
		/**
		* @brief �������ɼ�������Ϣ
		* @param [in] dis ���ɼ�������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetMaxDistance(GlbRenderDouble* dis);
		/**
		* @brief ��ȡ��Ⱦ˳��
		* @return ��Ⱦ˳��
		*/	
		GlbRenderInt32* GetRenderOrder();
		/**
		* @brief ������Ⱦ˳��
		* @param [in] order ��Ⱦ˳��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRenderOrder(GlbRenderInt32* order);
		/**
		* @brief ��ȡ����˳��
		* @return ����˳��
		*/
		GlbRenderInt32* GetLoadOrder();
		/**
		* @brief ���ü���˳��
		* @param [in] order ����˳��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetLoadOrder(GlbRenderInt32* order);
		/**
		* @brief ��ȡ������Ϣ
		* @return ������Ϣ
		*/
		GlbRenderString* GetRMessage();
		/**
		* @brief ���õ�����Ϣ
		* @param [in] msg ������Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetRMessage(GlbRenderString* msg);
		/**
		* @brief ��ȡ��ʾ��Ϣ
		* @return ��ʾ��Ϣ
		*/
		GlbRenderString* GetToolTip();
		/**
		* @brief ������ʾ��Ϣ
		* @param [in] tips ��ʾ��Ϣ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetToolTip(GlbRenderString* tips);
		/**
		* @brief ���ø߳�ģʽ
		* @param [in] mode �߳�ģʽ
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetAltitudeMode(GlbAltitudeModeEnum mode);
		/**
		* @brief ��ȡ�߳�ģʽ
		* @return �߳�ģʽ
		*/
		GlbAltitudeModeEnum GetAltitudeMode();
		/**
		* @brief ������Ⱦ���ж�������
		* @param [in] isShow true��ʾ��ʾ��false��ʾ����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetShow(glbBool isShow);
		/**
		* @brief ���л�(����)
		* @param [in] node xml������
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		//glbBool Load(xmlNodePtr node,glbWChar* relativepath );
		/**
		* @brief ���л�(����)
		* @param [in] node xml������
		* @param [in] relativepath ���·��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		//glbBool Save(xmlNodePtr node, glbWChar* relativepath);
		/**
		* @brief ���ݶ���id��ȡ����
		* @param [in] objId ����id	
		* @return  �ɹ����ض���ָ��
		-	 ʧ�ܷ���NULL
		*/
		CEasyGlbGlobeObject* GetObject(glbInt32 objId);
		/**
		* @brief ��Ӷ���
		* @param [in] obj ����ָ��	
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool AddObject(CEasyGlbGlobeObject* obj);
		/**
		* @brief ���ݶ���idɾ������
		* @param [in] objId ����id	
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveObject(glbInt32 objId);
		/**
		* @brief ɾ�����ж���		
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveAllObjects();

		/**
		* @brief ��ȡ��Ⱦ���ж�������	  
		* @return  ��������
		*/
		glbInt32 GetObjectCount();
		/**
		* @brief ����Ҫ��feature��������
		* @param [in] feature Ҫ��	
		* @return  �ɹ������½��Ķ���ָ��
		-	 ʧ�ܷ���NULL
		*/
		//CEasyGlbGlobeObject* CreateObject(CGlbFeature* feature);
		/**
		* @brief ��ȡ��Ⱦ����osg�ڵ� 
		* @return osg�ڵ�
		*/
		osg::Node* GetNode();
	private:
		// ����Renderer�е�������Ե�obj��,��renderorder,loadorder,minDistance.....
		void UpdateAttribute(CEasyGlbGlobeObject* obj);		
	private:
		CGlbWString               mpr_name;							//��Ⱦ������
		CGlbWString               mpr_geofield;						//�����ֶ�
		CGlbWString               mpr_labelfield;					//ע���ֶ�
		glbBool		              mpr_isground;	   				    //�Ƿ�������
		glbref_ptr<GlbRenderInfo> mpr_renderinfo;					//��Ⱦ������Ϣ
		GlbRenderDouble*          mpr_mindistance;					//��С�ɼ�����
		GlbRenderDouble*          mpr_maxdistance;					//���ɼ�����
		GlbRenderInt32*           mpr_loadorder;					//����˳��
		GlbRenderInt32*           mpr_renderorder;					//��Ⱦ˳��
		GlbRenderString*          mpr_message;						//������Ϣ
		GlbRenderString*          mpr_tooltips;						//��ʾ��Ϣ
		GlbAltitudeModeEnum       mpr_altitudemode;					//�߳�ģʽ
		CEasyGlbGlobe*            mpr_globe;
		osg::ref_ptr<osg::Node>   mpr_node;							//osg�ڵ�
		std::map<glbInt32,glbref_ptr<CEasyGlbGlobeObject>> mpr_objects;		//��������
		GlbCriticalSection	                           mpr_critical;		//�޸Ĳ��� ���� �ٽ���
		glbBool					  mpr_isShow;						//��Ⱦ����������
	};
}

