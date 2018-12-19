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
	* @brief 图例类
	*/
	class GLB_DLLCLASS_EXPORT CGlbGlobeLegend : public CGlbReference
	{
	public:
		CGlbGlobeLegend(glbInt32 vx,glbInt32 vy, glbInt32 vWidth,glbInt32 vHeight, CGlbColorRamp* colorramp);
		~CGlbGlobeLegend();

		/**
		* @brief 图例渲染
		* @param[in]  vx,3D视口的起始位置横坐标.
		* @param[in]  vy,3D视口的起始位置横坐标.
		* @param[in]  vWidth,3D视口的宽度.
		* @param[in]  vHeight,3D视口的高度.
		* return
		*/
		void RenderElement(/*glbInt32 vx,glbInt32 vy, glbInt32 vWidth,glbInt32 vHeight*/);
		/**
		* @brief 设置视口大小和位置
		* @param[in]  vx,3D视口的起始位置横坐标.
		* @param[in]  vy,3D视口的起始位置横坐标.
		* @param[in]  vWidth,3D视口的宽度.
		* @param[in]  vHeight,3D视口的高度.
		* return
		*/
		void SetViewPort(glbInt32 vx,glbInt32 vy, glbInt32 vWidth,glbInt32 vHeight);
		/**
		* @brief 获取视口大小和位置
		* @param[out]  vx,3D视口的起始位置横坐标.
		* @param[out]  vy,3D视口的起始位置横坐标.
		* @param[out]  vWidth,3D视口的宽度.
		* @param[out]  vHeight,3D视口的高度.
		* return
		*/
		glbBool GetViewPort(glbInt32& vx,glbInt32& vy, glbInt32& vWidth,glbInt32& vHeight);
		/**
		* @brief 获取图例节点
		* return 图例节点
		*/
		osg::ref_ptr<osg::Camera> GetOsgNode();

		/**
		* @brief 设置图例尺寸（像素为单位）
		* @param[in]  width,图例宽.
		* @param[in]  height,图例高.
		* return
		*/
		void SetSize(glbInt32 width, glbInt32 height);

		/**
		* @brief 获取图例尺寸（像素为单位）
		* @param[out]  width,图例宽.
		* @param[out]  height,图例高.
		* return
		*/
		void GetSize(glbInt32& width, glbInt32& height);

		/**
		* @brief 设置图例是否在窗口中停靠在GlbDockStyleEnum所定义的位置上
		* @param[in]  bDocked, true 停靠； false 不停靠
		* return
		*/
		void EnableDock(glbBool bDocked);

		/**
		* @brief 获取图例是否在窗口中停靠在GlbDockStyleEnum所定义的位置上
		* return true 停靠； false 不停靠
		*/
		glbBool IsDocked();

		/**
		* @brief 设置停靠位置
		* @param[in]  s, 停靠位置
		* return
		*/
		void SetDockStyle(GlbDockStyleEnum s);

		/**
		* @brief 获取停靠位置
		* @param[out]  s, 停靠位置
		* return 
		*/
		void GetDockStyle(GlbDockStyleEnum& s);

		/**
		* @brief 设置左下角位置(非停靠模式下)
		* @param[in]  leftBottom_x, 图例左下角的X坐标
		* @param[in]  leftBottom_y, 图例左下角的Y坐标
		* return 
		*/
		void SetLBCornerPos(glbDouble leftBottom_x, glbDouble leftBottom_y);

		/**
		* @brief 获取左下角位置(非停靠模式下)
		* @param[out]  leftBottom_x, 图例左下角的X坐标
		* @param[out]  leftBottom_y, 图例左下角的Y坐标
		* return 
		*/
		void GetLBCornerPos(glbDouble& leftBottom_x, glbDouble& leftBottom_y);

		/**
		* @brief 设置图例名称
		* @param[in]  title, 图例名称（默认为"图例"）
		* return 
		*/
		void SetTitle(const glbWChar* title);

		/**
		* @brief 获取图例名称
		* return 图例名称（默认为"图例"）
		*/
		const glbWChar* GetTitle();

		/**
		* @brief 设置属性名称
		* @param[in]  subTitle, 图例属性名称
		* return 
		*/
		void SetSubTitle(const glbWChar* subTitle);

		/**
		* @brief 获取属性名称
		* return 图例属性名称
		*/
		const glbWChar* GetSubTitile();

		/**
		* @brief 设置图例名称字体（暂未实现）
		* @param[in]  fontName, 图例名称的字体名称
		* return 
		*/
		void SetTitleFont(const glbWChar* fontName);

		/**
		* @brief 设置图例名称字号（暂未实现）
		* @param[in]  sz, 图例名称的字体大小
		* return 
		*/
		void SetTitleFontSize(glbDouble sz);
		
		/**
		* @brief 设置图例属性字体（暂未实现）
		* @param[in]  fontName, 图例属性的字体名称
		* return 
		*/
		void SetSublTitleFont(const glbWChar* fontName);

		/**
		* @brief 设置图例属性字号（暂未实现）
		* @param[in]  sz, 图例属性的字体大小
		* return 
		*/
		void SetSubTitleFontSize(glbDouble sz);

		/**
		* @brief 设置单值图例的列数
		* @param[in]  columnCount, 单值图例的列数，默认值为1
		* return 
		*/
		void SetColumnCount(glbInt32 columnCount = 1); 

		/**
		* @brief 获取单值图例的列数
		* return 单值图例的列数，默认值为1
		*/
		glbInt32 GetColumnCount();

		/**
		* @brief 设置列宽是否自定义(单值图例模式下，当单值图例列数大于1时，每列的列宽是否自定义)
		* @param[in]  bUserDefined, true: 用户定义每列的宽度；false：默认均匀分布
		* return 
		*/
		void EnableColumnWidthUserDefined(glbBool bUserDefined);

		/**
		* @brief 获取列宽是否自定义(单值图例模式下，当单值图例列数大于1时，每列的列宽是否自定义)
		* return true: 用户定义每列的宽度；false：默认均匀分布
		*/
		glbBool IsColumnWidthUserDefined();

		/**
		* @brief 添加列宽（单值图例模式下,列宽为自定义时)
		* @param[in]  width, 当前所添加的列宽度
		* return 
		*/
		void AddColumnWidth(glbDouble width);

		/**
		* @brief 清空列宽（单值图例模式下,列宽为自定义时)
		* return 
		*/
		void RemoveAllColumnWidth();

		/**
		* @brief 设置列宽索引为idx的列宽（单值图例模式下,列宽为自定义时)
		* @param[in]  idx, 索引
		* @param[in]  width, 当前索引待设置的列宽
		* return 
		*/
		glbBool SetColumnWidth(glbInt32 idx, glbDouble width);

		/**
		* @brief 获取列宽索引为idx的列宽（单值图例模式下,列宽为自定义时)
		* @param[in]  idx, 索引
		* @param[out]  width, 当前索引待获取的列宽
		* return 获取成功返回true， 获取失败返回false
		*/
		glbBool GetColumnWidth( glbInt32 idx, glbDouble& width );

		/**
		* @brief 获取列宽数据数量
		* return 列宽数量
		*/
		glbInt32 GetColumnWidthCount();

	private:

		/**
		* @brief 创建HUD相机，用以进行图例的渲染
		* return
		*/
		void CreateHUD();

		/**
		* @brief 设置HUD相机视口（根据停靠位置或图例左下角坐标，设置图例HUD相机的视口）
		* return
		*/
		void SetHUDViewPort();

		/**
		* @brief 创建矩形节点
		* @param[in]  pt1, 矩形左下角坐标
		* @param[in]  pt2, 矩形右上角坐标
		* return 新创建的矩形节点
		*/
		osg::ref_ptr<osg::Geometry> CreateRectDrawable(osg::Vec3 pt1, osg::Vec3 pt2);

		/**
		* @brief 创建图例边框
		* @param[in]  pt1, 图例边框左下角坐标
		* @param[in]  pt2, 图例边框右上角坐标
		* return 新创建的图例边框节点
		*/
		osg::ref_ptr<osg::Geometry> CreateFrameDrawable(osg::Vec3 pt1, osg::Vec3 pt2);

		/**
		* @brief 创建色带纹理
		* return
		*/
		void CreateTexture();

		/**
		* @brief 创建【位置，属性值标注】数据，用以图例的数据标注显示
		* return
		*/
		void MakeProLable();

	private:
		osg::ref_ptr<osg::Geode> pGeode;//图例数据总节点
		osg::ref_ptr<osg::Camera> m_pHUDCamera; //图例相机总结点
		osg::ref_ptr<osg::Viewport> m_viewport;//主相机视口
		osg::ref_ptr<osg::Texture1D> m_texture;//图例色带纹理
		osg::ref_ptr<osg::Drawable> legenddraw;//文字绘制节点

	public:
		GlbDockStyleEnum m_dockStyle;//图例位置
		glbBool m_bDocked;//图例是否停靠

		CGlbWString m_lgdName;//图例名称	
		CGlbWString m_lgdPro;//图例属性名称	

		glbInt32 m_width;//图例宽
		glbInt32 m_height;//图例高

		glbDouble m_leftBottom_x;//图例左下角坐标
		glbDouble m_leftBottom_y;

		glbInt32 m_offSet;//图例距离屏幕边缘距离

		glbDouble titleH;//表头所占高度
		glbDouble legend_OffsetW;//色条距左边框的距离
		glbDouble legend_OffsetH;//色条距下边框的距离
		glbDouble colorBar_W;//色条宽
		glbDouble colorBar_H;//色条高

		glbInt32 m_multyLgdColumn;//分段图例的列数（1，2，3，...）
		glbBool m_bColWidthUserDefined;//列宽是否自定义（列数大于1时）
		std::vector<glbDouble> m_ColWidthVec;//自定义列宽

		std::vector<std::pair<pointCoord /*pos*/, CGlbWString /*pro所对应的名称*/> > m_proLable;//图例标注的【位置，名称】
		CGlbColorRamp* m_ramp;//图例的色带数据
	};
}