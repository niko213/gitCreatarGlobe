#pragma once
#include <windows.h>
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Texture1D"
#include "osgGA/GUIEventHandler"
#include "osgText/text"
#include "osg/Drawable"
#include "GlbReference.h"
#include "GlbGlobeExport.h"
#include "GlbGlobeTypes.h"
#include "GlbColorRamp.h"

namespace GlbGlobe
{
	/**
	* @brief ͼ����
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeLegend : public CGlbReference
	{
	public:
		CGlbGlobeLegend(glbInt32 vx,glbInt32 vy, glbInt32 vWidth,glbInt32 vHeight, CGlbColorRamp* colorramp);
		~CGlbGlobeLegend();

		/**
		* @brief ͼ����Ⱦ
		* @param[in]  vx,3D�ӿڵ���ʼλ�ú�����.
		* @param[in]  vy,3D�ӿڵ���ʼλ�ú�����.
		* @param[in]  vWidth,3D�ӿڵĿ��.
		* @param[in]  vHeight,3D�ӿڵĸ߶�.
		* return
		*/
		void RenderElement(/*glbInt32 vx,glbInt32 vy, glbInt32 vWidth,glbInt32 vHeight*/);
		/**
		* @brief �����ӿڴ�С��λ��
		* @param[in]  vx,3D�ӿڵ���ʼλ�ú�����.
		* @param[in]  vy,3D�ӿڵ���ʼλ�ú�����.
		* @param[in]  vWidth,3D�ӿڵĿ��.
		* @param[in]  vHeight,3D�ӿڵĸ߶�.
		* return
		*/
		void SetViewPort(glbInt32 vx,glbInt32 vy, glbInt32 vWidth,glbInt32 vHeight);
		/**
		* @brief ��ȡ�ӿڴ�С��λ��
		* @param[out]  vx,3D�ӿڵ���ʼλ�ú�����.
		* @param[out]  vy,3D�ӿڵ���ʼλ�ú�����.
		* @param[out]  vWidth,3D�ӿڵĿ��.
		* @param[out]  vHeight,3D�ӿڵĸ߶�.
		* return
		*/
		glbBool GetViewPort(glbInt32& vx,glbInt32& vy, glbInt32& vWidth,glbInt32& vHeight);
		/**
		* @brief ��ȡͼ���ڵ�
		* return ͼ���ڵ�
		*/
		osg::ref_ptr<osg::Camera> GetOsgNode();

		/**
		* @brief ����ͼ���ߴ磨����Ϊ��λ��
		* @param[in]  width,ͼ����.
		* @param[in]  height,ͼ����.
		* return
		*/
		void SetSize(glbInt32 width, glbInt32 height);

		/**
		* @brief ��ȡͼ���ߴ磨����Ϊ��λ��
		* @param[out]  width,ͼ����.
		* @param[out]  height,ͼ����.
		* return
		*/
		void GetSize(glbInt32& width, glbInt32& height);

		/**
		* @brief ����ͼ���Ƿ��ڴ�����ͣ����GlbDockStyleEnum�������λ����
		* @param[in]  bDocked, true ͣ���� false ��ͣ��
		* return
		*/
		void EnableDock(glbBool bDocked);

		/**
		* @brief ��ȡͼ���Ƿ��ڴ�����ͣ����GlbDockStyleEnum�������λ����
		* return true ͣ���� false ��ͣ��
		*/
		glbBool IsDocked();

		/**
		* @brief ����ͣ��λ��
		* @param[in]  s, ͣ��λ��
		* return
		*/
		void SetDockStyle(GlbDockStyleEnum s);

		/**
		* @brief ��ȡͣ��λ��
		* @param[out]  s, ͣ��λ��
		* return 
		*/
		void GetDockStyle(GlbDockStyleEnum& s);

		/**
		* @brief �������½�λ��(��ͣ��ģʽ��)
		* @param[in]  leftBottom_x, ͼ�����½ǵ�X����
		* @param[in]  leftBottom_y, ͼ�����½ǵ�Y����
		* return 
		*/
		void SetLBCornerPos(glbDouble leftBottom_x, glbDouble leftBottom_y);

		/**
		* @brief ��ȡ���½�λ��(��ͣ��ģʽ��)
		* @param[out]  leftBottom_x, ͼ�����½ǵ�X����
		* @param[out]  leftBottom_y, ͼ�����½ǵ�Y����
		* return 
		*/
		void GetLBCornerPos(glbDouble& leftBottom_x, glbDouble& leftBottom_y);

		/**
		* @brief ����ͼ������
		* @param[in]  title, ͼ�����ƣ�Ĭ��Ϊ"ͼ��"��
		* return 
		*/
		void SetTitle(const glbWChar* title);

		/**
		* @brief ��ȡͼ������
		* return ͼ�����ƣ�Ĭ��Ϊ"ͼ��"��
		*/
		const glbWChar* GetTitle();

		/**
		* @brief ������������
		* @param[in]  subTitle, ͼ����������
		* return 
		*/
		void SetSubTitle(const glbWChar* subTitle);

		/**
		* @brief ��ȡ��������
		* return ͼ����������
		*/
		const glbWChar* GetSubTitile();

		/**
		* @brief ����ͼ���������壨��δʵ�֣�
		* @param[in]  fontName, ͼ�����Ƶ���������
		* return 
		*/
		void SetTitleFont(const glbWChar* fontName);

		/**
		* @brief ����ͼ�������ֺţ���δʵ�֣�
		* @param[in]  sz, ͼ�����Ƶ������С
		* return 
		*/
		void SetTitleFontSize(glbDouble sz);
		
		/**
		* @brief ����ͼ���������壨��δʵ�֣�
		* @param[in]  fontName, ͼ�����Ե���������
		* return 
		*/
		void SetSublTitleFont(const glbWChar* fontName);

		/**
		* @brief ����ͼ�������ֺţ���δʵ�֣�
		* @param[in]  sz, ͼ�����Ե������С
		* return 
		*/
		void SetSubTitleFontSize(glbDouble sz);

		/**
		* @brief ���õ�ֵͼ��������
		* @param[in]  columnCount, ��ֵͼ����������Ĭ��ֵΪ1
		* return 
		*/
		void SetColumnCount(glbInt32 columnCount = 1); 

		/**
		* @brief ��ȡ��ֵͼ��������
		* return ��ֵͼ����������Ĭ��ֵΪ1
		*/
		glbInt32 GetColumnCount();

		/**
		* @brief �����п��Ƿ��Զ���(��ֵͼ��ģʽ�£�����ֵͼ����������1ʱ��ÿ�е��п��Ƿ��Զ���)
		* @param[in]  bUserDefined, true: �û�����ÿ�еĿ�ȣ�false��Ĭ�Ͼ��ȷֲ�
		* return 
		*/
		void EnableColumnWidthUserDefined(glbBool bUserDefined);

		/**
		* @brief ��ȡ�п��Ƿ��Զ���(��ֵͼ��ģʽ�£�����ֵͼ����������1ʱ��ÿ�е��п��Ƿ��Զ���)
		* return true: �û�����ÿ�еĿ�ȣ�false��Ĭ�Ͼ��ȷֲ�
		*/
		glbBool IsColumnWidthUserDefined();

		/**
		* @brief ����п���ֵͼ��ģʽ��,�п�Ϊ�Զ���ʱ)
		* @param[in]  width, ��ǰ����ӵ��п��
		* return 
		*/
		void AddColumnWidth(glbDouble width);

		/**
		* @brief ����п���ֵͼ��ģʽ��,�п�Ϊ�Զ���ʱ)
		* return 
		*/
		void RemoveAllColumnWidth();

		/**
		* @brief �����п�����Ϊidx���п���ֵͼ��ģʽ��,�п�Ϊ�Զ���ʱ)
		* @param[in]  idx, ����
		* @param[in]  width, ��ǰ���������õ��п�
		* return 
		*/
		glbBool SetColumnWidth(glbInt32 idx, glbDouble width);

		/**
		* @brief ��ȡ�п�����Ϊidx���п���ֵͼ��ģʽ��,�п�Ϊ�Զ���ʱ)
		* @param[in]  idx, ����
		* @param[out]  width, ��ǰ��������ȡ���п�
		* return ��ȡ�ɹ�����true�� ��ȡʧ�ܷ���false
		*/
		glbBool GetColumnWidth( glbInt32 idx, glbDouble& width );

		/**
		* @brief ��ȡ�п���������
		* return �п�����
		*/
		glbInt32 GetColumnWidthCount();

	private:

		/**
		* @brief ����HUD��������Խ���ͼ������Ⱦ
		* return
		*/
		void CreateHUD();

		/**
		* @brief ����HUD����ӿڣ�����ͣ��λ�û�ͼ�����½����꣬����ͼ��HUD������ӿڣ�
		* return
		*/
		void SetHUDViewPort();

		/**
		* @brief �������νڵ�
		* @param[in]  pt1, �������½�����
		* @param[in]  pt2, �������Ͻ�����
		* return �´����ľ��νڵ�
		*/
		osg::ref_ptr<osg::Geometry> CreateRectDrawable(osg::Vec3 pt1, osg::Vec3 pt2);

		/**
		* @brief ����ͼ���߿�
		* @param[in]  pt1, ͼ���߿����½�����
		* @param[in]  pt2, ͼ���߿����Ͻ�����
		* return �´�����ͼ���߿�ڵ�
		*/
		osg::ref_ptr<osg::Geometry> CreateFrameDrawable(osg::Vec3 pt1, osg::Vec3 pt2);

		/**
		* @brief ����ɫ������
		* return
		*/
		void CreateTexture();

		/**
		* @brief ������λ�ã�����ֵ��ע�����ݣ�����ͼ�������ݱ�ע��ʾ
		* return
		*/
		void MakeProLable();

	private:
		osg::ref_ptr<osg::Geode> pGeode;//ͼ�������ܽڵ�
		osg::ref_ptr<osg::Camera> m_pHUDCamera; //ͼ������ܽ��
		osg::ref_ptr<osg::Viewport> m_viewport;//������ӿ�
		osg::ref_ptr<osg::Texture1D> m_texture;//ͼ��ɫ������
		osg::ref_ptr<osg::Drawable> legenddraw;//���ֻ��ƽڵ�

	public:
		GlbDockStyleEnum m_dockStyle;//ͼ��λ��
		glbBool m_bDocked;//ͼ���Ƿ�ͣ��

		CGlbWString m_lgdName;//ͼ������	
		CGlbWString m_lgdPro;//ͼ����������	

		glbInt32 m_width;//ͼ����
		glbInt32 m_height;//ͼ����

		glbDouble m_leftBottom_x;//ͼ�����½�����
		glbDouble m_leftBottom_y;

		glbInt32 m_offSet;//ͼ��������Ļ��Ե����

		glbDouble titleH;//��ͷ��ռ�߶�
		glbDouble legend_OffsetW;//ɫ������߿�ľ���
		glbDouble legend_OffsetH;//ɫ�����±߿�ľ���
		glbDouble colorBar_W;//ɫ����
		glbDouble colorBar_H;//ɫ����

		glbInt32 m_multyLgdColumn;//�ֶ�ͼ����������1��2��3��...��
		glbBool m_bColWidthUserDefined;//�п��Ƿ��Զ��壨��������1ʱ��
		std::vector<glbDouble> m_ColWidthVec;//�Զ����п�

		std::vector<std::pair<pointCoord /*pos*/, CGlbWString /*pro����Ӧ������*/> > m_proLable;//ͼ����ע�ġ�λ�ã����ơ�
		CGlbColorRamp* m_ramp;//ͼ����ɫ������
	};
}