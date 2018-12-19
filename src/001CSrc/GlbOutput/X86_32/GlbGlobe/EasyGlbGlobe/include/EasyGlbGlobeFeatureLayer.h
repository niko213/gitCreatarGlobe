#pragma once

#include "EasyGlbGlobeLayer.h"
#include "GlbFeature.h"
#include "GlbTin.h"
#include <osg\Polytope>
#include <osg\BoundingBox>
#include "EasyGlbGlobeRenderer.h"

namespace GlbGlobe
{
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeFeatureLayer : public CEasyGlbGlobeLayer
	{
	public:
		CEasyGlbGlobeFeatureLayer(void);
		~CEasyGlbGlobeFeatureLayer(void);

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
		glbBool Show(glbBool show,glbBool isOnState=false);

		glbBool SetGlobe(CEasyGlbGlobe* globe);

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

		/* Ҫ��ͼ��ӿ� */
		/**
		* @brief ͼ�����
		*				 
		*/
		void Update();
		/**
		* @brief ����ͼ������ݹ�����
		* @param [in] filter ������
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetFilter(CGlbQueryFilter* filter);
		/**
		* @brief ��ȡͼ������ݹ�����
		* 				
		* @return  ���ݹ�����
		*/
		CGlbQueryFilter* GetFilter();
		/**
		* @brief ���õ�ǰ����Ŀɼ�����
		* @param [in] extent1 �ɼ�����1
		* @param [in] extent2 �ɼ�����2
		* @note �ɼ����������Ϊ����������Сͼ�����ݼ��Ĳ�ѯ���
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool SetVisibleTerrainExtent(CGlbExtent* extent1, CGlbExtent* extent2);
		/**
		* @brief ���һ��ͼ����Ⱦ��
		* @param [in] renderer ��Ⱦ��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool AddRenderer(CEasyGlbGlobeRenderer* renderer); 
		/**
		* @brief ɾ��һ��ͼ����Ⱦ��
		* @param [in] idx ��Ⱦ�����
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveRenderer(glbInt32 idx); 
		/**
		* @brief ɾ������ͼ����Ⱦ��
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		glbBool RemoveAllRenderers();
		/**
		* @brief ��ȡͼ����Ⱦ����
		* 			
		* @return  ��Ⱦ������
		*/
		glbInt32 GetRendererCount(); 
		/**
		* @brief ��ȡһ��ͼ����Ⱦ��
		* @param [in] idx ��Ⱦ�����
		* @return  �ɹ�������Ⱦ��ָ��
		-	 ʧ�ܷ���NULL
		*/
		CEasyGlbGlobeRenderer* GetRenderer(glbInt32 idx); 
	private:
		glbBool IsEqual(CGlbExtent* extent, CGlbExtent* extent_other);
		glbInt32 ComputeFitLoopdepth(GlbGlobeTypeEnum globe_type, osg::Camera* p_osgcamera, CGlbExtent* p_layer_extent);
		osg::BoundingBoxd ComputeVisibleExtent(osg::Polytope cv, CGlbExtent* extent, GlbGlobeTypeEnum globe_type, glbInt32 loopdepth);
		CGlbTin* ExtentToTin(CGlbExtent* p_extent);
	private:
		glbref_ptr<IGlbFeatureCursor> mpr_p_cursor;									//Ҫ���α�
		std::map<glbInt32/*feature oid*/,glbInt32/*feature oid*/> mpr_features;		//װ�ص�Ҫ��		
		std::map<glbInt32/*feature oid*/,glbByte/*state*/> mpr_ftstate;				//Ҫ��״̬			
		glbref_ptr<CGlbQueryFilter> mpr_query_filter;								//���ݹ�����
		glbref_ptr<CGlbExtent> mpr_visible_extent;									//
		glbref_ptr<CGlbExtent> mpr_p_visible_extent_onterrain;						//������ɼ����򡾵������꡿
		std::vector<glbref_ptr<CEasyGlbGlobeRenderer>> mpr_renderers;				//��Ⱦ������
		glbref_ptr<CGlbQueryFilter> mpr_temp_query_filter;

		GlbCriticalSection	mpr_critical;							//�޸Ĳ��� ���� �ٽ���
		GlbCriticalSection	mpr_queryfilter_critical;				//���ݹ����� �ٽ���
	};
}

