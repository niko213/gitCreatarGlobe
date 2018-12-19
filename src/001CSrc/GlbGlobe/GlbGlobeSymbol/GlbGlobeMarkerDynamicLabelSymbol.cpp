#include "StdAfx.h"
#include "GlbGlobeMarkerDynamicLabelSymbol.h"
#include "osg/Billboard"
#include "osg/Texture2D"
#include "osgDB/ReadFile"
#include "osg/BlendColor"
#include <osgDB/WriteFile>
#include "GlbString.h"

#include "agg_renderer_scanline.h"
#include "agg_scanline_u.h"
#include "agg_scanline_p.h"
#include "platform/win32/agg_win32_bmp.h"
#include "agg_pixfmt_rgb.h"
#include "agg_span_allocator.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_gradient_alpha.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_span_converter.h"
#include "agg_basics.h"

#include "GlbPoint.h"
#include "CGlbGlobe.h"
#include "GlbGlobeView.h"

#include "platform/agg_platform_support.h"
#include "util/agg_color_conv.h"
#include "util/agg_color_conv_rgb8.h"
#include <agg_conv_dash.h>

#include "osg/BlendFunc"
#include "osg/TexEnv"
#include "osg/Material"

#include "Drawer2D.h"
#include "agg_span_image_filter_rgba.h"
#include "agg_image_accessors.h"
#include "agg_renderer_base.h"
#include "agg_pixfmt_rgba.h"

#include <osg/Depth>
#include "GlbLog.h"

using namespace GlbGlobe;
#define WIDTHBYTES(bites) (((bites) + 31) / 32 * 4);

CGlbGlobeMarkerDynamicLabelSymbol::CGlbGlobeMarkerDynamicLabelSymbol(void)
{
}

CGlbGlobeMarkerDynamicLabelSymbol::~CGlbGlobeMarkerDynamicLabelSymbol(void)
{
}

osg::Node * CGlbGlobeMarkerDynamicLabelSymbol::Draw( CGlbGlobeRObject *obj,IGlbGeometry *geo )
{
	glbref_ptr<GlbRenderInfo> renderInfo = obj->GetRenderInfo();
	GlbMarkerDynamicLabelSymbolInfo *labelInfo = 
		static_cast<GlbMarkerDynamicLabelSymbolInfo *>(renderInfo.get());
	if(labelInfo == NULL)
		return NULL;
	if(labelInfo->textInfo == NULL && labelInfo->imageInfo == NULL)
		return NULL;
	mpr_rObj = obj;
	CGlbFeature *feature = obj->GetFeature();
	mpr_fadeMode = (GlbGlobeDynamicLabelFadeTextEnum)labelInfo->fadeMode->GetValue(feature);

	CGlbGlobe *globe = obj->GetGlobe();
	if(NULL == globe) return NULL;
	osg::MatrixTransform *group = new osg::MatrixTransform;

	int sizeWidth = 0,sizeHeight = 0;
	std::vector<CGlbWString> wsRenderContainer;
	glbDouble bytes = 0.0;//记录对齐方式参考字节数
	glbInt32 textSize = 0;
	CGlbWString content = L"";
	CGlbWString textFont = L"宋体";
	GlbGlobeMultilineAlignTypeEnum multilineAlign = GLB_MULTILINEALG_LEFT;
	glbBool isBold = false,isItalic = false,isUnderline = false;
	if(labelInfo->textInfo)
	{
		if(labelInfo->textInfo->size)
			textSize = labelInfo->textInfo->size->GetValue(feature);
		if(labelInfo->textInfo->content)
			content = labelInfo->textInfo->content->GetValue(feature);
		if(labelInfo->textInfo->font)
			textFont = labelInfo->textInfo->font->GetValue(feature);
		if(labelInfo->textInfo->multilineAlign)
			multilineAlign = (GlbGlobeMultilineAlignTypeEnum)labelInfo->textInfo->multilineAlign->GetValue(feature);

		if(labelInfo->textInfo->isBold)
			isBold = labelInfo->textInfo->isBold->GetValue(feature);//粗体
		if(labelInfo->textInfo->isItalic)
			isItalic = labelInfo->textInfo->isItalic->GetValue(feature);//斜体
		if(labelInfo->textInfo->isUnderline)
			isUnderline = labelInfo->textInfo->isUnderline->GetValue(feature);//下划线
	}

	glbInt32 textCount = content.size();
	if(textCount == 0)
		return NULL;

	//处理多行文本对齐以及单行最大长度
	if(labelInfo->textInfo)
		wsRenderContainer = DealMultilineAlignText(content,multilineAlign,textFont,textSize,bytes);

	//确定没有特殊样式前的整体文字宽高
	glbDouble x = 0.0,y = 0.0;
	if(wsRenderContainer.size() == 1)
	{
		x = bytes;
		y += textSize;
	}
	else
	{
		x = bytes;
		y = textSize * wsRenderContainer.size();
	}

	osg::ref_ptr<osg::Image> tempImage = NULL;
	if(labelInfo->imageInfo && labelInfo->imageInfo->dataSource)
	{
		CGlbWString datalocate = labelInfo->imageInfo->dataSource->GetValue(feature);
		glbInt32 index = datalocate.find_first_of(L'.');
		if(index == 0)
		{// 处理当前执行文件的相对路径情况 ./AAA/....
			CGlbWString execDir = CGlbPath::GetExecDir();
			datalocate = execDir + datalocate.substr(1,datalocate.size());
		}
		osg::ref_ptr<osgDB::Options> op = new osgDB::Options();
		op->setObjectCacheHint(osgDB::Options::CACHE_NONE);
		tempImage = osgDB::readImageFile(datalocate.ToString(),op.get());
		if(tempImage)
		{
			sizeWidth = x;
			sizeHeight = y;
		}
		else if (datalocate.length() > 0)
		{
			GlbLogWOutput(GLB_LOGTYPE_ERR,L"读取标注图像文件(%s)失败. \r\n",datalocate.c_str());
		}				
	}
	mpr_texImage = tempImage;

	//根据文本特殊效果延展文字整体的宽高
	DealBufferSizeExtend(textSize,isBold,isItalic,isUnderline,tempImage,x,y,sizeWidth,sizeHeight);
	//根据image的maxSize,调整背景图片的宽高且只处理图片
	if(labelInfo->imageInfo)
		DealExtendImageByMaxSize(labelInfo->imageInfo,feature,sizeWidth,sizeHeight);

	glbBool isLimit = false;
	glbDouble ratio = 1.0;
	if(labelInfo->geoInfo->isLimit)
		isLimit = labelInfo->geoInfo->isLimit->GetValue(feature);
	if(labelInfo->geoInfo->ratio)
		ratio = labelInfo->geoInfo->ratio->GetValue(feature);

	int bits = 0;
	unsigned char * buffer  = NULL;
	bits = WIDTHBYTES(sizeWidth * 32);
	bits *= sizeHeight;
	buffer = new unsigned char[bits];
	mpr_aggBuffer = buffer;
	memset(buffer,0,sizeof(unsigned char) * sizeWidth * sizeHeight * 4);
	agg::rendering_buffer rbuf(buffer,sizeWidth,sizeHeight,-sizeWidth * 4);
	//agg::rendering_buffer rbuf(tempImage->data(),sizeWidth,sizeHeight,-sizeWidth * 4);

	agg::pixfmt_rgba32 pixf(rbuf);
	renb_type renb(pixf);

	glbInt32 textbackColor = 0xFFFFFFFF;
	glbInt32 textbackOpacity = 100;
	if(labelInfo->textInfo->backColor)
		textbackColor = labelInfo->textInfo->backColor->GetValue(feature);
	if(labelInfo->textInfo->backOpacity)
		textbackOpacity = labelInfo->textInfo->backOpacity->GetValue(feature);

	if(tempImage)
		renb.clear(agg::rgba(1.0,1.0,1.0,0.5));
	else
		renb.clear(agg::rgba(0.0,0.0,0.0,0.0));

	if(0xFFFFFFFF != textbackColor && textbackOpacity != 0)
	{
		osg::Vec4 backColor = GetColor(textbackColor);
		if(tempImage)
			renb.clear(agg::rgba(backColor.x(), backColor.y(), backColor.z(),textbackOpacity / 200.0));
		else
			renb.clear(agg::rgba(backColor.x(), backColor.y(), backColor.z(),textbackOpacity / 100.0));
	}

	if(labelInfo && (labelInfo->imageInfo || labelInfo->textInfo))
		DealRenderImageAndText(wsRenderContainer,wsRenderContainer.size(),
		content,bytes,textSize,tempImage,labelInfo->imageInfo,labelInfo->textInfo,
		sizeWidth,sizeHeight,renb,buffer,feature);

	ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	osg::ref_ptr<GlbGlobeAutoTransform> autoTransform = new GlbGlobeAutoTransform(globe);
	GlbGlobeBillboardModeEnum mode = GLB_BILLBOARD_SCREEN;
	if(labelInfo->mode)
	{
		mode = (GlbGlobeBillboardModeEnum)labelInfo->mode->GetValue(feature);
		if (mode == GLB_BILLBOARD_SCREEN)//屏幕
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if (mode == GLB_BILLBOARD_AXIS)//轴向
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(mode == GLB_BILLBOARD_AUTOSLOPE)//自动旋转
		{
			autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			autoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			autoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
	}

	GlbGlobeLabelAlignTypeEnum positionAlign = GLB_LABELALG_LEFTBOTTOM;
	if(labelInfo->labelAlign)
		positionAlign = (GlbGlobeLabelAlignTypeEnum)labelInfo->labelAlign->GetValue(feature);

	osg::ref_ptr<osg::Geometry> labelGeom = NULL;
	osg::Vec3 corner(0.0,0.0,0.0);////GLB_LABELALG_LEFTBOTTOM
	osg::Vec3 width,height;

	if (!isLimit)
	{
		width = osg::Vec3(sizeWidth * ratio,0.0,0.0);
		height = osg::Vec3(0.0,sizeHeight * ratio,0.0);
		corner = ComputeCornerByAlign(sizeWidth,sizeHeight,ratio,positionAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
	}
	else
	{
		width = osg::Vec3(sizeWidth/* * ratio*/,0.0,0.0);
		height = osg::Vec3(0.0,sizeHeight/* * ratio*/,0.0);
		corner = ComputeCornerByAlign(sizeWidth,sizeHeight,1.0,positionAlign);
		labelGeom = osg::createTexturedQuadGeometry(corner,width,height);
		autoTransform->setAutoScaleToScreen(true);
		autoTransform->setMinimumScale(0.0);
		//if(-1 == maxSize)
		//	autoTransform->setMaximumScale(DBL_MAX);
		//else
		autoTransform->setMaximumScale(/*maxSize * */ratio);
	}

	//osg::ref_ptr<osg::Material> material = dynamic_cast<osg::Material *>(stateset->getAttribute(osg::StateAttribute::MATERIAL));
	//if(material == NULL)
	//	material = new osg::Material;
	//material->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,1,1));
	//material->setAlpha(osg::Material::FRONT_AND_BACK,1.0);
	//stateset->setAttribute(material,osg::StateAttribute::ON);
	//透明度
	osg::ref_ptr<osg::BlendFunc> pBlendFun = new osg::BlendFunc(osg::BlendFunc::ONE/*SRC_ALPHA*/,osg::BlendFunc::ONE_MINUS_SRC_ALPHA);	//设置混合方程
	stateset->setAttributeAndModes(pBlendFun.get(),osg::StateAttribute::ON);
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	stateset->setMode( GL_NORMALIZE, osg::StateAttribute::ON );

	osg::ref_ptr<osg::Depth> depth = dynamic_cast<osg::Depth *>(stateset->getAttribute(osg::StateAttribute::DEPTH));
	if(!depth.valid())
	{
		depth = new osg::Depth;
		depth->setWriteMask(false);
		stateset->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);
	}
	else
	{
		if(depth->getWriteMask())
			depth->setWriteMask(false);
	}

	//osg::ref_ptr<osg::TexEnv> texenv=new osg::TexEnv;
	//texenv->setMode(osg::TexEnv::MODULATE);
	////设置纹理环境
	//stateset->setTextureAttribute(0,texenv.get());
	labelGeom->setStateSet(stateset);

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geode> outGeode = new osg::Geode;
	osg::ref_ptr<GlbGlobeAutoTransform> outAutoTransform = new GlbGlobeAutoTransform(globe);
	//内框
	osg::ref_ptr<osg::Geometry> lineGeometry = CreateOutlineQuad(corner,width,height);
	geode->addDrawable(labelGeom);
	//geode->addDrawable(lineGeometry);
	autoTransform->addChild(geode);
	group->addChild(autoTransform);
	//添加外框
	if(isLimit)
	{
		osg::ref_ptr<osg::Geometry> outlineGeometry = CreateOutlineQuad(corner,width,height);
		if(NULL == outlineGeometry) return group;
		//outGeode->addDrawable(outlineGeometry);
		outAutoTransform->addChild(outGeode);
		if(GLB_BILLBOARD_SCREEN == mode)
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
		else if(GLB_BILLBOARD_AXIS == mode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(0.0f,0.0f,1.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		else if(GLB_BILLBOARD_AUTOSLOPE == mode)
		{
			outAutoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_AXIS);
			outAutoTransform->setAxis(osg::Vec3(1.0f,0.0f,0.0f));
			outAutoTransform->setNormal(osg::Vec3(0.0f,-1.0f,0.0f));
		}
		outAutoTransform->setAutoScaleToScreen(true);
		outAutoTransform->setMinimumScale(DBL_MAX);
		outAutoTransform->setMaximumScale(/*maxSize * */ratio);
		group->addChild(outAutoTransform);
	}

	glbDouble xOffset = 0.0,yOffset = 0.0,zOffset = 0.0;
	if(labelInfo->xOffset && labelInfo->yOffset && labelInfo->zOffset)
	{
		xOffset = labelInfo->xOffset->GetValue(feature);
		yOffset = labelInfo->yOffset->GetValue(feature);
		zOffset = labelInfo->zOffset->GetValue(feature);
		osg::Matrix m;
		m.makeTranslate(xOffset,yOffset,zOffset);
		group->setMatrix(m);
	}

	return group;
}

void CGlbGlobeMarkerDynamicLabelSymbol::DealImage(osg::Vec2d leftUp,
	osg::Vec2d rightDown,osg::Image *image,renb_type &renb
	,unsigned char * buffer,GlbRenderImageInfo *imageInfo,CGlbFeature *feature)
{
	if(imageInfo == NULL || image == NULL)
		return;

	glbInt32 imageColor = 0xFFFFFFFF;
	glbInt32 imageOpacity = 100;
	if(imageInfo->color)
		imageColor = imageInfo->color->GetValue(feature);
	if(imageInfo->opacity)
		imageOpacity = imageInfo->opacity->GetValue(feature);

	unsigned char* pdata = NULL;
	agg::int8u apha(imageOpacity * 255 / 100.0);//图片透明度
	typedef agg::renderer_base<agg::pixfmt_rgba32> renderer_base_type;
	typedef agg::renderer_scanline_aa_solid<renderer_base_type> renderer_scanline_type;
	renderer_scanline_type rensl(renb);
	agg::rasterizer_scanline_aa<> ras;
	agg::scanline_u8 sl;

	if(image)
	{
		//stride应该是width的倍数
		agg::rendering_buffer rbuf_img(NULL,0,0,0);
		GLenum dataType = (GLenum)image->getPixelFormat();

		if (dataType == GL_RGB)
		{
			rbuf_img = agg::rendering_buffer(image->data(),
				image->s(),image->t(), - 3 * image->s());
		}
		else if (dataType == GL_RGBA)
		{
			int width = image->s();
			int height = image->t();
			int cnt = image->s() * image->t() * 3;
			pdata = new unsigned char[cnt];
			unsigned char *data = image->data();
			for(int i = 0; i <image->t();i++)
				for(int j =0; j < image->s(); j++)
				{
					unsigned char* r = data + ( i * image->s() + j) * 4;
					unsigned char* g = data + ( i * image->s() + j) * 4 + 1;
					unsigned char* b = data + ( i * image->s() + j) * 4 + 2;
					unsigned char* a = data + ( i * image->s() + j) * 4 + 3;
					//if(*a != 255)
					//	int a = 1;

					pdata[(i * image->s() + j) * 3] = *r;
					pdata[(i * image->s() + j) * 3 + 1] = *g;
					pdata[(i * image->s() + j) * 3 + 2] = *b;
				}
				rbuf_img = agg::rendering_buffer(pdata,image->s(),image->t(), - 3 * image->s());
		}

		agg::pixfmt_rgba32 pixf_img(rbuf_img);
		typedef agg::span_allocator<agg::rgba8> span_allocator_type;
		span_allocator_type span_alloc;
		typedef agg::span_interpolator_linear<> interpolator_type;
		agg::trans_affine img_mtx;
		interpolator_type ip_img(img_mtx);

		typedef std::vector<agg::int8u> alphaF_type;
		alphaF_type alphaF(image->s());
		for (int i = 0; i < image->s();i++)
			alphaF[i] = apha;

		//if (labelRelation == GLB_LABEL_SURROUND_IMAGE)
		img_mtx.translate(leftUp.x(),leftUp.y());
		img_mtx.invert();

		agg::trans_affine alpha_mtx;
		interpolator_type ip_alpha(alpha_mtx);
		typedef agg::gradient_x gradientF_type;
		gradientF_type grF;

		typedef agg::span_gradient_alpha<agg::rgba8,
			interpolator_type,gradientF_type,alphaF_type> alpha_span_gen_type;
		alpha_span_gen_type alpha_span_gen(ip_alpha,grF,alphaF,0,150);

		typedef agg::span_image_filter_rgb_bilinear_clip<agg::pixfmt_rgba32,
			interpolator_type> pic_span_gen_type;
		pic_span_gen_type pic_span_gen(pixf_img,agg::rgba(1,1,1,1),ip_img);

		typedef agg::span_converter<pic_span_gen_type,
			alpha_span_gen_type> span_gen_type;
		span_gen_type span_gen(pic_span_gen,alpha_span_gen);

		agg::renderer_scanline_aa<renderer_base_type,
			span_allocator_type,span_gen_type> my_renderer(renb,span_alloc,span_gen);

		ras.move_to_d(leftUp.x(),leftUp.y()); 
		ras.line_to_d(leftUp.x(), rightDown.y()); 
		ras.line_to_d(rightDown.x(), rightDown.y()); 
		ras.line_to_d(rightDown.x(), leftUp.y()); 
		agg::render_scanlines(ras,sl,my_renderer);

		if(pdata)
		{
			delete[] pdata;
			pdata = NULL;
		}

		//用agg渲染透明面，使渲染的可以设置图片颜色
		agg::rasterizer_scanline_aa<> polygonRas; 
		agg::path_storage ps;
		agg::trans_affine mtx;
		typedef agg::conv_transform<agg::path_storage> ell_ct_type;
		ell_ct_type ctell(ps,mtx);
		ps.move_to(leftUp.x(),leftUp.y());
		ps.line_to(leftUp.x(), rightDown.y());
		ps.line_to(rightDown.x(), rightDown.y());
		ps.line_to(rightDown.x(), leftUp.y());
		polygonRas.add_path(ctell);
		osg::Vec4 polygonColor = GetColor(imageColor);

		if(imageColor != 0xFFFFFFFF)
			agg::render_scanlines_aa_solid(polygonRas,sl,renb,
			agg::rgba8(polygonColor.r() * 255,polygonColor.g() * 255,
			polygonColor.b() * 255,apha * 0.5));//图片透明度

		unsigned width = renb.width();
		unsigned height = renb.height();
		if(dataType == GL_RGBA)
		{//还原透明通道
			unsigned char *data = image->data();
			int offset = height - leftUp.y();
			int row = 0;
			for(int x = leftUp.y(); x < leftUp.y() + image->t();x++)
			{
				int col = 0;
				for(int y = leftUp.x(); y < leftUp.x() + image->s();y++)
				{
					unsigned char* a = data + ((image->t()-1-row) * image->s() + col ) * 4 + 3;
					buffer[((height-1-x) * width + y) * 4 + 3] = *a * imageOpacity / 100.0;
					col++;
				}
				row++;
			}
		}
	}
}

void CGlbGlobeMarkerDynamicLabelSymbol::DealText(osg::Vec2d leftUp,CGlbWString content,
	renb_type &renb,GlbRenderTextInfo *textInfo,CGlbFeature *feature)
{
	if(textInfo == NULL || content.size() == 0)
		return;

	glbInt32 textFontColor = 0x00000000;
	if(textInfo->color)
		textFontColor = textInfo->color->GetValue(feature);

	CGlbWString textFont = L"黑体";
	glbInt32 textSize = 1;
	if(textInfo->font)
		textFont = textInfo->font->GetValue(feature);//字体
	if(textInfo->size)
		textSize = textInfo->size->GetValue(feature);
	glbBool isBold = false,isItalic = false,isUnderline = false;
	if(textInfo->isBold)
		isBold = textInfo->isBold->GetValue(feature);//粗体
	if(textInfo->isItalic)
		isItalic = textInfo->isItalic->GetValue(feature);//斜体
	if(textInfo->isUnderline)
		isUnderline = textInfo->isUnderline->GetValue(feature);//下划线

	agg::rasterizer_scanline_aa<> ras;  
	agg::scanline_p8 sl; 

	osg::Vec4 textColor = GetColor(textFontColor);
	agg::font_engine_win32_tt_int16 font(::GetDC(::GetActiveWindow())); 
	agg::font_cache_manager< agg::font_engine_win32_tt_int16 > font_manager(font); 
	font.height(textSize); //字体高度
	font.width(0); //字体宽度

	if(isItalic)
		font.italic(true); //斜体
	font.flip_y(true); //上下翻转
	font.hinting(false); //字体修正
	if (isBold)
		font.weight(100000);//文字加粗
	font.create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8); 

	//根据图片与文字的轴模式设置起始位置
	double x = leftUp.x(),y = leftUp.y();
	double beginX = x,beginY = y;
	const wchar_t *text = content.c_str();
	bool glyphAvailable = false;

	for(;*text;text++) 
	{ 
		//取字模 
		const agg::glyph_cache* glyph = font_manager.glyph(*text);
		if(glyph) 
		{ 
			// 初始化gray8_adaptor实例 
			font_manager.init_embedded_adaptors(glyph, x, y); 
			agg::render_scanlines_aa_solid(font_manager.gray8_adaptor(),
				font_manager.gray8_scanline(), renb, agg::rgba8(textColor.r() * 255,
				textColor.g() * 255, textColor.b() * 255, textColor.a() * 255)); 

			if (*text == ' ')
			{
				beginX += glyph->advance_x;
				beginY += glyph->advance_y;
			}
			// 前进 
			x += glyph->advance_x; 
			y += glyph->advance_y; 
		} 
	}

	if(isUnderline)
	{
		agg::path_storage ps;
		ps.move_to(beginX,beginY + 2);//下划线总是遮挡字，把下划线往下移两个线宽
		ps.line_to(x,y + 2);
		agg::conv_stroke<agg::path_storage, agg::vcgen_markers_term> csps(ps); 
		csps.width(2);
		ras.add_path(csps);
		agg::render_scanlines_aa_solid(ras,sl,renb,
			agg::rgba8(textColor.r() * 255,textColor.g() * 255,textColor.b() * 255,textColor.a() * 255));
	}
}

std::vector<CGlbWString> GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::DealMultilineAlignText(
	CGlbWString content,GlbGlobeMultilineAlignTypeEnum multilineAlign,CGlbWString textFont,glbInt32 textSize,glbDouble &bytes)
{
	std::vector<CGlbWString> wsContainer;

	glbInt32 textCount = content.size();
	std::vector<int> cutContainer;
	for (int i = 0; i < textCount;++i)
	{
		wchar_t wchar = content.at(i);
		if (wchar == '\n' || wchar == '~')
			cutContainer.push_back(i + 1);
	}

	if(content.at(content.size() - 1) != '\n' && cutContainer.size() > 0)
		cutContainer.push_back(textCount);

	glbInt32 tempCount = cutContainer.size();
	//没有多行的时候
	if(tempCount < 1)
	{
		wsContainer.push_back(content);

		//计算单行时,字符串宽度
		agg::font_engine_win32_tt_int16 font(::GetDC(::GetActiveWindow())); 
		font.height(textSize); //字体高度
		font.width(0); //字体宽度
		font.create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8); 
		agg::font_cache_manager< agg::font_engine_win32_tt_int16 > font_manager(font); 
		glbDouble x = 0.0,y = 0.0;
		const wchar_t *text = content.c_str();
		for(;*text;text++) 
		{
			//取字模 
			const agg::glyph_cache* glyph = font_manager.glyph(*text);
			if(glyph) 
			{
				// 初始化gray8_adaptor实例 
				font_manager.init_embedded_adaptors(glyph, x, y); 
				x += glyph->advance_x; 
				y += glyph->advance_y; 
			} 
		}
		bytes = x;

		return wsContainer;
	}

	int j = cutContainer.size()/* - 1*/;
	for (int i = 0; i < j;++i)
	{
		if (i == 0)
			wsContainer.push_back(content.substr(0,cutContainer.at(i) - 1));
		else if(i == j - 1)
			wsContainer.push_back(content.substr(cutContainer.at(i - 1),cutContainer.at(i) - cutContainer.at(i - 1)));
		else
			wsContainer.push_back(content.substr(cutContainer.at(i - 1),cutContainer.at(i) - cutContainer.at(i - 1) - 1));
	}

	agg::font_engine_win32_tt_int16 font(::GetDC(::GetActiveWindow())); 
	font.height(textSize); //字体高度
	font.width(0); //字体宽度
	font.create_font(textFont.ToString().c_str(),agg::glyph_ren_agg_gray8); 
	agg::font_cache_manager< agg::font_engine_win32_tt_int16 > font_manager(font); 

	for(glbInt32 i = 0; i < wsContainer.size();++i)
	{
		glbDouble x = 0.0,y = 0.0;
		CGlbWString tempContent = wsContainer.at(i);
		const wchar_t *text = tempContent.c_str();
		for(;*text;text++) 
		{
			//取字模 
			const agg::glyph_cache* glyph = font_manager.glyph(*text);
			if(glyph) 
			{
				// 初始化gray8_adaptor实例 
				font_manager.init_embedded_adaptors(glyph, x, y); 
				x += glyph->advance_x; 
				y += glyph->advance_y; 
			} 
		}
		if(x > bytes)
			bytes = x;
	}

	std::vector<CGlbWString> wsRenderContainer;
	if(multilineAlign == GLB_MULTILINEALG_RIGHT)
	{
		for (glbInt32 i = 0; i < wsContainer.size(); ++i)
		{
			glbDouble x = 0.0,y = 0.0;
			CGlbWString temp = wsContainer.at(i);
			const wchar_t *text = temp.c_str();
			for(;*text;text++) 
			{
				//取字模 
				const agg::glyph_cache* glyph = font_manager.glyph(*text);
				if(glyph) 
				{
					// 初始化gray8_adaptor实例 
					font_manager.init_embedded_adaptors(glyph, x, y); 
					x += glyph->advance_x; 
					y += glyph->advance_y; 
				} 
			}
			if(x != bytes)
			{
				glbInt32 tempSize = (bytes - x) / (textSize / 2);
				CGlbWString temp2 = L"";
				for(glbInt32 i = 0; i < tempSize;++i)
				{
					temp2.push_back(' ');
				}
				temp2 += temp;
				wsRenderContainer.push_back(temp2);
				continue;
			}
			wsRenderContainer.push_back(temp);
		}
	}
	else if(multilineAlign == GLB_MULTILINEALG_CENTER)
	{
		for (glbInt32 i = 0; i < wsContainer.size(); ++i)
		{
			CGlbWString temp = wsContainer.at(i);
			glbDouble x = 0.0,y = 0.0;
			const wchar_t *text = temp.c_str();
			for(;*text;text++) 
			{
				//取字模 
				const agg::glyph_cache* glyph = font_manager.glyph(*text);
				if(glyph) 
				{
					// 初始化gray8_adaptor实例 
					font_manager.init_embedded_adaptors(glyph, x, y); 
					x += glyph->advance_x; 
					y += glyph->advance_y; 
				} 
			}
			if(x != bytes)
			{
				glbInt32 tempSize = (bytes - x) / (textSize / 2) / 2;
				CGlbWString temp2 = L"";
				for(glbInt32 i = 0; i < tempSize;++i)
				{
					temp2.push_back(' ');
				}
				temp2 += temp;
				wsRenderContainer.push_back(temp2);
				continue;
			}
			wsRenderContainer.push_back(temp);
		}
	}
	else if(multilineAlign == GLB_MULTILINEALG_LEFT)
	{
		for (glbInt32 i = 0; i < wsContainer.size(); ++i)
		{
			CGlbWString temp = wsContainer.at(i);
			wsRenderContainer.push_back(temp);
		}
	}
	return wsRenderContainer;
}

//void GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::DealOpacityByPercent( unsigned char * buffer,
//	osg::Vec2d leftUp,glbInt32 sizeWidth,glbInt32 sizeHeight,glbInt32 textSize,double percent )
//{
//	if(mpr_fadeMode == GLB_FADETEXT_TOPTOBOTTOM)
//	{
//		double height = sizeHeight - ((sizeHeight - leftUp.y()) * percent + leftUp.y());
//		for(int i = 0; i < height ;i++)
//			for(int j = 0 ; j < sizeWidth ; j++)
//			{
//				buffer[(i * sizeWidth + j) * 4 + 3] = 0;
//			}
//	}
//	else
//	{
//		double width = leftUp.x() + (sizeWidth - leftUp.x()) * percent;
//		for(int i = 0; i < sizeHeight ;i++)
//		{
//			for(int j = width ; j < sizeWidth ; j++)
//			{
//				buffer[(i * sizeWidth + j) * 4 + 3] = 0;
//			}
//		}
//	}
//}

void GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::DealRenderImageAndText(
	std::vector<CGlbWString> wsRenderContainer,glbInt32 customMultilineSize,
	CGlbWString content,glbInt32 bytes,glbInt32 textSize,osg::Image *tempImage,
	GlbRenderImageInfo *imageInfo,GlbRenderTextInfo *textInfo,glbInt32 sizeWidth,
	glbInt32 sizeHeight,renb_type &renb,unsigned char * buffer,CGlbFeature *feature)
{
	if(tempImage)
		tempImage->scaleImage(sizeWidth,sizeHeight,tempImage->r());

	mpr_bufferSize.set(sizeWidth,sizeHeight);
	if (customMultilineSize > 1)
	{
		for (int i = 0; i < wsRenderContainer.size();++i)
			DealText(osg::Vec2((sizeWidth / 2.0 - bytes / 2.0) / 2.0 , sizeHeight / 2.0 - customMultilineSize * textSize / 2.0 + (i + 1) * textSize),wsRenderContainer.at(i),renb,textInfo,feature);
	}
	else
	{
		//因为现在因为特效扩展的都是textSize / 2,所以这里是textSize / 4.0
		double x = (sizeWidth / 2.0 - bytes / 2.0) / 2.0, y = sizeHeight / 2.0 + textSize / 4.0;
		DealText(osg::Vec2(x,y),content,renb,textInfo,feature);
	}
}

osg::Geometry * GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::CreateOutlineQuad( osg::Vec3 cornerVec,osg::Vec3 widthVec,osg::Vec3 heightVec )
{
	osg::Geometry *geom = new osg::Geometry;
	osg::ref_ptr<osg::StateSet> geomState = geom->getOrCreateStateSet();
	osg::ref_ptr<osg::Material> geomMaterial = 
		dynamic_cast<osg::Material *>(geomState->getAttribute(osg::StateAttribute::MATERIAL));
	if(geomMaterial == NULL)
		geomMaterial = new osg::Material;
	static int i = 0;
	geomMaterial->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(1,1,0,1));
	geomMaterial->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,0));
	geomState->setAttribute(geomMaterial,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
	osg::ref_ptr<osg::Vec3dArray> vertexArray = new osg::Vec3dArray(4);
	(*vertexArray)[0] = cornerVec + heightVec;
	(*vertexArray)[1] = cornerVec;
	(*vertexArray)[2] = cornerVec + widthVec;
	(*vertexArray)[3] = cornerVec + widthVec + heightVec;
	geom->setVertexArray(vertexArray);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::ref_ptr<osg::Vec3dArray> normals = new osg::Vec3dArray(1);
	(*normals)[0] = widthVec ^ heightVec;
	(*normals)[0].normalize();
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new DrawArrays(PrimitiveSet::LINE_LOOP,0,4));
	return geom;
}

void GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::DealBufferSizeExtend( 
	glbInt32 textSize,glbBool isBold,glbBool isItalic,
	glbBool isUnderline,osg::Image *image,glbDouble simplelineX,
	glbDouble simplelineY,glbInt32 &sizeWidth,glbInt32 &sizeHeight )
{
	if(isBold || isItalic)
		simplelineX += textSize / 2;
	if(isUnderline)
		simplelineY += textSize / 2;

	if(image == NULL)//表示sizeHeight现在为0
	{
		sizeHeight = simplelineY;
	}

	if(simplelineX > sizeWidth)
		sizeWidth = simplelineX;
	if(simplelineY > sizeHeight)
		sizeHeight = simplelineY;
}

void GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::DealExtendImageByMaxSize(
	GlbRenderImageInfo *imageInfo,CGlbFeature *feature,glbInt32 &sizeWidth,glbInt32 &sizeHeight)
{
	glbInt32 maxSize = -1;
	if(imageInfo && imageInfo->maxSize)
		maxSize =imageInfo->maxSize->GetValue(feature);

	if(maxSize < 100.0)
		return;
	sizeWidth *= maxSize / 100.0;
	sizeHeight *= maxSize / 100.0;
}

unsigned char * GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::GetAggBuffer()
{
	return mpr_aggBuffer;
}

osg::Image * GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::GetTexImage()
{
	return mpr_texImage.get();
}

osg::Vec2i GlbGlobe::CGlbGlobeMarkerDynamicLabelSymbol::GetAggBufferWidthAndHeight()
{
	return mpr_bufferSize;
}
