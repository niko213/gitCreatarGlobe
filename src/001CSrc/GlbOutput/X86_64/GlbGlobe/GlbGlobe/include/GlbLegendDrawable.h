#ifndef TEXTDRAWABLE_H
#define TEXTDRAWABLE_H
#include<windows.h>
#include<gl/gl.h>
#include<gl/GLU.h>
//#include <gl/glut.h>
#include<osg/Drawable>

/**
* @brief 图例文字绘制
*/
namespace GlbGlobe
{
	class CGlbGlobeLegend;
	class LegendDrawable : public osg::Drawable
	{
	public:
		LegendDrawable()
		{
		}
		LegendDrawable(CGlbGlobeLegend* ubcLgd)
		{
			m_ubcLgd = ubcLgd;

			m_titleSize = 16;
			m_proSize = 14;
			m_valueSize = 12;
		}
		~LegendDrawable()
		{
		}
		LegendDrawable(const LegendDrawable& latlonline, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY) :
		osg::Drawable(latlonline,copyop)
		{
		}
		META_Object(osg, LegendDrawable)
		void drawString( char* str) const;
		void drawWString( wchar_t* wstr) const;
		virtual void drawImplementation(osg::RenderInfo &renderInfo) const;
		void selectFont(int size, int charset, const char* face, bool bBold)const;

		void drawTitleName() const;
		void drawProName()const;
		void drawProValue()const;

	public:
		CGlbGlobeLegend* m_ubcLgd;
		int  m_titleSize;//图例名称字大小
		int  m_proSize;//属性名称字大小
		bool m_bShowTitle;//是否显示图例名称
		int  m_valueSize;//属性值字大小
	};
}
#endif