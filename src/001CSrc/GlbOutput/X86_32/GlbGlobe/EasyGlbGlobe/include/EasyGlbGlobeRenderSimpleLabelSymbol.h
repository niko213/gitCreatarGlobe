#pragma once
#include "EasyGlbGlobeSymbol.h"
#include "EasyGlbGlobeAutoTransform.h"
#include "../font_win32_tt/agg_font_win32_tt.h"
//AGG
#include "agg_rendering_buffer.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_path_storage.h"
#include "agg_conv_stroke.h"
#include "agg_vcgen_markers_term.h"
#include "agg_renderer_scanline.h"
#include "agg_conv_contour.h"

namespace GlbGlobe
{
	class CEasyGlbGlobeRenderSimpleLabelSymbol : public CGlbReference
	{
	public:
		CEasyGlbGlobeRenderSimpleLabelSymbol(void);
		~CEasyGlbGlobeRenderSimpleLabelSymbol(void);

		osg::Node *Draw(GlbRenderSimpleLabel* info,IGlbGeometry *geo,
			CGlbFeature *feature,CEasyGlbGlobe *globe);
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

