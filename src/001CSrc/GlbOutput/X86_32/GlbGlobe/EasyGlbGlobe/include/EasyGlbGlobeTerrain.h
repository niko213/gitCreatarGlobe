#pragma once
#include "GlbReference.h"
#include "GlbWString.h"
#include "EasyGlbGlobeTypes.h"
#include "IGlbDataEngine.h"
#include <osg/Switch>
#include "libxml/tree.h"

namespace GlbGlobe
{
	class CEasyGlbGlobe;
	class GLB_DLLCLASS_EXPORT CEasyGlbGlobeTerrain : public CGlbReference
	{
	public:
		/**
		* @brief ���ι��캯��
		* @param [in] globe ������������	  
		* @return  ��
		*/
		CEasyGlbGlobeTerrain(CEasyGlbGlobe* globe);
		~CEasyGlbGlobeTerrain(void);
		/**
		* @brief ���õ������ݼ�
		* @param [in] tdataset �������ݼ�	  
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		//glbBool SetTerrainDataset(IGlbTerrainDataset* tdataset);
		/**
		* @brief ��ȡ�������ݼ� 
		* @return  �ɹ����ص������ݼ�ָ��
		-	 ʧ�ܷ���NULL
		*/
		//IGlbTerrainDataset* GetTerrainDataset();
		/**
		* @brief ���γ�ʼ��	   
		* @return  �ɹ�����true
		-	 ʧ�ܷ���false
		*/
		//glbBool Initialize();
		/**
		* @brief ��ȡ�����Ƿ��ʼ��	   
		* @return  �ѳ�ʼ������true
		-	 û�г�ʼ������false
		*/
		//glbBool IsInitialized();
		/**
		* @brief ���θ���
		* @param [in] domlayers domͼ�㼯��
		* @param [in] demlayers demͼ�㼯��
		* @param [in] terrainobjs �����λ��ƶ��󼯺�
		* @param [in] mTerrainobjs �����޸Ķ��󼯺�
		* @return  ��
		*/
		//void Update(std::vector<glbref_ptr<CGlbGlobeDomLayer>> &domlayers,
		//	std::vector<glbref_ptr<CGlbGlobeDemLayer>> &demlayers,
		//	std::vector<glbref_ptr<CEasyGlbGlobeRObject>>  &terrainobjs, 
		//	std::vector<glbref_ptr<CEasyGlbGlobeRObject>>  &mTerrainobjs);
		/**
		* @brief ��������  
		* @return  ��
		*/
		//void Dispose();
	};

}

