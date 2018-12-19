#pragma once
#include "glbglobesymbol.h"
#include "GlbGlobeMarker2DShapeSymbol.h"
#include "GlbGlobeAutoTransform.h"
#include "../font_win32_tt/agg_font_win32_tt.h"

// 简单标注符号 用于点像素符号
namespace GlbGlobe
{
	class CGlbGlobeRenderSimpleLabelSymbol : public CGlbReference
	{
	public:
		CGlbGlobeRenderSimpleLabelSymbol(void);

		~CGlbGlobeRenderSimpleLabelSymbol(void);

		osg::Node *Draw(GlbRenderSimpleLabel* info,IGlbGeometry *geo,
											CGlbFeature *feature,CGlbGlobe *globe);
	private:
		void DealText(osg::Vec2d leftUp,CGlbWString content,renb_type &renb,
			GlbRenderTextInfo *textInfo,CGlbFeature *feature);
		std::vector<CGlbWString> DealTextContent(GlbRenderTextInfo *textRenderInfo,
			CGlbFeature *feature,glbDouble &bytes);
		void DealComputeBufferSize(glbInt32 multilineSize,glbInt32 textSize,CGlbWString textFont,
			CGlbWString content,glbInt32 bytes,glbBool isItalic,
			glbInt32 &sizeWidth,glbInt32 &sizeHeight);
		void DealRenderText(glbBool isEmpty,std::vector<CGlbWString> wsRenderContainer,
			CGlbWString content,glbInt32 textSize,GlbRenderTextInfo *textInfo,
			glbInt32 sizeHeight,renb_type &renb,CGlbFeature *feature);
	private:
		agg::font_engine_win32_tt_int16 *mpr_font;
	};
}