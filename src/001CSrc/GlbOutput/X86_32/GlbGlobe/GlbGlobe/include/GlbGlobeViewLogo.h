/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
*
* @file    GlbGlobeViewLogo.h
* @brief   ��ͼ���Ԫ��LOGO
*
* @version 1.0
* @author  ����
* @date    2014-6-9 10:29
*********************************************************************/
#pragma once
#include "GlbGlobeView.h"
#include "GlbWString.h"
#include <osg/Camera>
#include <osgViewer/View>
namespace GlbGlobe
{
	class CGlbGlobe;
	class CGlbGlobeView;
	/**
	* @brief ��ͼ���Ԫ��LOGO��
	* @version 1.0
	* @author  ����
	* @date    2014-6-9 10:43
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeViewLogo : public osg::Camera
	{
		/**
		* @brief ˽���࣬״̬���ص�
		* @version 1.0
		* @author  ����
		* @date    2014-5-27 10:01
		*/
		class CGlbGlobeLogoUpdateCallback : public osg::NodeCallback  
		{
		public:
			/**
			* @brief ���캯��
			*/
			CGlbGlobeLogoUpdateCallback()
			{
				positive = true;
			}
			/**
			* @brief ��������
			*/
			~CGlbGlobeLogoUpdateCallback(){}
			/**
			* @brief �ص�����
			*/
			void operator()(osg::Node* node, osg::NodeVisitor* nv);			
		private:
			bool positive;
		};

	public:
		/**
		* @brief ���캯��
		*/	
		//CGlbGlobeViewLogo(CGlbGlobeView* pview,int width,int height,CGlbGlobe* pglobe);

		/**
		* @brief���캯��                                                      
		* @param [in] width : ��Ļ���
		*		  [in] height : ��Ļ�߶�
		*		  [in] label : logo ����
		*		  [in] labelColor : logo������ɫ
		*		  [in] image	  : ͼƬ
		*		  [in] bDynamic   : �Ƿ�����ѭ���˶�
		*/
		CGlbGlobeViewLogo(glbInt32 width,glbInt32 height,const glbWChar* label,glbInt32 labelColor,const glbWChar* image, glbBool bDynamic);
		/**
		* @brief ��������
		*/
		~CGlbGlobeViewLogo(void);
		/**
		* @brief ����LOGOλ��
		* @param [in] width : ��Ļ���
		*		  [in] height : ��Ļ�߶�
		*/
		void SetPosition(glbInt32 left,glbInt32 top,glbInt32 width,glbInt32 height);
		/**
		* @brief ����LOGO����
		* @param [in] imageData : ͼƬȫ·��
		*/
		void SetImageLogo(CGlbWString path);
		/**
		* @brief ����LOGO����
		* @param [in] label : ����
		*/
		void SetLabel(const glbWChar* label,glbInt32 labelColor);

	private:
		/**
		* @brief ����LOGO�ڵ�
		*/
		osg::ref_ptr<osg::Geode> createViewLogo();
	private:
		osg::ref_ptr<osg::Geode> mpr_geode;
		glbInt32				 mpr_posLeft;
		glbInt32				 mpr_posTop;
		glbInt32				 mpr_width;
		glbInt32				 mpr_height;
		CGlbWString				 mpr_label;
		glbInt32				 mpr_labelColor;
		CGlbWString				 mpr_imagePath;
		glbBool					 mpr_bInitialized;
	};
}
